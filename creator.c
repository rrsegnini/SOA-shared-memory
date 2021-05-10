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
#include <gtk/gtk.h>
#include <glib.h>
#include <pthread.h>
#include <math.h>

#include <ctype.h>
#include "pshm.h"

typedef struct 
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *table;
  char *shmpath;
    int buff_size;
} t_args;

void set_headers(GtkWidget *table, char *name, int hd, int tl, int print_buff_info){
    GtkWidget *pid_lbl;
    GtkWidget *datetime_lbl;
    GtkWidget *key_lbl;
    char str[30];
    if (print_buff_info){
        sprintf(str, "In:%d\nPID", hd);
        pid_lbl = gtk_label_new (str);
        sprintf(str, "%s\nDatetime", name);
        datetime_lbl = gtk_label_new (str);
        sprintf(str, "Out:%d\nKey", tl);
        key_lbl = gtk_label_new (str);
        
    } else {
        pid_lbl = gtk_label_new ("\nPID");
        key_lbl = gtk_label_new ("\nKey");
        datetime_lbl = gtk_label_new ("\nDatetime");
    }
    

    gtk_widget_show (pid_lbl);
    gtk_widget_show (datetime_lbl);
    gtk_widget_show (key_lbl);
    gtk_table_attach_defaults (GTK_TABLE (table), pid_lbl, 0, 1, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE (table), datetime_lbl, 1, 2, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE (table), key_lbl, 2, 3, 0, 1);
}

void set_waiting_screen(GtkWidget *table){
    GtkWidget *pid_lbl;
    GtkWidget *datetime_lbl;
    GtkWidget *key_lbl;
    pid_lbl = gtk_label_new ("Waiting");
    datetime_lbl = gtk_label_new ("for producers");
    key_lbl = gtk_label_new ("...");
    gtk_widget_show (pid_lbl);
    gtk_widget_show (datetime_lbl);
    gtk_widget_show (key_lbl);
    gtk_table_attach_defaults (GTK_TABLE (table), pid_lbl, 0, 1, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE (table), datetime_lbl, 1, 2, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE (table), key_lbl, 2, 3, 0, 1);
}

void *read_buf(void *args){
    GtkWidget *grid;
    GtkWidget *table;
    GtkWidget *scroll_wndw;
    GtkWidget *logs;
    GtkWidget *pid_val_lbl;
    GtkWidget *datetime_val_lbl;
    GtkWidget *key_val_lbl;
    GtkWidget *pid_val_logs_lbl;
    GtkWidget *datetime_val_logs_lbl;
    GtkWidget *key_val_logs_lbl;
    GtkWidget *name_ttl;
    GtkWidget *log_ttl;
    t_args *data = (t_args *)args;
    char *shmpath = data->shmpath;
    int buff_size = data->buff_size;

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

    /* Initialize semaphores as process-shared */
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
        /* Wait for 'sem1' to be posted before touching
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
        char str[30];
        table = gtk_table_new (data->buff_size, 2, TRUE); 
        set_headers(table, shmpath, shmp->hd, shmp->tl, 1);
        while (cont != 0){         
            timeinfo = localtime(&shmp->buf[j].t); 
            strftime(buff, 20, "%T", timeinfo); 

            if (shmp->hd == j){
                sprintf(str, "*%d", shmp->buf[j].id);
            } else if (shmp->tl == j+1){
                sprintf(str, "+%d", shmp->buf[j].id);
            } else {
                sprintf(str, "%d", shmp->buf[j].id);
            }
            
            pid_val_lbl = gtk_label_new (str);
            sprintf(str, "%s", buff);
            datetime_val_lbl = gtk_label_new (str);
            sprintf(str, "%d", shmp->buf[j].key);
            key_val_lbl = gtk_label_new (str);

            gtk_widget_show (pid_val_lbl);
            gtk_widget_show (datetime_val_lbl);
            gtk_widget_show (key_val_lbl);

            /* get GTK thread lock */
            gdk_threads_enter ();
            gtk_table_attach_defaults (GTK_TABLE (table), pid_val_lbl, 0, 1, j+1, j+2);
            gtk_table_attach_defaults (GTK_TABLE (table), datetime_val_lbl, 1, 2, j+1, j+2);
            gtk_table_attach_defaults (GTK_TABLE (table), key_val_lbl, 2, 3, j+1, j+2);
            /* release GTK thread lock */
            gdk_threads_leave ();

            cont--;
            j++;
            if (j == shmp->BUF_SIZE) j = 0;    

        }
        gdk_threads_enter ();
        gtk_widget_destroy(data->table);
        gtk_widget_destroy(data->grid);
        grid = gtk_table_new (1, 3, TRUE);

        name_ttl = gtk_label_new (shmpath);
        gtk_widget_show (name_ttl);
        log_ttl = gtk_label_new ("Logs");
        gtk_widget_show (log_ttl);

        scroll_wndw = gtk_scrolled_window_new(NULL, NULL);
        logs = gtk_table_new (data->buff_size, 2, TRUE); 
        set_headers(logs, shmpath, 0, 0, 0);
        char buff_logs[20]; 
        char str_logs[30];
        for (int l=0; l<shmp->cnt_gbl; l++){
            timeinfo = localtime(&shmp->log[l].t); 
            strftime(buff_logs, 20, "%T", timeinfo); 

            
            sprintf(str_logs, "%d", shmp->log[l].id);
            pid_val_logs_lbl = gtk_label_new (str_logs);
            sprintf(str_logs, "%s", buff_logs);
            datetime_val_logs_lbl = gtk_label_new (str_logs);
            sprintf(str_logs, "%d", shmp->log[l].key);
            key_val_logs_lbl = gtk_label_new (str_logs);

            gtk_widget_show (pid_val_logs_lbl);
            gtk_widget_show (datetime_val_logs_lbl);
            gtk_widget_show (key_val_logs_lbl);

            gtk_table_attach_defaults (GTK_TABLE (logs), pid_val_logs_lbl, 0, 1, l+1, l+2);
            gtk_table_attach_defaults (GTK_TABLE (logs), datetime_val_logs_lbl, 1, 2, l+1, l+2);
            gtk_table_attach_defaults (GTK_TABLE (logs), key_val_logs_lbl, 2, 3, l+1, l+2);
            
        }
        gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll_wndw), logs);

        // gtk_table_attach_defaults (GTK_TABLE (grid), name_ttl, 0, 1, 0, 1);
        // gtk_table_attach_defaults (GTK_TABLE (grid), table, 0, 1, 1, 2);
        // gtk_table_attach_defaults (GTK_TABLE (grid), log_ttl, 1, 2, 0, 1);
        // gtk_table_attach_defaults (GTK_TABLE (grid), scroll_wndw, 1, 2, 1, 2);
        gtk_table_attach_defaults (GTK_TABLE (grid), table, 0, 1, 0, 1);
        gtk_table_attach_defaults (GTK_TABLE (grid), scroll_wndw, 2, 3, 0, 1);

        gtk_container_add (GTK_CONTAINER (data->window), grid);
        gtk_widget_show (logs);
        gtk_widget_show (scroll_wndw);
        gtk_widget_show (table);
        gtk_widget_show (grid);
        gtk_widget_show (data->window);
        data->table = table;
        data->grid = grid;
        /* release GTK thread lock */
        gdk_threads_leave ();

        
        
        if (sem_post(&shmp->sem2) == -1){
            errExit("sem_post");
        }
            
    }

}


void destroy (GtkWidget *widget, gpointer data)
{
  gtk_main_quit ();
}

int
main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));
    if (argc < 2) {
        fprintf(stderr, "Usage: %s /shm-path\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    GtkWidget *window;
    GtkWidget *pid_lbl;
    GtkWidget *datetime_lbl;
    GtkWidget *key_lbl;
    GtkWidget *grid;
    GtkWidget *table;
    t_args t1_args;
    pthread_t tid;

    /* init threads */
    gdk_threads_init ();
    gdk_threads_enter ();

    /* init gtk */
    gtk_init(&argc, &argv);

    /* init random number generator */
    srand ((unsigned int) time (NULL));

    /* create a window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_signal_connect (GTK_OBJECT (window), "destroy",
                GTK_SIGNAL_FUNC (destroy), NULL);

    gtk_container_set_border_width (GTK_CONTAINER (window), 20);

    /* create a grid */
    grid = gtk_table_new (1, 3, TRUE);
    table = gtk_table_new (atoi(argv[2]), 2, TRUE);
    gtk_table_attach_defaults (GTK_TABLE (grid), table, 0, 1, 0, 1);
    gtk_container_add (GTK_CONTAINER (window), grid);
    // Set headers
    set_waiting_screen(grid);
    
    /* show everything */
    gtk_widget_show (table);
    gtk_widget_show (grid);
    gtk_widget_show (window);

    /* create the threads */
    t1_args.window = window;
    t1_args.table = table;
    t1_args.grid = grid;
    t1_args.shmpath = argv[1];
    t1_args.buff_size = atoi(argv[2]);
    pthread_create (&tid, NULL, read_buf, &t1_args);

    /* enter the GTK main loop */
    gtk_main ();
    gdk_threads_leave ();

    return 0;
}