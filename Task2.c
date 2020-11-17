#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <pthread.h>
#include <fcntl.h> /* For O_* constants */

#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

struct shm_struct {
	int buffer [10];
	unsigned counter;
};

unsigned remove_from_buffer(volatile struct shm_struct *shmp);

void add_to_buffer(volatile struct shm_struct *shmp, int element);

int main(int argc, char **argv)
{
	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	
	unsigned var1 = 0, var2 = 0;
	int shmid = -1;
	struct shmid_ds *shm_buf;

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);

	shmp->counter = 0; //Initalize the counter

	pid_t pid = fork();

	if (pid == 0) {
		/* here's the child, acting as consumer */
		while (var2 != 100) {
			/* read from shmem */
			var2 = remove_from_buffer(shmp);
			printf("Received %d\n", var2); fflush(stdout);
		}

		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	}
	else{
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
			var1++;	
			printf("Sending %d\n", var1); fflush(stdout);
			add_to_buffer(shmp, var1);
		}
		/*if the child never gets runtime the buffer will be overridden without the chiled ever reading it*/
		usleep(rand() % 1901 + 100);
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	}
}

/*Becasue of the use of a circular buffer we dont need to worry about the buffer being full because 
we will just write over the existing data. We dont remove any data so we do not need to worry about 
removing a number from an empty buffer*/
unsigned remove_from_buffer(volatile struct shm_struct *shmp){
	unsigned ret;
	if(shmp->counter != 0){
		ret = shmp->buffer[shmp->counter - 1];
		shmp->buffer[shmp->counter - 1] = 0;
	}
	else{ ret = shmp->buffer[9]; }
	return ret;
}
void add_to_buffer(volatile struct shm_struct *shmp, int element){
	shmp->buffer[shmp->counter] = element;
	shmp->counter++;
	if(shmp->counter == 10){ shmp->counter = 0; }; //circular buffer. if we have reached the end we start from the begining
}
