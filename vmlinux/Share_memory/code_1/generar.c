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
#define START 100
#define END 3000
#define SEM_PRINT "/sem_print"
#define SEM_NUM "/sem_num"
#define SEM_PRODUCTOR "/sem_productor"

int main() {
    int shmid;
    key_t key;
    char *shm;
    char buff[SHMSZ];
    char last_value[SHMSZ] = "";//
    int current_value = START;
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

    // Abrir o crear los semaforos
    sem_print = sem_open(SEM_PRINT, O_CREAT, 0666, 0);
    sem_num = sem_open(SEM_NUM, O_CREAT, 0666, 0);
    sem_productor = sem_open(SEM_PRODUCTOR, O_CREAT, 0666, 0);

    if (sem_print == SEM_FAILED || sem_num == SEM_FAILED || sem_productor == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    while (1) {
        sem_wait(sem_num);  // Esperar a que sea el turno de num

        strncpy(buff, shm, SHMSZ - 1);  // Copiamos desde shm a buff.
        buff[SHMSZ - 1] = '\0';  // Aseguramos la terminacion en nulo.

        //printf("GenNum leyo: %s\n", buff);

        if (strlen(buff) != 0) {
            // Si el valor en la memoria compartida contiene una coma, ha sido procesado por `producto`
            if (strchr(buff, ',') != NULL) {
                int read_value = atoi(buff);
                //printf("GenNum comparando %d con %d\n", read_value, atoi(last_value));

                if (read_value != atoi(last_value)) {
                    if (current_value <= END) {
                        current_value++;  // Incrementamos el valor
                        printf("num generado: %d\n", current_value);

                        // Convertimos el numero a cadena y lo escribimos en la memoria compartida
                        snprintf(buff, SHMSZ, "%d", current_value);
                        strncpy(shm, buff, SHMSZ);

                        snprintf(last_value, SHMSZ, "%d", read_value);
                        sem_post(sem_print);  // Notificar a print que hay un nuevo valor
                    } else {
                        printf("El generadortermino.\n");
                        break;  // Terminamos la generacion de numeros
                    }
                }
            }
        } else {
            printf("memoria vacia\n");
        }

        //  sleep(1); 
    }

    // Cerrar los semaforos
    sem_close(sem_print);
    sem_close(sem_num);
    sem_close(sem_productor);

    return 0;
}
