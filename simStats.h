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
// process id specific stats
int  			numCreated_proc(ProgramStats* ps, int procID);
int  			numKills_proc(ProgramStats* ps, int procID);
int  			numCompleted_proc(ProgramStats* ps, int procID);
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
double  		ratio_turnaroundToexecution(ProgramStats* ps);
double  		thruput(ProgramStats* ps, int stop_time);

#endif


/// notes to self

// step 1: kill and create and enqueue are  two methods that need an additional parameter of type PS
// edit - made program stats global!
// step 2: 	in createAndEnqueue:
// 				each time we create, 
// 					++numCreated
// 				each time we terminate
// 					++ numCompleted
// 					add systime-creationtime to total total_turnaround
// 					add actual exec time to total_execution
// step 3: 	in kill:
// 				++numKills
// step 4: write simStats.c methods now