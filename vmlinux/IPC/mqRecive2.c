#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXSIZE     128

void die(char *s)
{
  perror(s);
  exit(1);
}

struct msgbuf
{
    long    mtype;
    char    mtext[MAXSIZE];
};

void receive_message(int msqid, long mtype, int *counter)
{
    struct msgbuf rcvbuffer;

    if (msgrcv(msqid, &rcvbuffer, MAXSIZE, mtype, 0) < 0)
      die("msgrcv");

    printf("%s (mtype: %ld)\n", rcvbuffer.mtext, rcvbuffer.mtype);
    (*counter)++;
}

int main()
{
    int msqid1, msqid2;
    key_t key = 1234;
    key_t key2 = 5678;

    if ((msqid1 = msgget(key, 0666)) < 0)
      die("msgget()");

    if ((msqid2 = msgget(key2, 0666)) < 0)
      die("msgget()");

    int counter_cs = 0;
    int counter_ucsp = 0;
    int counter_uni = 0;

    for (int i = 0; i < 1000; i++)
    {
        receive_message(msqid1, i % 10 + 1, &counter_cs);
        receive_message(msqid1, i % 10 + 1, &counter_ucsp);
        receive_message(msqid2, i % 100 + 11, &counter_uni);
    }

    printf("cs: Total de mensajes recibidos: %d\n", counter_cs);
    printf("ucsp: Total de mensajes recibidos: %d\n", counter_ucsp);
    printf("uni: Total de mensajes recibidos: %d\n", counter_uni);

    int total = counter_cs + counter_ucsp + counter_uni;

    if (total == 2000)
    {
        printf("Total de mensajes recibidos: %d\n", total);
        printf("Todo correcto!\n");
    }
    else
    {
        printf("Total de mensajes recibidos: %d\n", total);
        printf("Error en el total de mensajes recibidos!\n");
    }

    exit(0);
}
