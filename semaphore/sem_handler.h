typedef struct sem_element_type
{
    void            	    *data;
    struct sem_element_type *next;
} sem_E_type;

typedef struct sem_queue_type
{
    sem_E_type              *head;
    sem_E_type              *tail;
    unsigned int     	     size;
} sem_Q_type;

typedef struct sem
{
    unsigned int 			sem_count; //value of the semaphore (0 or 1)
    unsigned int 			num_threads_waiting; //number of threads that are waiting on the semaphore
    sem_Q_type 				queue; //queue that holds TCBs of blocked threads
} mysem;

mysem * mysem_create(int value);

void mysem_up(mysem* sem);

void mysem_down(mysem* sem);

void mysem_delete(mysem* sem);

int mysem_waitCount(mysem* sem);

int mysem_value(mysem* sem);
