/*
 * (C) The University of Kent and Simon Cooksey 2015.
 */

#ifndef __Server_h_
#define __Server_h_

#define HTTPD_SERVER_PORT           80
#define HTTPD_MAX_REQ_LENGTH        4095
#define HTTPD_MAX_RESPONSE_LENGTH   4096
#define HTTPD_MAX_HDR_LENGTH        1024
#define HTTPD_MAX_FNAME_LENGTH      1024
#define HTTPD_MAX_MIME_LENGTH       256

typedef struct handler_t {
    const char * uri;
    void (*handler)(void * param);
    void * extra_data;

    handler_t * next;
} handler_t;

void http_server_start();
void http_server_run(const void * threading_argument);
void http_server_add_handler(const char * uri, void (*handler)(void*), void * extra_data);

#endif // __Server_h_
