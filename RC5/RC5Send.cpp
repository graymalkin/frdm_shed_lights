#include "mbed.h"

DigitalOut RC5led(PTC1);
Ticker RC5carrier;

void toggleRC5led()   //function that toggles the led for the 36KHz carrier
{
    RC5led=!RC5led;
}

// Replace this by PWM ? or leave it in
void RC5sendbit(int rc5bit)   //Function to transmit a bit
{
    //36kHz: 1/36000 = 27.78µS - 32 x = 888.9
    //38kHz: 1/38000 = 26.32µS - 32 x = 842.1
    //40kHz: 1/40000 = 25.00µS - 32 x = 800.0
    if (rc5bit == 0) {
        //Logic 0: 36kHz Carrier 889µS on, 889µS off (32 pulses = 888.9)
        RC5carrier.attach_us(&toggleRC5led, 13.5); // Was 14, Should be 13.9?
        wait_us(864); //Wait 889µS should be 864?
        RC5carrier.detach();
        wait_us(864);
        RC5led = 0;
    } else {
        //Logic 1: Carrier 889µS off, 889uS on
        wait_us(864);
        RC5carrier.attach_us(&toggleRC5led, 13.5);
        wait_us(864);
        RC5carrier.detach();
        RC5led = 0;
    }
}

void sendRC5code(int togglebit, int adres, int commando)
{
    int copyadres;
    int copycommando;
    RC5sendbit(1); //startbit 1
    RC5sendbit(1); //extra commandbit, leave it 1
    RC5sendbit(togglebit); // toggle bit finishes preamble
    //5 data bits from the adres:
    adres = adres & 0x1F; //1F = b00011111
    copyadres = (adres & 0x10) >>4;
    RC5sendbit(copyadres);
    copyadres = (adres & 0x08) >>3;
    RC5sendbit(copyadres);
    copyadres = (adres & 0x04) >>2;
    RC5sendbit(copyadres);
    copyadres = (adres & 0x02) >>1;
    RC5sendbit(copyadres);
    copyadres = adres & 0x01;
    RC5sendbit(copyadres);
    //6 data bits from the command:
    commando = commando & 0x3F; //3F = b00111111
    copycommando = (commando & 0x20) >>5;
    RC5sendbit(copycommando);
    copycommando = (commando & 0x10) >>4;
    RC5sendbit(copycommando);
    copycommando = (commando & 0x08) >>3;
    RC5sendbit(copycommando);
    copycommando = (commando & 0x04) >>2;
    RC5sendbit(copycommando);
    copycommando = (commando & 0x02) >>1;
    RC5sendbit(copycommando);
    copycommando = commando & 0x01;
    RC5sendbit(copycommando);
}

void sendRC5raw(int data)
{
    RC5sendbit(1); //startbit 1
    RC5sendbit(1); //extra commandbit, leave it 1
    
    for(int i  = 11; i >= 0; i--)
    {
        RC5sendbit((data >> i) & 0x01);
    }
}