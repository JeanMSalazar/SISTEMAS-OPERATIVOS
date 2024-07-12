#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sigchld_handler(int sig);
int signalON = 0;//flag

int main(int argc, char *argv[])
{

    int pid = atoi(argv[1]);

    signal(SIGINT, sigchld_handler);

    for (;;){
    	if(signalON == 1){
	signalON=0;	
    	kill(pid, 2);//envia un signal a p1 
    	}
    }
    return 0;
}

void sigchld_handler(int sig)
{
    signalON = 1;
    printf("Received\n");
}
