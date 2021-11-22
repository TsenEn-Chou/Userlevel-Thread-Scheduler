#ifndef LINK_LIST_H
#define LINK_LIST_H

#include <stdio.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

enum ThreadState {
	kThreadRunning,
	kThreadReady,
	kThreadWaiting,
	kThreadTerminated,
	kFake
};

typedef struct TimeGroup TimeGroup; 
struct TimeGroup {
	unsigned int ready_q_time;// The time the thread stays in the ready queue
	unsigned int waiting_q_time;// The time the thread stays in the waiting queue or event queue

	unsigned int runable_time;//The time the thread can own the CPU
	unsigned int sleep_time;//How long will the thread be awakened
};

typedef void (*entry_function_t)(void);

typedef struct ThreadControlBlock TCB; 
struct ThreadControlBlock {
	unsigned int tid;
	char *job_name;
	int base_priority;// H = 0, M = 1, L = 2
	int current_priority;
	enum ThreadState state;
	entry_function_t p_function;	
	TimeGroup thread_time;
	bool cancel_mode;// 1 = Deferred cancellation, 0 = Asynchronous cancellation
	ucontext_t thread_context;
	TCB *next_tcb;
};

typedef struct ListItem list_t; 
struct ListItem {
	bool have_node; 
	TCB *head;
	TCB *tail;
};

void InitQueue(list_t *queue);
void InsertTailNode(list_t *queue, TCB *node);
TCB* CutNode(list_t *queue, TCB **Next);
void GetTailNode(list_t *queue,int priority);
TCB **FindNode(list_t *queue, char *job_name);
 #endif