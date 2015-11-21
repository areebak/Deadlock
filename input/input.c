// procfile.c
// Thao Bach
// Helena Kleinschmidt
// Aida Zhumabekova

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "procfile.h"

int main(){
  int numTypes;        // numTypes
  int i, j;            // counter for iterating through lines in pg.txt and printing out contents of struct
  FILE * fp;        // file pointer

  // Open "pg.txt"
  fp = fopen ("pg.txt", "r");

  // Set numTypes to the first number in "pg.txt"
  fscanf(fp, "%d", &numTypes);

  // struct array that holds numTypes and procTypes specified in pg.txt
  struct Procfile proc[numTypes];

  // Iterate through lines 2 to the end of the file
  for (i = 0; i < numTypes; i++){
    // Set the values for the properties of structs inside the struct array proc
    fscanf(fp, "%s %d %d %d %d", proc[i].processType, &proc[i].avgCPU, &proc[i].avgBurst,
    &proc[i].avgInter, &proc[i].avgIO);
  }

  // Print out contents of struct after addition
  for(j = 0; j < numTypes; j++){
    printf("processType[%d]: %s\n", j, proc[j].processType);
    printf("avgCPU[%d]: %d\n", j, proc[j].avgCPU);
    printf("avgBurst[%d]: %d\n", j, proc[j].avgBurst);
    printf("avgInter[%d]: %d\n", j, proc[j].avgInter);
    printf("avgIO[%d]: %d\n", j, proc[j].avgIO);
  }

  fclose(fp);

  return(0);
}
