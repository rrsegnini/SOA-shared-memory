#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

// extern int BUF_SIZE;   /* Maximum size for exchanged string */


/* Define a structure that will be imposed on the shared
    memory object */

double ran_expo(double mean){
    double u;
    double lambda = 1 / (double)mean; 
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}

struct msg {
    int id; /* Producer id */
    time_t t; /* Creation datetime */
    int key; /* Random key between 0 and 4 id */
};

struct shmbuf {
    sem_t        sem1;            /* POSIX unnamed semaphore */
    sem_t        sem2;            /* POSIX unnamed semaphore */
    sem_t        sem3;            /* POSIX unnamed semaphore */
    sem_t        sem4;            /* POSIX unnamed semaphore */
    int          hd;              /* In pointer */
    int          tl;              /* Out pointer */ 
    int          cnt;             /* Number of messages */
    int          cnt_gbl;         /* Total amount of messages generated by all proucers*/
    int          cnt_prd;         /* Total amount of producers */
    int          cnt_csm;         /* Total amount of consumers */
    int          BUF_SIZE;
    int          exit;
    struct msg   log[1024];   /* Buffer */
    struct msg   buf[];   /* Buffer */
};