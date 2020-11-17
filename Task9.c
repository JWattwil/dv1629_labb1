#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct threadArgs {
	unsigned int id;
	unsigned int numThreads;
	int squaredID;
};

void* child(void* params) {
	
	struct threadArgs *args = (struct threadArgs*) params;
	unsigned int childID = args->id;
	unsigned int numThreads = args->numThreads;
	args->squaredID = childID * childID;
	printf("Greetings from child #%u of %u\n", childID, numThreads);
	return args;
}

int main(int argc, char** argv) {
	pthread_t* children; // dynamic array of child threads
	struct threadArgs* args; // argument buffer
	unsigned int numThreads = 0;
	void *result;
	unsigned int *resultArray;
	// get desired # of threads
	if (argc > 1){
		numThreads = atoi(argv[1]);
	}
	resultArray = calloc(numThreads, sizeof(unsigned int));
	children = malloc(numThreads * sizeof(pthread_t)); // allocate array of handles
	for (unsigned int id = 0; id < numThreads; id++) {
		// create threads
		args = malloc(sizeof(struct threadArgs));
		args->id = id;
		args->numThreads = numThreads;
		pthread_create(&(children[id]), // our handle for the child
			NULL, // attributes of the child
			child, // the function it should run
			(void*)args); // args to that function
	}
	printf("I am the parent (main) thread.\n");
	for (unsigned int id = 0; id < numThreads; id++) {
		pthread_join(children[id], (void**)&args );
		*(resultArray + id) = args->squaredID;
	}
	free(children); // deallocate array
	printf("----------------------------------------------------------------------\n");
	for (unsigned int i = 0; i < numThreads; i++){
		printf("Thread ID: %d, square: %d\n" , i, *(resultArray + i));
	}
	return 0;
}
