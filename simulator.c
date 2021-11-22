#include "os2021_thread_api.h"

int main(int argc ,char** argv){
	InitAllQueues();
	char *name="task3";
	char *functions = "function3";
	int p  = 0;
	int cancel = 0;
	OS2021_ThreadCreate(name, functions, p, cancel);
	char *name1="task4";
	char *functions1 = "function4";
	OS2021_ThreadCreate(name1, functions1, p, cancel);
	char *name2="task5";
	char *functions2 = "function5";
	OS2021_ThreadCreate(name2, functions2, p, cancel);
	StartSchedulingSimulation();
	return 0;
}