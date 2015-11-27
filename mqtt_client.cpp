/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#include "C12832.h"
#include "mqtt_client.h"
#include "mqtt_event_handlers.h"

extern C12832 shld_lcd;

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
