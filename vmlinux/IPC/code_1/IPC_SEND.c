#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MAXSIZE 128

void die(char*);
void sign_handler_INT(int);

int flag=0;
int loop_child=0;
int loop_parent=0;

int flag_p, flag_child;

struct msgbuf {
	long mtype;
	char mtext[MAXSIZE];
};

int msqid_c, msqid_p;
int msflg_c, msflg_p;
size_t buflen_c, buflen_p;
key_t key_c, key_p;
int val_rand_c, val_rand_p;
struct msgbuf sbuf_c;
struct msgbuf sbuf_p;

void send_message(int msqid, struct msgbuf *sbuf, int val_rand, key_t key, int msflg, size_t *buflen)
{
    struct msqid_ds msqid_ds;

    *sbuf = (struct msgbuf){val_rand, ""};
    *buflen = strlen(sbuf->mtext) + 1;

    if (val_rand < 11) {
        key = 1234;
        msflg = IPC_CREAT | 0660;
    } else {
        key = 12345;
        msflg = IPC_CREAT | 0606;
    }

    if ((msqid = msgget(key, msflg)) < 0) {
        die("msgget");
    }

    if (msgctl(msqid, IPC_STAT, &msqid_ds) < 0) {
        die("msgctl(IPC_STAT)");
    }

    if (val_rand < 11) {
        msqid_ds.msg_perm.gid = 1004;
    } else {
        msqid_ds.msg_perm.uid = 1002;
    }

    if (msgctl(msqid, IPC_SET, &msqid_ds) < 0) {
        die("msgctl(IPC_SET)");
    }
}

void handle_child()
{
    while (loop_child < 1000) {
        send_message(msqid_c, &sbuf_c, val_rand_c, key_c, msflg_c, &buflen_c);
        sprintf(sbuf_c.mtext, "\nCHP, envio exitoso %d\n\n", val_rand_c);

        int retries = 5;
        while (retries > 0) {
            if (msgsnd(msqid_c, &sbuf_c, buflen_c, IPC_NOWAIT) < 0) {
                if (errno == EAGAIN) {
                    retries -= 1;
                    usleep(5000);
                    continue;
                }
                die("msgsnd");
            } else {
                printf("CHP msqid enviado exitosamente\n\n");
                break;
            }
        }

        loop_child += 1;
    }

    printf("\n\nCHp fin del loop 1000\n\n");
    exit(0);
}

void handle_parent()
{
    while (loop_parent < 1000) {
        send_message(msqid_p, &sbuf_p, val_rand_p, key_p, msflg_p, &buflen_p);
        sprintf(sbuf_p.mtext, "\nPP: value: %d", val_rand_p);

        int retries = 5;
        while (retries > 0) {
            if (msgsnd(msqid_p, &sbuf_p, buflen_p, IPC_NOWAIT) < 0) {
                if (errno == EAGAIN) {
                    retries -= 1;
                    usleep(5000);
                    continue;
                }
                die("msgsnd");
            } else {
                printf("PP:\n Mensajes enviados con exito\n\n");
                break;
            }
        }

        loop_parent++;
    }

    printf("\n\nPP Fin del loop 1000\n\n");
    exit(0);
}

int main(int argv, int **argc)
{
    signal(SIGINT, sign_handler_INT);
    time_t t;
    srand((unsigned)time(&t));
    pid_t child_pid = fork();

    while (1) {
        sleep(2);
        printf("waiting signal\n");
        if (flag) {
            flag = 0;
            if (child_pid == 0) {
                handle_child();
            } else {
                handle_parent();
            }
        }
        usleep(5000);
    }

    exit(0);
}

void die(char *s)
{
    perror(s);
    exit(1);
}

void sign_handler_INT(int signum)
{
    flag = 1;
}