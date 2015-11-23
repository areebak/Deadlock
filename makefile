all: sim

sim: sim.c qs/pqueue.c qs/event.c qs/queue.c simStats.c qs/process.c
	gcc -o sim sim.c qs/pqueue.c qs/event.c qs/queue.c simStats.c qs/process.c -I.
