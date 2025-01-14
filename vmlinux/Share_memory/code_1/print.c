#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

#define SHMSZ 27
#define SEM_PRINT "/sem_print"
#define SEM_NUM "/sem_num"
#define SEM_PRODUCTOR "/sem_productor"

int main() {
    int shmid;
    key_t key;
    char *shm;
    char buff[SHMSZ];
    char last_value[SHMSZ] = ""; //
    sem_t *sem_print, *sem_num, *sem_productor;
    FILE *file;

    key = 5678;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    sem_print = sem_open(SEM_PRINT, O_CREAT, 0666, 0);
    sem_num = sem_open(SEM_NUM, O_CREAT, 0666, 0);
    sem_productor = sem_open(SEM_PRODUCTOR, O_CREAT, 0666, 0);

    if (sem_print == SEM_FAILED || sem_num == SEM_FAILED || sem_productor == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    strncpy(shm, "", SHMSZ);

    while (1) {
        sem_wait(sem_print);  // Esperar a que haya un nuevo valor para leer

        strncpy(buff, shm, SHMSZ - 1);  // Copiamos desde shm a buff.
        buff[SHMSZ - 1] = '\0';  // Aseguramos la terminacion en nulo.

        // Comprobamos si el valor leido es diferente del ultimo valor
        if (strlen(buff) != 0 && strcmp(buff, last_value) != 0) {
            printf("Num: %s\n", buff);
            strncpy(last_value, buff, SHMSZ);
        } else if (strlen(buff) == 0) {
            printf("memoria vacia\n");
        }

        if (strcmp(buff, "2000") == 0 || strstr(buff, "2000,4000") != NULL) {  // Verificamos si hemos llegado al valor final
            break;
        }

        // Decidir cual semaforo liberar
        if (strstr(buff, ",") != NULL) {
            // Formato "numero,multiplicacion" indica que productor escribio
            sem_post(sem_num);  // Permitir que num actue
        } else {
            // Formato "numero" indica que num escribio
            sem_post(sem_productor);  // Permitir que productor actue
        }

        //sleep(1);
    }

    // Cerrar el archivo
    fclose(file);

    // Cerrar los semaforos
    sem_close(sem_print);
    sem_close(sem_num);
    sem_close(sem_productor);

    // Eliminar los semaforos
    sem_unlink(SEM_PRINT);
    sem_unlink(SEM_NUM);
    sem_unlink(SEM_PRODUCTOR);

    return 0;
}
