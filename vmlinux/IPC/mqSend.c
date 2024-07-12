#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

void send_message(int msqid, long mtype, char *message)
{
    struct msgbuf sbuf;
    size_t buflen = strlen(message) + 1;

    sbuf.mtype = mtype;
    strcpy(sbuf.mtext, message);

    if (msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT) < 0)
    {
        die("msgsnd");
    }
}

void update_counter(int *counter)
{
    (*counter)++;
}

int main()
{
    int msqid1, msqid2;
    int msgflg = IPC_CREAT | 0666;

    key_t key = 1234;
    key_t key2 = 5678;

    if ((msqid1 = msgget(key, msgflg )) < 0)
      die("msgget");

    if ((msqid2 = msgget(key2, msgflg )) < 0)
      die("msgget");

    srand(getpid());//comprobar para que sirve

    int counter1 = 0;
    int counter2 = 0;

    for (int i = 0; i < 1000; i++)
    {
        int num = rand() % 100 + 1;
        char message[MAXSIZE];

        if (fork() == 0)
        {
            if (num <= 10 && num % 2 == 0)
            {
                sprintf(message, "soy hijo y genere un RND %d", num);
                printf("Hijo: %s\n", message);
                send_message(msqid1, num, message);
                update_counter(&counter1);
            }
            else if (num <= 10 && num % 2 != 0)
            {
                sprintf(message, "soy hijo y genere un RND %d", num);
                printf("Hijo: %s\n", message);
                send_message(msqid1, num, message);
                update_counter(&counter1);
            }
            else
            {
                sprintf(message, "soy hijo y genere un RND %d", num);
                printf("Hijo: %s\n", message);
                send_message(msqid2, num, message);
                update_counter(&counter2);
            }

            exit(0);
        }

        if (num <= 10 && num % 2 == 0)
        {
            sprintf(message, "soy papa y genere un RND %d", num);
            printf("Padre: %s\n", message);
            send_message(msqid1, num, message);
            update_counter(&counter1);
        }
        else if (num <= 10 && num % 2 != 0)
        {
            sprintf(message, "soy papa y genere un RND %d", num);
            printf("Padre: %s\n", message);
            send_message(msqid1, num, message);
            update_counter(&counter1);
        }
        else
        {
            sprintf(message, "soy papa y genere un RND %d", num);
            printf("Padre: %s\n", message);
            send_message(msqid2, num, message);
            update_counter(&counter2);
        }

        sleep(1);
    }

     printf("Cola 1: Total de mensajes enviados: %d\n", counter1);
    printf("Cola 2: Total de mensajes enviados: %d\n", counter2);

    exit(0);


    /*los usuarios tienen que estar agrupados o en un grupo
      crear dos colas una para recivir datos menores a 10 y otra para los mayores a 10, cada grupo debe pertenecer a una u otra cola 
      es decir crear dos de estas: 
    int msqid;
    int msgflg = IPC_CREAT | 0666;

    una para cada cola

    ipcrm -q;
    */
}
