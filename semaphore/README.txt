CSCE 351             Project 2            README
Levi Hassel, Eugene Kuznetsov and Colben Aldrich
================================================

ORGANIZATION:
The project is split across many files.
Files from Project 1 Solution: alarm_handler.c/h, queue.c/h, thread_handler.c/h,
exception_handler.S
New/Updated Files: project2.c, sem_handler.c/h, sem_queue.c/h

alarm_handler.c/h: Contains function that runs after each interrupt.
queue.c/h: Contains all functions for queue designed to hold TCB threads.
thread_handler.c/h: CContains all thread functions for the scheduler.
exception_handler.S: injects code into alt_exception_entry.S after every
interrupt.
project2.c: Contains the main function, os_primitive(), which creates the bear
and bees and starts the interrupt alarm, and contains definitions for bee()
and bear(), the producer and cosumer functions.
sem_handler.c/h: Contains all semaphore functions which control its operations
(create, up, down, delete etc.).
sem_queue.c/h: Slighlty modified version of queue.c/h designed to hold blocked
threads for the semaphore.

HOW TO COMPILE AND RUN:
After giving the DE2 board power, connect it to a computer that has all the
needed files and is setup to run Quartus. Open the Quartus II programmer and
load program/configure the board. Next open NIOS II 12.0 Software Build Tools
for Eclipse and load the desired project to run. After setting up the run
configurations, simply running your congifuration will begin the program.
For more detail on the steps go here:
http://cse.unl.edu/~witty/class/csce351/Project/prelab/prelab.htm.

EXPECTED OUTPUT:
The program prints output in respect to the honey pot. It shows every time
the number of honey portions changes, who was responsible (the bear or a bee)
and how many times that bear/bee has done the same thing. Once a bee has
deposited 15 portions of honey total, it commits suicide. Once the bear has
eaten the honey pot five times, it commits suicide. At this point, the program
returns to the main function and carries out an endless loop.