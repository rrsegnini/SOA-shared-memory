#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

extern int BUF_SIZE;   /* Maximum size for exchanged string */

/* Define a structure that will be imposed on the shared
    memory object */

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
    int          BUF_SIZE;
    struct msg   buf[];   /* Data being transferred */
};