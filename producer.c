/*
Esta es una familia de procesos, todos usando exactamente el mismo código, 
que se vinculan al buffer reservado por el creador. Con tiempos de espera 
aleatorios generarán mensajes que colocarán en el buffer (administrado circularmente). 
Cada productor recibe como argumentos de la lı́nea de comandos el nombre del buffer 
compartido y un parámetro que indique la media en segundos de los tiempos aleatorios, 
siguiendo una distribución exponencial, que deben esperar antes de agregar un nuevo 
mensaje en el buffer. Por supuesto, este acceso debe darse de manera sincronizada ya 
que el buffer es compartido por múltiples procesos. El usuario puede ejecutar cuantas 
veces lo desee este programa, creando un nuevo productor que compite por el buffer, 
aunque cada vez podrı́a indicarse una media de espera diferente. 
Al crearse un productor, este incrementa el contador de productores vivos. 
Estos procesos repiten un ciclo de espera aleatoria y fabricación de mensajes hasta 
que algún tipo de bandera global en memoria compartida indique que el sistema se debe suspender. 
En este caso, los productores terminan, decrementan el contador de productores vivos, 
y despliegan su identificación y algunas estadı́sticas básicas (número de mensajes producidos, 
acumulado de tiempos esperados, acumulado de tiempo que estuvo bloqueado por semáforos, etc.). 
Si no hay espacio en el buffer, el productor queda suspen- dido hasta que se libere un espacio. 
No se puede usar busy waiting. El formato especı́fico del mensaje puede ser definido por 
cada grupo de trabajo, pero debe incluir al menos: Identificación del productor Fecha y 
hora de creación Llave aleatoria entre 0 y 4. 
Cada vez que un mensaje logra ser puesto en el buffer, 
se debe desplegar un mensaje a la consola describiendo la acción realizada incluyendo el 
ı́ndice de la entrada donde se dejó el mensaje y la cantidad de productores y consumidores 
vivos al instante de este evento.
*/

#include <time.h>
#include <string.h>
#include "pshm.h"

int logged;
int cnt_msgs;
double cnt_tms;
int cnt_smp;


int
main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));

    if (argc != 3) {
        fprintf(stderr, "Usage: %s /shm-path string\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *shmpath = argv[1];
    int mean = atoi(argv[2]);

    int fd = shm_open(shmpath, O_RDWR, 0);
    if (fd == -1)
        errExit("shm_open");

    struct shmbuf *shmp = mmap(NULL, sizeof(*shmp),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED)
        errExit("mmap");
    
    struct msg msg1;
    
    while (1){        
        msg1.id = getpid();
        msg1.t = time(NULL);
        msg1.key = rand() % 5;

        time_t bef_sem;
        time(&bef_sem);

        if (sem_wait(&shmp->sem3) == -1){
            errExit("sem_wait");
        }

        if (sem_wait(&shmp->sem2) == -1){
            errExit("sem_wait");
        }

        time_t aft_sem;
        time(&aft_sem);
        int seconds = difftime(aft_sem, bef_sem);
        cnt_smp += seconds;

        // Add to log
        if (!logged){
            shmp->cnt_prd++;
            logged=1;
        }

        if (shmp->exit){
            shmp->cnt_prd = shmp->cnt_prd - 1;
            if (sem_post(&shmp->sem1) == -1)
                errExit("sem_post");
        
            if (sem_post(&shmp->sem4) == -1)
                errExit("sem_post");
            break;
        }

        shmp->log[shmp->cnt_gbl] = msg1;
        shmp->cnt_gbl++;
        cnt_msgs++;

        shmp->buf[shmp->tl] = msg1;
        fprintf(stderr, "Message posted by %d!\n Position: %d \n Producers:%d Consumers:%d\n", getpid(), shmp->tl, shmp->cnt_prd, shmp->cnt_csm);
        
        shmp->tl++;
        if (shmp->tl == shmp->BUF_SIZE){
            shmp->tl = 0;
        }

        shmp->cnt++;

        if (sem_post(&shmp->sem1) == -1)
            errExit("sem_post");
        
        if (sem_post(&shmp->sem4) == -1)
            errExit("sem_post");
       
        double wait = ran_expo((double)mean);
        cnt_tms += wait;
        fprintf(stderr, "\nSleeping for: %f\n", wait);
        sleep(wait);
        
    }

    fprintf(stderr, "\n\t**SUMMARY**\n\tMessages: %d\n\tWaited time: %f\n\tBlocked time: %d\n\nBye!\n", cnt_msgs, cnt_tms, cnt_smp);
    

    exit(EXIT_SUCCESS);
}