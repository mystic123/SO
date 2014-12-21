/*
 * Systemy Operacyjne
 * Zadanie zaliczeniowe nr 2
 * Autor: Paweł Kapica, 334579
 *
 * plik list.c
 *
 */
#include <stdlib.h>
#include "list.h"
#include "err.h"

void init(list** l)
{
	list* x = malloc(sizeof(list));
	x->head = NULL;
	x->tail = NULL;
	x->size = 0;
	*l = x;
}

void deinit(list** l)
{
	while ((*l)->head != NULL) {
		rmvFirst(*l);
	}
	free(*l);
}

void insert(list* l, node* n)
{
	if (l->head == NULL)
		l->head = n;
	if (l->tail != NULL)
		l->tail->next = n;
	l->tail = n;
	l->size++;
}

void rmvFirst(list* l)
{
	if (l->size > 0) {
		node* tmp = l->head;
		l->head = l->head->next;
		free(tmp);
		l->size--;
	}
	if (l->size == 0) {
		l->head = NULL;
		l->tail = NULL;
	}
}

void rmvPid(list* l, pid_t pid)
{
	if (l->size > 0) {
		node* n = l->head;
		if (n->pid == pid)
			rmvFirst(l);
		else {
			while (n->next->pid != pid)
				n = n->next;
			node* tmp = n->next;
			n->next = n->next->next;
			l->size--;
			free(tmp);
		}
	}
	if (l->size == 0) {
		l->head = NULL;
		l->tail = NULL;
	}
}

int empty(list* l)
{
	return (l->size == 0) ? 1 : 0;
}

node* createNode(pid_t pid, pthread_t ptr)
{
	node* n = malloc(sizeof(node));
	n->pid = pid;
	n->ptr = ptr;
	n->next = NULL;
	return n;
}