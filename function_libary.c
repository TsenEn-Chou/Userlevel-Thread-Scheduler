#include "function_libary.h"

void function1(void) 
{
	// may terminated
	unsigned int a = ~0;

	while (a != 0) 
	{
		a -= 1;
	}
}

void function2(void) 
{
	// run infinite
	unsigned int a = 0;

	while (1) 
	{
		a = a + 1;
	}
}

void function3(void)
{
	
	fprintf(stdout, "task3: good morning~\n");
	fflush(stdout);
	OS2021_ThreadWaitTime(100);
	fprintf(stdout, "task3: good morning again~\n");
	fflush(stdout);
}

void function4(void)
{
	// sleep 5s
	fprintf(stdout, "task4: good morning~\n");
	fflush(stdout);
	OS2021_ThreadWaitEvent(1);
	fprintf(stdout, "task4: wake up by task5~\n");
	fflush(stdout);
}

void function5(void)
{

	fprintf(stdout, "task5: good morning~\n");
	fflush(stdout);
	OS2021_ThreadSetEvent(1);
	fprintf(stdout, "task4: wake up task4~\n");
	fflush(stdout);

}

void function6(void)
{


	fprintf(stdout, "task6: good morning~\n");
	fflush(stdout);


}
