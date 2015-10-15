#include <stdio.h>

#include <string.h>
#include "mbed.h"
 
#include "C12832.h"
#include "EthernetInterface.h"
#include "NetworkAPI/buffer.hpp"
#include "NetworkAPI/select.hpp"
#include "NetworkAPI/ip/address.hpp"
#include "NetworkAPI/tcp/socket.hpp"
#include "RC5Send.h"
#include "SDFileSystem.h"

#include "debugging.h"
#include "mime.h"
#include "server.h"

Serial host(USBTX, USBRX);
C12832 shld_lcd (D11, D13, D12, D7, D10);   /* LCD on the shield (128x32) */
DigitalOut red_led(LED1);
SDFileSystem sd_card(PTE3, PTE1, PTE2, PTE4, "sd"); // MOSI, MISO, SCK, CS
Ticker lights_ticker;
volatile int current_light_state = 0xf6f;

void send_light_command(void * command)
{
    sendRC5raw((int) command);
    current_light_state = (int) command;
}

void poke_lights()
{
    sendRC5raw(current_light_state);
    
    red_led = 0;
    wait_ms(10);
    red_led = 1;
    wait_ms(10);   
}

int main (void)
{
    http_server_start();
    
    //void http_server_add_handler(const char * uri, void (*handler)(void*), void * extra_data)
    http_server_add_handler("/on", send_light_command, (void *) 0xf6F);
    http_server_add_handler("/off", send_light_command, (void *) 0xf73);
    http_server_add_handler("/dim1", send_light_command, (void *) 0xf71);
    http_server_add_handler("/dim2", send_light_command, (void *) 0xf75);

    lights_ticker.attach(poke_lights, 120);

    http_server_run(NULL);
    // Thread http_server = Thread(http_server_run);

    // while(true)
    //     Thread::yield();
}


