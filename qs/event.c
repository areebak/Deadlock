// event.c
// Areeba Kamal
// Helena Kleinschmidt

#include "event.h"

Event* createEvent(Process* p, int resrc, int timestamp, int type) {
	Event* ev = malloc(sizeof(Event));
	ev->p = p;
	ev->resrc = resrc;
	ev->timestamp = timestamp;
	ev->type = type;
	return ev;
}

// int eventString(Event* ev) {
// 	printf("EVENT\tProcess %p\t %d(%d)", ev->p, ev->timestamp, ev->type);
// 	return 0;
// }
