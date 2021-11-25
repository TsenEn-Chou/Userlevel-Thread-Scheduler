#include "link_list.h"

void InitQueue(list_t *queue) {
	int i;
	for (i = 0;i<3;i++) {
		TCB *head = (TCB*)calloc(1,sizeof(TCB));
		head->state = kFake;
		head->next_tcb = NULL;
		queue[i].head = head;
		queue[i].have_node = false;
	}
}

void InsertTailNode(list_t *queue, TCB *node) {

	if (queue[node->current_priority].tail == NULL) {
		queue[node->current_priority].head->next_tcb = node;
		queue[node->current_priority].have_node = true;
	} else
		queue[node->current_priority].tail->next_tcb = node;
	queue[node->current_priority].tail = node;
	node->next_tcb = NULL;
}

TCB *CutNode(list_t *queue, TCB **node) {
	register TCB *cut = (*node);

	if (queue[cut->current_priority].tail == cut) {	
		queue[cut->current_priority].have_node = false;
		(*node) = NULL;
		GetTailNode(queue, cut->current_priority);
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

TCB **FindNode(list_t *queue, char *job_name) {
	int i;
	register TCB **ptr;
	for (i = 0; i < 3; i++) {
		ptr = &(queue[i].head->next_tcb);
		while (*ptr) {
			if (strcmp((*ptr)->job_name,job_name) == 0)
				return ptr;
			ptr = &(*ptr)->next_tcb;
		}
	}
	return NULL;
}