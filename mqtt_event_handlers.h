/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#ifndef __mqtt_event_handlers_h_
#define __mqtt_event_handlers_h_

#include "MQTTEthernet.h"
#include "MQTTClient.h"

/**
 * Send a JSON encoded object with the current light state over MQTT.
 *
 *   Lights on:    {"lights": "on"}
 *   Lights dim 1: {"lights" : "dim1"}
 *   Lights dim 2: {"lights" : "dim2"}
 *   Lights off:   {"lights" : "off"}
 */
int query_light_state(MQTT::Client<MQTTEthernet, Countdown>& m_client);

/**
 * Set the lights to a new state based on the payload of an MQTT MessageData
 *
 *   Lights on:     0
 *   Lights dim 1:  1
 *   Lights dim 1:  2
 *   Lights off:    3
 */
void set_light_state(MQTT::MessageData& message_data);

/**
 * Send a JSON encoded object with the current temperature and humidity readings over MQTT.
 *
 * Data:   {"dht22": [23.2, 42.8]}
 * Where the 1st array element is the temperature, and the second is the humidity.
 */
int read_dht22(MQTT::Client<MQTTEthernet, Countdown>& m_client);

/**
 * Send a JSON encoded object with the particle count readings over MQTT.
 *
 * Data:   {"particle_count": 4.2}
 */
int read_gy2y10(MQTT::Client<MQTTEthernet, Countdown>& m_client);

int send_topics(MQTT::Client<MQTTEthernet, Countdown>& m_client);

int location(MQTT::Client<MQTTEthernet, Countdown>& m_client);

int send_mappable(MQTT::Client<MQTTEthernet, Countdown>& m_client);


/**
 * Display sensor readings on the LCD
 */
void update_screen();

/**
 * Re-transmit the current light setting.
 */
void poke_lights();

#endif // __mqtt_event_handlers_h_
