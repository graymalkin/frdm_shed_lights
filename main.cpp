#include "mbed.h"

#include "C12832.h"

#include "EthernetInterface.h"
#include "HTTPServer.h"
#include "FsHandler.h"
#include "SDFileSystem.h"
#include "RpcHandler.h"
#include "mbed_rpc.h"


#include "NetworkAPI/buffer.hpp"
#include "NetworkAPI/select.hpp"
#include "NetworkAPI/ip/address.hpp"
#include "NetworkAPI/tcp/socket.hpp"
#include "RC5Send.h"

#include "mime.h"
#include <stdio.h>
#include <string.h>
 
#define HTTPD_SERVER_PORT        80
#define HTTPD_MAX_REQ_LENGTH     4095
#define HTTPD_MAX_HDR_LENGTH     1024
#define HTTPD_MAX_FNAME_LENGTH   1024
#define HTTPD_MAX_MIME_LENGTH    256
#define MURI_MAX_RESP_LENGTH     1024
#define I2C_BUFFER_SIZE          255
#define MAX_CLIENTS              5
#define LISTENING_PORT           1234

Serial host(USBTX, USBRX);
C12832 shld_lcd (D11, D13, D12, D7, D10);   /* LCD on the shield (128x32) */
DigitalOut red_led(LED1);
SDFileSystem sd_card(PTE3, PTE1, PTE2, PTE4, "sd"); // MOSI, MISO, SCK, CS
EthernetInterface eth;
TCPSocketServer server;
TCPSocketConnection client;

#define DEBUGGING
#ifdef DEBUGGING
#define DBG(x) x
#else
#define DBG(x)
#endif
 
char buffer[HTTPD_MAX_REQ_LENGTH+1];
char httpHeader[HTTPD_MAX_HDR_LENGTH+1];
char contentType[HTTPD_MAX_MIME_LENGTH+1];
char filename[HTTPD_MAX_FNAME_LENGTH+1];
char obuf[MURI_MAX_RESP_LENGTH+1];
char *uristr;
char *eou;
char *qrystr;

int get_mime(const char * uri);

FILE *fp;
int rdCnt;
 
void get_file(char* uri)
{
    DBG(host.printf("get_file %s\n", uri);)
    char *lstchr = strrchr(uri, NULL) -1;
    if ('/' == *lstchr) {
        DBG(host.printf("Open directory /sd%s\n", uri);)
        *lstchr = 0;
        sprintf(filename, "/sd%s", uri);
        DIR *d = opendir(filename);
        if (d != NULL) {
            sprintf(httpHeader,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: Close\r\n\r\n");
            client.send(httpHeader,strlen(httpHeader));
            sprintf(httpHeader,"<html><head><title>Directory Listing</title></head><body><h1>%s</h1><ul>", uri);
            client.send(httpHeader,strlen(httpHeader));
            struct dirent *p;
            while((p = readdir(d)) != NULL) {
                DBG(host.printf("%s\n", p->d_name);)
                sprintf(httpHeader,"<li>%s</li>", p->d_name);
                client.send(httpHeader,strlen(httpHeader));
            }
        }
        closedir(d);
        DBG(host.printf("Directory closed\n");)
        sprintf(httpHeader,"</ul></body></html>");
        client.send(httpHeader,strlen(httpHeader));
    } else {
        sprintf(filename, "/sd%s", uri);
        fp = fopen(filename, "r");
        if (fp == NULL) {
            sprintf(httpHeader,"HTTP/1.1 404 Not Found \r\nContent-Type: text\r\nConnection: Close\r\n\r\n");
            client.send(httpHeader,strlen(httpHeader));
            client.send(uri,strlen(uri));
        } else {
            int mime = get_mime(uri);
            if(mime >= 0)
                sprintf(httpHeader,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\nConnection: Close\r\n\r\n", mime_types[mime].mime);
            else
                sprintf(httpHeader,"HTTP/1.1 200 OK\r\nConnection: Close\r\n\r\n");

            client.send(httpHeader,strlen(httpHeader));
            while ((rdCnt = fread(buffer, sizeof( char ), 4095, fp)) == 4095) {
                client.send(buffer, rdCnt);
            }
            client.send(buffer, rdCnt);
            fclose(fp);
        }
    }
}
 
void get_cgi(char* uri)
{
    char *result;
    // muri(uri, data, obuf);
    if (!strncmp(obuf, "200 ", 4)) {
        result = obuf +4;
        sprintf(httpHeader,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: Close\r\n\r\n");
        client.send(httpHeader,strlen(httpHeader));
        client.send(result,strlen(result));
        DBG(host.printf("result:  %s", result);)
    } else {
        sprintf(httpHeader,"HTTP/1.1 ");
        client.send(httpHeader,strlen(httpHeader));
        client.send(obuf,strlen(obuf));
        sprintf(httpHeader,"\r\n");
        client.send(httpHeader,strlen(httpHeader)); 
    }
    
}
 

int get_mime(const char * uri)
{
    char * ext = strrchr(uri, '.');
    for(int i = 0; i < 641; i++){
        if(strcmp(ext, mime_types[i].extension) == 0){
            DBG(host.printf("Found extension %s for mime %s\n", ext, mime_types[i].mime);)
            return i;
        }
    }
    DBG(host.printf("Could not find MIME type for extension %s (length: %d)\n", ext, strlen(ext));)
    return -1;
}

int main (void)
{
//    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();

    DBG(host.baud(115200);)

    shld_lcd.cls();
    shld_lcd.locate(1,1);
    shld_lcd.printf("IP Address is %s\n", eth.getIPAddress());
 
//    TCPSocketServer server;
    server.bind(HTTPD_SERVER_PORT);
    server.listen();
    
    // init_dio(); //initialize pmd digital IO
 
    while (true) {
        DBG(host.printf("\nWait for new connection...\r\n");)
        server.accept(client);
        client.set_blocking(false, 1500); // Timeout after (1.5)s
 
        DBG(host.printf("Connection from: %s\r\n", client.get_address());)
        while (true) {
            int n = client.receive(buffer, sizeof(buffer));
            if (n <= 0) break;
            DBG(host.printf("Recieved Data: %d\r\n\r\n%.*s\r\n",n,n,buffer);)
            if (n >= 1024) {
                sprintf(httpHeader,"HTTP/1.1 413 Request Entity Too Large \r\nContent-Type: text\r\nConnection: Close\r\n\r\n");
                client.send(httpHeader,strlen(httpHeader));
                client.send(buffer,n);
                break;
            } else {
                buffer[n]=0;
            }
            if (!strncmp(buffer, "GET ", 4)) {
                uristr = buffer + 4;
                eou = strstr(uristr, " ");
                if (eou == NULL) {
                    sprintf(httpHeader,"HTTP/1.1 400 Bad Request \r\nContent-Type: text\r\nConnection: Close\r\n\r\n");
                    client.send(httpHeader,strlen(httpHeader));
                    client.send(buffer,n);
                } else {
                    *eou = 0;
                    if (!strncmp(uristr, "/muri/", 6)) {
                        get_cgi(uristr+6);
                    } else {
                        get_file(uristr);
                    }
                }
            }
        }
 
        client.close();
    }
}



// EthernetInterface interface;
// HTTPServer svr;
// //     SDFileSystem(PinName mosi, PinName miso, PinName sclk, PinName cs, const char* name);
// SDFileSystem sd_card(PTE3, PTE1, PTE2, PTE4, "sd"); // MOSI, MISO, SCK, CS

// void handle_packet(Buffer * buffer);


// void jon_test(){
//     red_led = 1;
//     sendRC5raw(0xf7a);
//     wait(0.1);
//     red_led = 0;
// }

// int main()
// {
//     jon_test();

//     HTTPFsRequestHandler::mount("/sd/", "/");
//     svr.addHandler<HTTPFsRequestHandler>("/");
    
//     interface.init();
//     interface.connect();
    
//     shld_lcd.locate(1,1);
//     shld_lcd.printf("IP: %s:80", interface.getIPAddress());

//     if (!svr.start(80, &interface)) {
//         error("Server not starting !");
//         exit(0);
//     }
    
//     while(1) {
//         svr.poll();
//     }
// }


// void
// handle_packet(Buffer * buffer)
// {
//     char strn[256]; 
//     int size = buffer->size();
//     if(!size)
//         return;
        
//     // Read the 1st char of the buffer into cmd
//     char cmd;
//     buffer->read(&cmd, 1);

//     if(size > 255){
//         buffer->read(&strn, 255);
//         strn[255] = '\0';
//     } else {
//         buffer->read(&strn, size);
//         strn[size] = '\0';
//     }
//     DBG(host.printf("%s\n", strn);)
    
//     shld_lcd.locate(1,20);

//     switch(cmd)
//     {
//         case 'O':
//             // Switch lights on
//             shld_lcd.printf("Lights on. ");
//             break;
//         case 'F':
//             // Switch lights off
//             shld_lcd.printf("Lights off.");
//             break;
//         case 'D':
//             // Dim lights
//             shld_lcd.printf("Lights dim.");
//             break;
//         default:
//             //shit
//             return;
//     }
// }
