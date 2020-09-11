#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h> /* For O_* constants */

#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200


const char *semName1 = "my_sema1";
const char *semName2 = "my_sema2";


int main(int argc, char **argv)
{
	struct shm_struct {
		int buffer [10];
		unsigned counter;
	};
	

	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	
	int var1 = 0, var2 = 0, shmid = -1;
	struct shmid_ds *shm_buf;

	sem_t *sem_id1 = sem_open(semName1, O_CREAT, O_RDWR, 1);
	sem_t *sem_id2 = sem_open(semName2, O_CREAT, O_RDWR, 0);

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);

	shmp->counter = 0;

	pid_t pid = fork();;

	int remove_from_buffer( void ){
			int temp;
			if(shmp->counter != 0){
				temp = shmp->buffer[shmp->counter - 1];
				shmp->buffer[shmp->counter - 1] = 0;
			}
			else
			{
				temp = shmp->buffer[9];
				shmp->buffer[9] = 0;
			}
			
			return temp;
		}

	void add_to_buffer(int element){
		shmp->buffer[shmp->counter] = element;
		shmp->counter++;
		if(shmp->counter == 10){shmp->counter = 0;};
	}


	if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
			var1++;

			sem_wait(sem_id1);
			printf("Sending %d\n", var1); fflush(stdout);
			add_to_buffer(var1);

			sem_post(sem_id2);	
		}

		sem_close(sem_id1);
		sem_close(sem_id2);
		wait(&status);
		sem_unlink(semName1);
		sem_unlink(semName2);

		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100) {
			/* read from shmem */
			sem_wait(sem_id2);
			var2 = remove_from_buffer();
			printf("Received %d\n", var2); fflush(stdout);
			sem_post(sem_id1);
			
		}
		sem_close(sem_id1);
		sem_close(sem_id2);

		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	}
}
