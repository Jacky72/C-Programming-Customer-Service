#include "cq.h"

/* valgrind --leak-check=full --track-origins=yes -s ./cq 10 1 1 1 1 */
/* gcc -Wall -ansi -pedantic -g cq.c -o cq */

/* This function creates a new customer queue with the given capacity (m) */
queue_t* create_customer_queue(int capacity) {

    /* Allocate memory for the queue */
    queue_t* queue = (queue_t*) malloc(sizeof(queue_t));
    /* Check if memory allocation was successful */
    if (queue == NULL) {
        printf("Error: Memory allocation failed for queue_t.\n");
        return NULL;
    }

    /* Set the initial values for the queue */
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->capacity = capacity;

    /* Allocate memory for the customers in the queue */
    queue->customers = (customer_t*) malloc(capacity * sizeof(customer_t));
    /* Check if memory allocation was successful */
    if (queue == NULL) {
        printf("Error: Memory allocation failed for customer_t array.\n");
        free(queue);
        return NULL;
    }

    /* Return the newly created queue */
    return queue;
}

bool is_not_integer(int x) {
    return false;
}

int main(int argc, char** argv) {
    /* Declare variables at the beginning of the block */
    int m, tc, tw, td, ti, complete = 1, teller1_served, teller2_served, teller3_served, teller4_served;
    queue_t* c_queue; /* The First In First Out Queue*/
    pthread_t customer_thread, teller1_thread,teller2_thread,teller3_thread,teller4_thread; /* Threads that are created to run functions */
    pthread_mutex_t reader_mutex, writer_mutex; /* Declear the Reader and Writer Mutex*/
    customer_thread_args* customer_args; /* Argument to pass to the Customer Function by the Customer Thread*/
    teller_thread_args* teller1_args, *teller2_args, *teller3_args, *teller4_args;  /* Arguments to pass to the Teller Function by the Teller Thread*/
    FILE* write; 
    mutex_args_t mutex_args; /* Pass the Mutex Arguments */
    service_time_t service_times; /* Initialize the Service Waiting Time*/
    pthread_cond_t c_queue_not_empty = PTHREAD_COND_INITIALIZER; /* Initialize the condition variable */

    /* Checks that the number of input elements are correct*/
    if (argc == 6) {
        m = atoi(argv[1]);
        tc = atoi(argv[2]);
        tw = atoi(argv[3]);
        td = atoi(argv[4]);
        ti = atoi(argv[5]);

        if ((m <=0) || (is_not_integer(m))){
            printf("\nI'm Sorry, but please enter a valid amount of Customer that is wanting to seek for help.\n\n");
            exit(1);
        }

        if ((tc <=0) || (is_not_integer(tc))){
            printf("\nI'm Sorry, but please enter a valid time input to enqueue customers to the queue.\n\n");
            exit(1);
        }

        if ((tw <=0) || (is_not_integer(tw)) || (td <=0) || (is_not_integer(td)) || (ti <=0) || (is_not_integer(ti))){
            printf("\nI'm Sorry, but please enter a valid time input (Higher than 0, since you need time to serve Customer.).\n\n");
            exit(1);
        }

        /* Initialize mutexes */
        if (pthread_mutex_init(&reader_mutex, NULL) != 0) {
            printf("\nError initializing reader_mutex\n");
            exit(1);
        }
        if (pthread_mutex_init(&writer_mutex, NULL) != 0) {
            printf("\nError initializing reader_mutex\n");
            exit(1);
        }

        /* Open output file for writing */
        write = fopen("r_log.txt", "w");
        if (write == NULL) {
            printf("\nError: Unable to open output file.\n");
            exit(1);
        }

        fprintf(write, "========================================================================\n");
        fprintf(write, "|                             Customer Log                             |\n");
        fprintf(write, "========================================================================\n\n");

        /* Create a customer queue */
        c_queue = create_customer_queue(m);

        /* Create mutex_args struct and fill it with mutex pointers */
        mutex_args.reader_mutex = &reader_mutex;
        mutex_args.writer_mutex = &writer_mutex;
        mutex_args.c_queue_not_empty = &c_queue_not_empty;

        /* Set the time for the three different services. */
        service_times.tw = tw;
        service_times.td = td;
        service_times.ti = ti;

        /* Initialize the Arguments to input to the Customer_Thread_Function */
        customer_args = (customer_thread_args*)malloc(sizeof(customer_thread_args));
        customer_args->m = m;
        customer_args->c_queue = c_queue;
        customer_args->tc = tc;
        customer_args->write = write;
        customer_args->mutex_args = mutex_args;
        customer_args->complete = &complete;

        /* Initialize the Arguments to input to the Teller1 */
        teller1_served = 0;
        teller1_args = (teller_thread_args*)malloc(sizeof(teller_thread_args));
        teller1_args->c_queue = c_queue;
        teller1_args->service_time = &service_times;
        teller1_args->write = write;
        teller1_args->mutex_args = mutex_args;
        teller1_args->complete = &complete;
        teller1_args->tellerID = 1;
        teller1_args->teller_served = teller1_served;

        /* Initialize the Arguments to input to the Teller2 */
        teller2_served = 0;
        teller2_args = (teller_thread_args*)malloc(sizeof(teller_thread_args));
        teller2_args->c_queue = c_queue;
        teller2_args->service_time = &service_times;
        teller2_args->write = write;
        teller2_args->mutex_args = mutex_args;
        teller2_args->complete = &complete;
        teller2_args->tellerID = 2;
        teller2_args->teller_served = teller2_served;

        /* Initialize the Arguments to input to the Teller3 */
        teller3_served = 0;
        teller3_args = (teller_thread_args*)malloc(sizeof(teller_thread_args));
        teller3_args->c_queue = c_queue;
        teller3_args->service_time = &service_times;
        teller3_args->write = write;
        teller3_args->mutex_args = mutex_args;
        teller3_args->complete = &complete;
        teller3_args->tellerID = 3;
        teller3_args->teller_served = teller3_served;

        /* Initialize the Arguments to input to the Teller4 */
        teller4_served = 0;
        teller4_args = (teller_thread_args*)malloc(sizeof(teller_thread_args));
        teller4_args->c_queue = c_queue;
        teller4_args->service_time = &service_times;
        teller4_args->write = write;
        teller4_args->mutex_args = mutex_args;
        teller4_args->complete = &complete;
        teller4_args->tellerID = 4;
        teller4_args->teller_served = teller4_served;

        /* Create threads */
        pthread_create(&customer_thread, NULL, customer, (void*)customer_args);
        pthread_create(&teller1_thread, NULL, teller, (void*)teller1_args);
        pthread_create(&teller2_thread, NULL, teller, (void*)teller2_args);
        pthread_create(&teller3_thread, NULL, teller, (void*)teller3_args);
        pthread_create(&teller4_thread, NULL, teller, (void*)teller4_args);

        /* Wait for threads to finish */
        pthread_join(customer_thread, NULL);
        pthread_join(teller1_thread, NULL);
        pthread_join(teller2_thread, NULL);
        pthread_join(teller3_thread, NULL);
        pthread_join(teller4_thread, NULL);

        fprintf(write, "\n========================================================================\n");
        fprintf(write, "\nTeller Statistic\n");
        fprintf(write, "\nTeller-1 serves %d customers.\n", teller1_args->teller_served);
        fprintf(write, "\nTeller-2 serves %d customers.\n", teller2_args->teller_served);
        fprintf(write, "\nTeller-3 serves %d customers.\n", teller3_args->teller_served);
        fprintf(write, "\nTeller-4 serves %d customers.\n", teller4_args->teller_served);
        fprintf(write, "========================================================================\n");

        /* Free the customer queue, and the Customer and Teller Arguments */
        free_customer_queue(c_queue);
        free(customer_args);
        free(teller1_args);
        free(teller2_args);
        free(teller3_args);
        free(teller4_args);

        /* Destroy mutexes */
        pthread_mutex_destroy(&reader_mutex);
        pthread_mutex_destroy(&writer_mutex);

        fclose(write);

        return 0;
    } else {
        printf("Error: Expected 5 arguments, but received %d.\n", argc - 1);
        return 1;
    }
}