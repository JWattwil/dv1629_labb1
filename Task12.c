#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

const char *names[5];
char chopsticks[5][5];

int getLeft(int id){
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
int getRight(int id){
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

void *action (void *arg){
    unsigned long int id = (unsigned long int) arg;
    
    while(1) {
        sleep(rand() %10 + 2);  //Thinking
        printf("%s goes for the left chopstick...\n", names[id]);
        if(chopsticks[id][getLeft(id)] == 0 && chopsticks[getLeft(id)][id] == 0){
            chopsticks[id][getLeft(id)] = names[id][2];
            chopsticks[getLeft(id)][id] = names[id][2];
            printf("%s is thinking -> Got left chopstick\n", names[id]);
            sleep(rand() %3 + 1);   //Thinking
        }
        if(chopsticks[id][getLeft(id)] == names[id][2] && chopsticks[getLeft(id)][id] == names[id][2]){
            printf("%s goes for the right chopstick...\n", names[id]);
            if(chopsticks[id][getRight(id)] == 0 && chopsticks[getRight(id)][id] == 0){
                chopsticks[id][getRight(id)] = names[id][2];
                chopsticks[getRight(id)][id] = names[id][2];
                printf("%s is eating -> Has both chopsticks\n", names[id]);

                sleep(rand() %20 + 10);     //Eating

                // Dropp right chopstick
                chopsticks[id][getRight(id)] = 0;
                chopsticks[getRight(id)][id] = 0;
                printf("%s dropped the right chopstick\n", names[id]);

                // Dropp left chopstick
                chopsticks[id][getLeft(id)] = 0;
                chopsticks[getLeft(id)][id] = 0;
                printf("%s dropped the left chopstick\n", names[id]);
            }
        }
    }  
}

int main()
{
    printf("Seating professors...\n");
    unsigned int nProf = 5;
    unsigned long int id;
    pthread_t *professors;
    professors = malloc( nProf * sizeof(pthread_t) );

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