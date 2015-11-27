/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#include "C12832.h"
#include "DHT22.h"
#include "GP2Y10.h"
#include "mqtt_event_handlers.h"
#include "RC5Send.h"
#include "state.h"

extern DigitalOut red_led;
extern DigitalOut green_led;
extern DigitalOut blue_led;
extern C12832 shld_lcd;


int current_light_state = LIGHTS_ON;

extern volatile int state;

DHT22 dht22(PTB18);
DHT22_data_t dht22_data = {0,0,0};

GP2Y10 particle_counter(PTB11, PTC10);
int particle_count = 0;

char buf[50];
MQTT::Message message;

int query_light_state(MQTT::Client<MQTTEthernet, Countdown>& m_client)
{
    switch(current_light_state)
    {
        case LIGHTS_ON:
            sprintf(buf, "{\"lights\": \"on\"}");
            break;
        case LIGHTS_DIM1:
            sprintf(buf, "{\"lights\": \"dim1\"}");
            break;
        case LIGHTS_DIM2:
            sprintf(buf, "{\"lights\": \"dim2\"}");
            break;
        case LIGHTS_OFF:
            sprintf(buf, "{\"lights\": \"off\"}");
            break;
         default:
            sprintf(buf, "{\"lights\": \"invalid\"}");
            break;
    }


    message.qos = MQTT::QOS0;   // Send at least once
    // Do not null terminate -- we have a length field, and it will piss off the JS front end
    message.payloadlen = strlen(buf);
    message.payload = (void*)buf;
    return m_client.publish("sjc80/light_state", message);
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

void update_screen()
{
    dht22.read(&dht22_data);
    shld_lcd.locate(1,10);
    shld_lcd.printf("Humidity: %0.1f %%", ((float)dht22_data.humidity/10));
    shld_lcd.locate(1,20);
    shld_lcd.printf("Temperature: %0.1f C", ((float)dht22_data.temp/10));
}

void poke_lights()
{
    sendRC5raw(current_light_state);
}