#ifndef OS2021_API_H
#define OS2021_API_H

#define STACK_SIZE 8192

#include "list.h"
#include "function_libary.h"

void CreateContext(ucontext_t *, ucontext_t *, void *);
void InitAllQueues();

int OS2021_ThreadCreate(char *job_name, char *p_function, int priority, int cancel_mode);
void OS2021_ThreadCancel(char *job_name);

void OS2021_ThreadWaitEvent(int event_id);
void OS2021_ThreadSetEvent(int event_id);
void OS2021_ThreadSuspend(int msec);



void RunTask();

void TimerCalc();
void Dispatcher();

void ResetTimer(int a);
void AlarmHandler(int a);
void SigtstpHandler(int recv);



#endif
