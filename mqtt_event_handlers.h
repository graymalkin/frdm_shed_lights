/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#ifndef __mqtt_event_handlers_h_
#define __mqtt_event_handlers_h_

#include "MQTTEthernet.h"
#include "MQTTClient.h"

int query_light_state(MQTT::Client<MQTTEthernet, Countdown>& m_client);
void set_light_state(MQTT::MessageData& message_data);
int read_dht22(MQTT::Client<MQTTEthernet, Countdown>& m_client);
int read_gy2y10(MQTT::Client<MQTTEthernet, Countdown>& m_client);

void update_screen();
void poke_lights();

#endif // __mqtt_event_handlers_h_
