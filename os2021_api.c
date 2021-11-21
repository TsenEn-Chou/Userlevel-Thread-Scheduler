#include "os2021_api.h"

// H = 0, M = 1, L = 2
list_t ready_queue[3] = {0};
list_t waiting_queue[3] = {0};
list_t terminate_queue = {0};

TCB *running_thread = {0};

const static entry_function_t funct_adr[7] = {
	0,
	function1,
	function2,
	function3,
	function4,
	function5,
	function6
};

// const static char *state_str[4] = {
// 	"THREAD_RUNNING",
// 	"THREAD_READY",
// 	"THREAD_WAITING",
// 	"THREAD_TERMINATED"
// };

int tidMax = 0;


struct itimerval Signaltimer;
bool Simulating = false, Alarming = false;
int pidMax = 0;

ucontext_t dispatch_context;
ucontext_t report_context;
ucontext_t timer_context;

/****************************************
*   CreateContext                       *
*   Context     :   Configured_Context  *
*   nextContext :   Successor_Context   *
*   func        :   Function            *
****************************************/

void CreateContext(ucontext_t *context, ucontext_t *next_context, void *func)
{
	//getcontext(context);
	context->uc_stack.ss_sp = malloc(STACK_SIZE);
	context->uc_stack.ss_size = STACK_SIZE;
	context->uc_stack.ss_flags = 0;
	context->uc_link = next_context;
	//makecontext(context,(void (*)(void))func,0);
}

void InitAllQueues(){
	InitQueue(ready_queue);
	InitQueue(waiting_queue);
}

int OS2021_ThreadCreate(char *job_name, char *p_function, int priority, int cancel_mode){
	if (p_function[8] < '1' || p_function[8] > '6')
		return -1;
	TCB *data = calloc(1, sizeof(TCB));
	int funct_id = p_function[8] - '0';
	data->p_function = funct_adr[funct_id];
	data->tid = ++tidMax;
	data->state = kThreadReady;
	data->cancel_mode = cancel_mode;
	data->job_name = malloc(strlen(job_name) + 1);// +1 mean append '\0' to job_name
	data->in_ready_q = 1;
	strncpy(data->job_name, job_name, strlen(job_name) + 1);
	data->priority = priority;
	//CreateContext(&data->thread_context, &timer_context, &RunTask);
	InsertTailNode(ready_queue,data);
	return data->tid;
}

void OS2021_ThreadCancel(char *job_name){

}
