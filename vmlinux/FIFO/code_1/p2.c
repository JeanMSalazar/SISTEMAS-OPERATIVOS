#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#define MAX_BUF 1024

//P2
int contador=0;


int main(int argc, char *argv[])
{
    int fd;
    char * myfifo = "/tmp/myfifo2";
    char buf[MAX_BUF];
    int pid = atoi(argv[1]);//atoi convierte string to integer

    /* open, read, and display the message from the FIFO */
    fd = open(myfifo, O_RDONLY);
    for(;contador <= 10;){
    read(fd, buf, MAX_BUF);
    printf("numero recivido: %s\n", buf);
    contador++;
    kill(pid, 2);//envia un signal
    sleep(1);
   
    }
    close(fd);

    return 0;
}

