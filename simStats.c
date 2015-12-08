// simStats.c
// Helena Kleinschmidt
// Areeba Kamal

#include "simStats.h"
#include <stdlib.h>
#include <stdio.h>

// init program stats
ProgramStats* initPS(int numProcs){
  ProgramStats* ps = (ProgramStats*)malloc(sizeof(ProgramStats)); 
  ps->numCreated = (int*)malloc(sizeof(int) * numProcs);
  ps->numKills = (int*)malloc(sizeof(int) * numProcs);
  ps->numCompleted = (int*)malloc(sizeof(int) * numProcs);
  ps->total_turnaround = (int*)malloc(sizeof(int) * numProcs);
  ps->total_execution = (int*)malloc(sizeof(int) * numProcs);
  initPSFields(ps, numProcs);
  return ps;
}

// init program stats
void initPSFields(ProgramStats* ps, int numProcs) {
	int i; 
	for(i = 0; i < numProcs; i++) {
		ps->numProcs = numProcs;
		ps->numCreated[i] = 0;
  		ps->numKills[i] = 0;
  		ps->numCompleted[i] = 0;
  		ps->total_turnaround[i] = 0;
  		ps->total_execution[i] = 0;
	}
}

// total process stats
int	total_created(ProgramStats* ps) {
	int i;
	int total = 0; 
	for(i = 0; i < ps->numProcs; i++) {
		total += ps->numCreated[i];
	} 
	return total;
}

// total process stats
int	total_kills(ProgramStats* ps) {
	int i;
	int total = 0; 
	for(i = 0; i < ps->numProcs; i++) {
		total += ps->numKills[i];
	} 
	return total;
}

// total process stats
int	total_completed(ProgramStats* ps) {
	int i;
	int total = 0; 
	for(i = 0; i < ps->numProcs; i++) {
		total += ps->numCompleted[i];
	} 
	return total;
}

// total process ratios
double ratio_completedToCreated(ProgramStats* ps) {
	double completed = (double)(total_completed(ps));
	double created = (double)(total_created(ps));
	return completed / created;
}

// total process ratios
double ratio_killedToCreated(ProgramStats* ps) {
	double kills = (double)(total_kills(ps));
	double created = (double)(total_created(ps));
	return kills / created;
}

// total time stats
int	total_turnaround(ProgramStats* ps) {
	int i;
	int total = 0; 
	for(i = 0; i < ps->numProcs; i++) {
		total += ps->total_turnaround[i];
	} 
	return total;
}

// total time stats
int	total_execution(ProgramStats* ps) {
	int i;
	int total = 0; 
	for(i = 0; i < ps->numProcs; i++) {
		total += ps->total_execution[i];
	} 
	return total;
}

// total time ratios
double ratio_executionToTurnaround(ProgramStats* ps) {
	double turnaround = (double)(total_turnaround(ps));
	double execution = (double)(total_execution(ps));
	return execution / turnaround;
}

// total time ratios
double  thruput(ProgramStats* ps, int stop_time) {
	int completed = total_completed(ps);
	return  ((double) (completed)) / stop_time; 
}
