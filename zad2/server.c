#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <errno.h>
#include "global.h"
#include "err.h"
#include "list.h"

#define NUM_Q 4

long long completed[MAXL][MAXK]; /* completed data from commitees */

long long perm_to_vote;
long long valid_votes;
long long invalid_votes;

pid_t connected_comm[MAXM];
pthread_t com_threads[MAXM];

int completed_comm;

list* connected_reports = NULL;

int L,K,M;

/* synchronization */

pthread_attr_t attr;
pthread_mutex_t list_mutex;
pthread_rwlock_t rwlock;


int msgq[NUM_Q]; /* IPC queues for communicaton */

void *handle_com(void *data)
{
	long m; /* commitee id to handle */
	int i,j,end,n,err,entries, oldtype;
	long long comm_data[MAXL][MAXK]; /* data sent by commitee */
	long long voted,valid_votes;
	long long perm_to_vote;
	
	if ((err = pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype)) != 0)
		syserr (err, "setcanceltype");
	
	com_msg c_msg = {0,0,0,0,0};
	com_r_msg r_msg = {0,0,0};
	end = 0;
	c_msg = *(com_msg*)data;
	free((com_msg*)data);
	m = (long)c_msg.m;
	perm_to_vote = c_msg.l;
	voted = c_msg.k;
	valid_votes = 0;
	
	fprintf(stderr, "wontek komisji %d start\n", c_msg.m);
	fflush(stderr);
	
	for (i = 0; i < MAXL; i++)
		for (j = 0; j < MAXK; j++)
			comm_data[i][j] = 0;

	/* send response to commitee */
	r_msg.msg_type = m+10000L;
	r_msg.w = 1;
	entries = 0;
	if ((err = msgsnd(msgq[1], &r_msg, sizeof(com_r_msg) - sizeof(long), 0)) != 0)
		syserr(err, "msgsnd");

	/* read data */
	while (!end) {
		if ((n = msgrcv(msgq[0], &c_msg, sizeof(com_msg) - sizeof(long), m, 0)) <= 0)
			syserr(0, "Error in msgrcv\n");
		if (c_msg.l != -1) {
			entries++;
			comm_data[c_msg.l-1][c_msg.k-1] += c_msg.n;
			valid_votes += c_msg.n;
		}
		else {
			end = 1;
		}
	}
	
	/* update global data */
	if ((err = pthread_rwlock_wrlock(&rwlock)) != 0)
		syserr (err, "rwlock failed");
	pthread_cleanup_push(pthread_rwlock_unlock, &rwlock);
	
	for (i = 0; i < MAXL; i++)
		for (j = 0; j < MAXK; j++)
			completed[i][j] += comm_data[i][j];
		
	valid_votes += valid_votes;
	invalid_votes += voted - valid_votes;
	perm_to_vote += perm_to_vote;
	completed_comm++;
	
	/* send response */
	r_msg.msg_type = m;
	r_msg.w = entries;
	r_msg.sumn = valid_votes;
	
	if ((err = msgsnd(msgq[1], &r_msg, sizeof(com_r_msg) - sizeof(long), 0)) != 0)
		syserr(err, "msgsnd");
	
	connected_comm[c_msg.m-1] = -1;
	
	if ((err = pthread_rwlock_unlock(&rwlock)) != 0)
		syserr (err, "unlock failed");
	pthread_cleanup_pop(0);
	
	fprintf(stderr, "wontek komisji %d koniec\n", c_msg.m);
	fflush(stderr);
	
	return (void*)m;
}

void *handle_rep(void *data)
{
	int i,j,l,err,oldtype;
	pid_t pid;
	
	if ((err = pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype)) != 0)
		syserr (err, "setcanceltype");
	
	rep_msg msg = {0,0,0};
	rep_r_msg1 r_msg1 = {0,0,0,0,0,0};
	rep_r_msg2 r_msg2 = {0,0,0};
	memset(r_msg2.r,0, sizeof(int)*MAXK);
	msg = *(rep_msg*)data;
	free((rep_msg*)data);
	pid = msg.pid;
	l = msg.l;
	
	fprintf(stderr, "wontek raportu %d start\n", pid);
	fflush(stderr);
	
	if ((err = pthread_rwlock_rdlock(&rwlock)) != 0)
		syserr (err, "lock failed");
	pthread_cleanup_push(pthread_rwlock_unlock, &rwlock);
	
	/* send summary */
	r_msg1.msg_type = (long)pid;
	r_msg1.x = completed_comm;
	r_msg1.k = M;
	r_msg1.y = perm_to_vote;
	r_msg1.z = valid_votes;
	r_msg1.v = invalid_votes;
	
	if ((err = msgsnd(msgq[3], &r_msg1, sizeof(rep_r_msg1) - sizeof(long), 0)) != 0)
		syserr(err, "msgsnd");
	
	r_msg2.msg_type = (long)pid;
	r_msg2.k=K;
	
	/* send list data */
	if (l != 0) {
		r_msg2.l = l;
		
		for (i = 0; i < K; i++)
			r_msg2.r[i] = completed[l-1][i];
		
		if ((err = msgsnd(msgq[3], &r_msg2, sizeof(rep_r_msg2) - sizeof(long), 0)) != 0)
			syserr(err, "msgsnd");
	}
	else {
		for (j = 0; j < L; j++) {
			for (i = 0; i < K; i++) {
				r_msg2.l = j+1;
				r_msg2.r[i] = completed[j][i];
			}
			
			if ((err = msgsnd(msgq[3], &r_msg2, sizeof(rep_r_msg2) - sizeof(long), 0)) != 0)
				syserr(err, "msgsnd");
		}
	}
	
	if ((err = pthread_rwlock_unlock(&rwlock)) != 0)
		syserr (err, "unlock failed");
	pthread_cleanup_pop(0);
	
	/* end message */
	r_msg2.k = -1;
	
	if ((err = msgsnd(msgq[3], &r_msg2, sizeof(rep_r_msg2) - sizeof(long), 0)) != 0)
		syserr(err, "msgsnd");
	
	/* remove pid from list */
	if ((err = pthread_mutex_lock(&list_mutex)) != 0)
		syserr (err, "lock failed");
	pthread_cleanup_push(pthread_mutex_unlock, &list_mutex);
	
	rmvPid(connected_reports, pid);
	
	if ((err = pthread_mutex_unlock(&list_mutex)) != 0)
		syserr (err, "unlock failed");
	pthread_cleanup_pop(0);
	
	fprintf(stderr, "wontek raportu %d start\n", pid);
	fflush(stderr);
	
	return 0;
}

void exit_server(int sig)
{
	int i,err;
	
	/* kill threads */
	for (i = 0; i < MAXM; i++) {
		if (connected_comm[i] > 0) {
			if ((err = pthread_cancel(com_threads[i])) != 0)
				syserr(err, "Error in pthread_cancel");
			kill((pid_t)connected_comm[i], SIGINT);
		}
	}
	
	while (empty(connected_reports) != 1) {
		kill(connected_reports->head->pid, SIGINT);
		rmvFirst(connected_reports);
	}
	
	/* delete queues */
	i = 0;
	while (i < NUM_Q && msgq[i] != -1) {
		if (msgctl(msgq[i], IPC_RMID, 0) == -1)
			syserr(0, "msgctl RMID");
		i++;
	}
	
	/* destroy mutex */
	
	if ((err = pthread_rwlock_destroy(&rwlock)) != 0)
		syserr(err, "rwlock destroy");
	
	if ((err = pthread_mutex_destroy(&list_mutex)) != 0)
		syserr(err, "mutex destroy");
	
	deinit(&connected_reports);
	exit(0);
}

int main(int argc, char* argv[])
{
	int n,err;
	struct msqid_ds result;
	com_msg c_msg = {0, 0, 0, 0, 0};
	com_r_msg cr_msg = {0,0,0};
	rep_msg r_msg = {0,0,0};
	init(&connected_reports);
	
	L = (argc==4) ? strtol(argv[1], NULL, 0) : 0;
	K = (argc==4) ? strtol(argv[2], NULL, 0) : 0;
	M = (argc==4) ? strtol(argv[3], NULL, 0) : 0;
	
	fprintf(stderr,"Serwer uruchomiony z parametrami: L = %d K = %d M = %d\n", L, K, M);
	
	/* creating queues */
	if ((msgq[0] = msgget(CSKEY, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
		syserr(0, "msgget");
	
	if ((msgq[1] = msgget(CRKEY, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
		syserr(0, "msgget");
	
	if ((msgq[2] = msgget(RSKEY, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
		syserr(0, "msgget");
	
	if ((msgq[3] = msgget(RRKEY, 0666 | IPC_CREAT | IPC_EXCL)) == -1)
		syserr(0, "msgget");
	
	if (signal(SIGINT,  exit_server) == SIG_ERR)
		syserr(0, "signal");
	
	/* init mutexes */
	
	if ((err = pthread_mutex_init(&list_mutex, 0) != 0))
		syserr (err, "mutex init failed");
	
	if ((err = pthread_rwlock_init(&rwlock, 0) != 0))
		syserr (err, "rwlock init failed");

	while(1) {
		if (msgctl(msgq[0], IPC_STAT, &result) == -1)
			syserr(0, "Error in msqctl\n");
		
		if (result.msg_qnum > 0) {
			memset(&c_msg, 0, sizeof(com_msg));
			if ((n = msgrcv(msgq[0], &c_msg, sizeof(com_msg) - sizeof(long), HELLOMSG, IPC_NOWAIT)) <= 0)
				if (errno != 0 && errno != ENOMSG)
 					syserr(errno, "Error in msgrcv\n");
			
				if (c_msg.msg_type != 0) {
				if (c_msg.m <= M && connected_comm[c_msg.m-1] == 0) {
					connected_comm[c_msg.m-1] = c_msg.pid;
					if ((err = pthread_attr_init(&attr)) != 0 )
						syserr(err, "Error in pthread_attr_init\n");
					if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0)
						syserr(err, "Error in pthread_attr_setdetachstate\n");
					
					com_msg* msg = (com_msg*) malloc(sizeof(com_msg));
					memset(msg, 0, sizeof(com_msg));
					*msg = c_msg;
					if ((err = pthread_create(&com_threads[c_msg.m-1], &attr, handle_com, (void*) msg)) != 0)
						syserr(err, "Error in create\n");
				}
				else { /* denying access*/
					cr_msg.msg_type = (long)(c_msg.m+10000L);
					cr_msg.w = -1;
					if ((err = msgsnd(msgq[1], &cr_msg, sizeof(com_r_msg) - sizeof(long), 0)) != 0)
						syserr(err, "msgsnd");
				}
			}
		}
		
		if (msgctl(msgq[2], IPC_STAT, &result) == -1)
			syserr(0, "Error in msqctl\n");
		
		if (result.msg_qnum > 0) {
			if ((n = msgrcv(msgq[2], &r_msg, sizeof(rep_msg) - sizeof(long), 0, IPC_NOWAIT)) <= 0)
				syserr(0, "msgrcv");
			if (errno != 0 && errno != ENOMSG)
				syserr(errno, "Error in msgrcv ENOMSG\n");
			
			if (r_msg.l > L) { /* wrong parameter */
				kill(r_msg.pid, SIGUSR1);
			}
			else {
				if ((err = pthread_attr_init(&attr)) != 0 )
					syserr(err, "Error in pthread_attr_init\n");
				if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0)
					syserr(err, "Error in pthread_attr_setdetachstate\n");
				
				rep_msg* msg = (rep_msg*) malloc(sizeof(rep_msg));
				memset(msg, 0, sizeof(rep_msg));
				*msg = r_msg;
				node* n = createNode(r_msg.pid, 0);
				
				if ((err = pthread_mutex_lock(&list_mutex)) != 0)
					syserr (err, "lock failed");
				
				insert(connected_reports, n);
				
				if ((err = pthread_mutex_unlock(&list_mutex)) != 0)
					syserr (err, "unlock failed");
				
				if ((err = pthread_create(&(n->ptr), &attr, handle_rep, (void*) msg)) != 0)
					syserr(err, "Error in create\n");
			}
		}
		
	}
	return 0;
}