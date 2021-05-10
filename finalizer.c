/*
Este programa se encarga de cancelar todo el sistema de procesos, 
enviando mensajes de finalización a cada consumidor vivo usando 
el buzón diseñado para este proyecto, e indicándo- le a los 
productores que cesen actividades con alguna bandera global en 
memoria compartida. Una vez que la cantidad de productores y 
consumidores llega a cero, el buffer compartido es liberado. 
El finalizador deberá dar mensajes y todas las estadı́sticas 
posibles de su gestión.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#include <ctype.h>
#include "pshm_ucase.h"

int
main(int argc, char *argv[])
{
    if (argc < 1) {
        fprintf(stderr, "Usage: %s /shm-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *shmpath = argv[1];

    /* Create shared memory object and set its size to the size
        of our structure. */

    int fd = shm_open(shmpath, O_RDWR, 0);
    if (fd == -1)
        errExit("shm_open");

    struct shmbuf *shmp = mmap(NULL, sizeof(*shmp),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED)
        errExit("mmap");

    
    /* Wait for 'sem1' to be posted by peer before touching
        shared memory. */

    // if (sem_wait(&shmp->sem1) == -1){
    //     errExit("sem_wait");
    // }
    
    shmp->exit = 1;
    
    if (sem_post(&shmp->sem1) == -1){
        errExit("sem_post");
    }
    if (sem_post(&shmp->sem2) == -1){
        errExit("sem_post");
    }
    if (sem_post(&shmp->sem3) == -1){
        errExit("sem_post");
    }
    if (sem_post(&shmp->sem4) == -1){
        errExit("sem_post");
    }
            
    

    /* Unlink the shared memory object. Even if the peer process
        is still using the object, this is okay. The object will
        be removed only after all open references are closed. */

    shm_unlink(shmpath);

    exit(EXIT_SUCCESS);
}