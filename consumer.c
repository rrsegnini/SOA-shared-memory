/*
Esta es una familia de procesos, todos usando exactamente el mismo código, 
que se vinculan al buffer reservado por el creador y que con tiempos de 
espera aleatorios consumen mensajes tomados del buffer. 
Cada consumidor recibe como argumentos de la lı́nea de comandos 
el nombre del buffer compartido y un parámetro
que indique la media en segundos de los tiempos aleatorios, 
siguiendo una distribución exponencial, que deben esperar antes de consumir 
el siguiente mensaje del buffer administrado circularmente. 
Por supuesto, este acceso debe darse de manera sincronizada ya que el 
buffer es compartido por múltiples procesos. 
El usuario puede ejecutar cuantas veces lo desee este programa, 
creando un nuevo consumidor que compite por el buffer, 
aunque cada vez podrı́a indicarse una media de espera diferente. 
Cuando se crea un consumidor, lo primero que éste hace es incrementar 
el contador de consumidores activos. Estos procesos repiten un ciclo de 
espera aleatoria y consu- mo de mensajes hasta que lean un mensaje especial 
que indique que el sistema se deba suspender, o cuando al leer un mensaje 
este incluya una llave (número entero entre 0 y 4) que sea igual al 
process id o PID del consumidor módulo 5. 
En cualquiera de estos dos casos, el consumidor termina, decrementa el 
contador de consumidores activos, despliega su identificación y algunas 
estadı́sticas básicas (número de mensajes consumidos, 
acumulado de tiempos esperados, acumulado de tiempo que 
estuvo bloqueado por semáforos, etc.). 
Si no hay mensajes en el buffer, el consumidor queda suspendido hasta 
que aparezca uno. No se puede usar busy waiting. 
Cada vez que un mensaje logra ser leı́do del buffer, se debe desplegar 
un mensaje a la consola describiendo la acción realizada incluyendo el 
ı́ndice de la entrada de adonde se tomó el mensaje y la cantidad de 
productores y consumidores vivos al instante de este evento).
*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pshm_ucase.h"

int
main(int argc, char *argv[])
{
    time_t t;
    srand((unsigned) time(&t));

    if (argc != 3) {
        fprintf(stderr, "Usage: %s /shm-path string\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *shmpath = argv[1];
    char *string = argv[2];
    size_t len = strlen(string);

    if (len > BUF_SIZE) {
        fprintf(stderr, "String is too long\n");
        exit(EXIT_FAILURE);
    }

    /* Open the existing shared memory object and map it
        into the caller's address space. */

    int fd = shm_open(shmpath, O_RDWR, 0);
    if (fd == -1)
        errExit("shm_open");

    struct shmbuf *shmp = mmap(NULL, sizeof(*shmp),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED)
        errExit("mmap");
    while (1){
        /* Wait until peer says that it has finished accessing
            the shared memory. */

        if (sem_wait(&shmp->sem2) == -1)
            errExit("sem_wait");

        /* Write modified data in shared memory to standard output. */

        write(STDOUT_FILENO, &shmp->buf, len);
        write(STDOUT_FILENO, "\n", 1);
        
        sleep(rand() % 5);
    }
    

    exit(EXIT_SUCCESS);
}