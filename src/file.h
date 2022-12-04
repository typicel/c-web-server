#ifndef FILE_H
#define FILE_H

struct filedata {
    int size;
    void* data;
};

extern struct filedata* file_load(char* filename);
extern void file_free(struct filedata* filedata);

#endif

