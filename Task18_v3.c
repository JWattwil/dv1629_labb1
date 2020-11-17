/* Sequential version of Quick sort */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define KILO (4)//(1024)
#define MEGA (4*4)//(1024*1024)
#define MAX_ITEMS (64*MEGA)
#define swap(v, a, b) {unsigned tmp; tmp=v[a]; v[a]=v[b]; v[b]=tmp;}

static int *v;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct 
thread_args
{
    int *dataset;
	unsigned low;
	unsigned high;
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

void* 
quick_sort(void *arguments)//(int *v, unsigned low, unsigned high)
{
	
	struct thread_args *args = arguments;
    struct thread_args send_struct;
    unsigned pivot_index;

    pthread_t thread;
    char low_exist, high_exist;
    int thread_error;

    /* no need to sort a vector of zero or one element */
    if (args->low >= args->high)
        return NULL;

    /* select the pivot value */
    pivot_index = (args->low + args->high)/2;

    
    /* partition the vector */
    pivot_index = partition(args->dataset, args->low, args->high, pivot_index);

    
    /*new thread handels the lower portion*/
    send_struct.dataset = args->dataset;
    send_struct.low = args->low;
    send_struct.high = pivot_index-1;    
    
    thread_error = pthread_create(&thread, NULL, quick_sort, &send_struct);                            
    if (thread_error){                          
        printf("ERROR; return code from pthread_create(thread) is %d\n", thread_error);                            
        exit(-1);                          
    }

    /*current thread handles the upper portion*/
    args->low = pivot_index + 1;
    quick_sort(args);

    /*wait for the new thread to be finished before going up a level*/
    thread_error = pthread_join(thread, NULL);                            
        if (thread_error){                          
            printf("ERROR; return code from pthread_join(thread) is %d\n", thread_error);                            
            exit(-1);                          
        }

    return NULL;
}

int
main(int argc, char **argv)
{
    init_array();
    //print_array();
	
    struct thread_args sendStruct;
	void *send_ptr;
    sendStruct.dataset = v;
	sendStruct.low = 0;
	sendStruct.high = MAX_ITEMS-1;
	send_ptr = &sendStruct;
    quick_sort(send_ptr);

    print_array();
    return 0;
}
