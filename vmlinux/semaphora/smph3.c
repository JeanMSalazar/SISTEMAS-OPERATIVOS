#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define SHM_KEY 0x4444

int main()
{
    // Obteniendo la memoria compartida
    int shm_id = shmget(SHM_KEY, sizeof(char) * 16, 0666);
    if (shm_id < 0)
    {
        perror("shmget error");
        exit(1);
    }

    char *shared_memory = (char *)shmat(shm_id, NULL, 0);
    if (shared_memory == (char *)-1)
    {
        perror("shmat error");
        exit(2);
    }

    printf("%s\n", shared_memory);

    shmdt(shared_memory);
    return 0;
}