#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void sigusr1_handler(int sig);
void sigusr2_handler(int sig);

int main()
{
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);

    int fd;
    char *myfifo = "/tmp/myfifo";

    mkfifo(myfifo, 0666);
    fd = open(myfifo, O_RDONLY);

    for (;;) {
        int received_number;
        read(fd, &received_number, sizeof(received_number));
        printf("Received number: %d\n", received_number);
        kill(getppid(), SIGUSR2); // envia un signal al proceso p3
    }

    close(fd);
    unlink(myfifo);

    return 0;
}

void sigusr1_handler(int sig)
{
    // interrumpe el programa con sleep
}

void sigusr2_handler(int sig)
{
    // interrumpe el programa con sleep
}
