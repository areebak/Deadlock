// process.h
// Thao Bach
// Helena Kleinschmidt
// Aida Zhumabekova

#ifndef PROCESS_H
#define PROCESS_H
#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"

typedef struct Process {
	char* type;
	int burst_time;
	int io_fault_serv;
	int exec_time_left;
	int start_time;
	int start_onCPU;
	int time_until_io;
	CPU_STRUCT* cpu;
} Process;

Process* createProc(char* type, int burst_time, int io_fault_serv, int time_until_io, int start_time, int exec_time_left);
void setCPU(Process* p, CPU_STRUCT* cpu, int start_onCPU);
int procString(Process* p);

#endif
