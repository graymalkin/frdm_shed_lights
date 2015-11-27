/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#ifndef __state_h_
#define __state_h_

/**
 * States for the MQTT state machine
 */
typedef enum STATE {
    STATE_NONE,
    READ_SENSORS,
    SCREEN_WRITE
} STATE;

/**
 * States for the lights
 */
typedef enum LIGHT_STATE {
    LIGHTS_ON   = 0xf6f,
    LIGHTS_DIM1 = 0xf73,
    LIGHTS_DIM2 = 0xf71,
    LIGHTS_OFF  = 0xf75
} LIGHT_STATE;

#endif // __state_h_
