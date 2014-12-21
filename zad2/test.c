#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <errno.h>
#include <pthread.h>
#include "err.h"

#define MAXK 100

pthread_rwlock_t rwlock;

int main()
{
	int err;
	if ((err = pthread_rwlock_init(&rwlock, 0) != 0))
		syserr (err, "rwlock init failed");
	
	if ((err = pthread_rwlock_rdlock(&rwlock)) != 0)
		syserr (err, "lock failed");
	
	if ((err = pthread_rwlock_unlock(&rwlock)) != 0)
		syserr (err, "unlock failed");
	
	if ((err = pthread_rwlock_destroy(&rwlock)) != 0)
		syserr(err, "rwlock destroy");
	return 0;
}