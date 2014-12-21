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
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset */
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include "err.h"
#include "global.h"

int l;

void exit_signal(int sig)
{
	fprintf(stderr, "Utracono połączenie z serwerem.\n");
	exit(0);
}

void wrong_parameter(int sig)
{
	printf("Zły argument. Użycie: 1 parametr 0 < l <= L - nr listy\n");
	exit(0);
}

int main(int argc, char* argv[])
{
	int i,err,n,hello_q,rec_q;
	long long sum = 0;
	
	/* signals */
	if (signal(SIGINT, exit_signal) == SIG_ERR)
		syserr(0, "signal");
	
	if (signal(SIGUSR1, wrong_parameter) == SIG_ERR)
		syserr(0, "signal");
	
	data_msg msg = {0, 0, 0, 0, 0, 0};
	rep_r_msg1 r_msg1 = {0, 0, 0, 0, 0, 0};
	rep_r_msg2 r_msg2 = {0, 0, 0};
	
	memset(&r_msg2.r, 0, sizeof(long long)*MAXK);
	
	l = (argc==2) ? strtol(argv[1], NULL, 0) : 0;
	
	if (l < 0 || l > MAXM) {
		wrong_parameter(0);
	}
	
	/* set queues */
	if ((hello_q = msgget(HELLOKEY, 0)) == -1)
		syserr(0, "msgget");
	
	if ((rec_q = msgget(RRKEY, 0)) == -1)
		syserr(0, "msgget");
	
	msg.msg_type = HELLOMSG;
	msg.pid = getpid();
	msg.l = l;
	msg.n = RAP;
	
	if ((err = msgsnd(hello_q, &msg, sizeof(data_msg) - sizeof(long), 0)) != 0)
		syserr(err, "msgsnd");
	
	/* wait for message from server */
	if ((n = msgrcv(rec_q, &r_msg1, sizeof(rep_r_msg1) - sizeof(long), (long)getpid(), 0)) <= 0)
		syserr(errno, "msgrcv\n");
	
	printf("Przetworzonych komisji: %d / %d\n", r_msg1.x, r_msg1.k);
	printf("Uprawnionych do głosowania: %lld\n", r_msg1.y);
	printf("Głosów ważnych: %lld\n", r_msg1.z);
	printf("Głosów nieważnych: %lld\n", r_msg1.v);
	printf("Frekwencja: %lld%%\n", (r_msg1.y != 0) ? (((r_msg1.z + r_msg1.v)*100)/(r_msg1.y)) : 0);
	printf("Wyniki poszczególnych list:\n");
	
	/* receive data */
	do {
		if ((n = msgrcv(rec_q, &r_msg2, sizeof(rep_r_msg2) - sizeof(long), (long)getpid(), 0)) <= 0)
			syserr(errno, "msgrcv\n");
		
		if (r_msg2.k != -1) {
			for (i = 0; i < r_msg2.k; i++)
				sum+=r_msg2.r[i];
			
			printf("%d ", r_msg2.l);
			printf("%lld ", sum);
			for (i = 0; i < r_msg2.k; i++) {
				printf("%lld", r_msg2.r[i]);
				if (i < r_msg2.k-1)
					printf(" ");
			}
			printf("\n");
			sum = 0;
		}
	} while (l == 0 && r_msg2.k != -1);
	
	return 0;
}
