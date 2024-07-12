#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void sigusr1_handler(int sig);

int main()
{
    signal(SIGUSR1, sigusr1_handler);
    int fd;
    char *myfifo = "/tmp/myfifo";
    int contador = 0;
S
    mkfifo(myfifo, 0666); //creacion de fifo
    fd = open(myfifo, O_WRONLY);

    for (;;) {
        int random_number = rand() % 100; // Genera un numero ramdom
        printf("Sending random number: %d\n", random_number);
        write(fd, &random_number, sizeof(random_number));
        contador++;
        if (contador > 10) {
            break;
        }
        sleep(5); // espera 5 segundos
    }

    close(fd);
    unlink(myfifo);
i
    return 0;
}

void sigusr1_handler(int sig)
{
    // interrumpe el programa
}
