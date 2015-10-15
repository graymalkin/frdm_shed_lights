
#include <stdio.h>
#include <string.h>

#include "server.h"
#include "mime.h"
#include "C12832.h"
#include "EthernetInterface.h"
#include "NetworkAPI/buffer.hpp"
#include "NetworkAPI/select.hpp"
#include "NetworkAPI/ip/address.hpp"
#include "NetworkAPI/tcp/socket.hpp"
#include "SDFileSystem.h"

#include "debugging.h"

FILE *fp;
char buffer[HTTPD_MAX_REQ_LENGTH+1];
char httpHeader[HTTPD_MAX_HDR_LENGTH+1];
char contentType[HTTPD_MAX_MIME_LENGTH+1];
char path[HTTPD_MAX_FNAME_LENGTH+1];
char *uristr;
char *eou;
char *qrystr;

extern Serial host;
extern C12832 shld_lcd;

EthernetInterface eth;
TCPSocketServer server;
TCPSocketConnection client;
handler_t * function_list = NULL;

int rdCnt;

void http_ok(const char * contentType);
void http_serve_file(char * file, char * http_uri);

handler_t * http_find_function(const char * uri)
{
    for(handler_t * current_item = function_list; current_item != NULL; current_item = current_item->next)
    {
        if(strcmp(uri, current_item->uri) == 0)
            return current_item;
    }
    return NULL;
}

void http_server_add_handler(const char * uri, void (*handler)(void*), void * extra_data)
{

    handler_t * new_handler = (handler_t*)malloc(sizeof(handler_t));
    new_handler->uri = uri;
    new_handler->handler = handler;
    new_handler->extra_data = extra_data;
    new_handler->next = NULL;

    if(function_list == NULL)
    {
        function_list = new_handler;
        return;
    }


    handler_t * last = function_list;
    while(last->next != NULL)
        last = last->next;

    last->next = new_handler;
}

void http_server_start()
{
    eth.init();
    eth.connect();

    DBG(host.baud(115200);)

    shld_lcd.cls();
    shld_lcd.locate(1,1);
    shld_lcd.printf("IP Address is %s\n", eth.getIPAddress());
 
    server.bind(HTTPD_SERVER_PORT);
    server.listen();
}

void http_server_run()
{
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
                http_entity_too_large();
                break;
            } else {
                buffer[n]=0;
            }
            if (!strncmp(buffer, "GET ", 4)) {
                uristr = buffer + 4;
                eou = strstr(uristr, " ");
                if (eou == NULL) {
                    http_bad_request();
                } else {
                    *eou = 0;
                    get_file(uristr);
                }
            }
        }
        client.close();
    }
}


int get_mime(char * uri)
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

void get_file(char* uri)
{
    DBG(host.printf("get_file %s\n", uri);)

    // Construct the path to the file requested
    // TODO: Define /sd/ as the root
    sprintf(path, "/sd%s", uri);

    // TODO: Define the index page name
    if(is_regular_file(path))
    {
        // Client is requesting a normal file
        DBG(host.printf("Regular file %s requested.\n", path);)
        http_serve_file(path, uri);
    }
    else if(strcmp(uri, "/") == 0)
    {
        // Client is requesting the index.html page
        DBG(host.printf("Website index requested.\n");)
        sprintf(path, "/sd/index.html");
        http_serve_file(path, (char*)"/index.html");
    }
    else if(is_directory(path))
    {
        // Client is requesting index of directory
        DBG(host.printf("Index of %s directory requested.\n", path);)
    }
    else
    {
        handler_t * handler = http_find_function(uri);
        if(handler != NULL)
        {
            http_ok("text/html");
            sprintf(buffer, "%s\n", uri);
            client.send(buffer, strlen(buffer));

            handler->handler(handler->extra_data);

            sprintf(buffer, "Done.\n");
            client.send(buffer, strlen(buffer));
        }
        else{
            DBG(host.printf("%s (%s) was unhandled.\n", path, uri);)
            http_not_found(uri);
        }
    }
}

// Credit to http://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
int is_regular_file(const char *file)
{
    struct stat path_stat;
    stat(file, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int is_directory(const char *file)
{
    struct stat path_stat;
    stat(file, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

void http_ok(const char * contentType)
{
    DBG(host.printf("HTTP OK (200).\n");)
    if(contentType != NULL)
        sprintf(httpHeader, "HTTP/1.1 200 OK\r\n"
                            "Content-Type: %s\r\n"
                            "Connection: Close\r\n\r\n", contentType);
    else
        sprintf(httpHeader, "HTTP/1.1 200 OK\r\n"
                            "Connection: Close\r\n\r\n");
}

void http_not_found(char * uri)
{
    sprintf(httpHeader,"HTTP/1.1 404 Not Found \r\n"
                        "Content-Type: text/plain\r\n"
                        "Connection: Close\r\n\r\n");
    client.send(httpHeader,strlen(httpHeader));
    sprintf(buffer, "404.\r\n"
                    "%s not found on this server.", uri);
    client.send(buffer, strlen(buffer));
}

void http_entity_too_large()
{
    DBG(host.printf("HTTP Request Entity Too Large (413).\n");)
    sprintf(httpHeader,"HTTP/1.1 413 Request Entity Too Large \r\nContent-Type: text\r\nConnection: Close\r\n\r\n");
    client.send(httpHeader,strlen(httpHeader));
    client.send(buffer,strlen(buffer));
}

void http_bad_request()
{
    DBG(host.printf("HTTP Bad Request (400).\n");)
    sprintf(httpHeader,"HTTP/1.1 400 Bad Request \r\nContent-Type: text\r\nConnection: Close\r\n\r\n");
    client.send(httpHeader,strlen(httpHeader));
    client.send(buffer,strlen(buffer));
}

void http_serve_file(char * file, char * http_uri)
{
    // Open file in read mode
    fp = fopen(file, "r");
    // 404
    if (fp == NULL) {
        http_not_found(http_uri);
    }
    else
    {
        int mime = get_mime(http_uri);
        if(mime >= 0)
            http_ok(mime_types[mime].mime);
        else
            http_ok(NULL);

        client.send(httpHeader,strlen(httpHeader));
        while ((rdCnt = fread(buffer, sizeof( char ), HTTPD_MAX_RESPONSE_LENGTH, fp)) == HTTPD_MAX_RESPONSE_LENGTH) {
            client.send(buffer, rdCnt);
        }
        client.send(buffer, rdCnt);
        fclose(fp);
    }
}
