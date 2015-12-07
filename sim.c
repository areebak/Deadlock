// sim.c
// Areeba Kamal & Helena Kleinschmidt
// Adapted from CPUscheduler/sim.c (Bach, Kleinschmidt, Zhumabekova)

#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include "clockSim.h"
#include "simStats.h"
#include "sim.h"
#include "distribution.c"
#include "qs/queue.h"
#include "qs/pqueue.h"
#include "bankers_algorithm.h"
#include "qs/event.h"

// copied directly from http://stackoverflow.com/questions/3437404/min-and-max-in-c
// #define MAX(a,b) (((a)>(b))?(a):(b))
#define DEFAULT_INPUT_FILE "input/input.txt"
#define DEFAULT_SIM_TIME 500
#define MAX_LENGTH_PT 100
#define DEFAULT_PARTIAL_EXEC 0.5
#define DEFAULT_PARTIAL_RESRC 0.5
#define MIN_PARTIAL_EXEC 0.1
#define MIN_PARTIAL_RES 0.1
#define MAX_PARTIAL_EXEC 0.9
#define MAX_PARTIAL_RES 0.9

char*  FILE_NAME;
int    INCR; // 1 if user can run simulation step by step; 0 otherwise
int    ENABLE_VERBOSE;
int    SIM_TIME;
int    MODE; // 1 is deadlock avoidance; >1 is deadlock detection/recovery ~every MODE time units
int    NUM_PROCS;
int    NUM_RES;
float PARTIAL_EXEC;
float PARTIAL_RESRC;
Resource** RESRCS;
Process**  PROCESSES;
ProgramStats* ps; 

/*
 * Configure parameters.
 */
void setTIME  (int b) { SIM_TIME = b; }
void setVERBOSE    () { ENABLE_VERBOSE = 1; }
void setFILE(char *b) { FILE_NAME = b; }
void setMODE  (int b) { MODE = b; }
void setINCR       () { INCR = 1; }
void setPART_EXEC (float b) { PARTIAL_EXEC = b; }
void setPART_RES  (float b) { PARTIAL_RESRC = b; }

/*
 * Default command-line arguments if unspecified by user.
 */
void deflt() {
	if (!SIM_TIME) {
		setTIME(DEFAULT_SIM_TIME);
		if (ENABLE_VERBOSE) { printf("Simulation will run for %d units.\n", SIM_TIME); }
	}
	if (FILE_NAME == NULL) { setFILE(DEFAULT_INPUT_FILE); }
	if (ENABLE_VERBOSE)    { printf("Input file set to '%s'.\n", FILE_NAME); }
}

/*
 * Waits for user to press key to continue program execution. Will only get called in
 * main if INCR = 1. Copied directly from a solution at
 * http://stackoverflow.com/questions/10575478/wait-for-user-input-in-c
 */
int mygetch(void) {
	int ch;
	struct termios oldt, newt;
	tcgetattr (STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr (STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr (STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}

// ************************** FUNCTION DECLARATIONS ***************************

void parse_args(int argc, char* argv[]);
void read_input(char* file_name);
void readResourceArray(); 
void readProcessesArray(); 
int  advance_time(ClockSim *c, PQueue_STRUCT* event_q);
int  randomExecTime(int len, int lower_bound);
int  system_time(ClockSim* c);
void requestResources(Process* proc);
void claimResource(Process* proc, Resource* res, int how_many);
void createAndEnqueueEvent(PQueue_STRUCT* event_q, Process* proc, int timestamp, int type);
void releaseResource(Process* proc, Resource* res); 
int  sumProcMaxClaims(Process* proc); 
int  sumProcCurrUse(Process* proc); 
void evalProcProgress(PQueue_STRUCT* event_q, Process* proc, ClockSim* c);
void acquireResources();
void kill(Process* proc, PQueue_STRUCT* event_q, int timestamp);
void terminate(Process* proc, PQueue_STRUCT* event_q, int timestamp);
void restartProcess(Process* proc, PQueue_STRUCT* event_q, int timestamp);
PQueue_STRUCT* initEventQueue();
void event_updateStats(Process* proc, int timestamp, int type);
void kill_updateStats(Process* proc);
void handleDeadlock(PQueue_STRUCT* event_q, int timestamp);
void initProcessFields(Process* proc, int timestamp);
void reinitProcessFields(Process* proc, int timestamp);
void updateTimeRunSoFar(Process* proc, int timestamp, int calledFrom);

// ********************************** MAIN ************************************

int main(int argc, char* argv[]) {
	printf("Initializing simulation...\n");
	int i; // counter
	int jump; // keeps track of units of time between current and previous timestamps
	int thresh = MODE; // the threshold (timestamp) at which point the system must next check for deadlock
	srand(time(NULL)); // required for randomization methods to work
	parse_args(argc, argv); // read and set command-line parameters
	read_input(FILE_NAME); // read input file and configure simulation
	ClockSim c = clockSim; // <<<<<<<<<<<<<<<<<<<<<< INIT clock
	PQueue_STRUCT* event_q = initEventQueue(); // << INIT event queue

	ps = initPS(NUM_PROCS);  // INIT program stats struct (global)

	// test(); --> tests for bankers alg
	// exit(0); 
	// ***************************** INIT STATS *******************************

	// *************************** RUN SIMULATION *****************************
	while(c.time < SIM_TIME) {
		if (INCR) { printf("\n\nPress any key to advance time.\n\n"); mygetch(); }
		if (ENABLE_VERBOSE) { printf("\nSystem time: %d\n", c.time); }
		if (ENABLE_VERBOSE) { printf("Event queue: Size %d\n", getSizePQ(event_q)); printPQ(event_q); }
		if (getSizePQ(event_q)) {
			Event* ev = dequeuePQ(event_q)->event; // take next event from event Queue
			// if (ENABLE_VERBOSE) { printf("Handling event "); eventString(ev); printf("\n"); }
			switch(ev->type) {
				case 0: // process created
					requestResources(ev->proc);
					break;
				case 1: // process terminated
					terminate(ev->proc, event_q, system_time(&c));
					break;
				default:
					printf("Failed; event type '%d' unknown.\n", ev->type);
			}
		}
		acquireResources();
		if (c.time >= thresh) {	handleDeadlock(event_q, system_time(&c)); thresh = (thresh + MODE < c.time) ? (c.time + MODE) : (thresh + MODE); }
		for (i = 0; i < NUM_PROCS; i++) { evalProcProgress(event_q, PROCESSES[i], &c); }

		// ************************ GATHER STATS **************************
		jump = advance_time(&c, event_q);
	}
 	// ***************************** OUTPUT STATS *****************************
	///////////////////// DO THIS TOO
	printf("===================================================================================\n\nSimulation terminated.\n");
	// ******************************************************************************
	//
	//          TODO:  REMEMBER TO FREE MEMORY WE MALLOC!! >>> DOES THIS MEAN WHEN WE
	//                 DEQUEUE EVENTS AND KILL PROCESSES? RESOURCES CAN BE UNDONE AT SIMULATION TERMINATION
	//
	// ******************************************************************************
}

// *************************** FUNCTION DEFINITIONS ***************************
int  randomExecTime(int len, int lower_bound) { return exponential_rand(len, lower_bound); }
int  system_time(ClockSim* c) { return c->time; }


/*
 *
 */
void requestResources(Process* proc) {
	int i;
	for (i = 0; i < NUM_RES; i ++) {
		enqueueQ(RESRCS[i]->request_q, proc);
	}
}

/*
 * Read command-line arguments.
 */
void parse_args(int argc, char* argv[]) {
	if (ENABLE_VERBOSE) { printf("Parsing command-line arguments...\n"); }
	int opt = 0;
	static struct option long_options[] = {
		{"verbose",             no_argument, 0, 'v'},
		{"sim-time",      required_argument, 0, 't'},
		{"increment",           no_argument, 0, 'i'},
		{"input-file",    required_argument, 0, 'f'},
		{"partial-exec",  required_argument, 0, 'e'},
		{"partial-resrc", required_argument, 0, 'r'},
		{"avoid",               no_argument, 0, 'a'},
		{"detect",        required_argument, 0, 'd'},
		{0,                               0, 0,  0 }
	};
	while(1) {
		int option_index = 0;
		opt = getopt_long(argc, argv, "vt:if:e:r:ad:", long_options, &option_index);
		if (opt == -1)
		break;
		switch (opt) {
			case 'v':
				printf("Verbose enabled.\n");
				setVERBOSE();
				break;
			case 't':
				printf("Simulator will run for %d time units.\n", atoi(optarg));
				setTIME(atoi(optarg));
				break;
			case 'i':
				printf("Simulation incrementing manually.\n");
				setINCR();
				break;
			case 'f':
				printf("Input filename: %s.\n", optarg);
				setFILE(optarg);
				break;
			case 'e':
				setPART_EXEC(atof(optarg));
				if (PARTIAL_EXEC < MIN_PARTIAL_EXEC || PARTIAL_EXEC > MAX_PARTIAL_EXEC) {
					setPART_EXEC(DEFAULT_PARTIAL_EXEC);
					printf("Partial execution threshold out of bounds; set to default: %lf.\n", PARTIAL_EXEC);
				} else {
					printf("Partial execution threshold: %s", optarg);
				}
				break;
			case 'r':
				setPART_RES(atof(optarg));
				if (PARTIAL_RESRC < MIN_PARTIAL_RES || PARTIAL_RESRC > MAX_PARTIAL_RES) {
					setPART_RES(DEFAULT_PARTIAL_RESRC);
					printf("Partial resource acquisition threshold out of bounds; set to default: %lf.\n", PARTIAL_RESRC);
				} else {
					printf("Partial resource acquisition threshold: %s\n", optarg);
				}
				break;
			case 'a':
				printf("Deadlock avoidance activated.\n");
				setMODE(1);
				break;
			case 'd':
				printf("Deadlock detection and recovery activated.\n");
				setMODE(atoi(optarg));
				break;
			default:
				printf("^ See README for details.\nQuitting...\n");
				exit(0);
				break;
		}
	}
	// handle case where user enters unrecognized arguments
	if (optind < argc) {
		printf("Unrecognized flags: ");
		while (optind < argc)
		printf("%s ", argv[optind++]);
		printf("\n");
	}
	deflt();
}

/*
 * Read input file and configure simulation accordingly.
 */
void read_input(char* file_name) {
	if (ENABLE_VERBOSE) { printf("Reading input file...\n"); }
	FILE* file = fopen(file_name, "r");
	char line [MAX_LENGTH_PT];  // max length of a file
	int lineNum = 0; 
	if(file != NULL) {
		// vars to help track line number 
		int mcStart = 3; // beginning of line numbers specifying max claims
		int mcEnds; 	 // end of line numbers specifying max claims
		int iatStart; 	 // beginning of line numbers specifying mean length of time a process holds resource
		int iatEnds; 	 // end of line numbers specifying request intervals between process asking for resource 
		int etStart;	 // beginning of line numbers specifying length of time process retains resource
		int etEnds; 	 // beginning of line numbers specifying length of time process retains resource

		while(fgets ( line, MAX_LENGTH_PT, file) != NULL) { // read a line
			//printf("Read line %s\n", line);
			if(lineNum == 0) { // first line is number of processes
				NUM_PROCS = atoi(line); 
				PROCESSES = malloc(NUM_PROCS * sizeof(Process*)); 
				printf("Num processes is %d\n", NUM_PROCS);
				// init index vars since we know num procs
				mcEnds = mcStart + NUM_PROCS; 
				iatStart = mcEnds; 
				iatEnds = iatStart + NUM_PROCS; 
				etStart = iatEnds;
				etEnds = etStart + NUM_PROCS; 
				lineNum++; 
			} else if(lineNum == 1) { // second line is number of resources
				NUM_RES = atoi(line); 
				RESRCS = malloc(NUM_RES * sizeof(Resource*)); 
				printf("Num resources is %d\n", NUM_RES);
				lineNum++; 
			} else if(lineNum == 2) { // third line is number of instances of each resource
				int index0 = 0; // keep track of which index of resource array we are at, 
				char* numResInst;
				for(numResInst = strtok(line, " "); numResInst != NULL; numResInst = strtok(NULL, " ")) {
					Resource* RS = malloc(sizeof(Resource)); 
					RS->type = index0; 
					RS->total_inst = atoi(numResInst);
					RS->available = atoi(numResInst); 
					RS->request_q = initQ();
					RESRCS[index0] = RS; 
					index0++;
				} lineNum++;  
			} else if(lineNum >= mcStart && lineNum < mcEnds)  {// lines relevant to maxclaims for each resource - each line specifies max claims array for one process
				int index1 = lineNum - mcStart; 
				Process* PR = malloc(sizeof(Process)); // create new process
				PROCESSES[index1] = PR; 
				int* curr_use = malloc(sizeof(int) * NUM_RES);
				int a; 
				for(a = 0; a < NUM_RES; a++)
					curr_use[a] = 0;
				int* max_claims = malloc(sizeof(int) * NUM_RES); // init max claims array
				int i = 0; 
				char* numMC; // read line
				for(numMC = strtok(line, " "); numMC != NULL; numMC = strtok(NULL, " ")) {
					max_claims[i] = atoi(numMC); // store maxClaims array
					i++;
				} 
				// initialize process vars read in from input file
				PROCESSES[index1]->id = index1; 
				PROCESSES[index1]->curr_use = curr_use;
				PROCESSES[index1]->max_claims = max_claims; // set maxClaims array
				lineNum++;  								// go to next line
			} else if(lineNum >= iatStart && lineNum < iatEnds) {// lines relevant to interarrival time for each process - each line specifies interarrival time for one process
				int index2 = lineNum - iatStart; 
				PROCESSES[index2]->interarrivalTime = atoi(line); 
				lineNum++; 
			} else if(lineNum >= etStart && lineNum < etEnds) { // lines relevant to interarrival time for each process - each line specifies interarrival time for one process
				int index3 = lineNum - etStart; 
				PROCESSES[index3]->avg_execTime = atoi(line);
				// set default values of remaining process fields
				initProcessFields(PROCESSES[index3], 0); // time = 0
				lineNum++; 
			} 
		} readResourceArray(); 
		readProcessesArray(); 
		fclose(file);
	}
}

/*
 *
 */
void initProcessFields(Process* proc, int timestamp) {
	proc->timeRunSoFar = 0;
	proc->creationTime = timestamp;
	proc->actual_execTime = randomExecTime(proc->avg_execTime, 0);
	proc->executing = 0; 
	proc->start_firstPhase = -1; 
	proc->start_finalPhase = -1; 
	proc->end_firstPhase = -1; 
	proc->end_finalPhase = -1;
}


/*
 *
 */
void readResourceArray() {
	int a; 
	for(a = 0; a < NUM_RES; a++)
		printf("Resource Type: %d, with total instances %d and available instances %d\n", RESRCS[a]->type, RESRCS[a]->total_inst, RESRCS[a]->available); 
}

/*
 *
 */
void readProcessesArray() {
	int b, c; 
	for(b = 0; b < NUM_PROCS; b++) {
		printf("Process at index: %d\n", b);  
		for( c = 0; c < NUM_RES; c++) 
			printf("with id %d, exectime %d and interarrival time %d has, for R%d, max claims %d\n", PROCESSES[b]->id, PROCESSES[b]->avg_execTime, PROCESSES[b]->interarrivalTime, c, PROCESSES[b]->max_claims[c]);
	}
}

/*
 *
 */
PQueue_STRUCT* initEventQueue() {
	PQueue_STRUCT* event_q = initPQ();
	int i; 
	for (i = 0; i < NUM_PROCS; i++)
		createAndEnqueueEvent(event_q, PROCESSES[i], 0, 0); // type 0 is create
	return event_q;
}

/*
 *
 */
void claimResource(Process* proc, Resource* res, int how_many) { 
	res->available -= how_many;
	if (res->available < 0) { printf("Error: claimed more instances of resource than available. Aborting.\n"); exit(0); }
	proc->curr_use[res->type] += how_many;
	if (proc->curr_use[res->type] > proc->max_claims[res->type]) { printf("Error: allocated more instances of resource than necessary. Aborting.\n"); exit(0); }
	if (ENABLE_VERBOSE) { printf("%d Process with Process ID %d has been given %d instances of resource %d, leaving %d\n", res->total_inst, proc->id, how_many, res->type, res->available); }
}

/*
 *
 */
void releaseResource(Process* proc, Resource* res) {
	int num_relinq = proc->curr_use[res->type];
	res->available += num_relinq;
	proc->curr_use[res->type] = 0;
	if (ENABLE_VERBOSE) { printf("Process with Process ID %d has relinquished %d instances of resource %d\n", proc->id, num_relinq, res->type); }
}

/*
 *
 */
void kill(Process* proc, PQueue_STRUCT* event_q, int timestamp) {
	if (ENABLE_VERBOSE) { printf("Killing process %d...\n", proc->id); }
	int z; // release all resources
	for(z = 0; z < NUM_RES; z++)
		if (proc->curr_use[z]) { releaseResource(proc, RESRCS[z]); }
	kill_updateStats(proc);
	// createAndEnqueueEvent(event_q, proc, ); <-- NECESSARY BUT WRONG
	// reset process
	reinitProcessFields(proc, timestamp);
	createAndEnqueueEvent(event_q, proc, timestamp, 0); // type 0 is create
}

/*
 *
 */
void terminate(Process* proc, PQueue_STRUCT* event_q, int timestamp) {
		int z; // release all resources
	for(z = 0; z < NUM_RES; z++)
		if (proc->curr_use[z]) { releaseResource(proc, RESRCS[z]); }
	// reset process
	initProcessFields(proc, timestamp);
	createAndEnqueueEvent(event_q, proc, timestamp, 0); // type 0 is create
}

/*
 *
 */
void reinitProcessFields(Process* proc, int timestamp) {
	proc->actual_execTime = randomExecTime(proc->avg_execTime, 0);
	proc->executing = 0; 
	updateTimeRunSoFar(proc, timestamp, 0);
	proc->start_firstPhase = -1; 
	proc->start_finalPhase = -1; 
	proc->end_firstPhase = -1; 
	proc->end_finalPhase = -1;
}


/*
 *
 */
void acquireResources() {
	int i;
	for (i = 0; i < NUM_RES; i++) {
		Resource* this_resrc = RESRCS[i];
		Queue_STRUCT* request_q = this_resrc->request_q;
		if (ENABLE_VERBOSE) { printf("[%d] R %d request queue: ", this_resrc->available, i); printQ(request_q); }
		if (getSizeQ(request_q)) {
			Process* first_proc = peekQ(request_q)->process;
			// if the # of instances of res available meets/exceeds the number that first_proc still needs to reach max claims
			if (this_resrc->available >= first_proc->max_claims[i] - first_proc->curr_use[i]) {
				claimResource(dequeueQ(request_q)->process, this_resrc, first_proc->max_claims[i] - first_proc->curr_use[i]);
			} else if (this_resrc->available) {
				claimResource(first_proc, this_resrc, this_resrc->available);
			}
		}
	}
}

/*
 *
 */
void handleDeadlock(PQueue_STRUCT* event_q, int timestamp) {
	int i;
	setNumPrcRes(NUM_PROCS, NUM_RES);
	for (i = 0; i < NUM_PROCS; i++) {
		setMaxClaims(PROCESSES[i]->id, PROCESSES[i]->max_claims);
		setCurrUse(PROCESSES[i]->id, PROCESSES[i]->curr_use);
	}
	int* avb = (int*) malloc(sizeof(int) * NUM_RES);
	for (i = 0; i < NUM_RES; i++) {
		avb[i] = RESRCS[i]->available;
	}
	setAvailable(avb); 
	if(!runprocesses()) {
		Process* greediest = PROCESSES[0];
		for (i = 1; i < NUM_PROCS; i++) {
			greediest = sumProcCurrUse(greediest) > sumProcCurrUse(PROCESSES[i]) ? greediest : PROCESSES[i];
		}
		kill(greediest, event_q, timestamp);
	}
}

/*
 * Creates new event and enqueues in event queue.
 */
void createAndEnqueueEvent(PQueue_STRUCT* event_q, Process* proc, int timestamp, int type) {
	enqueuePQ(event_q, createEvent(proc, timestamp, type));
	event_updateStats(proc, timestamp, type); 
}

void event_updateStats(Process* proc, int timestamp, int type) {
	// helper vars
	int id = proc->id; 
	int proc_turnaroundTime = timestamp - proc->creationTime; 
	int proc_executionTime = proc->actual_execTime;
	// check type of event and update stats accordingly
	if(type) {	// This is a termination event
		ps->numCompleted[id]++;	// increment processes completed for this id
		ps->total_turnaround[id] += proc_turnaroundTime; // add turnaroundtime for this id
		ps->total_execution[id] += proc_executionTime; // add executiontime for this id
	} else { 	// This is a creation event
		ps->numCreated[id]++;	// increment processes created for this id
	}
}

void kill_updateStats(Process* proc) {
	ps->numKills[proc->id]++; 
}

/*
 * Advances the logical time of the discrete event simulation to the time at
 * which the next event takes place.
 */
int advance_time(ClockSim *c, PQueue_STRUCT *event_q) {
	int curr_time = c->time;
	NodePQ* temp = getMin(event_q);
	if (temp == NULL) {
		c->time += 1;
	} else {
		c->time = temp->priority;
	}
	if (ENABLE_VERBOSE) { (printf("System time advancing to %d\n", c->time)); }
	return c->time - curr_time;
}

/*
 *
 */
void updateTimeRunSoFar(Process* proc, int timestamp, int calledFrom) {
	// called from is binary. case 0 --> called because process killed; case 1 --> called when first phase ended
	if(calledFrom == 1)
		proc->timeRunSoFar += proc->end_firstPhase - proc->start_firstPhase; 
	else {
		if(proc->executing) {
			int t = 0;
			if(proc->start_firstPhase > -1) { 		// first phase has begun
				if(proc->end_firstPhase == -1) { 	// ... and first phase has not ended
					t = timestamp - proc->start_firstPhase;
				} else { 							// ... and first phase has ended
					if( proc->start_finalPhase > -1)// 			second phase has begun
						t = timestamp - proc->start_finalPhase;
				}
			}
			proc->timeRunSoFar += t; 
		}
	}
}
	
/*
 *
 */
void evalProcProgress(PQueue_STRUCT* event_q, Process* proc, ClockSim* c) {

	/* CALCULATE FIGURES FOR CHECKING WHAT CASE WE ARE IN */

	int curr_resrc_status = sumProcCurrUse(proc); // sum curr use
	int max_resrc_req = sumProcMaxClaims(proc); // set max need for resources
	int min_resrc_req = (int) (PARTIAL_RESRC * max_resrc_req); // set minimum requirement for resources
	if(min_resrc_req < 1) 	// each process requests at least 1 resource instance. cannot be 0. 
		min_resrc_req = 1;
	// first leg of process execution time - min_resrc_req quantity needed to complete phase
	int first_exec_phase =(int) PARTIAL_EXEC * proc->actual_execTime; 
	// final leg of process execution time - max claim quantity needed to complete phase
	int final_exec_phase = proc->actual_execTime - first_exec_phase; 

	/* SET CASE */

	int currentCase = -1;
	if(curr_resrc_status < min_resrc_req)
		currentCase = 0; 
	else if(curr_resrc_status >= min_resrc_req && curr_resrc_status < max_resrc_req )
		currentCase = 1; 
	else if(curr_resrc_status >= max_resrc_req)
		currentCase = 2; 

	if(ENABLE_VERBOSE) { printf("Case %d has been set\n", currentCase); }

	/* DETERMINE BEHAVIOR BASED ON CASE */

	switch(currentCase) { 
		case 0: // we do not have min resrc req
			if(ENABLE_VERBOSE) { printf("Process with id %d has insufficient resources to execute its first phase at time %d\n", proc->id, system_time(c)); }
			break; 	// do nothing - we don't have our min resrc req to run!
		case 1: // we have min resrc req but not full max claim req
			if(proc->executing) {
				if(system_time(c) - proc->start_firstPhase >= first_exec_phase) { // in first phase of execution-only need min resrc req which we now have!
					// we were previously executing - consider this phase complete - update stats and reverse executing
					proc->end_firstPhase = system_time(c);
					proc->executing = 0;
					updateTimeRunSoFar(proc, system_time(c), 1);
					if(ENABLE_VERBOSE) { printf("Process with id %d has finished executing its first phase - active time updated to %d\n", proc->id, proc->timeRunSoFar); }					
				}
			} else {
				// we were not executing, but now we have our min resrc req so we can - consider this phase has begun - update stats and reverse executing
				proc->start_firstPhase = system_time(c);
				proc->executing = 1;
				if(ENABLE_VERBOSE) { printf("Process with id %d is executing its first phase - started at time %d\n", proc->id, proc->start_firstPhase); }
			}
			break; 
		case 2: // we have full max claim req - begin final execution phase!
			if (!proc->executing) {
				// execute final phase, enqueue termination event
				proc->start_finalPhase = system_time(c);
				proc->executing = 1;
				createAndEnqueueEvent(event_q, proc, system_time(c)+(proc->actual_execTime - proc->timeRunSoFar), 1); // enqueue termination time for after final execution phase
				if(ENABLE_VERBOSE) { printf("Process with id %d is executing its final phase - will be terminated at time %d\n", proc->id, system_time(c)+(proc->actual_execTime - proc->timeRunSoFar)); }
			}
			break;
		default:
			printf("Failed; case type '%d' unknown.\n", currentCase);
	}
}

/*
 * Return the sum of the max_claims that proc has on each resource.
 */
int sumProcMaxClaims(Process* proc) {
	int sum, i;
	sum = 0;  
	for(i = 0; i < NUM_RES; i++)
		sum += proc->max_claims[i];
	return sum;
}

/*
 * Return the sum of the total number of instances of all resources that proc is currently holding.
 */
int sumProcCurrUse(Process* proc) {
	int sum, i;
	sum = 0;  
	for(i = 0; i < NUM_RES; i++)
		sum += proc->curr_use[i];
	return sum;
}

/*
 *
 */
int getPartialResrc(Process* proc) {
	// figure out partial resource status
	int proc_partial_resrc; 
	proc_partial_resrc = (int) (PARTIAL_RESRC * proc->max_claims[NUM_RES]);
	// minimum is 1 
	if(proc_partial_resrc < 1)
		proc_partial_resrc = 1; 

	return proc_partial_resrc; 
}
