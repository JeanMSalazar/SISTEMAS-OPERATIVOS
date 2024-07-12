#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define MAXSIZE 128



void die(char *s) {

         perror(s);

          exit(1);

}



struct msgbuf {

        long mtype;

        char mtext[MAXSIZE];

};

int main() {

    int msqid;

    key_t key = 12345;

    struct msgbuf rcvbuffer;



    if ((msqid = msgget(key,IPC_CREAT | 0606)) < 0)

        die("msgget()");



    while (1) {

        // Intentar recibir mensajes de cada tipo par desde 2 hasta 10

        for (int mtype = 11; mtype <= 100; mtype += 1) {

            while (1) {

                int result = msgrcv(msqid, &rcvbuffer, MAXSIZE, mtype, IPC_NOWAIT);

                if (result >= 0) {

                        printf("UNI, read type:%ld, text:%s\n", rcvbuffer.mtype, rcvbuffer.mtext);}

                else {

                    if (errno == ENOMSG) {

                        break;

                    } else {

                        die("msgrcv failed");

                    }

                }

            }

        }

        usleep(50000);

    }

    return 0;

}

