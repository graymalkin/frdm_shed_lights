/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#include <stdio.h>
#include <string.h>

#include "mbed.h"

#include "C12832.h"

#define MQTTCLIENT_QOS1 1
#define MQTTCLIENT_QOS2 1
#include "MQTTEthernet.h"
#include "MQTTClient.h"

#include "debugging.h"
#include "mime.h"
#include "DHT22.h"
#include "GP2Y10.h"
#include "DHT22.h"
#include "RC5Send.h"


Serial host(USBTX, USBRX);
C12832 shld_lcd (D11, D13, D12, D7, D10);   /* LCD on the shield (128x32) */
DigitalOut red_led(LED_RED);
DigitalOut green_led(LED_GREEN);
DigitalOut blue_led(LED_BLUE);

DHT22 dht22(PTB18);
GP2Y10 particle_counter(PTB11, PTC10);
Ticker lights_ticker, dht22_ticker, particle_ticker, screen_ticker;

enum STATE {
    STATE_NONE,
    READ_SENSORS,
    SCREEN_WRITE
} STATE;

enum LIGHT_STATE {
    LIGHTS_ON   = 0xf6f,
    LIGHTS_DIM1 = 0xf73,
    LIGHTS_DIM2 = 0xf71,
    LIGHTS_OFF  = 0xf75
} LIGHT_STATE;

DHT22_data_t dht22_data = {0,0,0};
int current_light_state = LIGHTS_ON;
int particle_count = 0;
volatile int state = STATE_NONE;
char buf[50];
MQTT::Message message;

int query_light_state(MQTT::Client<MQTTEthernet, Countdown>& m_client)
{
    char lights_on[] = "{\"lights\": \"on\"}";
    char lights_dim1[] = "{\"lights\": \"dim1\"}";
    char lights_dim2[] = "{\"lights\": \"dim2\"}";
    char lights_off[] = "{\"lights\": \"off\"}";
    char lights_invalid[] = "{\"lights\": \"invalid\"}";

    char * pub_str = lights_invalid;

    switch(current_light_state)
    {
        case LIGHTS_ON:
            pub_str = lights_on;
            break;
        case LIGHTS_DIM1:
            pub_str = lights_dim1;
            break;
        case LIGHTS_DIM2:
            pub_str = lights_dim2;
            break;
        case LIGHTS_OFF:
            pub_str = lights_off;
            break;
         default:
            break;
    }
    message.qos = MQTT::QOS0;   // Send at least once
    // Do not null terminate -- we have a length field, and it will piss off the JS front end
    message.payloadlen = strlen(pub_str);
    message.payload = (void*)pub_str;
    return m_client.publish("sjc80/light_state", message);
}

void poke_lights()
{
    sendRC5raw(current_light_state);
}

void set_light_state(MQTT::MessageData& message_data)
{
    MQTT::Message& message = message_data.message;
    // Copy the message payload in to a null terminated cstring
    char c_buf[2] = {0,0};
    c_buf[0] = *((char*)message.payload);

    // Convert the string to a long
    long command = strtol(c_buf, NULL, 10);

    switch (command)
    {
        case 0:
            // Lights on
            green_led = 0;
            blue_led = 0;
            sendRC5raw(LIGHTS_ON);
            current_light_state = LIGHTS_ON;
            break;
        case 1:
            // Lights dim 1
            green_led = 0;
            blue_led = 1;
            sendRC5raw(LIGHTS_DIM1);
            current_light_state = LIGHTS_DIM1;
            break;
        case 2:
            // Lights dim 2
            green_led = 1;
            blue_led = 0;
            sendRC5raw(LIGHTS_DIM2);
            current_light_state = LIGHTS_DIM2;
            break;
        case 3:
            // Lights off
            green_led = 1;
            blue_led = 1;
            sendRC5raw(LIGHTS_OFF);
            current_light_state = LIGHTS_OFF;
            break;
        default:
            // Invalid
            break;
    }
}

void update_screen()
{
    shld_lcd.locate(1,10);
    shld_lcd.printf("Humidity: %0.1f %%", ((float)dht22_data.humidity/10));
    shld_lcd.locate(1,20);
    shld_lcd.printf("Temperature: %0.1f C", ((float)dht22_data.temp/10));
}

int read_dht22(MQTT::Client<MQTTEthernet, Countdown>& m_client)
{
    dht22.read(&dht22_data);

    sprintf(buf, "{\"dht22\": [%0.2f, %0.2f]}", ((float)dht22_data.temp/10), ((float)dht22_data.humidity/10));
    message.qos = MQTT::QOS0;   // No QOS, just send and hope
    // Do not null terminate -- we have a length field, and it will piss off the JS front end
    message.payloadlen = strlen(buf);
    message.payload = (void*)buf;
    return m_client.publish("sjc80/dht22", message);
}

int read_gy2y10(MQTT::Client<MQTTEthernet, Countdown>& m_client)
{
    particle_count = (int)(particle_counter.read() * 100);

    sprintf(buf, "{\"particle_count\": %0.2f}", particle_count);
    message.qos = MQTT::QOS0;   // No QOS, just send and hope
    // Do not null terminate -- we have a length field, and it will piss off the JS front end
    message.payloadlen = strlen(buf);
    message.payload = (void*)buf;
    return m_client.publish("sjc80/particle_count", message);
}

int mqtt_connect(MQTTEthernet& ipstack, MQTT::Client<MQTTEthernet, Countdown>& m_client)
{
    char hostname[] = "192.168.16.19";
    int port = 1883;
    shld_lcd.locate(0,0);
    shld_lcd.printf("Connecting...\n");
    int result = ipstack.connect(hostname, port);
    shld_lcd.locate(0,0);
    if (result != 0)
        shld_lcd.printf("result from TCP connect is %d\n", result);
    else
        shld_lcd.printf("Connected to MQTT.");

    return m_client.connect();
}

int mqtt_subscriptions(MQTT::Client<MQTTEthernet, Countdown>& m_client)
{
    return m_client.subscribe("sjc80/set_lights", MQTT::QOS1, set_light_state);
}


void mqtt_error()
{
    shld_lcd.locate(0,0);
    shld_lcd.cls();
    shld_lcd.printf("MQTT Error.");
    wait_ms(5000);
    // If something goes wrong, just reboot. Nasty nasty hack.
    NVIC_SystemReset();
}

void set_update_screen()   { state = SCREEN_WRITE;  }
void set_update_lights()   {    }
void set_update_sensors()  { state = READ_SENSORS;    }

int main (void)
{
    // Mostly pinched from the HelloMQTT demo here
    // https://developer.mbed.org/teams/mqtt/code/HelloMQTT
    MQTTEthernet ipstack;
    MQTT::Client<MQTTEthernet, Countdown> m_client(ipstack);
    if(mqtt_connect(ipstack, m_client) == MQTT::FAILURE) {
        mqtt_error();
    }
    mqtt_subscriptions(m_client);

    // Turn off the LEDs
    red_led = 1;
    green_led = 1;
    blue_led = 1;

    // Initial read of sensors and push to screen.
    dht22.read(&dht22_data);
    update_screen();

    // Tickers to periodically update the state machine's state
    lights_ticker.attach(set_update_lights, 120); /* Every 3 minutes, send the current light code */
    wait_ms(500);                                 /* Space interrupts to avoid clashes */

    dht22_ticker.attach(set_update_sensors, 1);   /* Read sensor values every 1s */

    char s_state = STATE_NONE;
    while(1)
    {
        m_client.yield(500);

        // Take a copy of the save state and then reset the state machine for the next IRQ
        __disable_irq();
        s_state = state;
        state = STATE_NONE;
        __enable_irq();

        // Heartbeat
        if(s_state)
            red_led = 0;


        // Note: On error interrupts are disabled, and the loop will break reasonably quickly.
        //       This is so I can carry the state over to the next loop in order to try to
        //       reconnect. Interrupts will be re-enabled when the system copies the state into
        //       s_state
        switch (s_state)
        {
            case READ_SENSORS:
                if(read_dht22(m_client) == MQTT::FAILURE) {
                    mqtt_error();
                }
                if(read_gy2y10(m_client) == MQTT::FAILURE) {
                    mqtt_error();
                }
                if(query_light_state(m_client) == MQTT::FAILURE) {
                    mqtt_error();
                }
                update_screen();
                break;
            case SCREEN_WRITE:
                update_screen();
                break;
            default:
                break;
        }

        __disable_irq();
        state = STATE_NONE;
        __enable_irq();

        red_led = 1;
    }
}
