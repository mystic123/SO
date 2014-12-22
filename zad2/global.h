/*
 * Systemy Operacyjne
 * Zadanie zaliczeniowe nr 2
 * Autor: Paweł Kapica, 334579
 *
 * plik global.h
 *
 */
#ifndef GLOBALH
#define GLOBALH
#endif

#define MAXL 99
#define MAXK 99
#define MAXM 9999

#define OFFSET 10000L /* MAXM+1 */

#define HELLOKEY 0xaaaL /* hello queue key */
#define RRKEY 0xbbbL /* raport receive queue */
#define CSKEY 0xcccL /* commitee send queue */
#define CRKEY 0xdddL /* commitee receive queue */

#define HELLOMSG 666666L

#define COMM 123
#define RAP 456

/* oznaczenia jak w treści zadania */
typedef struct {
	long msg_type;
	pid_t pid;
	int m;
	int l;
	int k;
	int n;
} data_msg; /* hello and commitee data message type */

typedef struct {
	long msg_type;
	int w;
	int sumn;
} com_r_msg; /* response from server to commitee */

typedef struct {
	long msg_type;
	int x;
	int k;
	long long y;
	long long z;
	long long v;
} rep_r_msg1; /* message with summary */

typedef struct {
	long msg_type;
	int l;
	long long k;
	long long r[MAXK];
} rep_r_msg2; /* message with 1 list data */