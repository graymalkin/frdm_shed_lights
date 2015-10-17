/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

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

void query_light_state(void * param);

volatile int current_light_state = 0xf6f;

void send_light_command(void * command)
{
    sendRC5raw((int) command);
    current_light_state = (int) command;
    query_light_state(NULL);
}

void poke_lights()
{
    sendRC5raw(current_light_state);
    
    red_led = 0;
    wait_ms(10)
;    red_led = 1;
    wait_ms(10);   
}

void query_light_state(void * param)
{
    // HACK HACK HACK
    extern TCPSocketConnection client;
    extern char buffer[HTTPD_MAX_REQ_LENGTH+1];

    switch(current_light_state)
    {
        case 0xf6f:
            sprintf(buffer, "{\"lights\", \"%s\"}", "on");
            break;
        case 0xf73:
            sprintf(buffer, "{\"lights\", \"%s\"}", "off");
            break;
        case 0xf71:
            sprintf(buffer, "{\"lights\", \"%s\"}", "somewhat_dim");
            break;
        case 0xf75:
            sprintf(buffer, "{\"lights\", \"%s\"}", "quite_dim");
            break;
         default:
            sprintf(buffer, "{\"lights\", \"unknown\"}");
            break;           
    }

    client.send(buffer, strlen(buffer));
}

int main (void)
{
    http_server_start();
    
    //void http_server_add_handler(const char * uri, void (*handler)(void*), void * extra_data)
    http_server_add_handler("/on", send_light_command, (void *) 0xf6F);
    http_server_add_handler("/off", send_light_command, (void *) 0xf73);
    http_server_add_handler("/dim1", send_light_command, (void *) 0xf71);
    http_server_add_handler("/dim2", send_light_command, (void *) 0xf75);
    http_server_add_handler("/query", query_light_state, NULL);

    lights_ticker.attach(poke_lights, 120);

    http_server_run(NULL);
}


