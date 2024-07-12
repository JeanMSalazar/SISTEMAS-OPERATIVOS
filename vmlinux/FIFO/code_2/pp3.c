#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sigusr2_handler(int sig);

int main()
{
    signal(SIGUSR2, sigusr2_handler);

    // loop
    for (;;) {
        sleep(6);
    }

    return 0;
}

void sigusr2_handler(int sig)
{
    printf("Received SIGUSR2\n");
    kill(getppid(), SIGUSR1); // envia signal hacia pp1
}
