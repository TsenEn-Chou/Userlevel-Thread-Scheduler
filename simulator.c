#include "os2021_thread_api.h"

int main(int argc ,char** argv){
	InitAllQueues();
	char *functions1 = "function1";
	OS2021_ThreadCreate("f1", functions1, 0, 0);

	// char *functions3 = "function3";
	// OS2021_ThreadCreate("f3_1", functions3, 0, 0);
	// OS2021_ThreadCreate("f3_2", functions3, 0, 0);

	// char *name4="f4";
	// char *functions4 = "function4";
	// OS2021_ThreadCreate(name4, functions4, 1, 0);

	// char *name5="f5";
	// char *functions5 = "function5";
	// OS2021_ThreadCreate(name5, functions5, 1, 0);

	char *name6 = "idle job";
	char *functions6 = "function6";
	OS2021_ThreadCreate(name6, functions6, 2, 1);
	StartSchedulingSimulation();
	return 0;
}