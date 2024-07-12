#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SHMSZ 27

int main()
{
    int shmid;
    key_t key;
    char *shm = NULL;

    key = 5678;

    if ((shmid = shmget(key, SHMSZ, 0666 | IPC_CREAT)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    sprintf(shm, "100,200"); // Escribir un numero inicial en la memoria compartida

    printf("Numero inicial enviado a la memoria compartida.\n");

    return 0;
}
