// process.h
// Areeba Kamal
// Helena Kleinschmidt

#ifndef PROCESS_H
#define PROCESS_H
#include <stdlib.h>
#include <stdio.h>

typedef struct Process {
	int  exec_time;
	int* max_claims;
	int* curr_use;
	int* req_intervals;
	int* retain_time;
} Process;

Process* createProc(int exec_time, int* max_claims, int num_resrcs, int* req_intervals, int* retain_time);
// int procString(Process* p);

#endif
