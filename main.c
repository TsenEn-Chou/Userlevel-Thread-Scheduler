#include "list.h"

extern ready_queue;
int main(int argc ,char** argv){
	InitQueue();
	int i;
	for (i = 0;i<3;i++) {
		TCB *rhead = (TCB*)calloc(1,sizeof(TCB));
		rhead->state = kFake;
		rhead->next_tcb = NULL;
		ready_queue[i].head = rhead;

		TCB *whead = (TCB*)calloc(1,sizeof(TCB));
		whead->state = kFake;
		whead->next_tcb = NULL;
		waiting_queue[i].head = whead;  
	}
	return 0;
}