#ifndef SCHEDULING_SIMULATOR_H
#define SCHEDULING_SIMULATOR_H

#include <stdio.h>
#include <ucontext.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define STACK_SIZE 8192

#include "task.h"

enum TASK_STATE {
	TASK_RUNNING,
	TASK_READY,
	TASK_WAITING,
	TASK_TERMINATED
};

typedef struct ProcessData ProcessData;

typedef void (*Vtask)(void);
typedef void (*PDtask)(ProcessData*);

typedef struct Children_list {
	unsigned int tid;
	bool done;
	struct Children_list *next;
} Children_list;

struct ProcessData {
	unsigned int pid;
	unsigned int tid;
	char *Name;
	enum TASK_STATE state;
	Vtask taskPtr;
	PDtask taskPtr1;
	bool SmallQuantum;
	unsigned int QueueTime;
	unsigned int RunableTime;
	unsigned int SuspendTime;
	ucontext_t Context;
	struct ProcessData *Next;
	Children_list *Children;
	unsigned int parentid;
};


void CreateContext(ucontext_t *, ucontext_t *, void *);
ProcessData **FindNodeByPid(ProcessData **Next, int pid);
ProcessData *Cut_Linkedlist(ProcessData **Next);
void GetTailNode();
void InsertTailNode(ProcessData *data);
void hw_suspend(int msec_10);
void hw_wakeup_pid(int pid);
int hw_wakeup_taskname(char *task_name);
int hw_task_create(char *task_name);
void RunTask();
void Dispatcher();
void add_instruction();
void remove_instruction();
void listAllNode(register ProcessData *ptr);
void show_instruction();
void ResetTimer(int a);
void ALARM_Handler(int a);
void Timer_Calc();
void SIGTSTP_Handler(int recv);
void Start_ShellMode();

int OS2021_threadCreate(char *task_name, int priority, int parent_tid);
bool OS2021_threadDelete(int tid);
bool OS2021_threadWaitEvent(ProcessData* parent);
bool OS2021_threadWeakup(int tid);

#endif
