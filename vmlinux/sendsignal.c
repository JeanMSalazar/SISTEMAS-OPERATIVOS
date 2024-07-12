#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <PID_Padre> <PID_Hijo>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int pidPadre = atoi(argv[1]);
    int pidHijo = atoi(argv[2]);

    // Enviar SIGINT al proceso padre
    if (kill(pidPadre, SIGINT) == -1) {
        perror("Error enviando SIGINT al padre");
        return EXIT_FAILURE;
    }
    printf("SIGINT enviado a PID del padre: %d\n", pidPadre);

    // Enviar SIGINT al proceso hijo
    if (kill(pidHijo, SIGINT) == -1) {
        perror("Error enviando SIGINT al hijo");
        return EXIT_FAILURE;
    }
    printf("SIGINT enviado a PID del hijo: %d\n", pidHijo);

    return EXIT_SUCCESS;
}

