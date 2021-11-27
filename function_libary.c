#include "function_libary.h"

void Function1(void) 
{
	while (1) 
	{
		OS2021_ThreadCreate("random_1","Function2",2,1);
		OS2021_ThreadCreate("random_2","Function2",2,1);
		OS2021_ThreadWaitEvent(3);
		OS2021_ThreadCancel("random_1");
		OS2021_ThreadCancel("random_2");
		while(1);
	}
}

void Function2(void) 
{
	int the_num;

	int min = 65400;
	int max = 65410;
	
	while (1) 
	{
		srand(time(NULL));
		the_num = rand() % (max - min + 1) + min;
		if(the_num == 65409)
		{
			fprintf(stdout,"I found 65409.\n");
			fflush(stdout);
			OS2021_ThreadSetEvent(3);
			min = 0;
			max = 0;
		}
		OS2021_TestCancel();
	}
}

void Function3(void)
{
	while(1)
	{
		OS2021_ThreadWaitEvent(3);
		fprintf(stdout,"I fell in love with the operating system.\n");
		fflush(stdout);
	}
}

void Function4(void)
{

	while(1)
	{
		OS2021_ThreadWaitTime(1234);
		fprintf(stdout,"I found 65409.\n");
		fflush(stdout);
		OS2021_ThreadSetEvent(6);
		while(1);
	}
}

void Function5(void)
{
	while(1)
	{
		OS2021_ThreadWaitEvent(6);
		fprintf(stdout,"I fell in love with the operating system.\n");
		fflush(stdout);
		OS2021_ThreadWaitTime(86400000);
	}
}

void ResourceReclaim(void)
{
	while (1) 
	{
		OS2021_DeallocateThreadResource();
	}
}
