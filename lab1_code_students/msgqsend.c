#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   int int_msg;
};

int main(void) {
   struct my_msgbuf buf;
   int msqid;
   int len;
   key_t key;
   char input_str [255];
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
   printf("Enter lines of text, %d\ to quit:\n", 0);
   buf.mtype = 1; /* we don't really care in this case */

   while(fgets(input_str, 255, stdin) != NULL) {
      if( strcmp(input_str, "0") != 0){
         buf.int_msg = atoi(input_str); //Makes input into int
         memset(input_str, 0, sizeof(input_str)); //Clears the input string
      
         if(msgsnd(msqid, &buf, sizeof(buf.int_msg), 0) == -1)
            perror("msgsnd");
      }
      else
         break;      

   }
   buf.int_msg = 0;
   if(msgsnd(msqid, &buf, sizeof(buf.int_msg), 0) == -1)
      perror("msgsnd");

   if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
   }

   printf("message queue: done sending messages.\n");
   return 0;
}
