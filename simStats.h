// simStats.h
// Helena Kleinschmidt
// Areeba Kamal

#ifndef SimSTATS_H
#define SimSTATS_H

typedef struct ProgramStats {
	int numProcs; 
	int* numCreated;
	int* numKills;
	int* numCompleted;
	int* total_turnaround;
	int* total_execution;
} ProgramStats;

// init program stats
ProgramStats* 	initPS(int numProcs);
void 			initPSFields(ProgramStats* ps, int numProcs);
// total process stats
int				total_created(ProgramStats* ps);
int 			total_kills(ProgramStats* ps); 
int 			total_completed(ProgramStats* ps); 
// total process ratios
double  		ratio_completedToCreated(ProgramStats* ps);
double 			ratio_killedToCreated(ProgramStats* ps);
// total time stats
int  			total_turnaround(ProgramStats* ps);
int  			total_execution(ProgramStats* ps);
// total time ratios
double  		ratio_executionToTurnaround(ProgramStats* ps);
double  		thruput(ProgramStats* ps, int stop_time);

#endif
