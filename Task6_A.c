#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644

/*stuct to save to file so that Task6_B.c can read it from msgq.txt*/
struct my_msgbuf {
   long mtype;
   int int_msg;
};

int main(void) {
   struct my_msgbuf buf;
   int msqid;
   key_t key;
   char input_str [255];

   /*creating file that Task6_B.c can read from*/
   system("touch msgq.txt");

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }

   if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) {
      perror("msgget");
      exit(1);
   }
   printf("message queue: ready to send messages.\n");
   printf("Enter lines of text, \"%d\" to quit:\n", 0);
   buf.mtype = 1; // we don't really care in this case 

   while(fgets(input_str, 255, stdin) != NULL) {
      if( strcmp(input_str, "0") != 0){
         buf.int_msg = atoi(input_str); // makes input into int
         memset(input_str, 0, sizeof(input_str)); // clears the input string
      
         if(msgsnd(msqid, &buf, sizeof(buf.int_msg), 0) == -1) // saving the reference to the struct in memory to shared memory (file)
            perror("msgsnd");
      }
      else
         break;      

   }
   buf.int_msg = 0;
   if(msgsnd(msqid, &buf, sizeof(buf.int_msg), 0) == -1) // stores the exit command to the shared memory(file)
      perror("msgsnd");

   if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
   }

   printf("message queue: done sending messages.\n");
   return 0;
}
