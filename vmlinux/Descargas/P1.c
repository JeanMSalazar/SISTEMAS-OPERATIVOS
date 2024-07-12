#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include<signal.h>

//p1

void sigchld_handler(int sig);
int contador=0;
int signalON;
int main()
{
    signalON = 0;
    signal(SIGINT, sigchld_handler);
    int fd;
    char * myfifo = "/tmp/myfifoJulio";

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);

    /* write "Hi" to the FIFO */
    fd = open(myfifo, O_WRONLY);
    for(;;){
        if (signalON){
            signalON=0;
            write(fd, "Hi", sizeof("Hi"));
            contador = contador + 1;
            if (contador > 10) { 
                exit(0);
            }
        }

    }
    close(fd);

    /* remove the FIFO */
    unlink(myfifo);

    return 0;
}


void sigchld_handler(int sig){

    signalON = 1;

}
