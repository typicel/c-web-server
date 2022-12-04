#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include "file.h"
#include "mime.h"
#include "array.h"

#define PORT 12345 
#define BUFFER_SIZE 1024
#define MAX_DIR_LENGTH 2048 
#define MAX_RESPONSE_SIZE 262144
#define THREAD_POOL_SIZE 20

#define SERVER_FILES "../public"

typedef struct {
    pthread_t tid;
    array* socket_numbers;
} thread_info;


#endif