#ifndef LIST_H
#define LIST_H

#include <stdio.h>
//#include <ucontext.h>
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

typedef void (*entry_function_t)(void);

typedef struct ThreadControlBlock TCB; 
struct ThreadControlBlock {
	unsigned int tid;
	char *job_name;
	int priority;// H = 0, M = 1, L = 2
	enum ThreadState state;
	entry_function_t p_function;	
	unsigned int queue_time;
	unsigned int runable_time;
	unsigned int waiting_time;
	bool in_ready_q;
	bool cancel_mode;// 1 = Deferred cancellation, 0 = Asynchronous cancellation
	//ucontext_t thread_context;
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
TCB *FindNode(list_t *queue, char *job_name);
 #endif