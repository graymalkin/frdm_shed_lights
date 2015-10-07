/*
    Copyright (c) 2011 Pro-Serv

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

    Usage and assumptions:
    Based on TSOP 1738 IR device, Vcc to 5Volt, Gnd to Gnd, Out has pull-up of 10K
    to Vcc and Out has diode (Cathode to Out) connected to pin (p16) (Anode to p16)
    and pin (p16) has mode PullUp

    This will allow a "standard" RC5 device to be used to read it transmitted codes.
    Still work in progress (in due time) to add newer rc6 devices

    Excellent source on RC5 protocol: http://www.ustr.net/infrared/infrared1.shtml
 */

#ifndef MBED_H
#include "mbed.h"
#endif

#ifndef RC5_PIN_ASSTERED
#define RC5_PIN_ASSTERED   1
#endif

#ifndef RC5_DELAY_PERIOD
#define RC5_DELAY_PERIOD 4752
#endif

#ifndef RC5_INTERVAL_PERIOD
#define RC5_INTERVAL_PERIOD 1728
#endif

#ifndef RC5_FINSISH_PERIOD
#define RC5_FINISH_PERIOD 65000
#endif

#ifndef RC5_SAMPLE_COUNT
#define RC5_SAMPLE_COUNT    14
#endif

namespace RC5
{

/* RC5Detect adds a RC5 compatible data entry using InterruptIn & DigitalIn.
 *
 * This is done by sampling the specified pin at specific interrupt driven intervals
   and sampling the actual pin level
 */

class RC5Detect
{

protected:
    InterruptIn *_int;
    DigitalIn   *_inp;
    DigitalOut  *_out;     // out was added temporary for debugging
    Ticker      *_ticker;
    Timeout     *_delay;
    Timeout     *_finish;
    int         _stepCount;
    int         _pulseCount;
    int         _sampleInterval;
    int         _sampleDelay;
    int         _sampleFinish;
    int         _assertValue;
    int         _rc5Data;
    int         _rc5Cmd;
    int         _rc5Adr;
    int         _rc5Ready;

    /* initialise class
     *
     * @param PinName p is a valid pin for InterruptIn/DigitalIn &
     * @param PinMode m is a mode the InterruptIn/DigitalIn should use.
     */
    void init(PinName p, PinMode m) {
        _sampleInterval          = RC5_INTERVAL_PERIOD;
        _sampleDelay             = RC5_DELAY_PERIOD;
        _sampleFinish            = RC5_FINISH_PERIOD;
        _assertValue             = RC5_PIN_ASSTERED;
        _int = new InterruptIn( p );
        _inp = new DigitalIn( p );
        //_out = new DigitalOut( x );     // out to be added temporary for debugging
        //_out->write(1);                 // same, set high level
        _int->mode( m );
        _inp->mode( m );
        _stepCount = 0;
        _ticker = new Ticker;
        _delay = new Timeout;
        _finish = new Timeout;
        _int->fall(this, &RC5Detect::isr_int);
    }

public:

    friend class Ticker;
    friend class Timeout;

    /* PinDetect constructor(s) & overloading */
    RC5Detect() {
        error("Provide a valid PinName");
    }

    RC5Detect(PinName p) {
        init( p, PullUp);
    }

    RC5Detect(PinName p, PinMode m) {
        init( p, m );
    }

    /* PinDetect destructor */
    ~RC5Detect() {
        if ( _ticker ) delete( _ticker );
        if ( _delay )  delete( _delay );
        if ( _finish ) delete( _finish );
        if ( _inp )    delete( _inp );
        if ( _int )    delete( _int );
    }

    /* Set the sampling delay time in microseconds. */
    void setDelaySample(int i = RC5_DELAY_PERIOD) {
        _sampleDelay = i;
    }

    /* Set the sampling interval time in microseconds. */
    void setIntervalSample(int i = RC5_INTERVAL_PERIOD) {
        _sampleInterval = i;
    }

    /* Set the sampling finish time in microseconds. */
    void setFinishSample(int i = RC5_FINISH_PERIOD) {
        _sampleFinish = i;
    }

    /* Set the value used as assert. */ /* not yet implemented */
    void setAssertValue (int i = RC5_PIN_ASSTERED) {
        _assertValue = i & 1;
    }

    void mode(PinMode m) {
        _inp->mode( m );
    }

    bool getReadyState(void) {
        return _rc5Ready;
    }

    void setReadyState(int i) {
        _rc5Ready = i & 1;
    }

    int getRC5Address(void) {
        return _rc5Adr;
    }

    int getRC5Command(void) {
        return _rc5Cmd;
    }

protected:
    /* The interrupt service routines are: pin status, delay, interval and finish
     * the pin status is intended to capture start of transmission */
    void isr_int(void) {
        if (_stepCount == 0) {                                  // first trigger event?
            _stepCount++;
            _rc5Ready = _rc5Cmd = _rc5Adr = 0;                  //
            _delay->attach_us(this, &RC5Detect::isr_delay, _sampleDelay);
            _finish->attach_us(this, &RC5Detect::isr_finish, _sampleFinish);
        }
        _pulseCount++;
    }

    /* The delay is intended to hold of data sampling untill we get meaningfull bits */
    void isr_delay() {
        _ticker->attach_us( this, &RC5Detect::isr_interval, _sampleInterval );
        _rc5Data = _inp->read() & 0x01;                         // add first bit of RC5
        //_out->write(0);
        //_out->write(1);
    }

    /* The interval will sample all successive rc5 address and command bits */
    void isr_interval() {
        if (_stepCount == 10) {
            _ticker->detach();                                  // sampling is done now !
        }
        _rc5Data = (_rc5Data << 1) + (_inp->read() & 0x01);     // shift partial and add next bit
        _stepCount++;
        //_out->write(0);
        //_out->write(1);
    }

    /* The finish is intended to do cleanup at end of frame and prepare for next frame */
    void isr_finish() {
        if ((_stepCount == 11) && (_pulseCount > 10)) {
            _rc5Cmd = _rc5Data & 0x3f;                          // get command part of RC5
            _rc5Adr = (_rc5Data >> 6) & 0x1f;                   // get address part of RC5
            _rc5Ready = 1;
        }
        _pulseCount = _stepCount = _rc5Data = 0;                // init for next bit stream
    }

};

};                                                              // namespace RC5 ends.

using namespace RC5;

