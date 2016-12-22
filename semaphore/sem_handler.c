#include <stdio.h>
#include <assert.h>
#include <sys/alt_stdio.h>
#include <stdlib.h>
#include "alarm_handler.h"
#include "thread_handler.h"
#include "sem_handler.h"
#include "sem_queue.h"
#include "queue.h"


mysem * mysem_create(int value)
{
	/*
	It returns the starting address a semaphore
	variable. You can use malloc() to allocate memory
	space and initialize the internal data structure of
	the semaphore based on provided parameter.
	*/
	
    mysem *sem_pointer;
    
    sem_pointer                      = (mysem *)malloc(sizeof(mysem));
    if (sem_pointer == NULL)
    {
        printf("Unable to allocate space!\n");
        exit(1);
    }
    
    sem_pointer->sem_count           = value;
    sem_pointer->num_threads_waiting = 0;
    sem_pointer->queue.head = NULL;
    sem_pointer->queue.tail = NULL;
    sem_pointer->queue.size = 0;
    return sem_pointer;
}

void mysem_up(mysem* sem) //gate is open
{
    DISABLE_INTERRUPTS();
    
	sem->sem_count++;
	
	if (mysem_waitCount(sem) > 0) {
	    //the one that has been sleeping the longest is allowed to complete its down operation
		mythread_start(sem);
	    sem->num_threads_waiting--;
	    sem_dequeue(sem);
	}
	
	//Note: As such, after an up on a semaphore with threads sleeping on it, the semaphore
	//value is still 0 but there is one fewer sleeping threads

	ENABLE_INTERRUPTS();
}

void mysem_down(mysem* sem) //gate is closed
{
    DISABLE_INTERRUPTS();
	
    if (mysem_value(sem) == 0) {
	    //put thread to sleep without completing the down for the moment
		mythread_block(current_running_thread);
		sem_enqueue(current_running_thread, sem);
	    sem->num_threads_waiting++;

	    ENABLE_INTERRUPTS();

	    while (mysem_value(sem) == 0) {}; //Wait

	    DISABLE_INTERRUPTS();
	}
    sem->sem_count--;
    ENABLE_INTERRUPTS();

	//It performs the semaphore’s decrement operation. Check to see if the semaphore value is
	//greater than 0. If so, it decrements the value and just continues. If the value is 0, the
	//thread is put to sleep without completing the down for the moment. As such, the semaphore
	//value never falls below 0.
}

void mysem_delete(mysem* sem)
{
	//It deletes the memory space of a semaphore.
	free(sem);
}

int mysem_waitCount(mysem* sem)
{
	//Return the number of threads sleeping on the semaphore
	return sem->num_threads_waiting;
}

int mysem_value(mysem* sem)
{
    //Return the current value of the semaphore
    return sem->sem_count;
}
