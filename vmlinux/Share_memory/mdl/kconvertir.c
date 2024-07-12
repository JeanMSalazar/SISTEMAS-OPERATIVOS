#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

#define BUFFER_SIZE 128
#define SEM_PRINT "/sem_print"
#define SEM_NUM "/sem_num"
#define SEM_PRODUCTOR "/sem_productor"
#define PROC_FILE "/proc/proc_comm"

int main() {
    char buff[BUFFER_SIZE];
    char last_value[BUFFER_SIZE] = "";
    sem_t *sem_print, *sem_num, *sem_productor;
    int proc_fd;

    sem_print = sem_open(SEM_PRINT, O_CREAT, 0666, 0);
    sem_num = sem_open(SEM_NUM, O_CREAT, 0666, 0);
    sem_productor = sem_open(SEM_PRODUCTOR, O_CREAT, 0666, 0);

    if (sem_print == SEM_FAILED || sem_num == SEM_FAILED || sem_productor == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    proc_fd = open(PROC_FILE, O_RDWR);
    if (proc_fd < 0) {
        perror("open");
        exit(1);
    }

    lseek(proc_fd, 0, SEEK_SET);
    read(proc_fd, buff, BUFFER_SIZE);
    buff[BUFFER_SIZE - 1] = '\0';

    if (strlen(buff) == 0) {
        printf("Inicializando memoria compartida con '100,200'\n");
        strncpy(buff, "100,200", BUFFER_SIZE);
        write(proc_fd, buff, strlen(buff));
        snprintf(last_value, BUFFER_SIZE, "0");
        sem_post(sem_print);
    }

    while (1) {
        sem_wait(sem_productor);

        lseek(proc_fd, 0, SEEK_SET);
        read(proc_fd, buff, BUFFER_SIZE);
        buff[BUFFER_SIZE - 1] = '\0';

        if (strlen(buff) != 0 && strchr(buff, ',') == NULL) {
            int current_value = atoi(buff);

            if (current_value != atoi(last_value)) {
                int multiplied_value = current_value * 2;
                printf("numX2:%d,%d\n", current_value, multiplied_value);

                snprintf(buff, BUFFER_SIZE, "%d,%d", current_value, multiplied_value);
                write(proc_fd, buff, strlen(buff));

                snprintf(last_value, BUFFER_SIZE, "%d", current_value);
                sem_post(sem_print);
            }
        }
    }

    close(proc_fd);
    sem_close(sem_print);
    sem_close(sem_num);
    sem_close(sem_productor);

    return 0;
}
