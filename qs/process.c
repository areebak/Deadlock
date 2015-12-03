// process.c
// Areeba Kamal and Helena Kleinschmidt

#include "process.h"

Process* createProc(int exec_time, int* max_claims, int num_resrcs, int* req_intervals, int* retain_time) {
	Process* p = malloc(sizeof(Process));

	// init ints 
	p->exec_time = exec_time;

	// init arrays
	p->max_claims = max_claims;
	p->req_intervals = req_intervals;
	p->retain_time = retain_time;

	// init currUse
	int* curr_use = malloc(sizeof(int) * num_resrcs);
	int i; 
	for(i = 0; i < num_resrcs; i++)
		curr_use[i] = 0; 

	p->curr_use = curr_use;
	
	return p;
}


/* if n = 0 - num processes
// if n = 1, num resources
// mcend = 2 + numPrcs
// mlends = mcends + numPrcs
// ltends = mlends + numPrcs
// if n bw 2 and mcends 
// break line, feed into max claims (n -2 process)
// if n bw mcends and mlends (n - mcends)
// break line, feed into mean lenth
// if n bw mlends and ltends  (n - mlends)
// break line, feed into length of time*/
