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

![adjpri](https://github.com/TsenEn-Chou/Userlevel-Thread-Scheduler/blob/472da087e09d07c477476206b3bc59787ca4ef32/adjpri.gif)


If the thread gives up the CPU without running out of its TQ (Wait time or event), it increases its priority by one level.
If the thread runs out of its TQ,  it decrease its priority by one level.


If the priority of the thread is changed, the shell will print the information on the terminal.
e.g.,  The priority of thread xxx is changed form M to H.


