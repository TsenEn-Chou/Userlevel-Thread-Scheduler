#include "os2021_thread_api.h"

int main(int argc ,char** argv){
	InitAllQueues();
	char *functions1 = "Function1";
	OS2021_ThreadCreate("f1", functions1, 1, 1);

	char *functions3 = "Function3";
	OS2021_ThreadCreate("f3", functions3, 1, 0);
	OS2021_ThreadCreate("f4", "Function4", 1, 0);
	OS2021_ThreadCreate("f5", "Function5", 1, 0);

	StartSchedulingSimulation();
	return 0;
}