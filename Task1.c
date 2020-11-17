#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pid_t pid_0;
    pid_t pid_1;
    unsigned i;
    unsigned n_iterations = 1000;
    pid_0 = fork(); // Fork for first process
    pid_1 = fork(); // Fork for second process
    for (i = 0; i < n_iterations; ++i){

        if (pid_0 == 0){ //Runs first process
            printf("Child A = %d and pidID = %d \n", i,  getpid());

        }
        if(pid_1 == 0){ // Runs second process
            printf("Child B = %d and pidID = %d \n", i, getpid()); 
        }
        if(pid_0 != 0 || pid_1 != 0){ //runs third (Parent) process
            printf("P = %d and pidID = %d \n", i, getpid()); 
        }

    
    }  
}
