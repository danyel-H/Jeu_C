#include "string.h"

#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


char* get_chemin_fichier(char *);
int creer_fichier(char*, unsigned char, unsigned char);
int ouvrir_fichier(char*);

void set_board(int, unsigned char**, unsigned char, unsigned char);
unsigned char get_mur(int, int, int);
void set_mur(int, int, int, unsigned char);
unsigned char get_hauteur(int);
unsigned char get_largeur(int);

void set_pos(int, unsigned char, unsigned char);
unsigned char get_posY(int);
unsigned char get_posX(int);

char* get_nom_labyrinthe(char*);