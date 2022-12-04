#include "array.h"

int array_init(array* s) {

    // Initialize 
    pthread_mutex_init(&s->mutex, NULL);
    pthread_cond_init(&s->space_avail, NULL);
    pthread_cond_init(&s->items_avail, NULL);
    for(int i = 0; i < ARRAY_SIZE; i++){
        s->socket_nums[i] = -1;
    }

    s->curr_size = s->in = s->out = 0;
    s->num_reads = s->num_writes = 0;
    return 0;
}

// producer
int array_put(array* s, int socket_num){
    pthread_mutex_lock(&s->mutex);

    // Wait on consumer to signal that there's space available 
    while(s->curr_size == ARRAY_SIZE) pthread_cond_wait(&s->space_avail, &s->mutex); 
        s->socket_nums[s->in] = socket_num; 
        s->in = (s->in + 1) % ARRAY_SIZE;
        s->curr_size++;

        s->num_writes++;
    pthread_cond_signal(&s->items_avail); // Signal to the consumer that there are items available

    pthread_mutex_unlock(&s->mutex);
    return 0;
}

// consumer
int array_get(array* s, int* socket_num){
    pthread_mutex_lock(&s->mutex);

    // If there is nothing in the array, wait until producer signals there are items
    while(s->curr_size == 0) pthread_cond_wait(&s->items_avail, &s->mutex);
        *socket_num = s->socket_nums[s->out];
        s->out = (s->out + 1) % ARRAY_SIZE;
        s->curr_size--;

        s->num_reads++;
    pthread_cond_signal(&s->space_avail); // Signal to the producer that there is space in the array

    pthread_mutex_unlock(&s->mutex);


    return 0;
}

void array_free(array* s){
    pthread_mutex_destroy(&s->mutex);
    pthread_cond_destroy(&s->space_avail);
    pthread_cond_destroy(&s->items_avail);
    return;
}