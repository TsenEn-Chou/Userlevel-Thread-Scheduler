#include "os2021_thread_api.h"

// H = 0, M = 1, L = 2
list_t ready_queue[3] = {0};
list_t waiting_queue[3] = {0};
list_t event_queue[3] = {0};
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
	getcontext(context);
	context->uc_stack.ss_sp = malloc(STACK_SIZE);
	context->uc_stack.ss_size = STACK_SIZE;
	context->uc_stack.ss_flags = 0;
	context->uc_link = next_context;
	makecontext(context,(void (*)(void))func,0);
}

void InitAllQueues(){
	InitQueue(ready_queue);
	InitQueue(waiting_queue);
	InitQueue(event_queue);
}

int CheckBitMap(list_t *queue){
	int i;
	for (i = 0; i < 3; i++) {
		if(queue[i].have_node)
			return i;
	}
	return -1;
}

int CheckQueueHaveNode(list_t *queue, int priority){
	if(queue[priority].have_node)
		return 1;
	return 0;
}

int AssignTQ(TCB *node){
	switch (node->current_priority){
	case 0:
		return 8;
		break;
	case 1:
		return 16;
		break;
	case 2:
		return 24;
		break;	
	default:
		break;
	}
}

void RunTask()
{
	int i = CheckBitMap(ready_queue);
	running_thread = ready_queue[i].head->next_tcb;
	running_thread->state = kThreadRunning;
	running_thread->p_function();
	register TCB *tmp = CutNode(&terminate_queue, &running_thread);
	tmp->state = kThreadTerminated;
	InsertTailNode(&terminate_queue, tmp);
	setcontext(&timer_context);
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
	strncpy(data->job_name, job_name, strlen(job_name) + 1);
	data->base_priority = priority;
	data->current_priority = priority;// Priority changes priority based on thread behavior
	data->wait_evnt = -1;
	CreateContext(&data->thread_context, &timer_context, &RunTask);
	InsertTailNode(ready_queue,data);
	return data->tid;
}

void OS2021_ThreadCancel(char *job_name){
	TCB **cancel_node = FindNode(ready_queue, job_name);
	if(cancel_node){
		CutNode(ready_queue, cancel_node);
	}else{
		cancel_node	= FindNode(waiting_queue, job_name);
		CutNode(ready_queue, cancel_node);
	}
	
}


void OS2021_ThreadWaitEvent(int event_id){
	register TCB *running;
	running_thread->state = kThreadWaiting;
	running_thread->wait_evnt = event_id;
	running = CutNode(ready_queue, &running_thread);
	InsertTailNode(event_queue,running);

}

void OS2021_ThreadSetEvent(int event_id){
	int i;
	register TCB **ptr;
	register TCB *tmp;
	for(i = 0;i < 3; i++){
		if(CheckQueueHaveNode(event_queue,i)){
			ptr = &(event_queue[i].head->next_tcb);
			while(*ptr){
				if((*ptr)->wait_evnt == event_id){
					(*ptr)->state = kThreadReady;
					(*ptr)->wait_evnt = -1;
					tmp = CutNode(ready_queue, ptr);
					InsertTailNode(event_queue,tmp);
					break;
				}
				ptr = &(*ptr)->next_tcb;
			}	
		}	
	}
		
}

void OS2021_ThreadWaitTime(int msec){
	running_thread->state = kThreadWaiting;
	running_thread->thread_time.sleep_time = msec;
	swapcontext(&running_thread->thread_context, &timer_context);
}

void TimerCalc()
{
	register TCB *ptr;
	int i = CheckBitMap(ready_queue);
	int j = CheckBitMap(waiting_queue);
	int k = CheckBitMap(event_queue);
	if ( i == -1 && j == -1 && k == -1) {
		exit(0);	
	} 
	
	for( i = 0 ; i < 3 ; i ++){
		if(CheckQueueHaveNode(ready_queue,i)) {
			ptr = ready_queue[i].head->next_tcb;
			while(ptr){
				if(ptr->state == kThreadReady) {
					ptr->thread_time.ready_q_time += 10;
					ptr = ptr->next_tcb;
				}
			}
		}

		if(CheckQueueHaveNode(waiting_queue,i)) {
			ptr = waiting_queue[i].head->next_tcb;
			while(ptr){
				ptr->thread_time.waiting_q_time += 10;
				ptr->thread_time.sleep_time -= 10;
				ptr = ptr->next_tcb;
			}
		}

		if(CheckQueueHaveNode(event_queue,i)) {
			ptr = event_queue[i].head->next_tcb;
			while(ptr){
				ptr->thread_time.waiting_q_time += 10;
				ptr = ptr->next_tcb;
			}
		}
	}

	for(i = 0 ; i < 3 ; i++){
		if(CheckQueueHaveNode(waiting_queue,i)) {
			ptr = waiting_queue[i].head->next_tcb;
			while(ptr){
				if(ptr->thread_time.sleep_time <=0){
					ptr->state = kThreadReady;
				}
				ptr = ptr->next_tcb;
			}
		}
	}

	if (running_thread->state == kThreadRunning) {
		running_thread->thread_time.runable_time -= 10;
		if (running_thread->thread_time.runable_time <= 0) {
			running_thread->state = kThreadReady;
			if(running_thread->current_priority < 2){
				running_thread->current_priority += 1; // Time quantum is used up, increase priority
			}
			register TCB *running = CutNode(ready_queue, &running_thread);
			InsertTailNode(ready_queue, running);
		}
	}

	// Alarming = false;

	// //if simulate start raise(SIGTSTP);
	// if (Simulating)
	// 	raise(SIGTSTP);

	//SET To Running or Dispatcher
	if (running_thread->state == kThreadRunning)
		setcontext(&running_thread->thread_context);
	else
		setcontext(&dispatch_context);
}

void Dispatcher()
{
	int i = CheckBitMap(ready_queue);
	int j = CheckBitMap(waiting_queue);
	int k = CheckBitMap(event_queue);
	if ( i == -1 && j == -1 && k == -1) {
		exit(0);	
	} 

	//register TCB **ptr = &running_thread;

	for( i = 0 ; i<3 ; i++){
		j = CheckBitMap(ready_queue);
		if( j != -1){
			running_thread = ready_queue[j].head->next_tcb;
			running_thread->state = kThreadRunning;
			running_thread->thread_time.runable_time = AssignTQ(running_thread);
			setcontext(&running_thread->thread_context);	
		}
	}
	while(1);
}

