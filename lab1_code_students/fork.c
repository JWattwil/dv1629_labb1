#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pid_t pid_0;
    pid_t pid_1;
    unsigned i;
    unsigned niterations = 1000;
    pid_0 = fork();
    if (pid_0 == 0){
        for (i = 0; i < niterations; ++i)
            printf("A = %d, ", pid_0);
    } else{
        for (i = 0; i < niterations; ++i)
            printf("B = %d, ", pid_0);
    } 

    unsigned n;
    pid_1 = fork();
    if (pid_1 == 0){
        for (i = 0; n < niterations; ++n)
            printf("A = %d, ", pid_1);
    } else{
        for (i = 0; n < niterations; ++n)
            printf("C = %d, ", pid_1);
    }
    


    printf("\n");
}
