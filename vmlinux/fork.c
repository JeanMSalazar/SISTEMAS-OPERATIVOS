#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#define MAX_BUFFER 30

int c;
int counter=0;

int flag=0;

void signal_h(int snum);



int main()
{
	signal(SIGINT, signal_h);//se ejecuta cuando recibe un signal
	time_t t;
	srand((unsigned)time(&t));//para no repetir el ramdom
        int     fd1[2], fd2[2], nbytes;
	int childpid, num;
        pipe(fd1);
	pipe(fd2);
	char readbuffer[MAX_BUFFER];
	
	char * fin = readbuffer+30;
	for(char *i = readbuffer;i<fin;i++){
		*i= '\0';
	}

	childpid=fork();
for(;;){
	if(flag)
	{
		flag=0;
		if(childpid==0){
		for(int i=0; i<10; i++){
			close(fd1[1]);
			nbytes=read(fd1[0],readbuffer, sizeof(readbuffer));//read1
			printf("num > %s\n",readbuffer);
				
			num = atoi(readbuffer);
			num+=3;
			sprintf(readbuffer, "%d", num);
			close(fd2[0]);
			write(fd2[1], readbuffer, strlen(readbuffer)+1);//write2
		}
			exit(1);
		}
		else
		{
		for(int i=0; i<10; i++){
			//num=rand()%100;
			char buffer2[MAX_BUFFER];
			sprintf(buffer2, "%d", num);
			close(fd1[0]);
			write(fd1[1],buffer2,strlen(buffer2)+1);//write1
				
			close(fd2[1]);
			nbytes=read(fd2[0],buffer2,sizeof(buffer2));//read2
			printf("suma 3 > %s \n", buffer2);
			num=atoi(buffer2);
			num+=7;
			printf("valor final + 7 > %d\n\n", num);
		}
			exit(1);

        	}
	}
}

return 0;
}



void signal_h(int snum){
	flag=1;
}
