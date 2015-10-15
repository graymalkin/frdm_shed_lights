#ifndef __Server_h_
#define __Server_h_

#define HTTPD_SERVER_PORT           80
#define HTTPD_MAX_REQ_LENGTH        4095
#define HTTPD_MAX_RESPONSE_LENGTH   4095
#define HTTPD_MAX_HDR_LENGTH        1024
#define HTTPD_MAX_FNAME_LENGTH      1024
#define HTTPD_MAX_MIME_LENGTH       256

void get_file(char* uri);

int get_mime(const char * uri);
int is_regular_file(const char *file);
int is_directory(const char *file);
void http_ok(const char * contentType);
void http_not_found(char * uri);
void http_entity_too_large();
void http_bad_request();
void http_serve_file(char * file, char * http_uri);
void http_serve_directory(char * path, char * http_uri);


#endif // __Server_h_
