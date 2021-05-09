/*
Este programa gráfico será responsable de crear el buffer compartido por todos los procesos, 
de inicializar todas las variables de control asociadas al 
mismo (semáforos, banderas, contador de productores, contador de consumidores, etc.), 
y de monitorear de manera gráfica el estado de todo el sistema. 
El nombre del buffer, el tamaño en entradas para mensajes, y cualquier otro parámetro que 
cada grupo considere conve- niente serán recibidos de la lı́nea de comando al ser ejecutado 
desde consola. 
Usando GTK o algún equivalente, este programa mostrará constantemente el 
nombre y contenido del buffer (incluyendo indicadores del punto de entrada 
y el punto de salida del buffer), la cantidad de productores y consumidores, 
y una bitácora (scrollable) que registre todos los eventos del sistema. 
Inclusive, cuando el finalizador haya terminado al sistema, 
el creador seguirá vivo para poder examinar esta bitácora.
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
    if (argc < 2) {
        fprintf(stderr, "Usage: %s /shm-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *shmpath = argv[1];
    int buff_size = atoi(argv[2]);

    shm_unlink(shmpath);

    /* Create shared memory object and set its size to the size
        of our structure. */

    int fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR,
                        S_IRUSR | S_IWUSR);
    if (fd == -1)
        errExit("shm_open");

    if (ftruncate(fd, sizeof(struct shmbuf)) == -1)
        errExit("ftruncate");

    /* Map the object into the caller's address space. */

    struct shmbuf *shmp = mmap(NULL, sizeof(*shmp),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED)
        errExit("mmap");

    shmp->BUF_SIZE = buff_size;
    shmp->exit = 0;
    /* Initialize semaphores as process-shared, with value 0. */

    if (sem_init(&shmp->sem1, 1, 0) == -1){
        errExit("sem_init-sem1");
    }
        
    if (sem_init(&shmp->sem2, 1, 1) == -1){
        errExit("sem_init-sem2");
    }

    if (sem_init(&shmp->sem3, 1, shmp->BUF_SIZE) == -1){
        errExit("sem_init-sem3");
    }

    if (sem_init(&shmp->sem4, 1, 0) == -1){
        errExit("sem_init-sem4");
    }
        
    while (1){
        if (shmp->exit){
            break;
        }
        /* Wait for 'sem1' to be posted by peer before touching
            shared memory. */

        if (sem_wait(&shmp->sem1) == -1){
            errExit("sem_wait");
        }
        
        
        int j = shmp->hd;
        int cont = shmp->cnt;
        if (cont < 0){
            fprintf(stderr, "cont:%d\n", cont);
            break;
        } 
        struct tm * timeinfo;
        char buff[20]; 

        fprintf(stderr, "head: %d tail: %d cont:%d\n", shmp->hd, shmp->tl, cont);
        while (cont != 0){          
            timeinfo = localtime(&shmp->buf[j].t); 
            strftime(buff, 20, "%T", timeinfo); 
            fprintf(stderr, "%d %s %d %d\n", shmp->buf[j].id, buff, shmp->buf[j].key, cont);
            cont--;
            j++;
            if (j == shmp->BUF_SIZE) j = 0;

        }
        fprintf(stderr, "\n\n\n");
        
        if (sem_post(&shmp->sem2) == -1){
            errExit("sem_post");
        }
            
    }

 


    /* Unlink the shared memory object. Even if the peer process
        is still using the object, this is okay. The object will
        be removed only after all open references are closed. */

    shm_unlink(shmpath);

    exit(EXIT_SUCCESS);
}