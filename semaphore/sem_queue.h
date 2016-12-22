/* 
Description:
    Functions used to implement a sem_queue and its operations
*/

void sem_enqueue(void *data, mysem* sem);

void *sem_dequeue(mysem* sem);
