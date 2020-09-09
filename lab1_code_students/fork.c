#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pid_t pid_0;
    pid_t pid_1;
    unsigned i;
    unsigned niterations = 1000;
    pid_0 = fork();
    pid_1 = fork();
    for (i = 0; i < niterations; ++i){

        if (pid_0 == 0){ //Child
            printf("Child A = %d and pidID = %d \n", i,  getpid());

        }
        else {

            if(pid_1 == 0){
                printf("Child B = %d and pidID = %d \n", i, getpid()); 
            }

            else{
                printf("P = %d and pidID = %d \n", i, getpid()); 
            }
        }

    
    }  

    printf("\n");
}
