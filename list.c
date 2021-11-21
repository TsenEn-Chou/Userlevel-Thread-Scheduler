#include "list.h"
#include "hash_table.h"



void InitQueue(list_t *queue) {
	int i;
	for (i = 0;i<3;i++) {
		TCB *rhead = (TCB*)calloc(1,sizeof(TCB));
		rhead->state = kFake;
		rhead->next_tcb = NULL;
		queue[i].head = rhead;

	}
}

void InsertTailNode(list_t *queue, TCB *node, bool to_ready_q) {

	if (queue[node->priority].tail == NULL) {
		queue[node->priority].head->next_tcb = node;
	} else
		queue[node->priority].tail->next_tcb = node;

	queue[node->priority].tail = node;
	node->next_tcb = NULL;
}

TCB* CutNode(list_t *queue, TCB **node) {
	register TCB *cut = (*node);

	if (queue[cut->priority].tail == cut) {
		(*node) = NULL;
		GetTailNode(queue, cut->priority);
		return cut;
	} else {
		(*node) = (cut->next_tcb);
		return cut;
	}	
}

void GetTailNode(list_t *queue, int priority) {
	register TCB *ptr; 
	register TCB *tmp = NULL;	
	ptr = queue[priority].head->next_tcb;
	
	while (ptr) {
		tmp = ptr;
		ptr = ptr->next_tcb;
	}
	queue[priority].tail = tmp;
	
}

void *FindNode(list_t *queue, char *job_name) {
	int i;
	register TCB *ptr;
	for (i = 0; i < 3; i++) {
		ptr = queue[i].head->next_tcb;
	}
}