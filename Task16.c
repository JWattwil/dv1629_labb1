/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

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
    case 0: /* Initilaze matrix A*/
        for (i = 0; i < SIZE; i++)
            for (j = 0; j < SIZE; j++) {
                a[i][j] = 1.0;
            }
        break;
    case 1: /* Initilaze matrix B*/
        for (i = 0; i < SIZE; i++)
            for (j = 0; j < SIZE; j++) {
                b[i][j] = 1.0;
            }
        break;
    default:
        break;
    }
    return NULL;
}


void* matmul_seq(void* id)
{
    unsigned i = (unsigned)id;
    int j, k;

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
    rows = malloc(SIZE * sizeof(pthread_t));

    //Matix init
    for (int id = 0; id < 2; id++)
        pthread_create(&(rows[id]), NULL, init_matrix, (void*)id);

    for (int id = 0; id < 2; id++)
        pthread_join(rows[id], NULL);

    //Matix Calc
    for (int id = 0; id < SIZE; id++)
        pthread_create(&(rows[id]), NULL, matmul_seq, (void*)id);

    for (int id = 0; id < SIZE; id++)
        pthread_join(rows[id], NULL);

    //print_matrix();
}
