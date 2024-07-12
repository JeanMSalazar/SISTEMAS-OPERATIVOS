#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define KEY 0x4444

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
    // Creando o obteniendo el semaforo
    int id = semget(KEY, 1, 0666);
    if (id < 0)
    {
        perror("semget error");
        exit(11);
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
        // Proceso padre (minusculas)
        char *s = "abcdefgh";
        int l = strlen(s);
        for (int i = 0; i < l; ++i)
        {
            if (semop(id, &p, 1) < 0)   // Esperando por el semaforo
            {
                perror("semop p error");
                exit(13);
            }
            putchar(s[i]);
            fflush(stdout);
            sleep(rand() % 2);
            //segundo print
            putchar(s[i]);
            fflush(stdout);
            sleep(rand() % 2);
            if (semop(id, &v, 1) < 0)
            {
                perror("semop v error");
                exit(14);
            }
        }
    }
    else
    {
        // Proceso hijo (mayusculas)
        char *s = "ABCDEFGH";
        int l = strlen(s);
        for (int i = 0; i < l; ++i)
        {
            if (semop(id, &p, 1) < 0)   // Esperando por el semaforo
            {
                perror("semop p error");
                exit(15);
            }
            putchar(s[i]);
            fflush(stdout);//imprime sin que se llene el array
            sleep(rand() % 2);
            //segundo print
            putchar(s[i]);
            fflush(stdout);
            sleep(rand() % 2);
            if (semop(id, &v, 1) < 0)
            {
                perror("semop v error");
                exit(16);
            }
        }
    }

    return 0;
}