#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

const char *names[5];
char chopsticks[5][5];

pthread_mutex_t arbirator;

int 
getLeft(int id){
    switch (id)
    {
    case 0:
        return 4;
    case 1:
        return 0;
    case 2:
        return 1;
    case 3:
        return 2;
    case 4:
        return 3;
    default:
        return -1;
    }
}
int 
getRight(int id){
    switch (id)
    {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 4;
    case 4:
        return 0;
    default:
        return -1;
    }
}
void 
*action (void *arg){
    unsigned long int id = (unsigned long int) arg;
    
    while(1) {
        sleep(rand() %10 + 2); //Thinking
        if((chopsticks[id][getLeft(id)] == 0 && chopsticks[getLeft(id)][id] == 0) && (chopsticks[id][getRight(id)] == 0 && chopsticks[getRight(id)][id] == 0)){
            pthread_mutex_unlock(&arbirator);

            chopsticks[id][getLeft(id)] = names[id][2];
            chopsticks[getLeft(id)][id] = names[id][2];
            chopsticks[id][getRight(id)] = names[id][2];
            chopsticks[getRight(id)][id] = names[id][2];
            printf("%s has both chopsticks\n", names[id]);

            pthread_mutex_unlock(&arbirator);

            printf("%s starts eating\n", names[id]);
            sleep(rand() %20 + 10); //Eating

            // Dropp right chopstick
            chopsticks[id][getRight(id)] = 0;
            chopsticks[getRight(id)][id] = 0;
            // Dropp left chopstick
            chopsticks[id][getLeft(id)] = 0;
            chopsticks[getLeft(id)][id] = 0;
            printf("%s dropped the chopsticks\n", names[id]);
        }        
    }  
}

int 
main()
{
    printf("Seating professors...\n");
    unsigned int nProf = 5;
    unsigned long int id;
    pthread_t *professors;
    professors = malloc( nProf * sizeof(pthread_t) );
    
    if (pthread_mutex_init(&arbirator, NULL) != 0)
    {
        printf("\n ERROR: mutex init failed\n");
        exit(-1);
    }

    names[0] = "Tanenbaum";
    names[1] = "Bos";
    names[2] = "Lamport";
    names[3] = "Stallings";
    names[4] = "Sillberschartz";

    for (id = 1; id < nProf; id++){
        pthread_create(&(professors[id]), NULL, action, (void*)id);
    }
    action(0);

    return 0;
}