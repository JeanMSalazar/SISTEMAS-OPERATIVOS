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
    char last_value[SHMSZ] = "";  // Inicializamos last_value a una cadena vacia
    sem_t *sem_print, *sem_num, *sem_productor;

    // Usamos "5678" como la clave para el segmento de memoria compartida.
    key = 5678;

    // Ubicamos el segmento.
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // Ahora adjuntamos el segmento a nuestro espacio de datos.
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    // Abrir o crear los semáforos
    sem_print = sem_open(SEM_PRINT, O_CREAT, 0666, 0);
    sem_num = sem_open(SEM_NUM, O_CREAT, 0666, 0);
    sem_productor = sem_open(SEM_PRODUCTOR, O_CREAT, 0666, 0);

    if (sem_print == SEM_FAILED || sem_num == SEM_FAILED || sem_productor == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Verificamos si la memoria compartida esta vacia
    strncpy(buff, shm, SHMSZ - 1);
    buff[SHMSZ - 1] = '\0';

    if (strlen(buff) == 0) {
        // Inicializamos la memoria compartida con "100,0"
        printf("Inicializando memoria compartida con '100,0'\n");
        strncpy(shm, "100,200", SHMSZ);
        snprintf(last_value, SHMSZ, "0");
        sem_post(sem_print);  // Notificar a print que hay un nuevo valor
    }

    while (1) {
        sem_wait(sem_productor);

        strncpy(buff, shm, SHMSZ - 1);  // Copiamos desde shm a buff.
        buff[SHMSZ - 1] = '\0';  // Aseguramos la terminacion en nulo.

        //printf("Producto leyo: %s\n", buff);

        if (strlen(buff) != 0 && strchr(buff, ',') == NULL) {
            int current_value = atoi(buff);
            //printf("Producto comparando %d con %d\n", current_value, atoi(last_value));

            if (current_value != atoi(last_value)) {
                // Multiplicamos el numero por 2
                int multiplied_value = current_value * 2;
                //printf("Producto original: %d, Producto multiplicado: %d\n", current_value, multiplied_value);
                printf("numX2:%d,%d\n", current_value, multiplied_value);

                // Formateamos el resultado como '10,20' y lo escribimos en la memoria compartida
                snprintf(buff, SHMSZ, "%d,%d", current_value, multiplied_value);
                strncpy(shm, buff, SHMSZ);

                snprintf(last_value, SHMSZ, "%d", current_value);
                sem_post(sem_print);  // Notificar a print que hay un nuevo valor
            }
        }

        //sleep(1);  // Esperamos 1 segundo antes de la proxima lectura.
    }

    sem_close(sem_print);
    sem_close(sem_num);
    sem_close(sem_productor);

    return 0;
}
