/*
 * Systemy Operacyjne
 * Zadanie zaliczeniowe nr 2
 * Autor: Paweł Kapica, 334579
 *
 * plik raport.c
 *
 */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include "err.h"
#include "global.h"

int m;

void exit_signal(int sig)
{
	fprintf(stderr, "Utracono połączenie z serwerem.\n");
	exit(0);
}

int main(int argc, char* argv[])
{
	int i,j,l,k,n,err,hello_q,send_q,rec_q;
	
	if (signal(SIGINT, exit_signal) == SIG_ERR)
		syserr(0, "signal");
	
	data_msg msg = {0, 0, 0, 0, 0, 0};
	com_r_msg r_msg = {0, 0, 0};
	
	m = (argc==2) ? strtol(argv[1], NULL, 0) : 0;
	
	if (m <= 0 || m > MAXM) {
		printf("Zły argument. Użycie: 1 parametr 0 < l <= M - nr komisji\n");
		exit(0);
	}
	
	/* set queues */
	if ((hello_q = msgget(HELLOKEY, 0)) == -1)
		syserr(0, "msgget");
	
	if ((send_q = msgget(CSKEY, 0)) == -1)
		syserr(0, "msgget");
	
	if ((rec_q = msgget(CRKEY, 0)) == -1)
		syserr(0, "msgget");
	
	n = scanf("%d %d", &i, &j);
	
	/* establising connection with server */
	msg.msg_type = HELLOMSG;
	msg.pid = getpid();
	msg.m = m;
	msg.l = i;
	msg.k = j;
	msg.n = COMM;
	
	if ((err = msgsnd(hello_q, &msg, sizeof(data_msg) - sizeof(long), 0)) != 0)
		syserr(err, "msgsnd");
	
	/* waiting for message from server */
	if ((n = msgrcv(rec_q, &r_msg, sizeof(com_r_msg) - sizeof(long), (long)(m+OFFSET), 0)) <= 0)
		syserr(errno, "msgrcv\n");
	
	if (r_msg.w == -1) {
		printf("Odmowa dostępu.\n");
		exit(0);
	}
	
	msg.msg_type = (long)m;
	
	while(scanf("%d %d %d", &l, &k, &n) != EOF) {
		msg.l = l;
		msg.k = k;
		msg.n = n;
		if ((err = msgsnd(send_q, &msg, sizeof(data_msg) - sizeof(long), 0)) != 0)
			syserr(err, "msgsnd");
	}
	
	/* sending final message */
	msg.l = -1;
	if ((err = msgsnd(send_q, &msg, sizeof(data_msg) - sizeof(long), 0)) != 0)
		syserr(err, "msgsnd");
	
	/* waiting for response */
	if ((n = msgrcv(rec_q, &r_msg, sizeof(com_r_msg) - sizeof(long), (long)m, 0)) <= 0)
		syserr(errno, "msgrcv\n");
	
	/* print status */
	printf("Przetworzonych wpisów: %d\n", r_msg.w);
	printf("Uprawnionych do głosowania: %d\n", i);
	printf("Głosów ważnych: %d\n", r_msg.sumn);
	printf("Głosów nieważnych: %d\n", j - r_msg.sumn);
	printf("Frekwencja w lokalu: %d%%\n", (((j)*100)/i));
	
	return 0;
}
