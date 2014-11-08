/*
 * Systemy Operacyjne
 * Zadanie zaliczeniowe nr 1
 * Autor: Paweł Kapica, 334579
 *
 * plik pascal.c
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

#define max_chars 23 /* long int maximum characters */

char end_code[] = "end";

int main(int argc, char* argv[])
{
	int pipe_dsc1[2], pipe_dsc2[2]; /* pipes */
	long int N = (argc==2) ? strtol(argv[1], NULL, 0) : 0;
	char tmp[max_chars] = {0x0};
	char buf[max_chars] = {0x0};
	int i = 1;
	long int val;

	if (N <= 0) {
		printf("Wrong parameters. Use: 1 parameter n - nth row of Pascal's triangle to calculate.\n");
		return 0;
	}
	
	/* creating pipes for 1st child*/
	if (pipe(pipe_dsc1) == -1)
		syserr("Error in pipe in %d", getpid());
	if (pipe(pipe_dsc2) == -1)
		syserr("Error in pipe in %d", getpid());

	/* creating 1st child */
	switch(fork()) {
		case -1:
			syserr("Error in fork");
		case 0:
			/* duplicating i/o*/
			if (dup2(pipe_dsc1[0],0) == -1)
				syserr("Error in dup in %d", getpid());
			if (dup2(pipe_dsc2[1],1) == -1)
				syserr("Error in dup in %d", getpid());
			
			/* closing unused */
			if (close(pipe_dsc1[0]) == -1)
				syserr("Error in close in %d", getpid());
			if (close(pipe_dsc1[1]) == -1)
				syserr("Error in close in %d", getpid());
			if (close(pipe_dsc2[0]) == -1)
				syserr("Error in close in %d", getpid());
			if (close(pipe_dsc2[1]) == -1)
				syserr("Error in close in %d", getpid());
			
			execl("w", "w", (char*) 0);
			syserr("Error in execl in %d", getpid());
			break;
		default:
			/* closing unused */
			if (close(pipe_dsc1[0]) == -1)
				syserr("Error in close in %d", getpid());
			if (close(pipe_dsc2[1]) == -1)
				syserr("Error in close in %d", getpid());
			break;
	}

	/* calculations */
	val = 0;
	sprintf(buf, "%ld", val);
	if (i < N) {
		while(i<N) {
			if (write(pipe_dsc1[1], buf, sizeof(buf)) == -1)
				syserr("Error in write in %d", getpid());
			if (read(pipe_dsc2[0], tmp, sizeof(tmp)) == -1)
				syserr("Error in read in %d", getpid());
			i++;
			if (i==N) { /* time to end */
				if (write(pipe_dsc1[1], end_code, sizeof(end_code)) == -1)
					syserr("Error in write in %d", getpid());
			}
		}
	}
	else {
		if (write(pipe_dsc1[1], end_code, sizeof(end_code)) == -1)
			syserr("Error in write in %d", getpid());
	}

	/* reading results from children and printing to stdout */
	for (i = 0; i < N; i++) {
		if (read(pipe_dsc2[0], tmp, sizeof(tmp)) == -1)
			syserr("Error in read in %d", getpid());
		printf("%s",tmp);
		if (i < N-1)
			printf(" ");
	}
	printf("\n");

	if (wait(0) == -1)
		syserr("Error in wait in %d", getpid());
	
	return 0;
}