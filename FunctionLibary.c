#include "FunctionLibary.h"

void task1(void)   // may terminated
{
	unsigned int a = ~0;

	while (a != 0) {
		a -= 1;
	}
}

void task2(void) // run infinite
{
	unsigned int a = 0;

	while (1) {
		a = a + 1;
	}
}

void task3(void) // wait infinite
{
	hw_suspend(32768);
	fprintf(stdout, "task3: good morning~\n");
	fflush(stdout);
}

void task4(void) // sleep 5s
{
	hw_suspend(500);
	fprintf(stdout, "task4: good morning~\n");
	fflush(stdout);
}

void task5(void)
{
	int pid = hw_task_create("task3");

	hw_suspend(1000);
	fprintf(stdout, "task5: good morning~\n");
	fflush(stdout);

	hw_wakeup_pid(pid);
	fprintf(stdout, "Mom(task5): wake up pid %d~\n", pid);
	fflush(stdout);
}

void task6(void)
{
	for (int num = 0; num < 5; ++num) {
		hw_task_create("task3");
	}

	hw_suspend(1000);
	fprintf(stdout, "task6: good morning~\n");
	fflush(stdout);

	int num_wake_up = hw_wakeup_taskname("task3");
	fprintf(stdout, "Mom(task6): wake up task3=%d~\n", num_wake_up);
	fflush(stdout);
}

void task7(ProcessData *add)//add mean task7 memory address
{
	Children_list *Children_address[5];//store all Child address
	for (int num = 0; num < 5; ++num) {
		int Ctid;
		Ctid = OS2021_threadCreate("task8",'H', add->tid);
		Children_list *Children = (Children_list*) malloc(sizeof(Children_list));
		Children->tid = Ctid;
		Children_address[num] = Children;

	}

	//Link all child
	for (int num = 0; num < 5; ++num) {
		Children_address[num]->next = Children_address[num+1];
		if(num == 4)
			Children_address[num]->next = NULL;
	}
	add->Children = Children_address[0];
	OS2021_threadWaitEvent(add);

}

void task8(ProcessData *add)//add mean task8 memory address
{
	fprintf(stdout, "I love you");
	fflush(stdout);
	OS2021_threadWeakup(add->parentid);

}