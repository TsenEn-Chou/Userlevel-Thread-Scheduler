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
	// wait infinite
	fprintf(stdout, "task3: good morning~\n");
	fflush(stdout);
}

void function4(void)
{
	// sleep 5s
	fprintf(stdout, "task4: good morning~\n");
	fflush(stdout);
}

void function5(void)
{

	fprintf(stdout, "task5: good morning~\n");
	fflush(stdout);

}

void function6(void)
{


	fprintf(stdout, "task6: good morning~\n");
	fflush(stdout);


}
