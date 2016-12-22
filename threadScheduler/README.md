# PrototypeOS, Part 1: Thread Scheduler

#### Contributors
Levi Hassel and Eugene Kuznetsov

## Overview
The goal of this project was to design and implement a thread scheduler for our Prototype OS, running on an Altera DE2 board, which can only process one thread at a time. In order to accomplish this, we created a thread control block (TCB) to structure our threads’ characteristics and wrote many functions to handle the creation, priority-based scheduling and deletion of threads. We also designed a run queue to hold the threads that are waiting for execution.
When an interrupt occurs from the timer on the board, a flag is set, so that our assembly file myScheduler.S knows to call the scheduler and return the current stack pointer. The scheduler then stores this passed stack pointer in the correct TCB, updates the runQ to the next thread to run, and passes the correct stack pointer that corresponds with the thread to run next. The assembly file then finishes execution by setting this correct stack pointer and the frame pointer. Once a thread has finished running, it is destroyed. When all threads have finished, it returns to the main function. In our specific implementation, we create and schedule 12 threads.

## Organization
Due to time constraints, we heaped together all functionality of our scheduler into one file: project1.c. It contains the main function as well as all alarm, scheduling-related and miscellaneous functions (getFlag, resetFlag etc.). It also contains all the definitions for global variable/structure you would normally expect in a header file. In addition to this, we have an assembly file, myScheduler.S, that injects code into alt_exception_entry.S after every interrupt.

## How to Run
An Altera DE1/DE2 board is required to run this code. Follow the steps [here](http://cse.unl.edu/~witty/class/csce351/Project/prelab/prelab.htm) to program the board and prepare the necessary software on your computer. After setting up the run configurations, simply import the files in this directory and run your configuration to begin the program.

## Expected Output
Our program starts by printing the TCB structure information for every thread we create. It then prints the statements inside mythread() for each thread until either that thread finishes, in which it shows its destruction and how many times it ran, or an interrupt occurs. After either case, it begins printing mythread() for the next thread in the queue and carries on until all threads die. See output.txt for actual output produced when running code.

## References
https://www.altera.com/products/processors/overview.html
https://www.altera.com/solutions/partners/partner-profile/terasic-inc-/board/altera-de2-115-development-and-education-board.html