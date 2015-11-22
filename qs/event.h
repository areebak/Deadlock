// event.h
// Areeba Kamal
// Helena Kleinschmidt

#ifndef EVENT_H
#define EVENT_H
#include "process.h"

typedef struct Event {
	Process* p;
	int resrc; // if event involves resource, resrc is index of resource in RESRCS, else -1
	int timestamp;
	int type; // can be 0 (indicates event to create process)
		  // 	 or 1 (indicates event that terminate process)
		  // 	 or 2 (indicates event that kills process prematurely)
		  //	 or 3 (indicates event that will let process request resource)
		  // 	 or 4 (indicates event that will let process release resource)
} Event;

Event* createEvent(Process* p, int resrc, int timestamp, int type);
// int    eventString(Event* ev);

#endif
