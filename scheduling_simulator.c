#include "scheduling_simulator.h"

extern inline void InsertHeadNode(ProcessData **head, ProcessData *Data);

const static Vtask funct_adr[7] = {
	0,
	task1,
	task2,
	task3,
	task4,
	task5,
	task6
};

const static PDtask funct_adr1[3] = {
	0,
	task7,
	task8
};

const static char *StateStr[4] = {
	"TASK_RUNNING",
	"TASK_READY",
	"TASK_WAITING",
	"TASK_TERMINATED"
};

struct itimerval Signaltimer;
bool Simulating = false, Alarming = false;
int pidMax = 0;

ucontext_t Dispatch_Context;
ucontext_t ShellMode_Context;
ucontext_t Timer_Context;

ProcessData *ProcessQueue = 0;
ProcessData *ProcessTail = 0;
ProcessData *TerminatedStack = 0;

/****************************************
*   CreateContext                       *
*   Context     :   Configured_Context  *
*   nextContext :   Successor_Context   *
*   func        :   Function            *
****************************************/

void CreateContext(ucontext_t *Context, ucontext_t *nextContext, void *func)
{
	getcontext(Context);
	Context->uc_stack.ss_sp = malloc(STACK_SIZE);
	Context->uc_stack.ss_size = STACK_SIZE;
	Context->uc_stack.ss_flags = 0;
	Context->uc_link = nextContext;
	makecontext(Context,(void (*)(void))func,0);
}

/****************************************
*   FindNodeByPid                       *
*   Next        :   Pointer of Next     *
*   pid         :   Target pid          *
*   (WARNING)   :   RetVal may be zero! *
****************************************/

ProcessData** FindNodeByPid(ProcessData **Next, int pid)
{
	register ProcessData **tmp = Next;
	while (*tmp) {
		if ((*tmp)->pid == pid)
			return tmp;
		tmp = &((*tmp)->Next);
	}
	return 0;
}

/************************************************
*   Cut_Linkedlist                              *
*   Next         :  Pointer of Next of target   *
*   RetVal       :  Target Data Pointer         *
************************************************/

ProcessData* Cut_Linkedlist(ProcessData **Next)
{
	register ProcessData *Cut = (*Next);
	(*Next) = (Cut->Next);
	if (ProcessTail == Cut)
		GetTailNode();
	return Cut;
}

/************************************************
*   GetTailNode :   Update tail Pointer         *
*   (Warning)   :   Use tmp to avoid SEGFAULT   *
************************************************/

void GetTailNode()
{
	register ProcessData *ptr = ProcessQueue;
	register ProcessData *tmp = 0;
	while (ptr) {
		tmp = ptr;
		ptr = ptr->Next;
	}
	ProcessTail = tmp;
}

/************************************************
*   InsertTailNode  :   Insert node at tail     *
*   data            :   Pointer to the node     *
************************************************/

void InsertTailNode(ProcessData *data)
{
	if (ProcessTail == 0)
		ProcessQueue = data;
	else
		ProcessTail->Next = data;

	ProcessTail = data;
	data->Next = 0;
}

/************************************************
*   InsertHeadNode  :   Insert node at head     *
*   data            :   Pointer to the node     *
************************************************/

inline void InsertHeadNode(ProcessData **head, ProcessData * Data)
{
	Data->Next = *head;
	*head = Data;
	if (ProcessTail == 0)
		GetTailNode();
}

void hw_suspend(int msec_10)
{
	ProcessQueue->state = TASK_WAITING;
	ProcessQueue->SuspendTime = msec_10 * 10;
	swapcontext(&ProcessQueue->Context, &Timer_Context);
	return;
}

void hw_wakeup_pid(int pid)
{
	register ProcessData **ptr = FindNodeByPid(&ProcessQueue, pid);
	if (ptr && *ptr) {
		(*ptr)->state = TASK_READY;
		(*ptr)->SuspendTime = 0;
	}
	return;
}

int hw_wakeup_taskname(char *task_name)
{
	register ProcessData *ptr = ProcessQueue;
	register int counter = 0;
	while (ptr) {
		if ((ptr->state == TASK_WAITING) && (strncmp(ptr->Name, task_name, 5) == 0)) {
			++counter;
			ptr->state = TASK_READY;
			ptr->SuspendTime = 0;
		}
		ptr = ptr->Next;
	}
	return counter;
}

int hw_task_create(char *task_name)
{
	if (strncmp(task_name, "task", 4) != 0)
		return -1;

	if (task_name[4] < '1' || task_name[4] > '6')
		return -1;
	int funct_id = task_name[4] - '0';
	ProcessData *data = calloc(1, sizeof(ProcessData));
	data->pid = ++pidMax;
	data->Name = malloc(strlen(task_name) + 1);
	data->state = TASK_READY;
	data->taskPtr = funct_adr[funct_id];
	data->taskPtr1 = funct_adr1[funct_id];
	data->SmallQuantum = true;
	strncpy(data->Name, task_name, strlen(task_name) + 1);
	CreateContext(&data->Context, &Timer_Context, &RunTask);
	InsertTailNode(data);
	return data->pid;
}

void RunTask()
{
	ProcessQueue->state = TASK_RUNNING;
	ProcessQueue->taskPtr();
	register ProcessData *tmp = Cut_Linkedlist(&ProcessQueue);
	tmp->state = TASK_TERMINATED;
	InsertHeadNode(&TerminatedStack, tmp);
	setcontext(&Timer_Context);
}

void Dispatcher()
{
	if (ProcessQueue == 0)
		setcontext(&ShellMode_Context);

	register ProcessData **ptr = &ProcessQueue;

	while (*ptr != 0) {
		if ((*ptr)->state == TASK_READY) {
			register ProcessData *running = Cut_Linkedlist(ptr);
			InsertHeadNode(&ProcessQueue, running);
			running->state = TASK_RUNNING;
			running->RunableTime = (running->SmallQuantum) ? 10 : 20;
			setcontext(&running->Context);
		}
		ptr = &((*ptr)->Next);
	}

	while(1);
}

void add_instruction()
{
	char Command[256];
	fgets(Command, sizeof(Command), stdin);
	register char *ptr = strtok(Command + 1, " ");
	bool SmallQuantum = true;
	char FuncName[16];

	while (ptr != 0) {
		if (ptr[0] == 't') {
			strncpy(FuncName, ptr, 5);
			FuncName[5] = '\0';
		} else if (ptr[0] == '-' && ptr[1] == 't') {
			ptr = strtok(0, " ");
			if (ptr != 0 && ptr[0] == 'L')
				SmallQuantum = false;
		}
		ptr = strtok(0, " ");
	}

	if (hw_task_create(FuncName) > 0)
		ProcessTail->SmallQuantum = SmallQuantum;
	else
		printf("Cannot find : %s\n", FuncName);
}

void remove_instruction()
{
	int pid;
	scanf("%d", &pid);
	//Skip this command if pid cannot be parsed
	if (pid <= 0 || pid > pidMax)
		return;

	/*Search pid*/
	register ProcessData **ptr = FindNodeByPid(&ProcessQueue, pid);
	if (ptr == 0 || *ptr == 0) {
		ptr = FindNodeByPid(&TerminatedStack, pid);
		if (!ptr || *ptr == 0)
			return;
	}

	register ProcessData *RemoveNode = Cut_Linkedlist(ptr);

	free(RemoveNode->Name);
	free(RemoveNode->Context.uc_stack.ss_sp);
	free(RemoveNode);
}

void listAllNode(register ProcessData *ptr)
{
	while (ptr != 0) {
		printf("*\t%d\t%s\t%s\t%d\t*\n", ptr->pid, ptr->Name, StateStr[ptr->state],
		       ptr->QueueTime);
		ptr = ptr->Next;
	}
}

void show_instruction()
{
	printf("*************************************************\n");
	printf("*\tPID\tName\tTaskState\tWait\t*\n");
	listAllNode(ProcessQueue);
	listAllNode(TerminatedStack);
	printf("*************************************************\n");
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
	if (Simulating)
		return;
	else if (!ProcessQueue)
		setcontext(&ShellMode_Context);

	//Set I am alarming
	Alarming = true;

	if (ProcessQueue->state == TASK_RUNNING)
		swapcontext(&ProcessQueue->Context, &Timer_Context);
	else
		setcontext(&Timer_Context);
}

void Timer_Calc()
{
	ResetTimer(0);
	register ProcessData *ptr = ProcessQueue;
	if (ptr == 0)
		setcontext(&ShellMode_Context);

	if (ptr->state == TASK_RUNNING)
		ptr = ptr->Next;

	while (ptr != 0) {
		switch (ptr->state) {
		case TASK_READY:
			ptr->QueueTime += 10;
			break;
		case TASK_WAITING:
			ptr->SuspendTime -= 10;
			if (ptr->SuspendTime <= 0)
				ptr->state = TASK_READY;
			break;
		default:
			break;
		}
		ptr = ptr->Next;
	}

	if (ProcessQueue->state == TASK_RUNNING) {
		ProcessQueue->RunableTime -= 10;
		if (ProcessQueue->RunableTime <= 0) {
			ProcessQueue->state = TASK_READY;
			register ProcessData *Running = Cut_Linkedlist(&ProcessQueue);
			InsertTailNode(Running);
		}
	}

	Alarming = false;

	//if simulate start raise(SIGTSTP);
	if (Simulating)
		raise(SIGTSTP);

	//SET To Running or Dispatcher
	if (ProcessQueue->state == TASK_RUNNING)
		setcontext(&ProcessQueue->Context);
	else
		setcontext(&Dispatch_Context);
}

void SIGTSTP_Handler(int recv)
{
	//Set I am simulating
	Simulating = true;

	//Return if interrupt from alarming
	if (Alarming)
		return;

	/*Disable ALARM*/
	signal(SIGALRM, ResetTimer);

	Start_ShellMode();
	return;
}

void Start_ShellMode()
{
	char Command[16];
	while(printf ("$ ") && scanf("%s", Command)) {
		/*Add task*/
		if (strncmp(Command, "add", 3) == 0)
			add_instruction();
		/*Remove task*/
		else if (strncmp(Command, "remove", 6) == 0)
			remove_instruction();
		/*Show task*/
		else if (strncmp(Command, "ps", 2) == 0)
			show_instruction();
		/*Leave shell mode*/
		else if (strncmp(Command, "start", 5) == 0)
			break;
		else
			printf("\nError: Unknown Instruction: %s !\n", Command);
	}

	/*Write Back Timer*/
	Simulating = false;
	ResetTimer(0);
	signal(SIGALRM, ALARM_Handler);

	if (ProcessQueue != 0 && (ProcessQueue->state == TASK_RUNNING))
		return;
	else
		setcontext(&Dispatch_Context);
}

int main()
{
	/*Initial*/
	signal(SIGTSTP, SIGTSTP_Handler);

	/*Set Timer*/
	Signaltimer.it_interval.tv_usec = 0;
	Signaltimer.it_interval.tv_sec = 0;
	ResetTimer(0);

	/*Create Context*/
	CreateContext(&ShellMode_Context, &Dispatch_Context, &Start_ShellMode);
	CreateContext(&Dispatch_Context, &ShellMode_Context, &Dispatcher);
	CreateContext(&Timer_Context, &Dispatch_Context, &Timer_Calc);

	raise(SIGTSTP);

	return 0;
}
