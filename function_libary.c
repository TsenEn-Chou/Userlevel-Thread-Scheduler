#include "function_libary.h"
int i = 0;

void function1(void) 
{
	while (1) 
	{
		OS2021_ThreadCreate("random69_1","function2",1,0);
		OS2021_ThreadCreate("random69_2","function2",1,0);
		OS2021_ThreadWaitEvent(2);
		
		OS2021_ThreadCancel("random69_1");
		OS2021_ThreadCancel("random69_2");
		OS2021_ThreadCancel("random69_2");
		while(1);
	}
}

void function2(void) 
{
	int the_num;

	int min = 68;
	int max = 70;
	
	while (1) {
		srand(time(NULL));
		the_num = rand() %(max - min + 1) + min;
		if(the_num == 69){
			OS2021_ThreadSetEvent(2);
			//OS2021_ThreadSetEvent(7);
			//OS2021_ThreadWaitTime(10000);
			while(1);
		}
	}
}

void function3(void)
{
	while(1){
		OS2021_ThreadWaitEvent(2);
		fprintf(stdout,"I'm tired and want to sleep for 1000ms\n");
		fflush(stdout);
		OS2021_ThreadWaitTime(1000);
		i++;
		fprintf(stdout,"I come back, the i is %d\n",i);
		fflush(stdout);
		//OS2021_TsetCanCel();
	}
}

void function4(void)
{
	while(1){
		OS2021_ThreadWaitEvent(3);
	}
}

void function5(void)
{
	while(1){
		OS2021_ThreadSetEvent(2);
	}
}

void function6(void)
{
	while (1) {
		OS2021_DeallocateThreadResource();
	}
}
