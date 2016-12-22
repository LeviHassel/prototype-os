# PrototypeOS, Part 2: Producer/Consumer with Semaphores

#### Contributors
Levi Hassel and Eugene Kuznetsov

## Overview
The goal of this project was to design and implement a thread scheduler for our Prototype OS, running on an Altera DE2 board, which can only process one thread at a time. In order to accomplish this, we created a thread control block (TCB) to structure our threads’ characteristics and wrote many functions to handle the creation, priority-based scheduling and deletion of threads. We also designed a run queue to hold the threads that are waiting for execution.
When an interrupt occurs from the timer on the board, a flag is set, so that our assembly file myScheduler.S knows to call the scheduler and return the current stack pointer. The scheduler then stores this passed stack pointer in the correct TCB, updates the runQ to the next thread to run, and passes the correct stack pointer that corresponds with the thread to run next. The assembly file then finishes execution by setting this correct stack pointer and the frame pointer. Once a thread has finished running, it is destroyed. When all threads have finished, it returns to the main function. In our specific implementation, we create and schedule 12 threads.

## Organization
The project is split across many files.
Files from Project 1 Solution: alarm_handler.c/h, queue.c/h, thread_handler.c/h, exception_handler.S
New/Updated Files: project2.c, sem_handler.c/h, sem_queue.c/h

alarm_handler.c/h: Contains function that runs after each interrupt.
queue.c/h: Contains all functions for queue designed to hold TCB threads.
thread_handler.c/h: CContains all thread functions for the scheduler.
exception_handler.S: injects code into alt_exception_entry.S after every interrupt.
project2.c: Contains the main function, os_primitive(), which creates the bear and bees and starts the interrupt alarm, and contains definitions for bee() and bear(), the producer and cosumer functions.
sem_handler.c/h: Contains all semaphore functions which control its operations (create, up, down, delete etc.).
sem_queue.c/h: Slighlty modified version of queue.c/h designed to hold blocked threads for the semaphore.

## How to Run
An Altera DE1/DE2 board is required to run this code. Follow the steps [here](http://cse.unl.edu/~witty/class/csce351/Project/prelab/prelab.htm) to program the board and prepare the necessary software on your computer. After setting up the run configurations, simply import the files in this directory and run your configuration to begin the program.

## Expected Output
The program prints output in respect to the honey pot. It shows every time the number of honey portions changes, who was responsible (the bear or a bee) and how many times that bear/bee has done the same thing. Once a bee has deposited 15 portions of honey total, it commits suicide. Once the bear has eaten the honey pot five times, it commits suicide. At this point, the program returns to the main function and carries out an endless loop. See output.txt for actual output produced when running code.

## References
https://www.altera.com/products/processors/overview.html
https://www.altera.com/solutions/partners/partner-profile/terasic-inc-/board/altera-de2-115-development-and-education-board.html