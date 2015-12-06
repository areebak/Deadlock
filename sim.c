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
#include "bankers_algorithm.h"
#include "qs/event.h"


// copied directly from http://stackoverflow.com/questions/3437404/min-and-max-in-c
// #define MIN(a,b) (((a)<(b))?(a):(b))
#define DEFAULT_INPUT_FILE "input/input.txt"
#define DEFAULT_SIM_TIME 500
#define MAX_LENGTH_PT 100

enum DEADLOCK_ALG {
	avoid = 0,
	detect = 1
} algorithm;

char* FILE_NAME;
int   INCR; // 1 if user can run simulation step by step; 0 otherwise
int   ENABLE_VERBOSE;
int   SIM_TIME;
int   MODE; // 0 is deadlock avoidance; 1 is deadlock detection and recovery
int   NUM_PROCS;
int   NUM_RES;
int   N = 100; // MAKE THIS A COMMAND-LINE ARGUMENT!!!!!
Resource** RESRCS;
Process**  PROCESSES;

/*
 * Configure parameters.
 */
void setTIME  (int b) { SIM_TIME = b; }
void setVERBOSE    () { ENABLE_VERBOSE = 1; }
void setFILE(char *b) { FILE_NAME = b; }
void setMODE  (int b) { MODE = b; }
void setINCR       () { INCR = 1; }

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
int  advance_time(ClockSim *c, PQueue_STRUCT *event_q);
int  randomExecTime(int len, int lower_bound);
int  system_time(ClockSim* c);
void requestResources(Process* proc, Resource* res);
void claimResource(Process* proc, Resource* res);
void createAndEnqueueEvent(PQueue_STRUCT* event_q, Process* proc, Resource* res, int timestamp, int type);
void releaseResource(Process* proc, Resource* res); 
void activateProcess(Process* proc);
void deactivateProcess(Process* proc);
void acquireResources();
PQueue_STRUCT* initEventQueue();
void handleDeadlock(int mode, Event* ev);

// ********************************** MAIN ************************************

int main(int argc, char* argv[]) {

	printf("Initializing simulation...\n");
	int i; // counter
	int jump; // keeps track of units of time between current and previous timestamps
	int thresh = N; // the threshold (timestamp) at which point the system must next check for deadlock
	srand(time(NULL)); // required for randomization methods to work
	parse_args(argc, argv); // read and set command-line parameters
	read_input(FILE_NAME); // read input file and configure simulation
	// test(); 			   // bankers algorithm tests
	// test2(); 
	//exit(0);
	ClockSim c = clockSim; // <<<<<<<<<<<<<<<<<<<<<< INIT clock
	PQueue_STRUCT* event_q = initEventQueue(); // << INIT event queue

	// ***************************** INIT STATS *******************************

	// *************************** RUN SIMULATION *****************************

	while(c.time < SIM_TIME) {
		if (INCR) { printf("\n\nPress any key to advance time.\n\n"); mygetch(); }
		// int proc_turnaround = 0; // number of processes turned around
		if (ENABLE_VERBOSE) { printf("\nSystem time: %d\n", c.time); }
		if (ENABLE_VERBOSE) { printf("Event queue: Size %d\n", getSizePQ(event_q)); printPQ(event_q); }
		Event* ev = dequeuePQ(event_q)->event; // take next event from event Queue
		// if (ENABLE_VERBOSE) { printf("Handling event "); eventString(ev); printf("\n"); }
		switch(ev->type) {
			case 0: // process created
				activateProcess(ev->proc);
				requestResources(ev->proc, ev->res);
				break;
			case 1: // process terminated - release resources and deactivate?
				deactivateProcess(ev->proc);
				for(i = 0; i < NUM_RES; i++) { releaseResource(ev->proc, ev->res); }
				break;
			default:
				printf("Failed; event type '%d' unknown.\n", ev->type);
		}
		if (c.time >= thresh) {	handleDeadlock(MODE); thresh += N; }
		acquireResources();

		// ************************ GATHER STATS **************************

		////////////////////// HMMMM DO THIS

		jump = advance_time(&c, event_q);
	}
	// int stop_time = system_time(&c) - jump;

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
void activateProcess(Process* proc) { proc->activated = 1; }
void deactivateProcess(Process* proc) { proc->activated = 0; }
void requestResources(Process* proc, Resource* res) { enqueueQ(res->request_q, proc); }

/*
 * Read command-line arguments.
 */
void parse_args(int argc, char* argv[]) {
	if (ENABLE_VERBOSE) { printf("Parsing command-line arguments...\n"); }
	int opt = 0;
	static struct option long_options[] = {
		{"verbose",          no_argument, 0, 'v'},
		{"sim-time",   required_argument, 0, 't'},
		{"increment",        no_argument, 0, 'i'},
		{"input-file", required_argument, 0, 'f'},
		{"avoid",            no_argument, 0, 'a'},
		{"detect",     required_argument, 0, 'd'},
		{0,                            0, 0,  0 }
	};
	while(1) {
		int option_index = 0;
		opt = getopt_long(argc, argv, "vt:if:ad:", long_options, &option_index);
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
			case 'a':
				printf("Deadlock avoidance activated.\n");
				setMODE(avoid);
				break;
			case 'd':
				printf("Deadlock detection and recovery activated.\n");
				setMODE(detect);
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
			if(lineNum == 0) // first line is number of processes
			{ 
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
			} 
			else if(lineNum == 1) // second line is number of resources
			{
				NUM_RES = atoi(line); 
				RESRCS = malloc(NUM_RES * sizeof(Resource*)); 
				printf("Num resources is %d\n", NUM_RES);
				lineNum++; 
			} 
			else if(lineNum == 2) // third line is number of instances of each resource
			{
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
				} 
				lineNum++;  
			} 
			else if(lineNum >= mcStart && lineNum < mcEnds)  // lines relevant to maxclaims for each resource - each line specifies max claims array for one process
			{
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

				PROCESSES[index1]->activated = 0;
				PROCESSES[index1]->id = index1; 
				PROCESSES[index1]->curr_use = curr_use;
				PROCESSES[index1]->max_claims = max_claims; // set maxClaims array
				lineNum++;  								// go to next line
			} 
			else if(lineNum >= iatStart && lineNum < iatEnds) // lines relevant to interarrival time for each process - each line specifies interarrival time for one process
			{
				int index2 = lineNum - iatStart; 
				PROCESSES[index2]->interarrivalTime = atoi(line); 
				lineNum++; 
			} 
			else if(lineNum >= etStart && lineNum < etEnds) // lines relevant to interarrival time for each process - each line specifies interarrival time for one process
			{
				int index3 = lineNum - etStart; 
				PROCESSES[index3]->execTime = atoi(line); 
				lineNum++; 
			} 
		} 

		readResourceArray(); 
		readProcessesArray(); 
		fclose(file);
	}
}

void readResourceArray() {
	int a; 
	for(a = 0; a < NUM_RES; a++)
		printf("Resource Type: %d, with total instances %d and available instances %d\n", RESRCS[a]->type, RESRCS[a]->total_inst, RESRCS[a]->available); 
}

void readProcessesArray() {
	int b, c; 
	for(b = 0; b < NUM_PROCS; b++) {
		printf("Process at index: %d\n", b);  
		for( c = 0; c < NUM_RES; c++) 
			printf("with id %d, exectime %d and interarrival time %d has, for R%d, max claims %d\n", PROCESSES[b]->id, PROCESSES[b]->execTime, PROCESSES[b]->interarrivalTime, c, PROCESSES[b]->max_claims[c]);
	}
}

PQueue_STRUCT* initEventQueue() {
	PQueue_STRUCT* event_q = initPQ();
	int i, j; 
	for (i = 0; i < NUM_PROCS; i++) {
		for (j = 0; j < NUM_RES; j++)
			createAndEnqueueEvent(event_q, PROCESSES[i], RESRCS[j], 0, 0); // type 0 is create
	}
	return event_q;
}

void restartProcess(Process* proc, PQueue_STRUCT* event_q, ClockSim* c) {
	activateProcess(proc);
	int i, y;
	for (y = 0; y < NUM_RES; y++) {
		createAndEnqueueEvent(event_q, proc, RESRCS[y], system_time(c)+proc->interarrivalTime, 0); // type 0 is create
	}
}

void claimResource(Process* proc, Resource* res) { 
	res->available -= proc->max_claims[res->type]; 
	proc->curr_use[res->type] += proc->max_claims[res->type];
	if(ENABLE_VERBOSE) { printf("%d Process with Process ID %d has been given %d instances of resource %d, leaving %d\n", res->total_inst, proc->id, proc->max_claims[res->type], res->type, res->available); }
}

void releaseResource(Process* proc, Resource* res) {
	res->available += proc->max_claims[res->type];
	proc->curr_use[res->type] -= proc->max_claims[res->type];
	if(ENABLE_VERBOSE) { printf("Process with Process ID %d has relinquished %d instances of resource %d\n", proc->id, proc->max_claims[res->type], res->type); }
}

void kill(Process* proc) {
	deactivateProcess(proc); 
	int z; // release all resources
	for(z = 0; z < NUM_RES; z++)
		if (proc->curr_use[z]) { releaseResource(proc, RESRCS[z]); }
}

void acquireResources() {
	int i;
	for (i = 0; i < NUM_RES; i++) {
		Resource* this_resrc = RESRCS[i];
		Queue_STRUCT* request_q = this_resrc->request_q;
		if (ENABLE_VERBOSE) { printf("[%d] R %d request queue: ", this_resrc->available, i); printQ(request_q); }
		if (getSizeQ(request_q)) {
			Process* first_proc = peekQ(request_q)->process;
			if (this_resrc->available >= first_proc->max_claims[i]) {
				claimResource(dequeueQ(request_q)->process, this_resrc);
			}
		}
	}
}

void handleDeadlock(int mode, Event* ev) {
	int i;
	setNumPrcRes(NUM_PROCS, NUM_RES);
	for (i = 0; i < NUM_PROCS; i++) {
		setMaxClaims(PROCESSES[i]->id, PROCESSES[i]->max_claims);
		setCurrUse(PROCESSES[i]->id, PROCESSES[i]->curr_use);
	}
	int avb[] = (int)malloc(sizeof(int) * NUM_RES);
	for (i = 0; i < NUM_RES; i++) {
		avb[i] = RESRCS[i]->available;
	}
	setAvailable(avb); 
	if(!runprocesses()) {
		rollback(Event* ev);
	}
}

void rollback(Event* ev) {

}

/*
 * Creates new event and enqueues in event queue.
 */
void createAndEnqueueEvent(PQueue_STRUCT* event_q, Process* proc, Resource* res, int timestamp, int type) {
	enqueuePQ(event_q, createEvent(proc, res, timestamp, type));
}

/*
 * Advances the logical time of the discrete event simulation to the time at
 * which the next event takes place.
 */
int advance_time(ClockSim *c, PQueue_STRUCT *event_q) {
	int curr_time = c->time;
	// c->time = getMin(event_q)->priority;
	NodePQ* temp = getMin(event_q);
	c->time = temp->priority;
	if (ENABLE_VERBOSE) { (printf("System time advancing to %d\n", c->time)); }
	return c->time - curr_time;
}
