/*
 * Systemy Operacyjne
 * Zadanie zaliczeniowe nr 2
 * Autor: Paweł Kapica, 334579
 *
 * plik list.h
 *
 */
#ifndef LISTH
#define LISTH
#endif

#include <pthread.h>
#include <sys/types.h>

typedef struct node {
	pid_t pid;
	pthread_t ptr;
	struct node* next;
} node;

typedef struct {
	node* head;
	node* tail;
	int size;
} list;

void init(list** l);
void deinit(list** l);
void insert(list* l, node* n);
void rmvFirst(list* l);
void rmvPid(list* l, pid_t pid);
int empty(list* l);
node* createNode(pid_t pid, pthread_t ptr);