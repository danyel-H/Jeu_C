#include <stdlib.h>     /* Pour EXIT_SUCCESS */
#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>  /* Pour socket, bind */
#include <arpa/inet.h>   /* Pour sockaddr_in */
#include <string.h>      /* Pour memset */
#include <errno.h>       /* Pour errno */

struct cellule{
    struct sockaddr_in adresseClient;
    unsigned short port_tcp;
    struct cellule* queue;
};

typedef struct{
    struct cellule* debut;
    struct cellule* fin;
} file_attente;

void push_back(struct sockaddr_in, file_attente*, unsigned short);
void decaler(file_attente*);
void afficher_f(file_attente*);
void envoyer_tcp(int, unsigned char, size_t);
unsigned char lire_tcp(int);
void envoyer_udp(int, unsigned char, size_t, struct sockaddr_in);
unsigned char lire_udp(int);
