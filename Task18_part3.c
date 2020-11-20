/* Sequential version of Quick sort */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define KILO (1024)
#define MEGA (1024*1024)
#define MAX_ITEMS (64*MEGA)
#define swap(v, a, b) {unsigned tmp; tmp=v[a]; v[a]=v[b]; v[b]=tmp;}

static int *v;

sem_t mutex;
int thread_count;
int THREADS_MAX;

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

static void* 
quick_sort(void *arguments)
{
	struct thread_args *args = arguments;
    struct thread_args send_struct; // each thread need its own memory to send to the next thread
    unsigned pivot_index, low_index, high_index;

    pthread_t thread;
    int thread_error; // error handling
    int thread_comp = 0;

    low_index = args->low;
    high_index = args->high;

    /* no need to sort a vector of zero or one element */
    if (low_index >= high_index){
        return NULL;
    }

    /* select the pivot value */
    pivot_index = (low_index + high_index)/2;


    /* partition the vector */
    pivot_index = partition(args->dataset, args->low, args->high, pivot_index);  
    
    /* semaphores to stop race conditions*/
    sem_wait(&mutex);
    /* if thread_count is less than the maximum allowed number of threads we can set the thread comparison 
        flag(thread_comp) to 1 and tell the program that we are going to create a nother thread.*/
    if(thread_count < THREADS_MAX){thread_comp = 1; thread_count++;}
    sem_post(&mutex);

    send_struct.dataset = args->dataset;
    
    if (thread_comp == 1){
        /*new thread handels the lower portion*/
        send_struct.low = low_index;
        send_struct.high = pivot_index - 1;  

        thread_error = pthread_create(&thread, NULL, quick_sort, &send_struct);                            
        if (thread_error){                          
            printf("ERROR; return code from pthread_create(thread) is %d. Thread count: %d\n", thread_error, thread_count);                            
            exit(-1);                          
        }
        
        /* current thread handles the upper portion */
        args->high = high_index;
        args->low = pivot_index + 1;
        quick_sort(args);
    
        /* wait for the new thread to be finished before going up a level */
        thread_error = pthread_join(thread, NULL);                            
        if (thread_error){                          
            printf("ERROR; return code from pthread_join(thread) is %d. Thread count: %d\n", thread_error, thread_count);                            
            exit(-1);                          
        }
        /* the thread is joined, so the global counter can be decreased*/
        thread_count--;
        
    }
    /* if no new thread is created the current thread runs recursivly */
    else
    {
        if (args->low < pivot_index){
            send_struct.low = low_index;
            send_struct.high = pivot_index - 1; 
            quick_sort(&send_struct);
        }
        if (pivot_index < args->high){
            send_struct.low = pivot_index + 1;
            send_struct.high = high_index; 
            quick_sort(&send_struct);
        } 
    }
    return NULL;
}
/* brute force way to check that the algoritm worked. This is activated by using the third argument*/
static void 
check()
{
    for (unsigned i = 1; i < MAX_ITEMS; i++){
        if (v[i-1] > v[i]){
            printf("Something went wrong!\n");
            return;
        }
    }
    printf("No faults!\n");
    return;
}
int
main(int argc, char *argv[])
{   
    if(argc == 1){
        printf("ERROR: Amount of threads not specified!");
        return 0;
    }
    THREADS_MAX = atoi(argv[1]);

    /* main thread is still a thread */
    thread_count++;

    init_array();
    sem_init(&mutex, 0, 1);

    //print_array();

    struct thread_args sendStruct;
    sendStruct.dataset = v;
	sendStruct.low = 0;
	sendStruct.high = MAX_ITEMS-1;
    void *send_ptr = &sendStruct;
    
    quick_sort(send_ptr);

    //print_array();
    if(argc > 2){
        check();
    }
    
    sem_destroy(&mutex);
    return 0;
}