#include <stdio.h>
#include <assert.h>
#include <sys/alt_stdio.h>
#include <stdlib.h>
#include "alarm_handler.h"
#include "thread_handler.h"
#include "sem_handler.h"
#include "sem_queue.h"
#include "queue.h"

/* This function creates a semaphore structure and allocates memory for it on the board. The semaphore   
 * value is initialized to the provided argument and it returns the starting address a semaphore
 * variable. */
mysem * mysem_create(int value)
{
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

/* Increment the value of the semaphore. If one or more threads are sleeping on the semaphore,
 * the one that has been sleeping the longest is allowed to complete its down operation. As such,
 * after an up on a semaphore with threads sleeping on it, the semaphore value is still 0 but
 * there is one fewer sleeping threads. */
void mysem_up(mysem* sem)
{
    DISABLE_INTERRUPTS();
    
	sem->sem_count++;
	
	if (mysem_waitCount(sem) > 0) {
	    // the one that has been sleeping the longest is allowed to complete its down operation
		mythread_start(sem);
	    sem->num_threads_waiting--;
	    sem_dequeue(sem);
	}

	ENABLE_INTERRUPTS();
}


/* It performs the semaphore’s decrement operation. Check to see if the semaphore value is
 * greater than 0. If so, it decrements the value and just continues. If the value is 0, the
 * thread is put to sleep without completing the down for the moment. As such, the semaphore
 * value never falls below 0. */
void mysem_down(mysem* sem)
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
}

/* It deletes the memory space of a semaphore */
void mysem_delete(mysem* sem)
{
	free(sem);
}

/* Return the number of threads sleeping on the semaphore */
int mysem_waitCount(mysem* sem)
{
	return sem->num_threads_waiting;
}

/* Return the current value of the semaphore */
int mysem_value(mysem* sem)
{
    return sem->sem_count;
}
