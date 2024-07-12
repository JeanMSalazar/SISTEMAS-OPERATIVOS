#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define MAXSIZE 128

void die(char *s);

struct msgbuf
{
    long mtype;
    char mtext[MAXSIZE];
};

int main()
{
    key_t key_uni = 12345;
    int msqid_uni;
    struct msgbuf rcvbuffer;

    // Crear la cola de mensajes con permisos para el usuario y grupo
    if ((msqid_uni = msgget(key_uni, IPC_CREAT | 0660)) < 0)
        die("msgget()");

    while (1)
    {
        while (1)
        {
            int result = msgrcv(msqid_uni, &rcvbuffer, MAXSIZE, 0, IPC_NOWAIT);
            if (result >= 0)
            {
                printf("UNI read type message > %ld and text: %s\n", rcvbuffer.mtype, rcvbuffer.mtext);
            }
            else
            {
                if (errno == ENOMSG)
                {
                    break;
                }
                else
                {
                    die("msgrcv failed");
                }
            }
        }

        usleep(50000);
    }

    return 0;
}

void die(char *s)
{
    perror(s);
    exit(1);
}