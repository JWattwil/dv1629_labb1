/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define PROCESSORS 4
#define SIZE 1024

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

void* init_matrix(void* id)
{
    unsigned sel = (unsigned)id;
    int i, j;
    switch (sel)
    {
    case 0:
        for (i = 0; i < SIZE; i++)
            for (j = 0; j < SIZE; j++) {
                /* Simple initialization, which enables us to easy check
                * the correct answer. Each element in c will have the same
                * value as SIZE after the matmul operation.
                */
                a[i][j] = 1.0;
            }
        break;
    case 1:
        for (i = 0; i < SIZE; i++)
            for (j = 0; j < SIZE; j++) {
                /* Simple initialization, which enables us to easy check
                * the correct answer. Each element in c will have the same
                * value as SIZE after the matmul operation.
                */
                b[i][j] = 1.0;
            }
        break;

    default:
        break;
    }

    return NULL;
}


void* 
matmul_seq(void *thread_id)
{
    unsigned id = (unsigned)thread_id;
    int j, k;
	for (int i = 256*(id); i < 256*(id+1); i++)
		for (j = 0; j < SIZE; j++) {
			c[i][j] = 0.0;
			for (k = 0; k < SIZE; k++)
				c[i][j] = c[i][j] + a[i][k] * b[k][j];
		}
    
    return NULL;
}

static void
print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++)
	        printf(" %7.2f", c[i][j]);
	    printf("\n");
    }
}

int
main(int argc, char **argv)
{   
    pthread_t *rows;
    rows = malloc(PROCESSORS * sizeof(pthread_t));
	
    //Matix init
    for (int id = 0; id < 2; id++)
        pthread_create(&(rows[id]), NULL, init_matrix, (void*)id);

    for (int id = 0; id < 2; id++)
        pthread_join(rows[id], NULL);

    for (int id = 0; id < PROCESSORS; id++)
        pthread_create(&(rows[id]), NULL, matmul_seq, (void*)id);

    for (int id = 0; id < PROCESSORS; id++)
        pthread_join(rows[id], NULL);

    //print_matrix();
}
