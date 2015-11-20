#include <stdio.h>
#include <bankers_algorithm.h>
#define numPrc 3
#define numRes 3

int maxClaims[numPrc][numRes]; // index[i][j] holds max number of instances of resource j that process i can lay claim to  
int currUse[numPrc][numRes]; // index[i][j] holds number of instances of resource j that process i currently holds  
int available[numRes]; // index i holds number of instances of resource i available and currently not in use
int unexecuted[numPrc]; // index i says whether process i has been executed yet or not
int safeState[numPrc];
int safe; 
int processesLeft; 

int main() {

	initArrays();
	test2(); 
	initArrays();
	test(); 

}

void initArrays() {

	int i, j, k, l, m; // initialize maxclaims and curr use to zero
	for(i = 0; i < numPrc; i++) {
		for(j = 0; j < numRes; j++) {
			maxClaims[i][j] = 0; 
			currUse[i][j] = 0; 
		}
	}
	for(k = 0; k < numRes; k++) {
		available[k] = 0; 
	}

	for(l = 0; l < numPrc; l++) {
		unexecuted[l] = 1; 
	}

	for(m = 0; m < numPrc; m++) {
		safeState[m] = -1; 
	}
}

void test() {

	printf("\n\n Starting test 1:\n");
	setMaxClaims(0, 1, 7, 5);
	setMaxClaims(1, 0, 6, 5);
	setMaxClaims(2, 0, 6, 5);

	setCurrUse(0, 1, 0, 0);
	setCurrUse(1, 0, 6, 3); 
	setCurrUse(2, 0, 0, 1);

	setAvailable(1, 5, 2);

	readMaxClaims(); 
	readCurrUse(); 
	readAvailable(); 
	runprocesses();
	return;
}

void test2() {

	printf("\n\n Starting test 2:\n");
	setMaxClaims(0, 5, 6, 7);
	setMaxClaims(1, 3, 4, 5);
	setMaxClaims(2, 0, 6, 5);

	setCurrUse(0, 1, 0, 0);
	setCurrUse(1, 0, 0, 0); 
	setCurrUse(2, 4, 5, 6);

	setAvailable(1, 1, 1);

	readMaxClaims(); 
	readCurrUse(); 
	readAvailable(); 
	runprocesses();
	return;
}


void setMaxClaims(int pID, int maxR0, int maxR1, int maxR2) {

	maxClaims[pID][0] = maxR0; 
	maxClaims[pID][1] = maxR1; 
	maxClaims[pID][2] = maxR2; 
	return; 
}

void setCurrUse(int pID, int currR0, int currR1, int currR2) {

	currUse[pID][0] = currR0; 
	currUse[pID][1] = currR1; 
	currUse[pID][2] = currR2; 
	return; 
}

void setAvailable(int avbR0, int avbR1, int avbR2) {

	available[0] = avbR0; 
	available[1] = avbR1; 
	available[2] = avbR2; 
	return; 
}

void readMaxClaims() {

	int i;
	int j;
	for(i = 0; i < numPrc; i++)
		for(j = 0; j < numRes; j++)
			printf("For process %d, maxClaims to resource %d are %d\n", i, j, maxClaims[i][j]);
}

void readCurrUse() {

	int i; 
	int j; 
	for(i = 0; i < numPrc; i++)
		for(j = 0; j < numRes; j++)
			printf("For process %d, current use of resource %d are %d\n", i, j, currUse[i][j]);
}

void readAvailable() {

	int j;
	for(j = 0; j < numRes; j++)
		printf("For resource %d, number of available units are %d\n", j, available[j]);
}

void runprocesses() {

	int runningAProcess = 1; 
	safe = 0; 
	processesLeft = numPrc;
	while(processesLeft != 0) { // while all processes have not had their turn
		safe = 0; // anticipate unsafe state, walk through executed processes, looking for a process to run next
		int p, r;
		for(p = 0; p < numPrc; p++) {// this loop will run as long as we don't execute all our processes
			printf("Processes being executed is %d\n", p);
			if(unexecuted[p]) { 			// if this process has not been run yet
				runningAProcess = 1; 
				for(r = 0; r < numRes; r++) { // check if it is safe to run
					if(maxClaims[p][r] > currUse[p][r] + available[r]) {
						printf("Insufficient instances of resource %d for process %d\n", r, p);
						runningAProcess = 0; 
						break; // process not safe to run, break and move to next process
					} 
				}
				if(runningAProcess) {
					unexecuted[p] = 0; // if it is safe to run, execute it
					safe = 1; // safe process search in process
					printf("Executing process with pID %d\n", p);
					safeState[numPrc - processesLeft] = p; 
					processesLeft--; 
					int i; 
					for(i = 0; i < numRes; i++) {// increment available resources
						available[i] = available[i] + currUse[p][i];
						currUse[p][i] = 0; 
					}
					break; // process safely run, break and move to next process
				} 
			}
		}
	}
	if(safe == 0) 
		printf("Safe state not found!\n");
	else
		printSafeSeq();
}

void printSafeSeq() {

	printf("Safe sequence found!\nSafe sequence is:\n");
		int i;
		for(i = 0; i < numPrc; i++)
			printf("%d\n", safeState[i]);
		return;
}



