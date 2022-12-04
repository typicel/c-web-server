#ifndef ARRAY_H
#define ARRAY_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#define ARRAY_SIZE 20

// This interface implements a Circular Array  
typedef struct {
    int socket_nums[ARRAY_SIZE];           // The array
    int curr_size;                          // number of elements in the array
    int in; int out;                        // Where we will take elements in and out of the array 


    int num_reads, num_writes;

    pthread_mutex_t mutex;
    pthread_cond_t space_avail, items_avail;    // Our conditions to protect 
} array;

int  array_init(array *s);                   // initialize the array
int  array_put (array *s, int socket_num);   // place element into the array, block when full
int  array_get (array *s, int* socket_num);  // remove element from the array, block when empty
void array_free(array *s);                   // free the array's resources

#endif 