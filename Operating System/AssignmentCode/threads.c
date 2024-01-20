#include "threads.h"

void *customer(void* arg){
    customer_thread_args* args = (customer_thread_args*)arg;
    int i, id;
    char service_type;
    char* arrival_time;
    int* complete = args->complete;

    /* Open input file for reading and output file for writing */
    FILE* read = fopen("c_file.txt", "r");

    if (read == NULL) {
        printf("Error: Unable to open input file.\n");
        return NULL;
    }

    /* Read customer information from the input file and add them to the customer queue */
    for (i = 0; i < args->m && fscanf(read, "%d %c", &id, &service_type) == 2; i++) {
        customer_t* customer = (customer_t*)malloc(sizeof(customer_t));
        customer->id = id;
        customer->service_type = service_type;
        arrival_time = get_current_time();
        strcpy(customer->arrival_time, arrival_time);

        /* Locks the Queue (Reader Mutex) so No Dequeue is happenning during the Enqueue of new Customer */
        pthread_mutex_lock(args->mutex_args.reader_mutex);
        enqueue(args->c_queue, customer);
        pthread_mutex_unlock(args->mutex_args.reader_mutex);

        free(arrival_time);

        printf("\nEnqueue Customer... %d\n", i+1);

        /* Locks the Queue (Writer Mutex) to Write customer information to the output file */
        pthread_mutex_lock(args->mutex_args.writer_mutex);
        fprintf(args->write, "\n----------------------------------Enqueue---------------------------------\n");
        fprintf(args->write, "Customer %d : %c\nArrival time: %s\n", customer->id, customer->service_type, customer->arrival_time);
        fprintf(args->write, "------------------------------------------------------------------------\n\n");
        pthread_mutex_unlock(args->mutex_args.writer_mutex);

        /* Signal the condition variable to wake up one teller each Time*/
        pthread_cond_signal(args->mutex_args.c_queue_not_empty);

        sleep(args->tc);

        /* Free the customer after adding it to the queue */
        free(customer);
    }

    *complete = 0;

    fclose(read);

    return NULL;
}

void* teller(void* arg) {
    teller_thread_args* targs = (teller_thread_args*) arg;
    int customer_num, ti, tw, td;
    char service_type;
    char* start_time, *response_time, *completion_time, *terminate_time;
    customer_t customer;
    int* complete = targs->complete;

    ti = targs->service_time->ti;
    td = targs->service_time->td;
    tw = targs->service_time->tw;

    start_time = get_current_time();

    while (1) {
        /* Check if there is no more work left */
        if (queue_size(targs->c_queue) == 0 && *complete == 0) {
            printf("\nTeller finished.\n");
            break;
        }

        /* Lock the reader mutex and wait until there is at least one customer in the queue */
        pthread_mutex_lock(targs->mutex_args.reader_mutex);
        while (queue_size(targs->c_queue) == 0 && *complete == 1) {
            pthread_cond_wait(targs->mutex_args.c_queue_not_empty, targs->mutex_args.reader_mutex);
        }
        pthread_mutex_unlock(targs->mutex_args.reader_mutex);

        /* This repeat check is here since if the serve time is too fast, some of the Tellers might not even need to wake up, and when we
            Wake them up at the end, theres no more stuff in the queue, So we want them to check after they wake, and if there are no more in
            the queue, and all customer have been served, then they just end, instead of trying to get something from the queue.*/
        if (queue_size(targs->c_queue) == 0 && *complete == 0) {
            printf("\nTeller finished.\n");
            break;
        }

        /* Lock the reader mutex and dequeue the customer from the front of the queue */
        pthread_mutex_lock(targs->mutex_args.reader_mutex);
        customer = dequeue(targs->c_queue);
        pthread_mutex_unlock(targs->mutex_args.reader_mutex);

        /* Get the customer number and service type */
        customer_num = customer.id;
        service_type = customer.service_type;

        /* Get the current time as the response time */
        response_time = get_current_time();

        /* Lock the writer mutex before accessing the r_log file */
        pthread_mutex_lock(targs->mutex_args.writer_mutex);

        /* Write to the r_log file with the teller number, customer number, arrival time, and response time */
        fprintf(targs->write, "\n----------------------------------Process---------------------------------\n");
        fprintf(targs->write, "Teller: %d\nCustomer: %d\nArrival time: %s\nResponse time: %s\n", targs->tellerID, customer_num, customer.arrival_time , response_time);
        fprintf(targs->write, "------------------------------------------------------------------------\n");

        pthread_mutex_unlock(targs->mutex_args.writer_mutex);

        free(response_time);

        /* Serve the customer */
        switch (service_type) {
            case 'I':
                sleep(ti);
                break;
            case 'D':
                sleep(td);
                break;
            case 'W':
                sleep(tw);
                break;
            default:
                break;
        }

        /* Get the current time as the completion time */
        completion_time = get_current_time();

        pthread_mutex_lock(targs->mutex_args.writer_mutex);

        /* Write to the r_log file with the teller number, customer number, arrival time, and completion time */
        fprintf(targs->write, "\n---------------------------------Finish---------------------------------\n");
        fprintf(targs->write, "Teller: %d\nCustomer: %d\nArrival time: %s\nCompletion time: %s\n", targs->tellerID, customer_num, customer.arrival_time, completion_time);
        fprintf(targs->write, "------------------------------------------------------------------------\n");

        pthread_mutex_unlock(targs->mutex_args.writer_mutex);

        free(completion_time);

        /* Calculate how many Customers did the Teller Serve */
        targs->teller_served ++;
    }

    /* Get the current time as the termination time of the Teller */
    terminate_time = get_current_time();

    pthread_mutex_lock(targs->mutex_args.writer_mutex);

    /* Write to the r_log file with the teller number, customer number, arrival time, and completion time */
    fprintf(targs->write, "\n------------------------------------Terminate----------------------------\n");
    fprintf(targs->write, "Teller: %d\nCustomer Served: %d\nArrival time: %s\nCompletion time: %s\n", targs->tellerID, targs->teller_served, start_time, terminate_time);
    fprintf(targs->write, "------------------------------------------------------------------------\n");

    pthread_mutex_unlock(targs->mutex_args.writer_mutex);

    free(terminate_time);
    free(start_time);

    pthread_cond_signal(targs->mutex_args.c_queue_not_empty);

    /* Terminate the Thread After there are No more Customer left to join the c_queue and No more Customer in the current Queue */
    pthread_exit(NULL);

    return NULL;
}

/* This function gets the current time in the format HH:MM:SS */
char* get_current_time() {
    time_t current_time;
    struct tm* time_info;
    char* time_str;
    time(&current_time);
    time_info = localtime(&current_time);

    /* Allocate memory for the time string */
    time_str = (char*)malloc(sizeof(char) * 9);
    /* Check if memory allocation was successful */
    if (time_str == NULL) {
        printf("Error: Memory allocation failed for current time string.\n");
        return NULL;
    }

    /* Convert the time to a string */
    sprintf(time_str, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

    /* Return the time string */
    return time_str;

    /* Free the memory used by the time string */
    free(time_str);
}