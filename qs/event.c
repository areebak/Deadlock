// event.c
// Thao Bach
// Helena Kleinschmidt
// Aida Zhumabekova

#include "event.h"

Event* createEvent(Process* p, int timestamp, int type) {
	Event* ev = malloc(sizeof(Event));
	ev->p = p;
	ev->timestamp = timestamp;
	ev->type = type;
	return ev;
}

int eventString(Event* ev) {
	printf("EVENT\tProcess %p\t %d(%d)", ev->p, ev->timestamp, ev->type);
	return 0;
}
