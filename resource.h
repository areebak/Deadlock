// resource.h
// Areeba Kamal
// Helena Kleinschmidt

#include "qs/queue.c"

typedef struct Resource {
	int type;
	int total_inst;
	int available;
	Queue* request_q; // processes waiting to acquire the resource
} Resource;
