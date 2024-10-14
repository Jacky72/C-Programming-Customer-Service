Name: Zhenqi Zhang

Assignment Unit: Operating System

# Bank Customer Queue Program

This program simulates a Bank Customer Queue system using C language. 
It is designed to manage Customer requests for various banking services and having multiple Teller ( Unique Thread ) to handle Customer interactions concurrently.

## Features

- Customer Queue Management: The program maintains a queue to manage customer requests.
- Multi-Threading: Multiple threads are used to handle customer interactions concurrently.
- Service Time Differentiation: Different service times are assigned for withdrawal, deposit, and inquiry operations.
- Thread Synchronization: Mutexes and condition variables are used to synchronize access to the customer queue.
- Output Logging: The program provides logging functionality to record customer interactions and system events.

## Prerequisites

- C Compiler: The program requires a C compiler to compile and run the source code.

## How to Run

1. Compile the code using the C compiler make file function:

    make clean

    make

2. Run the program:

    valgrind --leak-check=full --track-origins=yes -s ./cq m tc tw td ti
        Where:
            m - The number of Customers to insert to the FIFO Queue.
            tc - Time to insert a Customer to the FIFO Queue.
            tw - Time it take to Serve the Customer to Withdrawal.
            td - Time it take to Serve the Customer to Deposit.
            ti - Time it take to Serve the Customer to Inquiry.

## Program Structure

- AssignmentCode:

    - 'cq.h' and 'cq.c': These files contain the Main Functions of the Program to interact with Customers.
    - 'queue.c' and 'queue.h': These files contain the functions and structs to setup and interact with the queue.
    - 'thread.c' and 'thread.h': These files contains the functions of the Customer thread and the Teller thread, and how they Interact.
    = 'makefile': File that is used to compile the required files together
    - 'c_file': This is the file containing the Customers and their Required Service.
    - 'r_log': This is the output file where all the process made by the program would be stored.

- readme: This is the file that you are reading right now. Shows how to compile the program and run the program.

- AssignmentOneReport: Specific Report for the Assignment. Explaining the decision made for the Assignment.
