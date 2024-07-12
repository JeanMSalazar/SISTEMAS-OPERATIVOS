#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // Incluir para funciones como sleep
#include <errno.h>

#define MAXSIZE 128

void die(char *s) {
    perror(s);
    exit(1);
}

struct msgbuf {
    long mtype;          // Tipo de mensaje, usado para filtrar los mensajes pares
    char mtext[MAXSIZE]; // Mensaje recibido
};

int main() {
    int msqid;
    key_t key = 1234;
    struct msgbuf rcvbuffer;

    if ((msqid = msgget(key, 0660)) < 0)
        die("msgget()");

    while (1) {
        // Intentar recibir mensajes de cada tipo par desde 2 hasta 10
        for (int mtype = 2; mtype <= 10; mtype += 2) {
            while (1) {
                int result = msgrcv(msqid, &rcvbuffer, MAXSIZE, mtype, IPC_NOWAIT);
                if (result >= 0) {
                    printf("CS read type message> %ld and text: %s\n", rcvbuffer.mtype, rcvbuffer.mtext);
                } else {
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
