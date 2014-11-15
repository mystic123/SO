/*
 * Systemy Operacyjne
 * Zadanie zaliczeniowe nr 1
 * Autor: Paweł Kapica, 334579
 *
 * plik w.c
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "err.h"

#define max_chars 23 /* long int maximum characters */

char end_code[] = "end";

int main()
{
	int pipe_dsc1[2], pipe_dsc2[2]; /* pipes */
	long int my_val = 1; /* value kept in the process */
	char buf[max_chars] = {0x0};
	char tmp[max_chars] = {0x0};
	int last = 1; /* indicates if process is last */
	int end = 0; /* indicates when end calculations */
	int buf_len;
	long int val;
	
	while (end == 0) {
		if ((buf_len = read (0, buf, sizeof(buf))) == -1) /* reading from parent */
			syserr("Error in read in %d", getpid());
		if (strncmp(buf, end_code, sizeof(end_code)) != 0) { /* calculations continue */
			val = strtol(buf, NULL, 0);
			sprintf(tmp, "%ld", my_val);
			if (last == 0) {
				if (write(pipe_dsc1[1], tmp, sizeof(tmp)) == -1) /* sending my_val to child */
					syserr("Error in write in %d", getpid());
				if ((buf_len = read (pipe_dsc2[0], buf, sizeof(buf))) == -1) /* waiting for children to complete step */
					syserr("Error in read in %d", getpid());
				if (write(1, buf, sizeof(buf)) == -1) /* sending signal to parent that step has ended*/
					syserr("Error in write in %d", getpid());
			}
			else { /* last process */
				if (write(1, tmp, sizeof(tmp)) == -1) /* sending signal that step is over*/
					syserr("Error in write in %d", getpid());

				/* creating pipes for child*/
				if (pipe(pipe_dsc1) == -1)
					syserr("Error in pipe in %d", getpid());
				if (pipe(pipe_dsc2) == -1)
					syserr("Error in pipe in %d", getpid());

				/* last process is creating next child */
				switch(fork()) {
					case -1:
						syserr("Error in fork\n");
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
						last = 0;
						/* closing unused */
						if (close(pipe_dsc1[0]) == -1)
							syserr("Error in close in %d", getpid());
						if (close(pipe_dsc2[1]) == -1)
							syserr("Error in close in %d", getpid());
						break;
				}
			}
			my_val = my_val + val; /* updating my_val */
		}
		else { /* time to end */
			end = 1;
			sprintf(tmp, "%ld", my_val);
			if (last == 0) {
				if (write(pipe_dsc1[1], end_code, sizeof(end_code)) == -1) /* sending end code to child */
					syserr("Error in write in %d", getpid());
				if (write(1, tmp, sizeof(tmp)) == -1) /* sending calculated value to parent*/
					syserr("Error in write in %d", getpid());
				do { /* sending values calculated by children to parent */
					if ((buf_len = read (pipe_dsc2[0], buf, sizeof(buf))) == -1)
						syserr("Error in read in %d", getpid());
					if (write(1, buf, sizeof(buf)) == -1)
						syserr("Error in write in %d", getpid());
				} while (strncmp(buf, end_code, sizeof(end_code)) != 0);
			}
			else { /* last process */
				if (write(1, tmp, sizeof(tmp)) == -1) /* sending calculated value to parent */
					syserr("Error in write in %d", getpid());
				if (write(1, end_code, sizeof(end_code)) == -1) /* sending end code to parent */
					syserr("Error in write in %d", getpid());
			}
		}
	}

	if (last == 0) {
		if (wait(0) == -1)
			syserr("Error in wait in %d", getpid());
	}
	
	return 0;
}