#ifndef __GP2Y10_h_
#define __GP2Y10_h_

#include "mbed.h"

#define SAMPLING_TIME 280
#define DELTA_TIME    40
#define SLEEP_TIME    9680

// Based on http://arduinodev.woofex.net/2012/12/01/standalone-sharp-dust-sensor/

class GP2Y10 {
public:
	GP2Y10(PinName _led, PinName _analog) :
		led(_led),
		analog(_analog)
		{}

	float read();
private:
	DigitalOut led;
	AnalogIn analog;
};

#endif // __GP2Y10_h_