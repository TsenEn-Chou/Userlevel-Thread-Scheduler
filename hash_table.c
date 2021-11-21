#include"hash_table.h"

void Insert(HashNode **hashtable, char *job_name ,void *job_address){
	int t = abs((int)job_name) % 7;
	HashNode *temp = hashtable[t];
	HashNode *new_node = (HashNode*)calloc(1,sizeof(HashNode));
	new_node->name = job_name;
	new_node->address = job_address;
	new_node->next = temp->next;
	temp->next = new_node;
}

void *Search(HashNode **hashtable, char *job_name){
	int hashindex = abs((int)job_name) % 7;
	HashNode *temp = hashtable[hashindex]->next;
	while(temp){
		if(!strcmp(temp->name,job_name))
			return temp->address;
		temp = temp->next;
	}
	return (void*)-1;
}

void FreeHashTable(HashNode **hashtable){
	int i = 0;
	HashNode *temp = NULL;
	HashNode *delete  = NULL;
	for(i = 0 ; i < 7 ; i++){
		temp = hashtable[i];
		delete = temp;
		while(temp){
			delete = temp;
			temp = temp->next;
			free(delete);
		}
	}
	free(hashtable);
}