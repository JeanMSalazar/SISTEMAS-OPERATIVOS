#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define SHMSZ 27
#define KEY 0x1111

//semaphora
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
    int shmid;
    key_t key;
    char *shm = NULL;
    char last_data[21] = "100";
    int num = 100;

    key = 5678;


    //semaphora
    int id = semget(KEY, 1, 0666);
    /*if (id < 0)
    {
        perror("semget");
        exit(11);
    }*/
    union semun u;
    u.val = 1;
    if (semctl(id, 0, SETVAL, u) < 0)
    {
        perror("semctl");
        exit(12);
    }


    //proyect
    if ((shmid = shmget(key, SHMSZ, 0666 | IPC_CREAT)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    char buff[21];
    int contador = 0;

        while (contador <= 1000) {
        //semaphoraP
        if (semop(id, &p, 1) < 0)
        {
            perror("semop p");
            exit(13);
        }

        strncpy(buff, shm, 20);
        buff[20] = '\0';

        if (strcmp(last_data, buff) != 0) {
            num++;
            sprintf(shm, "%d", num);
            strcpy(last_data, buff);
            printf("num generado: %d\n", num);
            contador++;
        }

        //semaphoraV
        /*if (semop(id, &v, 1) < 0)
        {
            perror("semop p");
            exit(14);
        }*/

        //usleep(100000);
        usleep(10000);
    }

    exit(0);
} 