// event.h
// Thao Bach
// Helena Kleinschmidt
// Aida Zhumabekova

#ifndef EVENT_H
#define EVENT_H
#include "process.h"

typedef struct Event {
	int type; // can be 0 (indicates event to create process and put into ready queue)
			  // 	 or 1 (indicates event that will cause process to leave system)
			  // 	 or 2 (indicates event that will cause process to leave for io service)
			  // 	 or 3 (indicates event that will return process to ready queue)
	Process* p;
	int timestamp;
} Event;

Event* createEvent(Process* p, int timestamp, int type);
int eventString(Event* ev);

#endif
