#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>

#define KEY 0x4444
#define SHM_KEY 0x5555

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

struct sembuf p = {0, -1, SEM_UNDO};
struct sembuf v = {0, +1, SEM_UNDO};

int main()
{
    // Creando o obteniendo el semáforo
    int sem_id = semget(KEY, 1, 0666);
    if (sem_id < 0)
    {
        perror("semget error");
        exit(11);
    }

    // Creando o obteniendo la memoria compartida
    int shm_id = shmget(SHM_KEY, sizeof(char) * 16, 0666 | IPC_CREAT);
    if (shm_id < 0)
    {
        perror("shmget error");
        exit(12);
    }

    char *shared_memory = (char *)shmat(shm_id, NULL, 0);
    if (shared_memory == (char *)-1)
    {
        perror("shmat error");
        exit(13);
    }

    int pid;
    pid = fork();
    srand(pid);

    if (pid < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid)
    {
        // Proceso padre (minúsculas)
        char *s = "abcdefgh";
        int l = strlen(s);
        for (int i = 0; i < l; ++i)
        {
            if (semop(sem_id, &p, 1) < 0)   // Esperando por el semáforo
            {
                perror("semop p error");
                exit(14);
            }
            shared_memory[i] = s[i];
            fflush(stdout);
            sleep(rand() % 2);
            if (semop(sem_id, &v, 1) < 0)
            {
                perror("semop v error");
                exit(15);
            }
        }
        shared_memory[l] = '\0'; // Terminar la cadena
    }
    else
    {
        // Proceso hijo (mayúsculas)
        char *s = "ABCDEFGH";
        int l = strlen(s);
        for (int i = 0; i < l; ++i)
        {
            if (semop(sem_id, &p, 1) < 0)   // Esperando por el semáforo
            {
                perror("semop p error");
                exit(16);
            }
            shared_memory[i + l] = s[i];
            fflush(stdout);
            sleep(rand() % 2);
            if (semop(sem_id, &v, 1) < 0)
            {
                perror("semop v error");
                exit(17);
            }
        }
        shared_memory[l * 2] = '\0'; // Terminar la cadena
    }

    shmdt(shared_memory);
    return 0;
}