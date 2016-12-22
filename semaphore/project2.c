#include <sys/alt_stdio.h>
#include "alarm_handler.h"
#include "thread_handler.h"
#include "sem_handler.h"
#include "sem_queue.h"
#include "queue.h"
#include <stdlib.h>

#define P 100
#define DELAY 200000
#define NUM_BEES 10
#define SIZE_OF_HONEY_POT 30
#define MAX_NUM_OF_POTS_CONSUMED 5
#define MAX_NUM_OF_HONEY_DEPOSITED 15

/* a delay time used to adjust the frequency of printf messages */
#define MAX 100000

void os_primitive();
void bee();
void bear();

mysem mutex;
mysem pot_full;

int portions = 0;

int main()
{
    os_primitive();
    return 0;
}

void os_primitive()
{
  unsigned int i;
  tcb *thread_pointer;

  mutex = *mysem_create(1);
  pot_full = *mysem_create(0); //pot not full at the beginning

  //Print Values for Testing***************************************************************
//  alt_printf("pot_full: Value of Semphore     =>  %x\n", mysem_value(&pot_full));
//  alt_printf("pot_full: Number of Threads Waiting   =>  %x\n", mysem_waitCount(&pot_full));
//  alt_printf("mutex: Value of Semphore     =>  %x\n", mysem_value(&mutex));
//  alt_printf("mutex: Number of Threads Waiting   =>  %x\n\n", mysem_waitCount(&mutex));
  
  //Create bees
  for (i = 0; i < NUM_BEES; i++)
  {
    thread_pointer = mythread_create(i, 4096, bee);   // 4B * 4096 entries = 16KB
    mythread_start(thread_pointer);
    mythread_join(thread_pointer);
  }
  
  //Create bear
  thread_pointer = mythread_create(NUM_BEES, 4096, bear);   // 4B * 4096 entries = 16KB
  mythread_start(thread_pointer);
  mythread_join(thread_pointer);
  
  if ( start_alarm_succeed() )
    alt_printf ("The bees and the bear are alive!\n");
  else
    alt_printf ("Unable to start the alarm\n");

/* an endless while loop */
  while (1)
  {
    alt_printf ("We are looping in main!\n");
    
    /* delay printf for a while */
    for (i = 0; i < 10*MAX; i++);
  }
  
  mysem_delete(&mutex);
  mysem_delete(&pot_full);
}

void bear() {
  unsigned int i;
  unsigned int j;
  
  for(i=0; i < MAX_NUM_OF_POTS_CONSUMED; i++) {
    mysem_down(&pot_full);
    
    //Store value for testing***************************************************************
    int temp;
    temp = portions;
    
    portions = 0; //eat all the honey
    alt_printf("Honey = %x: The bear ate all the honey for time %x\n\n", portions, i);
    
    //Print Values for Testing***************************************************************
//    alt_printf("pot_full: Value of Semphore     =>  %x\n", mysem_value(&pot_full));
//    alt_printf("pot_full: Number of Threads Waiting   =>  %x\n", mysem_waitCount(&pot_full));
//    alt_printf("mutex: Value of Semphore     =>  %x\n", mysem_value(&mutex));
//    alt_printf("mutex: Number of Threads Waiting   =>  %x\n\n", mysem_waitCount(&mutex));
    
    mysem_up(&mutex);
    for (j = 0; j < DELAY; j++){};
  }
  alt_printf("The bear commits suicide\n\n");
}

void bee() {
  unsigned int i;
  unsigned int j;
  
  for(i=0; i < MAX_NUM_OF_HONEY_DEPOSITED; i++) {
    mysem_down(&mutex);
    portions++; //add one portion
    if (portions == SIZE_OF_HONEY_POT) {
      alt_printf("Honey = %x: Bee %x can't add honey, the pot's already full\n", portions, mythread_getCurrentRunningTid());
      mysem_up(&pot_full);
    } else {
      alt_printf("Honey = %x: Bee %x added Portion %x of its honey\n", portions, mythread_getCurrentRunningTid(), i);
      mysem_up(&mutex);
    }
    
    //Print Values for Testing***************************************************************
//    alt_printf("pot_full: Value of Semphore     =>  %x\n", mysem_value(&pot_full));
//    alt_printf("pot_full: Number of Threads Waiting   =>  %x\n", mysem_waitCount(&pot_full));
//    alt_printf("mutex: Value of Semphore     =>  %x\n", mysem_value(&mutex));
//    alt_printf("mutex: Number of Threads Waiting   =>  %x\n\n", mysem_waitCount(&mutex));

    //adjust DELAY so only one deposit per run
    for (j = 0; j < DELAY; j++){};
  }
  alt_printf("Bee %x commits suicide\n\n", mythread_getCurrentRunningTid());
}
