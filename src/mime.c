#include <string.h>
#include <ctype.h>
#include "mime.h"

#define DEFAULT "application/octet-stream"

char* strlower(char* s){
    for(char* p = s; *p != '\0'; p++){
        *p = tolower(*p);
    }
    return s;
}

char* get_mime_type(char* filename){
    char* ext = strrchr(filename, '.');
    if(ext == NULL) return DEFAULT;
    ext++;

    // strlower(ext);

    if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0) { return "Content-type: text/html\r\n\r\n"; }
    if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0) { return "Content-type: image/jpg\r\n\r\n"; }
    if (strcmp(ext, "css") == 0) { return "Content-type: text/css\r\n\r\n"; }
    if (strcmp(ext, "js") == 0) { return "Content-type: application/javascript\r\n\r\n"; }
    if (strcmp(ext, "json") == 0) { return "Content-type: application/json\r\n\r\n"; }
    if (strcmp(ext, "txt") == 0) { return "Content-type: text/plain\r\n\r\n"; }
    if (strcmp(ext, "gif") == 0) { return "Content-type: image/gif\r\n\r\n"; }
    if (strcmp(ext, "png") == 0) { return "Content-type: image/png\r\n\r\n"; }

    return DEFAULT;
}