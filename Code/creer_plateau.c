/**
 * @author Hocquigny/De Sousa
 ***/

#include <stdlib.h>     /* Pour EXIT_SUCCESS */
#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "ncurses.h"
#include "files.h"
#include "string.h"
#include "game.h"

#define LIGNES_MAX 40
#define COLONNES_MAX 25

/*
Arguments :
1 : nom_fichier
2 : nombre de colonnes
3 : nombres de lignes
*/

int main(int argc, char *argv[]) 
{
    int i,j,  fd;
    unsigned char **board, nb_lignes,nb_colonnes;

    if(argc != 4)
    {
        printf("Pas assez d'arguments\n");
        printf("Format : nom_fichier nb_lignes nb_colonnes\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    nb_lignes = atoi(argv[2]);
    nb_colonnes = atoi(argv[3]);

    /* Vérification des dimensions du terminal */
    if((nb_colonnes > COLONNES_MAX) || (nb_lignes > LIGNES_MAX))
    {
        fprintf(stderr, 
                "Les dimensions de votre plateau sont trop grandes");
        exit(EXIT_FAILURE);
    } 

    fd = creer_fichier(argv[1], nb_lignes, nb_colonnes);

    if((lseek(fd, 0L, SEEK_SET)) == -1)
    {
        perror("Erreur lseek ");
        exit(EXIT_FAILURE);
    }

    /*On récupère le nombre de lignes et de colonnes afin de mettre à jour
    les nombres si le fichier existe déjà*/
    if(read(fd, &nb_colonnes, sizeof(unsigned char)) == -1)
    {
        perror("Erreur read ");
        exit(EXIT_FAILURE);
    }
    if(read(fd, &nb_lignes, sizeof(unsigned char)) == -1)
    {
        perror("Erreur read ");
        exit(EXIT_FAILURE);
    }

    board = (unsigned char **) malloc(sizeof(unsigned char*) * nb_lignes);
    for(i = 0; i < nb_lignes; i++)
    {
        board[i] = (unsigned char*) malloc(sizeof(unsigned char)* nb_colonnes);
    }

    printf("Votre plateau : \n");
    for(i = 0; i < nb_lignes; i++)
    {
        for(j = 0; j < nb_colonnes; j++)
        {
            if(read(fd, &(board[i][j]), sizeof(unsigned char)) == -1)
            {
                perror("Erreur read ");
                exit(EXIT_FAILURE);
            }
            printf("%d ", board[i][j]);
        }
    }
    
    for(i = 0; i < nb_lignes; i++)
    {
        free(board[i]);
    }
    free(board);
	
    close(fd);

    return EXIT_SUCCESS;
}