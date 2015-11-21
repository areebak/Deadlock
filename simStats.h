// simStats.h
// Thao Bach
// Helena Kleinschmidt
// Aida Zhumabekova

#ifndef SimSTATS_H
#define SimSTATS_H

#include "PQQ/pqueue.h"

typedef struct QStats {
	int fin_len_q;
	int avg_len_q;
	int sum_q_len;
} QStats;

typedef struct PQStats {
	int fin_len_q;
	int avg_len_q;
	int sum_q_len;
} PQStats;

typedef struct ProcStats {
	char* type;
	int numCompleted;
	int thruput;
	int last_turn_time;
	int long_turn_time;
	int sum_turnaround;
	int avg_turn_time;
} ProcStats;

typedef struct CPUStats {
	int cpu;
	int active_time;
	int contxt_sw_time;
	int idle_time;
} CPUStats;

int  lenQ(Queue_STRUCT* q);
void addQ(QStats* qs, Queue_STRUCT* q);
double  avgLenQ(QStats* qs, int len_time);
int  lenPQ(PQueue_STRUCT* q);
void addPQ(PQStats* qs, PQueue_STRUCT* q);
double  avgLenPQ(PQStats* qs, int len_time);
int  numCompleted(ProcStats* ps);
double  thruput(ProcStats* ps, int stop_time);
int  lastTurnTime(ProcStats* ps);
int  longTurnTime(ProcStats* ps);
int  avgTurnTime(ProcStats* ps);
int  activeTime(CPUStats* cs);
int  contxtSwTime(CPUStats* cs);
int  idleTime(CPUStats* cs);

#endif
