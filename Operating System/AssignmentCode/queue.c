#include "queue.h"

/* This function adds a new customer to the end of the given queue */
int enqueue(queue_t* queue, customer_t* customer) {
    int index;

    /* Check if the queue is already full */
    if (queue->size == queue->capacity) {
        printf("Customer Queue already Full!!! Can't Add.\n");
        return 0;
    }

    /* Calculate the index for the new customer */
    index = (queue->rear + 1) % queue->capacity;

    /* Add the new customer to the queue */
    queue->customers[index] = *customer;
    queue->rear = index;
    queue->size++;

    return 1;
}

/* This function removes the first customer from the queue (Since it is FIFO queue) */
customer_t dequeue(queue_t* queue) {
    customer_t customer;

    /* Check if the queue is empty */
    if (queue->size == 0) {
        printf("Customer Queue is Empty!!! Can't Remove.\n");
        customer.id = -1;
        customer.service_type = ' ';
        return customer;
    }

    /* Get the customer at the front of the queue */
    customer = queue->customers[queue->front];

    /* Update the front index of the queue */
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    return customer;
}

/* Checks the size of the c_queue */
int queue_size(queue_t* queue) {
    return queue->size;
}

/* This function frees the memory used by the given customer queue */
void free_customer_queue(queue_t* queue) {
    /* Free the memory used by the customers */
    free(queue->customers);
    /* Free the memory used by the queue */
    free(queue);
}