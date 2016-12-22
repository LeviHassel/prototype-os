#include <stdlib.h>
#include <system.h>
#include <sys/alt_stdio.h>
#include <sys/alt_alarm.h>
#include <alt_types.h>

#define ALARMTICKS(x) ((alt_ticks_per_second()*(x))/10)
// Note that ALARMTICKS(5) = 1 * 5 / 10 or 0.5 seconds

#define MAX 100

//Priority levels defined here (each runs for PN * quantum_length)
#define P0 4
#define P1 3
#define P2 2

// disable an interrupt
#define DISABLE_INTERRUPTS() { \
asm("wrctl status, zero"); \
}
// enable an interrupt
#define ENABLE_INTERRUPTS() { \
asm("movi et, 1"); \
asm("wrctl status, et"); \
}

struct TCB {
	alt_u32 threadID;
	alt_u32 *stackAddress; 		// where stack begins
	alt_u32 *destroyAddress;	// pointer that tells thread where to go after its destruction
	alt_u32 stackSize;
	alt_u32 *stackPointer; 		// save the current stack pointer so we can return to this function
	alt_u32 threadState; 		// 0: Not Running, 1: Running
	alt_u32 threadPriority; 	// 0, 1 or 2
	alt_u32 runCount;			// number of times thread has ran
	alt_u32 timeRunning; 		// not used
};

//initialize all functions
void updateQueue();
void setAlarmTime(alt_u32 time);
void mythread(alt_u32 threadID);
void destroyThread();
void mythread_create();
alt_u32 nextPriority();
alt_u32 mythread_set_priority(alt_u32 threadID);
void mythread_join();
void prototype_os(alt_alarm, alt_u32);
alt_u32 myinterrupt_handler(void *);

alt_u32 * MainSP;		// pointer to return to main function
alt_u32 quantum_length; // specifies how long each thread runs
alt_u32 global_flag; 	// signifies an interrupt has occurred to myScheduler.S
alt_u32 destroy_flag;   // 1 when last thread ran was destroyed, 0 otherwise
struct TCB * runQ [12]; // queue is implemented so the current running thread will always be last
alt_32 threadCount; 	// contains # of threads in runQ, runQ[threadCount] points to current running thread
alt_32 noThreads; 		// 1 when runQ is empty, 0 otherwise
alt_u32 threadIndex; 	// points to the thread that will run next

int main()
{
	quantum_length = 1;
	global_flag = 0;
	destroy_flag = 0;
	threadCount = -1;
	noThreads = 1;
	threadIndex = 0;
	MainSP = 3;			//known bad value, worked better than setting it to NULL

	static alt_alarm alarm;
	static alt_u32 context; //context for myinterrupt_handler
	prototype_os(alarm, context);

	while(1) {
	    alt_printf("All threads are dead!\n");

	    int i;
	    for(i=1; i<300; i++) {} //delay
	}

	return 0;
}

/* This function schedules an alarm callback during a timer interrupt service */
void prototype_os(alt_alarm alarm, alt_u32 context)
{
	int i;
	int num_threads = 12;

	for (i = 0; i < num_threads; i++) {
		mythread_create();
	}

	alt_printf("Created all threads\n");
	for(i=0;i<1000000;i++){}

	// initialize the timer and its interrupt handler
	alt_alarm_start(&alarm, alt_ticks_per_second(), myinterrupt_handler, (void*)&context);

	mythread_join();
}

/* When a timer interrupt on the board occurs, this function begins executing */
alt_u32 myinterrupt_handler(void * context)
{
	global_flag = 1;
	alt_u32 nextAlarmTime;

    if (noThreads == 0) {
    	nextAlarmTime = nextPriority();
    } else {
    	nextAlarmTime = 0; //stops alarm
    }

	return ALARMTICKS(nextAlarmTime);
}

/* This function is called from myScheduler.S and schedules the next thread to run*/
void * mythread_scheduler(void * context)
{
	DISABLE_INTERRUPTS();

	if (noThreads == 1) {
		context = (void *) MainSP;
	} else {
		if (MainSP == 3) {
			MainSP = (alt_u32 *) context;
		} else if (destroy_flag == 0){
			//save the stack pointer to current TCB
			runQ[threadCount]->stackPointer = (alt_u32 *) context;
		} else {
			destroy_flag = 0;
		}

    	updateQueue();
    	context = (void *) runQ[threadCount]->stackPointer;
	}

	ENABLE_INTERRUPTS();

	return context;
}


/* Creates and allocates memory for a thread and its TCB, which contains many important properties
 * that are intialized here; this also adds the TCB to the run queue, which the scheduler uses to
 * pick the next thread to run */
void mythread_create()
{
    threadCount++;
    noThreads = 0;

	//init struct
	struct TCB *thread;
	thread = (struct TCB *) malloc(sizeof(struct TCB));

	// create thread stack space
	alt_u32 *stackAddr;
	stackAddr = (alt_u32 *)malloc(8000);

    // set values in TCB
    thread->threadID = threadCount;
	thread->stackAddress = (alt_u32 *)malloc(8000);
	thread->destroyAddress = (alt_u32 *) destroyThread;
	thread->stackSize = 7000;
	thread->stackPointer = (alt_u32 *) (stackAddr + 7000);
	thread->threadState = 0;
	thread->threadPriority = (alt_u32) mythread_set_priority(threadCount);
	thread->runCount = 0;
	thread->timeRunning = 0;

	alt_u32 *array;
	array = thread->stackPointer;
	array[18] = (alt_u32) mythread; //Initialize the instruction to be the function
	array[17] = 1; //estatus, bit for interrupt enabled
	array[5] = (alt_u32) threadCount;		//This sends the threadID to the myThread function argument
	array[0] = (alt_u32) destroyThread;
	array[-1] = (alt_u32) (thread->stackAddress + thread->stackSize);  //This is for the frame pointer

    // add to runQ
	runQ[threadCount] = thread;

	alt_printf("TCB thread %x:\n"
			"	threadID		=	%x \n"
			"	stackAddress	=	%x \n "
			"	runCount		=	%x\n"
			"	destroyAddr	=	%x\n"
			"	stackSize		=	%x \n "
			"	stackPointer	=	%x \n "
			"	threadState	=	%x \n "
			"	threadPriority	=	%x \n\n\n",
			threadCount,
			thread->threadID,
			thread->stackAddress,
			thread->runCount,
			thread->destroyAddress,
			thread->stackSize,
			thread->stackPointer,
			thread->threadState,
			thread->threadPriority);
}

/* Returns only once all of the threads have died */
void mythread_join()
{
	alt_printf("Joining threads\n");

	while (noThreads == 0) {}
}

/* Sets the priority for a thread based on its threadID */
alt_u32 mythread_set_priority(alt_u32 threadID)
{
	alt_u32 priority;

	if (threadID % 3 == 0) {
		priority = P0;
	} else if (threadID % 3 == 1) {
		priority = P1;
	} else {
		priority = P2;
	}

	return priority;
}

/* Deletes the entire contents of the thread and its TCB, fress the space and runs out the time quantum,
 * allowing the scheduler to pick the next thread */
void destroyThread()
{
	DISABLE_INTERRUPTS();

	alt_printf("\nDestroying thread: %x; Remaining threads: %x\n", (runQ[threadCount]->threadID),threadCount);
	alt_printf("This thread ran %x time(s)\n\n", runQ[threadCount]->runCount);

	// delete the TCB and thread stack space
    free((alt_u32) runQ[threadCount]->stackAddress);
    free((alt_u32) runQ[threadCount]);

    if(threadCount == 0){
    	noThreads = 1;
    } else {
    	threadCount--;
    }

    destroy_flag = 1;

	ENABLE_INTERRUPTS();

	while(1){}
}

/* Calculates runtime for thread based on its priority and the quantum */
alt_u32 nextPriority()
{
    alt_u32 time;

    if(noThreads == 0) {
    	time = (runQ[threadIndex]->threadPriority)*quantum_length;
    } else {
    	time = quantum_length;
    }

    return time;
}

/* Selects the next thread to run by swapping the last-run thread with the next-to-run thread
 * and updating the threadIndex to equal the threadId of the next-to-run thread in the runQ */
void updateQueue() {
    struct TCB * temp;
    temp = runQ[threadCount];
    runQ[threadCount] = runQ[threadIndex];
    runQ[threadIndex] = temp;
    runQ[threadIndex]->threadState = 0;
    threadIndex = (threadIndex+1)%threadCount;
    runQ[threadCount]->threadState = 1;
    runQ[threadCount]->runCount++;
}

void resetFlag() {
	global_flag = 0;
}

alt_u32 getFlag() {
    return global_flag;
}

/* Prints out messages to show thread is running correctly, is not explicitly called anywhere */
void mythread(alt_u32 threadID)
{
	alt_printf("\n\n");
	int i, j, n;
	n = (threadID % 2 == 0)? 10: 15;
	for (i = 0; i < n; i++) {
		alt_printf("This is message %x of thread # %x.\n", i, threadID);
		for (j = 0; j < MAX; j++);
	}
}
