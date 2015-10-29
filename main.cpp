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
#include "DHT22.h"
#include "GP2Y10.h"
#include "server.h"
#include "DHT22.h"


Serial host(USBTX, USBRX);
C12832 shld_lcd (D11, D13, D12, D7, D10);   /* LCD on the shield (128x32) */
DigitalOut red_led(LED1);
SDFileSystem sd_card(PTE3, PTE1, PTE2, PTE4, "sd"); // MOSI, MISO, SCK, CS
DHT22 dht22(PTB18);
GP2Y10 particle_counter(PTB19, PTC10);

Ticker lights_ticker, dht22_ticker, particle_ticker, screen_ticker;

void query_light_state(void * param);

DHT22_data_t dht22_data = {0,0,0};
volatile int current_light_state = 0xf6f;
volatile int particle_count = 0;

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
    wait_ms(10);
    red_led = 1;
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

void query_sensor(void * param)
{
    extern TCPSocketConnection client;
    extern char buffer[HTTPD_MAX_REQ_LENGTH+1];

    sprintf(buffer, "{\"value\", %d}\n", *((int *)param));
    client.send_all(buffer, strlen(buffer));
}

void read_dht22()
{
    dht22.read(&dht22_data);
}

void update_screen()
{
    shld_lcd.locate(1,10);
    shld_lcd.printf("Humidity: %0.1f %%", (float)dht22_data.humidity);
    shld_lcd.locate(1,20);
    shld_lcd.printf("Temperature: %0.1f C", (float)dht22_data.temp);
}

void read_gy2y10()
{
    particle_count = (int)(particle_counter.read() * 100);
}

int main (void)
{
    http_server_start();
    read_dht22();

    //void http_server_add_handler(const char * uri, void (*handler)(void*), void * extra_data)
    http_server_add_handler("/on", send_light_command, (void *) 0xf6F);
    http_server_add_handler("/off", send_light_command, (void *) 0xf73);
    http_server_add_handler("/dim1", send_light_command, (void *) 0xf71);
    http_server_add_handler("/dim2", send_light_command, (void *) 0xf75);

    http_server_add_handler("/query/lights", query_light_state, NULL);
    http_server_add_handler("/query/temperature", query_sensor, &(dht22_data.temp));
    http_server_add_handler("/query/humidity", query_sensor, &(dht22_data.humidity));
    http_server_add_handler("/query/particle_count", query_sensor, (void*)&particle_count);

    lights_ticker.attach(poke_lights, 120);
    wait_ms(50);

    dht22_ticker.attach(read_dht22, 10);        /* Read current humidity level every 10s */
    wait_ms(50);

    particle_ticker.attach(read_gy2y10, 10);    /* Read current particle count every 10s */
    wait_ms(50);

    screen_ticker.attach(update_screen, 5);

    http_server_run(NULL);
}


