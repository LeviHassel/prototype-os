#include <sys/alt_stdio.h>
#include <stdio.h>
#include <stdlib.h>

#include "sem_handler.h"
#include "sem_queue.h"
#include "queue.h"

void sem_enqueue(void *data, mysem* sem)
{
    sem_E_type  *elem;

    if ((elem = (sem_E_type *)malloc(sizeof(sem_E_type))) == NULL)
    {
        printf("Unable to allocate space for the semaphore queue!\n");
        exit(1);
    }
    elem->data = data;
    elem->next = NULL;
    
    if (sem->queue.head == NULL)
        sem->queue.head = elem;
    else
        sem->queue.tail->next = elem;
    sem->queue.tail = elem;

    sem->queue.size++;
}

void *sem_dequeue(mysem* sem)
{
	sem_E_type  *elem;
    void        *data = NULL;
    
    if (sem->queue.size != 0)
    {
        elem = sem->queue.head;
        if (sem->queue.size == 1)
            sem->queue.tail = NULL;
        sem->queue.head = sem->queue.head->next;
        
        sem->queue.size--;
        data = elem->data;
        free(elem);
    }
        
    return data;
}
