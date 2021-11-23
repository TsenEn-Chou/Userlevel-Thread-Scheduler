#include "os2021_thread_api.h"

int main(int argc ,char** argv){
	InitAllQueues();
	char *name="task3";
	char *functions = "function3";
	OS2021_ThreadCreate(name, functions, 0, 0);
	char *name1="task4";
	char *functions1 = "function4";
	OS2021_ThreadCreate(name1, functions1, 0, 0);
	char *name2="task5";
	char *functions2 = "function5";
	OS2021_ThreadCreate(name2, functions2, 0, 0);
	char *name3 = "idle task";
	char *functions3 = "function6";
	OS2021_ThreadCreate(name3, functions3, 2, 0);
	StartSchedulingSimulation();
	return 0;
}