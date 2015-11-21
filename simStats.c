// simStats.c
// Thao Bach
// Helena Kleinschmidt
// Aida Zhumabekova

#include "simStats.h"

int lenQ(Queue_STRUCT* q) {
	return getSizeQ(q);
}

void addQ(QStats* qs, Queue_STRUCT* q) {
	qs->sum_q_len += lenQ(q);
}

double avgLenQ(QStats* qs, int len_time) {
	return (double)qs->sum_q_len/len_time;
}

int lenPQ(PQueue_STRUCT* q) {
	return getSizePQ(q);
}

void addPQ(PQStats* qs, PQueue_STRUCT* q) {
	qs->sum_q_len += lenPQ(q);
}

double avgLenPQ(PQStats* qs, int len_time) {
	return (double)qs->sum_q_len/len_time;
}

int numCompleted(ProcStats* ps) {
	return ps->numCompleted;
}

double thruput(ProcStats* ps, int stop_time) {
	return  (double)numCompleted(ps)/stop_time;
}

int lastTurnTime(ProcStats* ps) {
	return ps->last_turn_time;
}

int longTurnTime(ProcStats* ps) {
	return ps->long_turn_time;
}

int avgTurnTime(ProcStats* ps) {
	if(numCompleted(ps)>0){return ps->sum_turnaround/numCompleted(ps);}
	return ps->sum_turnaround;
}

int activeTime(CPUStats* cs) {
	return cs->active_time;
}

int contxtSwTime(CPUStats* cs) {
	return cs->contxt_sw_time;
}

int idleTime(CPUStats* cs) {
	return cs->idle_time;
}
