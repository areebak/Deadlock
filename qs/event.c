// event.c
// Areeba Kamal
// Helena Kleinschmidt

#include "event.h"

Event* createEvent(Process* proc, int timestamp, int type) {
	Event* ev = malloc(sizeof(Event));
	ev->proc = proc;
	ev->timestamp = timestamp;
	ev->type = type;
	return ev;
}

// int eventString(Event* ev) {
// 	printf("EVENT\tProcess %p\t %d(%d)", ev->p, ev->timestamp, ev->type);
// 	return 0;
// }
