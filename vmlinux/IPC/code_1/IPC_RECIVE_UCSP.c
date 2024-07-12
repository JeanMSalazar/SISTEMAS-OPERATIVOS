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
    key_t key_ucsp = 1234;
    int msqid_ucsp;
    struct msgbuf rcvbuffer;

    // Crear la cola de mensajes con permisos para el usuario y grupo
    if ((msqid_ucsp = msgget(key_ucsp, IPC_CREAT | 0660)) < 0)
        die("msgget()");

    while (1)
    {
        for (int mtype = 1; mtype <= 10; mtype += 2)
        {
            while (1)
            {
                int result = msgrcv(msqid_ucsp, &rcvbuffer, MAXSIZE, mtype, IPC_NOWAIT);
                if (result >= 0)
                {
                    printf("UCSP read type message > %ld and text: %s\n", rcvbuffer.mtype, rcvbuffer.mtext);
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