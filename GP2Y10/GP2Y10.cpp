#include "GP2Y10.h"

// Based on http://arduinodev.woofex.net/2012/12/01/standalone-sharp-dust-sensor/
float GP2Y10::read()
{
	float voMeasured = 0;
	float calcVoltage = 0;
	float dustDensity = 0;

	led = 0;
	wait_us(SAMPLING_TIME);

	voMeasured = analog; // read the dust value

	wait_us(DELTA_TIME);
	led = 1; // turn the LED off
	wait_us(SLEEP_TIME);

	// 0 - 3.3V mapped to 0 - 1023 integer values
	// recover voltage
	calcVoltage = voMeasured * 3.3;

	// linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
	// Chris Nafis (c) 2012
	dustDensity = 0.17 * calcVoltage - 0.1;

	return dustDensity;
}