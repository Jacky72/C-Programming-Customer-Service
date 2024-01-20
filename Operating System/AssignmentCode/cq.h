#ifndef CQ_H
#define CQ_H

#include "queue.h"
#include "threads.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

/* Structure representing arguments for mutex */
typedef struct {
    pthread_mutex_t* reader_mutex; /* Mutex for controlling reader access */
    pthread_mutex_t* writer_mutex; /* Mutex for controlling writer access */
    pthread_cond_t*  c_queue_not_empty; /* Condition variable for signaling when customer inserted to queue */
} mutex_args_t;

/* Structure representing service times */
typedef struct {
    int tw;                        /* Time required for 'W'ithdrawal */
    int td;                        /* Time required for 'D'eposit */
    int ti;                        /* Time required for 'I'nquiry */
} service_time_t;

/* Structure representing arguments for the customer thread */
typedef struct{
    int m;                         /* Number of Customers to Insert to Queue */
    queue_t* c_queue;              /* FIFO queue */
    int tc;                        /* Time for each customer to join the Queue */
    FILE* write;                   /* File pointer for writing output */
    mutex_args_t mutex_args;       /* Arguments for mutex */
    int* complete;                 /* Variable to check if all the Customers are inserted */
} customer_thread_args;

/* Structure representing arguments for the teller thread */
typedef struct {
    queue_t* c_queue;              /* FIFO queue */
    service_time_t *service_time;  /* Pointer to service times (td, tw, ti) */
    FILE* write;                   /* File pointer for writing output */
    mutex_args_t mutex_args;       /* Arguments for mutex */
    int* complete;                 /* Variable to check if all the Customers are inserted */
    int tellerID;                  /* ID of the teller thread */
    int teller_served;             /* Number of customers served by the unique teller thread */
} teller_thread_args;

int main(int argc, char** argv);

bool is_not_integer(int x);

#endif
