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
#include "resource.h"

// copied directly from http://stackoverflow.com/questions/3437404/min-and-max-in-c
// #define MIN(a,b) (((a)<(b))?(a):(b))
#define DEFAULT_INPUT_FILE "input/input.txt"
#define DEFAULT_SIM_TIME 500

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
Resource* RESRCS;
Process*  PROCESSES;

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
// int  advance_time(ClockSim *c, PQueue_STRUCT *event_q);
// int  randomExecTime(int len, int lower_bound);
// int  randomFreq(int len, int lower_bound);
// int  leaveSystem(Process* p, ClockSim* c, ProcStats* proc_stats);
// int  system_time(ClockSim* c);
// void createAndEnqueueEvent(PQueue_STRUCT* event_q, Process* proc, int timestamp, int type, CPUStats* cpu_stats);
// Process* newProcess(Procfile* proc_type, int timestamp);
// Process* genRemovalEventForProc(PQueue_STRUCT* event_q, Process* p, ClockSim* c, CPU_STRUCT* cpu,CPUStats* cpu_stats);

// ********************************** MAIN ************************************

int main(int argc, char* argv[]) {
	printf("Initializing simulation...\n");
	int i, index, jindex; // counters
	int jump; // keeps track of units of time between current and previous timestamps
	srand(time(NULL)); // required for randomization methods to work
	parse_args(argc, argv); // read and set command-line parameters
	exit(0);
	read_input(FILE_NAME); // read input file and configure simulation

	// ************************ INIT QUEUES & CLOCK ***************************

	ClockSim c = clockSim;
	PQueue_STRUCT* event_q = initPQ();

	//////////////////////// THIS IS WHERE WE INITIALIZE THE WAITING QUEUES FOR EACH RESOURCE

	// Queue_STRUCT* ready_q = initQ();

	// ***************************** INIT STATS *******************************

	/////////////////////////// INIT STATS OBJECTS
	// QStats*        qstats = (QStats*)malloc(sizeof(QStats));

		// insert initial process creation event into event Queue

		///////////////////////////////////////
		// begin by surveying the first claim that each process has on a resource and generate
		// a request event for that process and that resource, with a timestamp randomly generated

		// createAndEnqueueEvent(event_q, newProcess(&proc_types[i], 0), 0, 0, cpu_stats);
	// }

	// *************************** RUN SIMULATION *****************************

	if (SIM_STOP == 0) {
		printf("Simulation time CANNOT be 0 or unspecified.\n");
		printf("Please specify a simulation stop time and rerun the program with the -t flag.\n");
		printf("Program terminated.\n");
	} else {
		while(c.time < SIM_TIME) { // SIM_TIME was specified by user
			if (INCR) { printf("\n\nPress any key to advance time.\n\n"); mygetch(); }
			// int proc_turnaround = 0; // number of processes turned around
			if (ENABLE_VERBOSE) { printf("\nSystem time: %d\n", c.time); }
			// if (ENABLE_VERBOSE) { printf("EVENT QUEUE: "); printPQ(event_q); }
			Event* ev = dequeuePQ(event_q)->event; // take next event from event Queue
			// if (ENABLE_VERBOSE) { printf("Handling event "); eventString(ev); printf("\n"); }
			switch(ev->type){
				case 0: // creationEvent, put process on ready Queue
					// putProcOnReadyQ(ev->p, ready_q);
					break;
				case 1: // process leaves system
					// proc_turnaround = leaveSystem(ev->p, &c, proc_stats);
					break;
				case 2: // process gets killed
					break;
				case 3: // process requests resource
					// ioService(event_q, ev->p, &c, cpu_stats);
					break;
				case 4: // process releases resource
					// putProcOnReadyQ(ev->p, ready_q);
					break;
				default:
					printf("Failed; event type '%d' unknown.\n", ev->type);
			}

			// ************************ GATHER STATS **************************

			////////////////////// HMMMM DO THIS

			// jump = advance_time(&c, event_q);
		}
	}
	// int stop_time = system_time(&c) - jump;

 	// ***************************** OUTPUT STATS *****************************

	///////////////////// DO THIS TOO

	printf("===================================================================================\n\nSimulation terminated.\n");

	// ******************************************************************************
	//
	//          TODO:  REMEMBER TO FREE MEMORY WE MALLOC!!
	//
	// ******************************************************************************
}

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
	FILE* fp = fopen(file_name, "r");

	////////////////////////////////////////// THIS IS WHERE WE SET:
		// - NUM_PROCS
		// - RESRCS

	// fscanf(fp, "%d", &NUM_PROC_TYPES);
	// for (index = 0; index < NUM_PROC_TYPES; index++) {

		/////////////////////////////////////// THIS IS WHERE WE SET:
		// - 2d matrix representing max claims each process has on each resource
		// - 2d matrix representing mean length of time between each process’ requests for each resource
		// - 2d matrix representing length of time each process expects to retain each resource

	// 	fscanf(fp, "%s %d %d %d %d",
	// }
	fclose(fp);
}

// *************************** FUNCTION DEFINITIONS ***************************

// int randomExecTime(int len, int lower_bound) { return exponential_rand(len, lower_bound); }
// int system_time(ClockSim* c) { return c->time; }
// int randomFreq(int len, int lower_bound);

// /*
//  * Advances the logical time of the discrete event simulation to the time at
//  * which the next event takes place.
//  */
// int advance_time(ClockSim *c, PQueue_STRUCT *event_q) {
// 	int curr_time = c->time;
// 	// c->time = getMin(event_q)->priority;
// 	NodePQ* temp = getMin(event_q);
// 	c->time = temp->priority;
// 	if (ENABLE_VERBOSE) { (printf("System time advancing to %d\n", c->time)); }
// 	return c->time - curr_time;
// }

// /*
//  * Frees process memory from program as it exits system and returns turnaround
//  * time for process.
//  */
// int leaveSystem(Process* p, ClockSim* c, ProcStats* proc_stats) {
// 	ProcStats* procS = getProcTypeProcStats(proc_stats, p);
// 	procS->numCompleted++;// increment num completed

// 	if (ENABLE_VERBOSE) {printf("Process %p left the system.\n", p);}
// 	int temp_start = p->start_time;
// 	free(p); // free process memory
// 	return system_time(c) - temp_start;
// }

// /*
// * Assigns process to CPU and generates event to remove it from CPU at a later time.
// */
// Process* genRemovalEventForProc(PQueue_STRUCT* event_q, Process* p, ClockSim* c, CPU_STRUCT* cpu, CPUStats* cpu_stats) {

// 	if (ENABLE_VERBOSE) { printf("Putting process %p on CPU %p; should last %d time units.\n-> i.e. ", p, cpu, MIN(QUANTUM, MIN(p->exec_time_left, p->time_until_io))); }
// 	// non-preemptive scheduling
// 	if (QUANTUM == 0) {
// 		// case where the process needs no more i/o; just runs until it's done
// 		if (p->exec_time_left < p->time_until_io) {
// 			createAndEnqueueEvent(event_q, p, system_time(c)+ p->exec_time_left + CONTEXT_SWITCH_COST, 1, cpu_stats);
// 		// case where process needs i/o at end of burst time
// 		} else if (p->time_until_io < p->burst_time) {
// 			createAndEnqueueEvent(event_q, p, system_time(c)+ p->time_until_io + CONTEXT_SWITCH_COST, 2, cpu_stats);
// 		}
// 	// preemptive
// 	} else {
// 		// case where process can complete in less than quantum and next i/o fault time
// 		if (p->exec_time_left <= QUANTUM && p->exec_time_left < p->time_until_io) {
// 			// an event is generated that will cause the process to leave the system
// 			createAndEnqueueEvent(event_q, p, system_time(c) + p->exec_time_left + CONTEXT_SWITCH_COST, 1, cpu_stats);
// 		// if the time remaining for the process to I/O fault is less than the quantum
// 		} else if (p->time_until_io <= QUANTUM) {
// 			// an event is generated that will cause the process to leave for I/O service
// 			createAndEnqueueEvent(event_q, p, system_time(c) + p->time_until_io + CONTEXT_SWITCH_COST, 2, cpu_stats);
// 		// else the process' quantum expires
// 		} else {
// 			// an event is generated that will return the process to the ready Queue
// 			createAndEnqueueEvent(event_q, p, system_time(c) + QUANTUM + CONTEXT_SWITCH_COST, 3, cpu_stats);
// 		}
// 	}
// 	return p;
// }

// /*
//  * Initialize new process to be created at a later time.
//  */
// Process* newProcess(Procfile* proc_type, int timestamp) {
// 	int new_burst_time = randomBurstTime(proc_type->avgBurst);
// 	int new_exec_time = randomExecTime(proc_type->avgCPU, new_burst_time);
// 	int new_io_time = DISABLE_IO_FAULT ? 0 : randomIOTime(proc_type->avgIO, 0);
// 	// init process of that type
// 	Process* proc = createProc(proc_type->processType,
// 		new_burst_time,
// 		new_io_time,
// 		new_burst_time,
// 		timestamp,
// 		new_exec_time);
// 	if(ENABLE_VERBOSE) { printf("Creating process %p of type %s with burst time %d, io time %d, total cpu time %d.\n", proc, proc->type, proc->burst_time, proc->io_fault_serv, proc->exec_time_left); }
// 	return proc;
// }

// /*
//  * Creates new event and enqueues in event queue.
//  */
// void createAndEnqueueEvent(PQueue_STRUCT* event_q, Process* proc, int timestamp, int type, CPUStats* cpu_stats) {
// 	enqueuePQ(event_q, createEvent(proc, timestamp, type));
// 	if (ENABLE_VERBOSE) { printf("%d(%d) should appear in event queue.\n", timestamp, type); }
// 	if(CONTEXT_SWITCH_COST){cpu_stats->contxt_sw_time += CONTEXT_SWITCH_COST;}
// }