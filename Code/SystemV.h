#include <stdlib.h>     /* Pour EXIT_SUCCESS */
#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>    /* Pour shmget, shmat, shmdt */
#include <errno.h>      /* Pour errno */
#include <sys/sem.h>  /* Pour semget, semop */
#include <sys/ipc.h>
#include <sys/msg.h>

#define REQUETES_CONTROLEUR -2
#define DEMANDE_IPC 1
#define ARRET_PROGRAMME 2
#define REPONSE_IPC 3

typedef struct{
  int cle_smp;
  int cle_sema;
} reponse_co_t;

/* Structure utilisee pour les requetes */
typedef struct {
  long type;
  int cible;
  union{
    int pid;
    reponse_co_t ipcs;
  } u1;
} requete_t;

typedef struct {
  long type;
  char chaine[30];
} reponse_t;

int creer_file(int, int);
void supprimer_file(int);
void envoyer_requete(int, requete_t);
requete_t receive_requete(int, requete_t, int);
int op_sema(int, int, int, int);
void supprimer_sema(int);
void init_sema(int, int, int);
void* attacher_smp(int);
int get_val(int, int);
int init_tableau(int, int, int);
void supprimer_smp(int);
int creer_smp(int, int, int);
void set_mur_ipc(void*, int , int , unsigned char);
unsigned char get_mur_ipc(void*, int, int);
unsigned char get_posY_ipc(void*);
unsigned char get_posX_ipc(void*);
int get_nb_murs_caches(void*);
int get_nb_murs_decouverts(void*);
unsigned char get_vie_ipc(void*);
void set_vie_ipc(void*, unsigned char);
