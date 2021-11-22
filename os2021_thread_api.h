#ifndef OS2021_API_H
#define OS2021_API_H

#define STACK_SIZE 8192

#include "link_list.h"
#include "function_libary.h"

void CreateContext(ucontext_t *, ucontext_t *, void *);
void InitAllQueues();
int CheckBitMap(list_t *queue);
int CheckQueueHaveNode(list_t *queue, int priority);
int AssignTQ(TCB *node);
void RunTask();

int OS2021_ThreadCreate(char *job_name, char *p_function, int priority, int cancel_mode);
void OS2021_ThreadCancel(char *job_name);
void OS2021_ThreadWaitEvent(int event_id);
void OS2021_ThreadSetEvent(int event_id);
void OS2021_ThreadWaitTime(int msec);

void TimerCalc();
void Dispatcher();

void ResetTimer(int a);
void AlarmHandler(int a);
void SigtstpHandler(int recv);



#endif
