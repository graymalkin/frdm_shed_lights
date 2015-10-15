#include <stdio.h>
#include <string.h>

#include "mbed.h"

#include "server.h"
#include "mime.h"
#include "EthernetInterface.h"
#include "NetworkAPI/buffer.hpp"
#include "NetworkAPI/select.hpp"
#include "NetworkAPI/ip/address.hpp"
#include "NetworkAPI/tcp/socket.hpp"
#include "SDFileSystem.h"

#include "debugging.h"


FILE *fp;
extern char buffer[HTTPD_MAX_REQ_LENGTH+1];
char httpHeader[HTTPD_MAX_HDR_LENGTH+1];
char contentType[HTTPD_MAX_MIME_LENGTH+1];
char path[HTTPD_MAX_FNAME_LENGTH+1];

extern TCPSocketConnection client;
extern Serial host;

int rdCnt;

int get_mime(const char * uri)
{
    char * ext = strrchr(uri, '.');
    for(int i = 0; i < MIME_TYPE_COUNT; i++){
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
    if(is_regular_file(path) != 0)
    {
        // Client is requesting a normal file
        DBG(host.printf("Regular file %s requested.\n", path);)
        http_serve_file(path, uri);
        return;
    }

    if(strcmp(uri, "/") == 0)
    {
        // Client is requesting the index.html page
        DBG(host.printf("Website index requested.\n");)
        sprintf(path, "/sd/index.html");
        http_serve_file(path, "/index.html");
        return;
    }

    // The file system doesn't like paths with a trailing /, so we need to remove
    //  that
    char *lstchr = strrchr(path, NULL) -1;
    if(*lstchr == '/')
        *lstchr = '\0';

    if(is_directory(path))
    {
        // Client is requesting index of directory
        DBG(host.printf("Index of %s directory requested.\n", path);)
        http_serve_directory(path, uri);
    }
    else
    {
        DBG(host.printf("%s (%s) was unhandled.\n", path, uri);)
        http_not_found(uri);
    }
}

int is_regular_file(const char *file)
{
    // if it's a file we should be able to fopen it
    FILE *fp_test = fopen(file, "r");
    if(fp_test == NULL){
        DBG(host.printf("%s is not a file.\n", file);)
        return 0;
    }

    DBG(host.printf("%s is a file.\n", file);)
    fclose(fp_test);
    return 1;
}

int is_directory(const char *file)
{
    // if it's a directory we should be able to opendir it
    DIR *dir_test = opendir(file);
    if(dir_test == NULL){
        DBG(host.printf("%s is not a directory.\n", file);)
        return 0;
    }

    DBG(host.printf("%s is a directory.\n", file);)
    closedir(dir_test);
    return 1;
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

void http_serve_directory(char * path, char * http_uri)
{
    DBG(host.printf("Path: [%s], length: %d\n", path, strlen(path));)
    DIR *dir = opendir(path);

    struct dirent *dirEntry;
    if (dir == NULL) {
        http_not_found(http_uri);
    }
    else
    {
        http_ok("text/html");

        sprintf(buffer, "<h1>Index of %s</h1>\n<hr />\n<pre>", path);
        client.send(buffer, strlen(buffer));

        while ((dirEntry = readdir(dir)) != NULL) {
            char * temp = (char*)malloc(strlen(path) + strlen(dirEntry->d_name));
            sprintf(temp, "%s/%s", path, dirEntry->d_name);
            if(is_directory(temp))
                sprintf(buffer, "<a href=\"./%s/\">%s/</a>\n", dirEntry->d_name, dirEntry->d_name);
            else
                sprintf(buffer, "<a href=\"./%s\">%s</a>\n", dirEntry->d_name, dirEntry->d_name);
            free(temp);
            client.send(buffer, strlen(buffer));
        }
        sprintf(buffer, "</pre>");
        client.send(buffer, strlen(buffer));
    }

}
