/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#ifndef __mqtt_client_h_
#define __mqtt_client_h_

#include "MQTTEthernet.h"
#include "MQTTClient.h"

#define MQTT_HOST "192.168.16.19"
#define MQTT_PORT 1883

/**
 * Attempt to connect to the MQTT Server.
 *
 * Returns: Success code.
 *    0: Success
 *   -1: Failure
 *   -2: Timeout
 */
int mqtt_connect(MQTTEthernet& ipstack, MQTT::Client<MQTTEthernet, Countdown>& m_client);

/**
 * Sets up the MQTT Subscriptions for the lights.
 *
 * Returns: Success code.
 *    0: Success
 *   -1: Failure
 *   -2: Timeout
 */
int mqtt_subscriptions(MQTT::Client<MQTTEthernet, Countdown>& m_client);

/**
 * Prints that an MQTT error has occured to the LCD and resets the device 5s later.
 */
void mqtt_error();

#endif // __mqtt_client_h_
