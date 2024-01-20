#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

/* Structure representing a customer */
typedef struct {
    int id;                        /* Customer ID */
    char arrival_time[9];          /* Arrival time of the customer */
    int start_time;                /* Start time of service for the customer */
    int end_time;                  /* End time of service for the customer */
    char service_type;             /* Type of service requested by the customer */
} customer_t;

/* Structure representing a queue */
typedef struct {
    customer_t* customers;         /* Array to store customers in the queue */
    int front;                     /* Index of the front element in the queue */
    int rear;                      /* Index of the rear element in the queue */
    int size;                      /* Current size of the queue */
    int capacity;                  /* Maximum capacity of the queue */
} queue_t;

int enqueue(queue_t* queue, customer_t* customer);

customer_t dequeue(queue_t* queue);

int queue_size(queue_t* queue);

void free_customer_queue(queue_t* queue);

#endif