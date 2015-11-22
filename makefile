all: sim

sim: sim.c PQQ/pqueue.c PQQ/event.c PQQ/queue.c simStats.c PQQ/process.c
	gcc -o sim sim.c PQQ/pqueue.c PQQ/event.c PQQ/queue.c simStats.c PQQ/process.c -I.
