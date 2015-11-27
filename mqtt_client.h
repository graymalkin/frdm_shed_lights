/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#ifndef __mqtt_client_h_
#define __mqtt_client_h_

#include "MQTTEthernet.h"
#include "MQTTClient.h"

int mqtt_connect(MQTTEthernet& ipstack, MQTT::Client<MQTTEthernet, Countdown>& m_client);
int mqtt_subscriptions(MQTT::Client<MQTTEthernet, Countdown>& m_client);
void mqtt_error();

#endif // __mqtt_client_h_
