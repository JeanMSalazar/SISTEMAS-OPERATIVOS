#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

//P1

void sigchld_handler(int sig);

int contador=0;
int signalON;

int main()
{
    signalON = 0;
    signal(SIGINT, sigchld_handler);
    int fd;
    char * myfifo = "/tmp/myfifo2";

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);
    fd = open(myfifo, O_WRONLY);//file descriptor

    for(;;){
        if (signalON){
            signalON=0;
	    int random_number = rand() % 100; // Genera un numero ramdom
	    char rn[20];
	    sprintf(rn, "%d", random_number);//funcion para convertir
	    printf("Sending random number: %d\n", atoi(rn));
	    write(fd, &rn, sizeof(rn));
	    contador++;   
            if (contador >= 10) { 
                break;
            }
        }
	sleep(5);

    }
    close(fd);

    /* remove the FIFO */
    unlink(myfifo);

    return 0;
}


void sigchld_handler(int sig){

    signalON = 1;

}
