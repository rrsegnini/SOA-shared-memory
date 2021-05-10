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
#include "pshm.h"

int logged;
int cnt_msgs;
double cnt_tms;
int cnt_smp;

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
    int mean = atoi(argv[2]);

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
    int sbreak = 0;
    int breaker = getpid() % 5;
    fprintf(stderr, "%d\n", breaker);
    while (1){
        time_t bef_sem;
        time(&bef_sem);
        
        if (shmp->cnt == 0){
            if (sem_wait(&shmp->sem4) == -1){
                errExit("sem_wait");
            }
        } 

        if (sem_wait(&shmp->sem2) == -1){
            errExit("sem_wait");
        }

        time_t aft_sem;
        time(&aft_sem);
        int seconds = difftime(aft_sem, bef_sem);
        cnt_smp += seconds;

        //Log consumer creation
        if (!logged){
            shmp->cnt_csm++;
            logged=1;
        }


        if (shmp->exit){
            shmp->cnt_csm = shmp->cnt_csm - 1;
            if (sem_post(&shmp->sem3) == -1)
                errExit("sem_post");
             
            if (sem_post(&shmp->sem1) == -1){
                errExit("sem_post");
            }
            break;
        }

        struct tm * timeinfo;
        char buff[20]; 
        timeinfo = localtime(&shmp->buf[shmp->hd].t);
        strftime(buff, 20, "%T", timeinfo); 
        fprintf(stderr, "Read message at position %d: PID:%d Time:%s Key:%d\n", shmp->hd, shmp->buf[shmp->hd].id, buff, shmp->buf[shmp->hd].key);
        fprintf(stderr, "\n");

        if (shmp->buf[shmp->hd].key == breaker){
            shmp->cnt_csm = shmp->cnt_csm - 1;
            sbreak = 1;
        }

        cnt_msgs++;

        shmp->hd++;
        if (shmp->hd == shmp->BUF_SIZE){
            shmp->hd = 0;
        }

        if (shmp->cnt > 0){
            shmp->cnt = shmp->cnt-1;
        }
        

        if (sem_post(&shmp->sem3) == -1)
            errExit("sem_post");

             
        if (sem_post(&shmp->sem1) == -1){
            errExit("sem_post");
        }


        if (sbreak==1){
            break;
        }
    
        double wait = ran_expo((double)mean);
        cnt_tms += wait;
        fprintf(stderr, "\nSleeping for: %f\n", wait);
        sleep(wait);
    }

    fprintf(stderr, "\n\t**SUMMARY**\n\tMessages: %d\n\tWaited time: %f\n\tBlocked time: %d\n\nBye!\n", cnt_msgs, cnt_tms, cnt_smp);
    

    exit(EXIT_SUCCESS);
}