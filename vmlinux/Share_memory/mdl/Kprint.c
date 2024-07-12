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

    while (1) {
        sem_wait(sem_print);

        lseek(proc_fd, 0, SEEK_SET);
        read(proc_fd, buff, BUFFER_SIZE);
        buff[BUFFER_SIZE - 1] = '\0';

        if (strlen(buff) != 0 && strcmp(buff, last_value) != 0) {
            printf("Num: %s\n", buff);
            strncpy(last_value, buff, BUFFER_SIZE);
        } else if (strlen(buff) == 0) {
            printf("memoria vacia\n");
        }

        if (strcmp(buff, "2000") == 0 || strstr(buff, "2000,4000") != NULL) {
            break;
        }

        if (strstr(buff, ",") != NULL) {
            sem_post(sem_num);
        } else {
            sem_post(sem_productor);
        }
    }

    close(proc_fd);
    sem_close(sem_print);
    sem_close(sem_num);
    sem_close(sem_productor);
    sem_unlink(SEM_PRINT);
    sem_unlink(SEM_NUM);
    sem_unlink(SEM_PRODUCTOR);

    return 0;
}
