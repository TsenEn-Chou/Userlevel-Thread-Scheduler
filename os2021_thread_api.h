#ifndef OS2021_API_H
#define OS2021_API_H

#define STACK_SIZE 8192

#include "link_list.h"
#include "function_libary.h"

void CreateContext(ucontext_t *, ucontext_t *, void *);
void InitAllQueues();
int CheckBitMap(list_t *queue);
int CheckQueueHaveNode(list_t *queue, int priority);
int AssignTQ(TCB **node);
void RunTask();
void ListAllNode(list_t *queue);

int OS2021_ThreadCreate(char *job_name, char *p_function, char *priority, int cancel_mode);
void OS2021_ThreadCancel(char *job_name);
void OS2021_ThreadWaitEvent(int event_id);
void OS2021_ThreadSetEvent(int event_id);
void OS2021_ThreadWaitTime(int msec);
void OS2021_DeallocateThreadResource();
void OS2021_TestCancel();

void TimerCalc();
void Dispatcher();

void ResetTimer(int a);
void AlarmHandler(int a);
void SigtStpHandler();

void StartSchedulingSimulation();


#endif
