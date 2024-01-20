#ifndef THREADS_H
#define THREADS_H

#include "queue.h"
#include "cq.h"

#include <time.h>

void *customer(void* arg);

void* teller(void* arg);

char* get_current_time();

#endif