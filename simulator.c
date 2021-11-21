#include "os2021_api.h"

int main(int argc ,char** argv){
	InitAllQueues();
	char *name="NTK";
	char *functions = "function1";
	int p  = 1;
	int cancel = 1;
	OS2021_ThreadCreate(name, functions, p , cancel);
	OS2021_ThreadCancel(name);	
	return 0;
}