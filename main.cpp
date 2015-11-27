/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#include <stdio.h>
#include <string.h>
#include "mbed.h"

#include "C12832.h"
#include "debugging.h"
#include "DHT22.h"
#include "GP2Y10.h"
#include "MQTTClient.h"
#include "mqtt_client.h"
#include "MQTTEthernet.h"
#include "mqtt_event_handlers.h"
#include "mime.h"
#include "RC5Send.h"
#include "state.h"

Serial host(USBTX, USBRX);
C12832 shld_lcd (D11, D13, D12, D7, D10);   /* LCD on the shield (128x32) */
DigitalOut red_led(LED_RED);
DigitalOut green_led(LED_GREEN);
DigitalOut blue_led(LED_BLUE);
Ticker lights_ticker, dht22_ticker, particle_ticker, screen_ticker;

volatile int state = STATE_NONE;

/*
 * These functions are called as an interrupt when tickers tick. They move the state machine to a
 * different state.
 */
void set_update_screen()   { state = SCREEN_WRITE;    }
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
    update_screen();

    // Tickers to periodically update the state machine's state
    lights_ticker.attach(poke_lights, 120);       /* Every 3 minutes, send the current light code */
    wait_ms(500);                                 /* Space interrupts to avoid clashes */

    dht22_ticker.attach(set_update_sensors, 5);   /* Read sensor values every 5s */

    char s_state = STATE_NONE;
    while(1)
    {
        sleep();
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
                // If we send too quickly, MQTT falls over >.>
                wait_ms(100);

                if(read_gy2y10(m_client) == MQTT::FAILURE) {
                    mqtt_error();
                }
                // If we send too quickly, MQTT falls over >.>
                wait_ms(100);

                if(query_light_state(m_client) == MQTT::FAILURE) {
                    mqtt_error();
                }
                // If we send too quickly, MQTT falls over >.>
                wait_ms(100);

                update_screen();
                break;
            case SCREEN_WRITE:
                update_screen();
                break;
            default:
                break;
        }

        // Reset the state machine
        __disable_irq();
        state = STATE_NONE;
        __enable_irq();
        red_led = 1;

        m_client.yield(500);

    }
}
