// process.c
// Thao Bach
// Helena Kleinschmidt
// Aida Zhumabekova

#include "process.h"

Process* createProc(int exec_time, int* max_claims, int num_resrcs, int* req_intervals, int* retain_time) {
	Process* p = malloc(sizeof(Process));
	p->exec_time = exec_time;
	p->max_claims = max_claims;
	p->curr_use = malloc(sizeof(int) * num_resrcs);
	p->req_intervals = req_intervals;
	p->retain_time = retain_time;
	return p;
}

// int procString(Process* p) {
// 	printf("PROCESS\t %p \tType %s", p, p->type);
// 	return 0;
// }
