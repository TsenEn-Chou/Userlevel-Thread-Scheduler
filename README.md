---
title: 
image: 
description: 
tags: Github, repo
---
# Userlevel-Thread-Scheduler

## Intro
This repo implemented a user-level multiple threads scheduling environment.
### How to use it?
#### 1. Static configuration thread
<font color = "red">Thread information must be configured before running in this environment.</font> 

Thread configuration using JSON format, the configure file is "init_threads.json".

The following information must be defined when creating a thread.

---
- **name**
    - A descriptive name for the thread. It is included purely as a debugging aid.


- **entry function**
    - One of the five functions: Function1 to Function5
    - All functions are located in function library.c.

- **priority**
    - Three priority levels, H, M, L.
    - You can decide the priority of the thread yourself.


- **cancel mode**
    - Can be 1 or 0.
    - If setting 1 means that the thread is [deferred cancellation](https://www.tutorialspoint.com/what-is-thread-cancellation) type. 
    - If setting 0 means that the thread is asynchronous cancellation type
---
Example:
```
{
	"Threads":[
		{
			"name" : "Naruto",
			"entry function" : "Function1",
			"priority": "M",
			"cancel mode": "0"
		},
		{
			"name" : "Sasuke",
			"entry function" : "Function3",
			"priority": "M",
			"cancel mode": "0"
		}
	]
}
```
#### 2. Function1 to Function5 implementations
As mentioned above, Threads need to define the entry function, which is the thread behavior when the thread is running. [Example](https://github.com/TsenEn-Chou/Userlevel-Thread-Scheduler/blob/main/function_libary.c)

:::danger
:warning: Do not modify ResourceReclaim function. This function is for reclaimer, which is an [idle thread](https://www.freertos.org/RTOS-idle-task.html) used to deallocate terminated threads resource. 
:::

#### 3.(optional) You can use OS2021_Thread API in Function1 to Funtion5

`int OS2021_ThreadCreate(char *name, char *p_function, char *priority, int cancel_mode)`
    
* Create a thread named `name` and set its priority to `priority`.
* If function_libary.c doesn’t have a function named `p_function`, API returns -1.
* The cancellation type of the thread is marked according to the cancel_mode.
* Return (Thread ID)TID of the created thread.

`void OS2021_ThreadCancel(char *name);`
* Cancel the thread named `name` according to the cancel mode of the thread.
* If the cancel mode is 0,  change the thread status to TERMINATED, and then the resource memory block will be reclaimed by the reclaimer.
* If the cancel mode is 1, inform the `name` thread that another thread wants to cancel it, then the `name` thread will be terminated when it enters the cancel point.

`void OS2021_threadWaitEvent(int event_id);`
* A total of 8 types of events, use numbers 0~7 to number the events.
* The running thread changes its state to WAITING and enters the event waiting queue corresponding to the event_id.
* The thread calling this API will print the event it wants to wait for on the terminal. 
    * e.g., xxx wants to wait for event 2.


`void OS2021_threadSetEvent(int event_id);`
* If there is currently a thread waiting for an event, the API will move the state of the thread from WAITING to READY and remove the thread from the event waiting queue corresponding to the event_id and put it in the ready queue. 
* If no threads are waiting for the event, nothing is done.
* If a thread is awakened, the wake-up information will printed on the terminal.
    * e.g., xxx1 changes the status of xxx2 to READY.

:warning:
If multiple threads are waiting for the same event, the thread with the highest priority will be awakened first. 

:warning:
If the priority is the same, it will be served according to the first-come, first-served (FCFS) algorithm.

![](https://i.imgur.com/ljGQ3bm.gif)

`void OS2021_ThreadWaitTime(int 10msec);` 
* The running task change its state to WAITING. 
* Change the state of the suspended task to READY after `10msec` * 10ms.


`void OS2021_DeallocateThreadResource();`
* This API will be used by ResourceReclaim().
* Deallocate the memory of the thread that has been terminated.


`void OS2021_TestCancel();`
* Allows a thread to be cancelled in a safe point.
* This is cancellation point, which if the thread cancel_mode is 1 and then there is another thread want to cancel this thread(set  cancel flag to 1), this thread will check cancel flag after call API, if  cancel flag = 1, this thread will change itself status to TERMINATED.

### Scheduling Policy
The scheduling policy for threads is a 
**3-level Non-preemptive Priority Feedback Queue**

![3-level Non-preemptive Priority Feedback Queue](https://i.imgur.com/NkcebMB.png)

* **Three priority level**
    * H – Round Robin(RR) with Time Quantum(TQ) 100ms
    * M – RR with TQ 200ms
    * L – RR with TQ 300ms


![](https://i.imgur.com/AjymuhK.gif)


If a thread with a higher priority than the currently running thread enters the ready queue, it must wait for the running thread to run out of TQ or voluntarily give up the CPU before switching to a higher thread.

![](https://i.imgur.com/DlTi4Ic.gif)



If the thread gives up the CPU without running out of its TQ (Wait time or event), it increases its priority by one level.
If the thread runs out of its TQ,  it decrease its priority by one level.


If the priority of the thread is changed, the shell will print the information on the terminal.
e.g.,  The priority of thread xxx is changed form M to H.

## Building the environment
install json-c library

```sudo apt-get install libjson-c-dev```

Git Hook install

```make```

compile simulator

```make simulator```

delete all output file

```make clean```

## Directories
* **main**
    * simulator.c
* **Thread function**
    * function_library.h
    * function_library.c 
* **Thread application interface(API) function**
    * os_2021_thread_api.h
    * os_2021_thread_api.c 

* **Thread configuration file**
    * init_threads.json

* **linked list opration**
    * link_list.c
    * link_list.h

* **hash_table(future plan will use)**
    * hash_table.c
    * hash_table.h

## Example
In this example will create two thread Naruto and Sasuke.
```
{
	"Threads":[
		{
			"name" : "Naruto",
			"entry function" : "Function1",
			"priority": "M",
			"cancel mode": "0"
		},
		{
			"name" : "Sasuke",
			"entry function" : "Function3",
			"priority": "M",
			"cancel mode": "0"
		}
	]
}
```
Naruto entry function is Function1, which will create two thread random_1 and random_2, set entry function is Function2, cancel_mode = 1, and then Naruto will wait event3, after someone set event3(may be is random_1 or random_2), Naruto will cancel random_1 and random_2.

```c=
void Function1(void) 
{
    int i,j;
	while (1) 
	{
		i = OS2021_ThreadCreate("random_1","Function2","L",1);
        ((i>0) ? fprintf(stdout,"Created random_1 successfully\n"):
        fprintf(stdout,"Failed to create random_1\n"));
        fflush(stdout);

		j = OS2021_ThreadCreate("random_2","Function2","L",1);
        ((j>0) ? fprintf(stdout,"Created random_2 successfully\n"):
        fprintf(stdout,"Failed to create random_2\n"));
        fflush(stdout);
        
		OS2021_ThreadWaitEvent(3);

		((i>0) ? OS2021_ThreadCancel("random_1"): "");
		((j>0) ? OS2021_ThreadCancel("random_2"): "");
        OS2021_ThreadCancel("f1");
	}
}
```

random_1 and random_2  will generate random numbers between 65401~654010 until they generate 65409. After generation, event3 will be set to wake up Naruto, and enter cancellation point.
```c=
void Function2(void) 
{
	int the_num;
    int min = 65405;
    int max = 65410;
    int flag = 0;
	while (1) 
	{
	    srand((unsigned)time(NULL));
		(!flag) ? the_num = rand() % (max - min + 1) + min: (the_num=0) ;
		if(the_num == 65409)
		{
            flag = 1;
			fprintf(stdout,"I found 65409.\n");
			fflush(stdout);
			OS2021_ThreadSetEvent(3);

		}
		OS2021_TestCancel();
	}
}
```

Sasuke entry function is Function3,which  will wait odd event(1, 3, 5, 7, 9), but in this example no one will set event1, so Sasuke will alway in event1 wait queue after call `OS2021_ThreadWaitEvent(odd);`
```c=
void Function3(void)
{
    int odd = 1;
	while(1)
	{
		fprintf(stdout,"Wait event%d.\n",odd);
		fflush(stdout);
		OS2021_ThreadWaitEvent(odd);
        odd +=2;
        if( odd == 9 ){
            while (1);
        }
	}
}
```

when envrionment running you can use 
* Ctrl+Z to reports thread information
* Ctrl+C to terminate the process

![](https://i.imgur.com/C5PLyi6.png)

## Other Example

There has 2 types case to test the `os2021_thread_api.c`, which is `function_library0.c` + `init_thread0.json` and `function_library1.c` + `init_thread1.json`

You could replace the original file with them. 
And the test case is modified from [OS2021_Hw3_Template by TsenEn-Chou](https://github.com/TsenEn-Chou/OS2021_Hw3_Template)

## Future plan
* Supports multiple scheduling policy
    * [ O(1) scheduler](https://en.wikipedia.org/wiki/O(1)_scheduler)
    * [Completely fair scheduler(CFS)](https://en.wikipedia.org/wiki/Completely_Fair_Scheduler)