#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct node{
	char *name;
	void *address;
	struct node *next;
}HashNode;

void Insert(HashNode **hashtable, char *job_name ,void *job_addr);
void *Search(HashNode **hashtable, char *job_name);
void FreeHashTable(HashNode **hashtable);

#endif