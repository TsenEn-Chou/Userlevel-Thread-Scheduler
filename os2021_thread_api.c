#include "os2021_thread_api.h"

// H = 0, M = 1, L = 2
list_t ready_queue[3] = {0};
list_t waiting_queue[3] = {0};
list_t event_queue[3] = {0};
list_t terminate_queue[3] = {0};

TCB **running_thread = {0};

const static entry_function_t funct_adr[7] = {
	0,
	Function1,
	Function2,
	Function3,
	Function4,
	Function5,
	ResourceReclaim
};

const static char *state_str[4] = {
	"RUNNING",
	"READY",
	"WAITING",
	"TERMINATED"
};

const static char *priority_str[3] = {
	"H",
	"M",
	"L"
};

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
	InitQueue(terminate_queue);
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

int AssignTQ(TCB **node){
	switch ((*node)->current_priority){
	case 0:
		return 100;
		break;
	case 1:
		return 200;
		break;
	case 2:
		return 300;
		break;	
	default:
		break;
	}
	return -1;
}

void RunTask()
{
	(*running_thread)->state = kThreadRunning;
	(*running_thread)->p_function();
	register TCB *tmp = CutNode(ready_queue, running_thread);
	tmp->state = kThreadTerminated;
	InsertTailNode(terminate_queue, tmp);
	setcontext(&timer_context);
}

void ListAllNode(list_t *queue){
	int i,j;
	TCB *ptr;
	char name[16] = {0};

	for( i = 0 ; i < 3 ; i ++){
		if(CheckQueueHaveNode(queue,i)) {
			ptr = queue[i].head->next_tcb;
			while(ptr){
				strncpy(name,ptr->job_name,16);
				for(j=0;j<15;j++){
					if(!name[j]){
						name[j] = 32;
					}
				}
				printf("*\t%d\t%s\t%s\t%s\t\t%s\t\t%d\t%d\t*\n", ptr->tid, name, 
				state_str[ptr->state],
				priority_str[ptr->base_priority],
				priority_str[ptr->current_priority],
				ptr->thread_time.ready_q_time,
				ptr->thread_time.waiting_q_time);

				ptr = ptr->next_tcb;
			}
		}
	}
}

int OS2021_ThreadCreate(char *job_name, char *p_function, int priority, int cancel_mode){
	int funct_id;
	TCB *data = calloc(1, sizeof(TCB));
	if(strncmp(p_function,"ResourceReclaim",15) == 0){
		data->p_function = funct_adr[6];
	}else{
		if(strncmp(p_function,"Function",8) == 0){
			funct_id = p_function[8] - '0';
			data->p_function = funct_adr[funct_id];
		}else{
			return -1;
		}
	}
	data->tid = ++tidMax;
	data->state = kThreadReady;
	data->cancel_mode = cancel_mode;
	data->kill = 0;
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
	register TCB **cancel_node = FindNode(ready_queue, job_name);
	register TCB *ptr;
	if(cancel_node){
		ptr = (*cancel_node);
		if(ptr->cancel_mode == 1){
			ptr->kill = 1;
		}else{
			ptr = CutNode(ready_queue, cancel_node);
			ptr->state = kThreadTerminated;
			InsertTailNode(terminate_queue, ptr);
		}
	}else{
		cancel_node	= FindNode(waiting_queue, job_name);
		if(cancel_node){
			ptr = (*cancel_node);
			if(ptr->cancel_mode == 1){
				ptr->kill = 1;
			}else{
				ptr = CutNode(waiting_queue, cancel_node);
				ptr->state = kThreadTerminated;
				InsertTailNode(terminate_queue, ptr);
			}
		}else{
			cancel_node	= FindNode(event_queue, job_name);
			if(cancel_node){
				ptr = (*cancel_node);
				if(ptr->cancel_mode == 1){
					ptr->kill = 1;
				}else{
					ptr = CutNode(event_queue, cancel_node);
					ptr->state = kThreadTerminated;
					InsertTailNode(terminate_queue, ptr);
				}
			}

		}
	}
	
}


void OS2021_ThreadWaitEvent(int event_id){
	register TCB *running;
	int pr;
	running = (*running_thread);
	running->state = kThreadWaiting;
	running->wait_evnt = event_id;
	fprintf(stdout,"%s wants to wait for event %d\n", running->job_name,(*running_thread)->wait_evnt);
	fflush(stdout);
	running = CutNode(ready_queue, running_thread);
	//Increase priority
	if(running->thread_time.runable_time>0){
		if(running->current_priority>0){
			pr = running->current_priority;
			running->current_priority -=1;
			fprintf(stdout,"The priority of thread %s is changed from %s to %s\n",running->job_name, priority_str[pr] ,priority_str[running->current_priority]);
			fflush(stdout);
		}
	}
	InsertTailNode(event_queue,running);
	swapcontext(&running->thread_context, &timer_context);
}

void OS2021_ThreadSetEvent(int event_id){
	int i;
	bool have;
	register TCB **ptr;
	register TCB *tmp = (*running_thread) ;
	for(i = 0;i < 3; i++){
		if(CheckQueueHaveNode(event_queue,i)){
			ptr = &(event_queue[i].head->next_tcb);
			while(*ptr){
				if((*ptr)->wait_evnt == event_id){
					have = 1;
					(*ptr)->state = kThreadReady;
					fprintf(stdout,"%s changes the status of %s to READY \n", tmp->job_name,(*ptr)->job_name);
					fflush(stdout);
					(*ptr)->wait_evnt = -1;
					tmp = CutNode(event_queue, ptr);
					InsertTailNode(ready_queue,tmp);
					break;
				}
				ptr = &(*ptr)->next_tcb;
			}	
		}
	}
	if(!have){
		fprintf(stdout,"%s: No threads are waiting evnet%d\n",(*running_thread)->job_name ,event_id);
		fflush(stdout);
	}
	have = 0;
}

void OS2021_ThreadWaitTime(int msec){
	register TCB *ptr = (*running_thread);
	int pr;
	ptr->state = kThreadWaiting;
	ptr->thread_time.sleep_time = msec * 10;
	ptr = CutNode(ready_queue, running_thread);
	//Increase priority
	if(ptr->thread_time.runable_time>0){
		if(ptr->current_priority>0){
			pr = ptr->current_priority;
			ptr->current_priority -=1;
			fprintf(stdout,"The priority of thread %s is changed from %s to %s\n",ptr->job_name, priority_str[pr] ,priority_str[ptr->current_priority]);
			fflush(stdout);
		}
	}
	InsertTailNode(waiting_queue, ptr);
	swapcontext(&ptr->thread_context, &timer_context);
}

void OS2021_DeallocateThreadResource(){
	int i = 0;
	TCB **temp = NULL;
	TCB *delete  = NULL;
	for(i = 0 ; i < 3 ; i++){
		if(CheckQueueHaveNode(terminate_queue,i)){
			temp = &terminate_queue[i].head->next_tcb;
			while(*temp){
				delete = CutNode(terminate_queue,temp);
				fprintf(stdout,"The memeory space by %s has been released.\n",delete->job_name);
				fflush(stdout);
				free(delete);
			}		
		}

	}
}

void OS2021_TestCancel(){
	if((*running_thread)->kill){
		register TCB *ptr = (*running_thread);
		ptr = CutNode(ready_queue, running_thread);
		ptr->state = kThreadTerminated;
		InsertTailNode(terminate_queue, ptr);
		swapcontext(&ptr->thread_context, &timer_context);
	}
}

void TimerCalc()
{
	ResetTimer(0);
	register TCB *ptr;
	register TCB **p_ptr;
	register TCB *running;
	int i = CheckBitMap(ready_queue);
	int j = CheckBitMap(waiting_queue);
	int k = CheckBitMap(event_queue);
	int pr;
	if ( i == -1 && j == -1 && k == -1) {
		exit(0);	
	} 
	
	for( i = 0 ; i < 3 ; i ++){
		if(CheckQueueHaveNode(ready_queue,i)) {
			ptr = ready_queue[i].head->next_tcb;
			while(ptr){
				if(ptr->state == kThreadReady) {
					ptr->thread_time.ready_q_time += 10;
				}
				ptr = ptr->next_tcb;
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
			p_ptr = &waiting_queue[i].head->next_tcb;
			while(*p_ptr){
				if((*p_ptr)->thread_time.sleep_time <=0){
					ptr = CutNode(waiting_queue, p_ptr);
					ptr->state = kThreadReady;
					InsertTailNode(ready_queue, ptr);	
				}
				if((*p_ptr) != NULL){
					p_ptr = &(*p_ptr)->next_tcb;
				}
			}
		}
	}
	if (((*running_thread) != NULL) && 
		(*running_thread)->state == kThreadRunning) {
			(*running_thread)->thread_time.runable_time -= 1;
			if ((*running_thread)->thread_time.runable_time <= 0) {
				(*running_thread)->state = kThreadReady;
				running = CutNode(ready_queue, running_thread);
				if(running->current_priority < 2){
					pr = running->current_priority;
					running->current_priority += 1; // Time quantum is used up, increase priority
					fprintf(stdout,"The priority of thread %s is changed from %s to %s\n",running->job_name, priority_str[pr] ,priority_str[running->current_priority]);
					fflush(stdout);
				}
				InsertTailNode(ready_queue, running);
			}
	}
	//SET To Running or Dispatcher
	if ( (*running_thread != NULL) &&(*running_thread)->state == kThreadRunning)
		setcontext(&(*running_thread)->thread_context);
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
	
	j = CheckBitMap(ready_queue);
	if(j> -1){
		running_thread = &(ready_queue[j].head->next_tcb);
		(*running_thread)->state = kThreadRunning;
		(*running_thread)->thread_time.runable_time = AssignTQ(running_thread);
		setcontext(&(*running_thread)->thread_context);	
	}

}

void ResetTimer(int a)
{
	Signaltimer.it_value.tv_sec = 0;
	Signaltimer.it_value.tv_usec = 10000;
	if (setitimer(ITIMER_REAL, &Signaltimer, NULL) < 0) {
		printf("ERROR SETTING TIME SIGALRM!\n");
	}
}

void ALARM_Handler(int a)
{
	int i = CheckBitMap(ready_queue);
	int j = CheckBitMap(waiting_queue);
	int k = CheckBitMap(event_queue);
	if ( i == -1 && j == -1 && k == -1) {
		exit(0);	
	}

	if((running_thread) != NULL){
		if ((*running_thread)->state == kThreadRunning)
			swapcontext(&(*running_thread)->thread_context, &timer_context);
		else
		setcontext(&timer_context);
	}

}

void SigtStpHandler(){
	signal(SIGALRM, ResetTimer);
	printf("\n****************************************************************************************\n");
	printf("*\tTID\tName\t\tState\tB_Priority\tC_Priority\tQ_Time\tW_Time\t*\t\n");
	ListAllNode(ready_queue);
	ListAllNode(waiting_queue);
	ListAllNode(event_queue);
	printf("****************************************************************************************\n");
	ResetTimer(0);
	signal(SIGALRM,ALARM_Handler);
}

void StartSchedulingSimulation(){
	signal(SIGTSTP,SigtStpHandler);
	OS2021_ThreadCreate("reclaimer", "ResourceReclaim", 2, 1);
	/*Set Timer*/
	Signaltimer.it_interval.tv_usec = 0;
	Signaltimer.it_interval.tv_sec = 0;
	ResetTimer(0);
	/*Create Context*/
	CreateContext(&dispatch_context, &timer_context, &Dispatcher);
	CreateContext(&timer_context, &dispatch_context, &TimerCalc);
	ResetTimer(0);
	signal(SIGALRM, ALARM_Handler);
	setcontext(&dispatch_context);
}