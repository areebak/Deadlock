// process.c
// Thao Bach
// Helena Kleinschmidt
// Aida Zhumabekova

#include "process.h"

Process* createProc(char* type, int burst_time, int io_fault_serv, int time_until_io, int start_time, int exec_time_left) {
	Process* p = malloc(sizeof(Process));
	p->type = type;
	p->burst_time = burst_time;
	p->io_fault_serv = io_fault_serv;
	p->time_until_io = time_until_io;
	p->start_time = start_time;
	p->exec_time_left = exec_time_left;
	return p;
}

void setCPU(Process* p, CPU_STRUCT* cpu, int start_onCPU) {
	p->cpu = cpu;
	p->start_onCPU = start_onCPU;
}

int procString(Process* p) {
	printf("PROCESS\t %p \tType %s", p, p->type);
	return 0;
}
