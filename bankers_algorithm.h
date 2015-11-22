/*
* bankers_algorithm.h
* Areeba Kamal and Helena Kleinschmidt
* CS 322 Final Project
*/ 

extern int setNumPrc(int setPrc);

extern int setNumRes(int setRes);

extern void initArrays();

extern void test();

extern void test2();

extern void setMaxClaims(int pID, int maxR[]);

extern void setCurrUse(int pID, int maxU[]);

extern void setAvailable(int avb[]);

extern void readMaxClaims();

extern void readCurrUse();

extern void readAvailable();

extern void runprocesses();

extern void printSafeSeq();