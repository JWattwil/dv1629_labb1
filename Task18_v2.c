/* Sequential version of Quick sort */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define KILO (1024)
#define MEGA (1024*1024)
#define MAX_ITEMS (64*MEGA)
#define swap(v, a, b) {unsigned tmp; tmp=v[a]; v[a]=v[b]; v[b]=tmp;}

static int *v;

struct thread_args
{
	int *arg1;
	unsigned low;
	unsigned high;
    pthread_t *threads;
    int next_thread;
};


static void
print_array(void)
{
    int i;
    for (i = 0; i < MAX_ITEMS; i++)
        printf("%d ", v[i]);
    printf("\n");
}

static void
init_array(void)
{
    int i;
    v = (int *) malloc(MAX_ITEMS*sizeof(int));
    for (i = 0; i < MAX_ITEMS; i++)
        v[i] = rand();
}

static unsigned
partition(int *v, unsigned low, unsigned high, unsigned pivot_index)
{
    /* move pivot to the bottom of the vector */
    if (pivot_index != low)
        swap(v, low, pivot_index);

    pivot_index = low;
    low++;

    /* invariant:
     * v[i] for i less than low are less than or equal to pivot
     * v[i] for i greater than high are greater than pivot
     */

    /* move elements into place */
    while (low <= high) {
        if (v[low] <= v[pivot_index])
            low++;
        else if (v[high] > v[pivot_index])
            high--;
        else
            swap(v, low, high);
    }

    /* put pivot back between two groups */
    if (high != pivot_index)
        swap(v, pivot_index, high);
    return high;
}

void* quick_sort(void *arguments)//(int *v, unsigned low, unsigned high)
{
	pthread_t thread;
	struct thread_args *args = arguments;
    unsigned pivot_index;

    /* no need to sort a vector of zero or one element */
    if (args->low >= args->high)
        return;

    /* select the pivot value */
    pivot_index = (args->low + args->high)/2;

    /* partition the vector */
    pivot_index = partition(v, args->low, args->high, pivot_index);

	struct thread_args sendArg;
    sendArg.arg1 = v;
    sendArg.threads = args->threads;
    sendArg.next_thread = (args->next_thread+1)%8;

	void *toSend;
    /* sort the two sub arrays */
    if (args->low < pivot_index){
        sendArg.low = args->low;
		sendArg.high = pivot_index-1;
		toSend = &sendArg;
        pthread_create(args->threads[args->next_thread], NULL, quick_sort, toSend); //quick_sort(v, low, pivot_index-1);
    }
    if (pivot_index < args->high){
        sendArg.low = pivot_index+1;
		sendArg.high = args->high;
		toSend = &sendArg;
        pthread_create(args->threads[args->next_thread], NULL, quick_sort, toSend); //quick_sort(v, pivot_index+1, high);
    }
	pthread_join(thread, NULL);
    return;
}

int
main(int argc, char **argv)
{
    init_array();
    //print_array();

    pthread_t *threads;
    threads = malloc(sizeof(pthread_t) * 8);
	
    struct thread_args sendArg;
    sendArg.arg1 = v;
	sendArg.low = 0;
	sendArg.high = MAX_ITEMS-1;
    sendArg.threads = threads;
    sendArg.next_thread = 0;

    void *toSend;
	toSend = &sendArg;

    quick_sort(toSend);
    //print_array();
}
