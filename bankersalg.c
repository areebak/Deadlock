#include <stdio.h>
#define numPrc 4
#define numRes 4

// array of max claims for each process for each resource
// array of current use for each process for each resource
// array of available

int[numPrc][numRes] maxClaims; // index[i][j] holds number of instances of resource j that process i can lay claim to  
int[numPrc][numRes] currUse; // index[i][j] holds number of instances of resource j that process i currently holds  
int[numRes] available; // index i holds number of instances of resource i available and currently not in use

int main() {

	// initialize maxclaims and curr use to zero
	int i; 
	int j; 
	for(i = 0; i < numPrc; i++) {
		for(j = 0; j < numRes; j++) {
			maxClaims[i][j] = 0; 
			currUse[i][j] = 0; 
		}
	}

	int k; 
	for(k = 0; k < numRes; k++) {
		available[k] = 0; 
	}
}


void setMaxClaims(int pID, int[] maxRes) {

	int i; 
	for(i = 0; i < numRes; i++)
		maxClaims[pID][i] = maxRes[i]; 
	return; 
}

