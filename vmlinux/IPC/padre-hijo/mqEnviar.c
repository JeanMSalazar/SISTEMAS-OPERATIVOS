
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <srand.h>	//	rand(), srand(time(NULL))
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

int flag_p;
int flag_child;

struct msgbuf
{
	long mtype;
	char mtext[MAXSIZE];
};

int msqid_c;
int msqid_p;

int msflg_c;
int msflg_p;

size_t buflen_c;
size_t buflen_p;

key_t key_c;
key_t key_p;

int val_rand_c;
int val_rand_p;

struct msgbuf sbuf_c;
struct msgbuf sbuf_p;

int main(int argv,int **argc)
{
	signal(SIGINT,sign_handler_INT);
	time_t t;
	srand((unsigned)time(&t));
	pid_t child_pid;
	flag_p=0;
	flag_child=0;
	child_pid=fork();
	while(1)
	{
		sleep(2);
		printf("Waiting for the signal INT\n");
		if(flag)
		{
			flag=0;
			if(child_pid==0)
			{
				while(loop_child<1000)
				{
					val_rand_c=rand()%100+1;
					struct msqid_ds msqid_ds_c;
					printf("CHp generate rand number > %d\n",val_rand_c);
					if(val_rand_c<11)
					{
						key_c=1234;
						msflg_c=IPC_CREAT|0660;
						if((msqid_c=msgget(key_c,msflg_c))<0)
						{
							die("msgget");
						}
						if(msgctl(msqid_c,IPC_STAT,&msqid_ds_c)<0)
						{
							die("msgctl(IPC_STAT)");
						}
						msqid_ds_c.msg_perm.gid=1004;
						if(msgctl(msqid_c,IPC_SET,&msqid_ds_c))
						{
							die("msgctl(IPC_SET)");
						}
					}
					else
					{
						msflg_c=IPC_CREAT|0606;
						key_c=12345;
						if((msqid_c=msgget(key_c,msflg_c))<0)
						{
							die("msgget");
						}
						if(msgctl(msqid_c,IPC_STAT,&msqid_ds_c)<0)
						{
							die("msgctl(IPC_STAT)");
						}
						msqid_ds_c.msg_perm.uid=1002;
						if(msgctl(msqid_c,IPC_SET,&msqid_ds_c)<0)
						{
							die("msgctl(IPC_SET)");
						}
					}
					sprintf(sbuf_c.mtext,"CHp, gen rand val %d",val_rand_c);
					sbuf_c.mtype=val_rand_c;
					buflen_c=strlen(sbuf_c.mtext)+1;
					int retries=5;
					while(retries>0)
					{
						if((msgsnd(msqid_c,&sbuf_c,buflen_c,IPC_NOWAIT))<0)
						{
							if(errno==EAGAIN)
							{
								retries-=1;
								usleep(5000);
								continue;
							}
							printf("%d, %ld, %s, %d",msqid_c,sbuf_c.mtype,sbuf_c.mtext,(int)buflen_c);
							die("msgsnd");
						}
						else
						{
							printf("CHp sent msqid with success\n\n");
							break;
						}
					}
					loop_child+=1;
				}
				printf("\n\nCHp end loop 1000\n\n");
				exit(0);
				flag_child=1;
				while(1)
				{
					sleep(1);
					if(flag_p&&flag_child)
					{
						exit(1);
					}
				}
			}
			else
			{
				while(loop_parent<1000)
                                {
					struct msqid_ds msqid_ds_p;
                                        val_rand_p=rand()%100+1;
                                        printf("PP gen. rand num > %d\n",val_rand_p);
                                        if(val_rand_p<11)
                                        {
                                                msflg_p=IPC_CREAT|0660;
                                                key_p=1234;
						if((msqid_p=msgget(key_p,msflg_p))<0)
						{
							die("msgget");
						}
						if(msgctl(msqid_p,IPC_STAT,&msqid_ds_p)<0)
						{
							die("msgctl(IPC_STAT)");
						}
						msqid_ds_p.msg_perm.gid=1004;
						if(msgctl(msqid_p,IPC_SET,&msqid_ds_p)<0)
						{
							die("msgctl(IPC_SET)");
						}
                        }
                        else
                        {
                        msflg_p=IPC_CREAT|0606;
                        key_p=12345;
						if((msqid_p=msgget(key_p,msflg_p))<0)
						{
							die("msgget");
						}
						if(msgctl(msqid_p,IPC_STAT,&msqid_ds_p)<0)
						{
							die("msgctl(IPC_STAT)");
						}
						msqid_ds_p.msg_perm.uid=1002;
						if(msgctl(msqid_p,IPC_SET,&msqid_ds_p)<0)
						{
							die("msgctl(IPC_SET)");
						}
                                        }
					sprintf(sbuf_p.mtext,"PP, gen rand val %d",val_rand_p);
					sbuf_p.mtype=val_rand_p;
					buflen_p=strlen(sbuf_p.mtext)+1;
					int retries=5;
					while(retries>0)
					{
						if((msgsnd(msqid_p,&sbuf_p,buflen_p,IPC_NOWAIT))<0)
						{
							if(errno==EAGAIN)
							{
								retries-=1;
								usleep(5000);
								continue;
							}
							printf("%d, %ld, %s, %d \n",msqid_p,sbuf_p.mtype,sbuf_p.mtext,(int)buflen_p);
							die("msgsnd");
						}
						else
						{
							printf("PP, send message with success\n\n");
							break;
						}
					}
                                        loop_parent++;
                                }
				printf("\n\nPP end lopp 1000\n\n");
				exit(0);
				flag_p=1;
				while(1)
				{
					sleep(1);
					if(flag_child&&flag_p)
					{
						exit(1);
					}
				}
			}
		}
	}
	exit(0);
}

void die (char*s)
{
	perror(s);
	exit(1);
}

void sign_handler_INT(int signum)
{
	flag=1;
}
