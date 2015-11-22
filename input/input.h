// procfile.h
// Thao Bach
// Helena Kleinschmidt
// Aida Zhumabekova

#ifndef PROCFILE_H
#define PROCFILE_H
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct Procfile{
  char processType[20];
  int avgCPU;
  int avgBurst;
  int avgInter;
  int avgIO;
}Procfile;

#endif
