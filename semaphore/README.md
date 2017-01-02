# PrototypeOS, Part 2: Producer/Consumer with Semaphores

### Contributors
Levi Hassel and Eugene Kuznetsov

## Overview
The goal of this project is to solve the Bear and the Honey Bees problem:
> There are 10 honeybees and a bear. They share a pot of honey. The pot is initially empty; its capacity is 30 equal-portions of honey. Each bee repeatedly gathers one portion of honey and puts it in the pot. In its life, each bee will gather 15 portions of honey. The bee that fills the pot awakens the bear. The bear sleeps until the pot is full, then eats the entire pot of honey and goes back to sleep. The bear wakes up to consume the honey 5 times. Note that while the bear is eating honey, no honeybees can add any honey to the pot. The bear and honeybees are represented as threads (1 bear thread and 10 honey bee threads).

In conjunction with the thread scheduler provided by the instructor (a slightly more streamlined version of our own thread scheduler from Project 1), we designed a semaphore to provide mutual exclusion and synchronization for our threads. From the previous project, we were able to schedule threads, so here we want to be able to block and queue threads based off parameters from the problem. These parameters will be the foundation of our logic for our semaphores and prevent deadlock and race conditions in our threads. We designed and implemented a semaphore structure to restrict behavior of the threads interacting with the pot of honey.

Each semaphore structure is given its own space on the stack and freed upon completion. The structure will contain a blocking/waiting queue along with the current semaphore value. This queue was created similar to the queue given in the project files. The *mysem_down()* and *mysem_up()* functions specifically use these queue functions to add and remove threads as they are locked and unlocked. These functions are structured to logically lock threads from accessing the pot of honey while handling interrupts. When a thread accesses the semaphore, *mysem_down()* is called, which locks it down and allows the thread to finish until *mysem_up()* is called. Any threads accessing *mysem_down()* while the semaphore is locked will be blocked until the current active thread has completed. The locking mechanism we chose to use in our structure was a simple while loop. We loop the thread to run out the time quantum to keep it from continuing until the semaphore was unlocked. 

## Organization
The project is split across many files.

**Provided files from Project 1:** *alarm_handler.c/h*, *queue.c/h*, *thread_handler.c/h*, *exception_handler.S*

**New Files:** *project2.c*, *sem_handler.c/h*, *sem_queue.c/h*

*alarm_handler.c/h*: Contains function that runs after each interrupt.

*queue.c/h*: Contains all functions for queue designed to hold TCB threads.

*thread_handler.c/h*: Contains all thread functions for the scheduler.

*exception_handler.S*: Injects code into *alt_exception_entry.S* after every interrupt.

*project2.c*: Contains *main()*, *os_primitive()*, which creates the bear and bees and starts the interrupt alarm, and contains definitions for *bee()* and *bear()*, the producer and cosumer functions.

*sem_handler.c/h*: Contains all semaphore functions which control its operations (create, up, down, delete etc.).

*sem_queue.c/h*: Slighlty modified version of *queue.c/h* designed to hold blocked threads for the semaphore.

## How to Run
An Altera DE1/DE2 board is required to run this code. Follow the steps [here](http://cse.unl.edu/~witty/class/csce351/Project/prelab/prelab.htm) to program the board and prepare the necessary software on your computer. After setting up the run configurations, simply import the files in this directory and run your configuration to begin the program.

## Expected Output
The program prints output in respect to the honey pot. It shows every time the number of honey portions changes, who was responsible (the bear or a bee) and how many times that bear/bee has done the same thing. Once a bee has deposited 15 portions of honey total, it commits suicide. Once the bear has eaten the honey pot five times, it commits suicide. At this point, the program returns to the main function and carries out an endless loop. See *output.txt* for actual output produced when running code.

## References
https://www.altera.com/products/processors/overview.html

https://www.altera.com/solutions/partners/partner-profile/terasic-inc-/board/altera-de2-115-development-and-education-board.html