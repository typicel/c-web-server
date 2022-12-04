#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/types.h>
#include "file.h"

// wackyness
void prepend(char* s, const char* t){
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}

struct filedata* file_load(char* filename){
    char* buffer, *p;
    struct stat buf;
    int bytes_read, bytes_remaining, total_bytes = 0;

    if(stat(filename, &buf) == -1) return NULL; // get size of file
    if(!(buf.st_mode & 0100000)) return NULL; // if is regular file

    FILE* fp = fopen(filename, "rb");
    if(fp == NULL) return NULL;

    bytes_remaining = buf.st_size;
    p = buffer = malloc(bytes_remaining);
    if(buffer == NULL) return NULL;

    while(bytes_read = fread(p, 1, bytes_remaining, fp), bytes_read != 0 && bytes_remaining > 0) {
        if(bytes_read == -1){
            free(buffer);
            return NULL;
        }

        bytes_remaining -= bytes_read;
        p += bytes_read;
        total_bytes += bytes_read;
    }

    struct filedata* filedata = malloc(sizeof *filedata);
    if(filedata == NULL) {
        free(buffer);
        return NULL;
    }

    filedata->data = buffer;
    filedata->size = total_bytes;
    return filedata;
}

void file_free(struct filedata *filedata){
    free(filedata->data);
    free(filedata);
}