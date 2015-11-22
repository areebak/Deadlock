// resource.c
// Areeba Kamal
// Helena Kleinschmidt

typedef struct Resource {
	int id;
}

// Every resource in the system will maintain a FIFO queue, 
// called the request queue, of processes waiting to acquire the resource.
