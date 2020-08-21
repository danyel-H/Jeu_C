#include "ncurses.h"
#include "files.h"
#include "SystemV.h"


#include <stdlib.h>     /* Pour EXIT_SUCCESS */
#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define CONNEXION_SERVEUR_MASTER 1
#define CONNEXION_SERVEUR_SLAVE 2
#define ENVOI_CARTE 3
#define OK_CARTE 4
#define ACTUALISATION 5
#define MORT 6
#define PAUSE 7
#define SORTIE_PAUSE 8
#define AJOUT_LEMMING 9
#define RETRAIT_LEMMING 10
#define EXPLOSION_LEMMING 11
#define BLOCAGE_LEMMING 12
#define DEMANDE_PAUSE 13
#define DEMANDE_SORTIE_PAUSE 14

#define TYPE_RETIRE 0
#define TYPE_TERRAIN 1
#define TYPE_MORT 2
#define TYPE_BLOQUE 3

#define LEMMING_1 49
#define LEMMING_2 50
#define LEMMING_3 51
#define LEMMING_4 52
#define LEMMING_5 53

typedef struct{
    unsigned char type;
    unsigned char nb_retrait;
    unsigned char x;
    unsigned char y;
    unsigned char action_recente;
} lemming;

void generer_labyrinthe_jeu(WINDOW *, unsigned char*, unsigned char, unsigned char);
unsigned char * get_board(int);
void actualiser_plateau(WINDOW*, lemming[5], lemming[5]);
void explosion_lemming(WINDOW *, unsigned char *, int, int);
unsigned char get_case(unsigned char *,unsigned char,unsigned char, unsigned char);
int partie_finie(lemming[5], lemming[5]);
void ecran_gagner(WINDOW*, int, int);
void ecran_perdu(WINDOW*, int, int);
void afficher_infos_editeur(WINDOW*, WINDOW*, WINDOW*, int);
void afficher_infos_labyrinthe(WINDOW*, WINDOW*, WINDOW*, unsigned char*);
void afficher_infos_lemmings(WINDOW*, lemming[5], lemming[5], int);