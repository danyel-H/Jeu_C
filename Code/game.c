#include "game.h"

/*Renvoie le plateau stocké dans un fichier à travers un tableau dynamiquement alloué*/
unsigned char * get_board(int fd)
{
	unsigned char *board, nb_lignes, nb_colonnes;

    lseek(fd, 0L, SEEK_SET);    
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
    
    board = (unsigned char *) malloc(sizeof(unsigned char) * (nb_lignes*nb_colonnes));
    
    if(read(fd, board, sizeof(unsigned char)* (nb_lignes*nb_colonnes)) == -1)
    {
        perror("Erreur read ");
        exit(EXIT_FAILURE);
    }

    return board;
}

/*Permet de placer les lemmings sur le labyrinthe*/
void actualiser_plateau(WINDOW *sous_fenetre2, lemming lemMaster[5], lemming lemSlave[5])
{
    int i;

    /*Actualisation des lemmings du Master*/
    for(i = 0; i < 5; i++)
    {
        if(lemMaster[i].type == TYPE_TERRAIN || lemMaster[i].type == TYPE_BLOQUE)
        {
            if(lemMaster[i].type == TYPE_TERRAIN)
            {
                wattron(sous_fenetre2,COLOR_PAIR(6));
                mvwaddch(sous_fenetre2, lemMaster[i].y, lemMaster[i].x, ' ');
                wattroff(sous_fenetre2,COLOR_PAIR(6));
            }
            else 
            {
                wattron(sous_fenetre2, COLOR_PAIR(8));
                mvwaddch(sous_fenetre2, lemMaster[i].y, lemMaster[i].x, ' ');
                wattroff(sous_fenetre2,COLOR_PAIR(8));
            }
        }
    }

    /*Actualisation des lemmings du Client*/
    for(i = 0; i < 5; i++)
    {
        if(lemSlave[i].type == TYPE_TERRAIN || lemSlave[i].type == TYPE_BLOQUE)
        {
            if(lemSlave[i].type == TYPE_TERRAIN)
            {
                wattron(sous_fenetre2,COLOR_PAIR(7));
                mvwaddch(sous_fenetre2, lemSlave[i].y, lemSlave[i].x, ' ');
                wattroff(sous_fenetre2,COLOR_PAIR(7));
            }
            else 
            {
                wattron(sous_fenetre2,COLOR_PAIR(9));
                mvwaddch(sous_fenetre2, lemSlave[i].y, lemSlave[i].x, ' ');
                wattroff(sous_fenetre2,COLOR_PAIR(9));
            }
        }
    }
}


/*
Récupère une case du plateau de jeu via des coordonnées X et Y
*/
unsigned char get_case(unsigned char *board,unsigned char nb_colonnes, unsigned char posX, unsigned char posY)
{
	int y, nb_car;

	y = nb_colonnes * posY;
	nb_car = y+posX;

    return board[nb_car];
}

/*Génère le labyrinthe à partir du tableau du plateau et des dimensions*/
void generer_labyrinthe_jeu(WINDOW *sous_fenetre2, unsigned char* board, unsigned char largeur, unsigned char hauteur)
{
	int i,j, k = 0;

	for(i =0; i < hauteur; i++)
        {
            for(j = 0; j < largeur; j++)
            {
                if(board[k] == 0)
                {
                    wattron(sous_fenetre2,COLOR_PAIR(1));
                    mvwaddch(sous_fenetre2, i, j, ' ');
                    wattroff(sous_fenetre2,COLOR_PAIR(1));
                }
                else if(board[k] == 1) /* Mur */
                {
                    wattron(sous_fenetre2, COLOR_PAIR(5));
                    mvwaddch(sous_fenetre2, i, j, ' ');
                    wattroff(sous_fenetre2, COLOR_PAIR(5));
                }
                k++;
            }
        }
}

/*Génère une explosion de lemming autour de ses coordonnées*/
void explosion_lemming(WINDOW *sous_fenetre2, unsigned char *board, int sourisX, int sourisY)
{
    int i,j;

    wattron(sous_fenetre2, COLOR_PAIR(4));
    for (i = sourisX - 2; i < sourisX +3; i++)
    {
        for (j = sourisY - 2; j < sourisY +3; j++)
        {
            mvwaddch(sous_fenetre2, j, i, ' ');
        }
    }
    wattroff(sous_fenetre2, COLOR_PAIR(4));
}

/*
Affiche les infos utiles à l'utilisateur sur l'interface de l'éditeur
*/
void afficher_infos_editeur(WINDOW* sous_fenetre, WINDOW* sous_fenetre2, WINDOW* sous_fenetre3, int fd)
{
	 	mvwprintw(sous_fenetre, 0,0, "Appuyez sur F2 pour quitter l'éditeur\nCliquez dans le labyrinthe afin de changer l'état d'un mur.\
        \nVous pouvez changer le nombre de vies en cliquant sur + et - dans la fenêtre d'etat");

        /*Actualisation de l'état*/

        wattron(sous_fenetre3, COLOR_PAIR(5));
        mvwaddch(sous_fenetre3, 1,0, ' ');
        wattroff(sous_fenetre3, COLOR_PAIR(5));
        mvwprintw(sous_fenetre3,1,1," : Mur visible");

}

/*Affiche les infos utiles à l'utilisateur sur l'interface du jeu
@param mode client choisi, 0 = master, 1 = slave
*/
void afficher_infos_labyrinthe(WINDOW* sous_fenetre, WINDOW* sous_fenetre2, WINDOW* sous_fenetre3, unsigned char * board)
{
	mvwprintw(sous_fenetre, 0,0, "Appuyez sur F2 pour quitter le jeu, votre but est d'exploser tous les lemmings adverse.\n\
Appuyez sur 1,2,3,4 ou 5 pour choisir un lemming et cliquez pour le placer, recliquez dessus pour le retirer.\n\
Si vous retirez un lemming en ayant choisi un lemming différent, vous placerez le lemming choisi à la place\n\
Faites un clic droit sur un lemming pour le faire exploser.\n\
Double-cliquez sur un lemming pour le bloquer.");


    /*Actualisation de l'état*/
    wattron(sous_fenetre3, COLOR_PAIR(6));
    mvwaddch(sous_fenetre3,0,0, ' ');
    wattroff(sous_fenetre3, COLOR_PAIR(6));
    mvwprintw(sous_fenetre3,0,1," : Lemmings Master");

    wattron(sous_fenetre3, COLOR_PAIR(7));
    mvwaddch(sous_fenetre3,1,0, ' ');
    wattroff(sous_fenetre3, COLOR_PAIR(7));
    mvwprintw(sous_fenetre3,1,1," : Lemmings Slave");

    wattron(sous_fenetre3, COLOR_PAIR(8));
    mvwaddch(sous_fenetre3,2,0, ' ');
    wattroff(sous_fenetre3, COLOR_PAIR(8));
    mvwprintw(sous_fenetre3,2,1," : Lemmings Master Bloqué");

    wattron(sous_fenetre3, COLOR_PAIR(9));
    mvwaddch(sous_fenetre3,3,0, ' ');
    wattroff(sous_fenetre3, COLOR_PAIR(9));
    mvwprintw(sous_fenetre3,3,1," : Lemmings Slave Bloqué");

    wattron(sous_fenetre3, COLOR_PAIR(5));
    mvwaddch(sous_fenetre3, 4,0, ' ');
    wattroff(sous_fenetre3, COLOR_PAIR(5));
    mvwprintw(sous_fenetre3,4,1," : Mur visible");
}

/*Affiche les infos des lemmings utiles à l'utilisateur sur l'interface du jeu
@param mode client choisi, 0 = master, 1 = slave
*/
void afficher_infos_lemmings(WINDOW * sous_fenetre3, lemming lemMaster[5], lemming lemSlave[5], int mode)
{
    int i, k = 0;
    char etats[4][10] = {"Retiré", "En jeu", "Mort", "Bloqué"};

    for (i = 5; i < 10; i++)
    {
        mvwprintw(sous_fenetre3, i, 0, "                                                                   ");
        if(!mode)
            mvwprintw(sous_fenetre3, i, 0, "Lemming numéro %d, type:%s", k+1, etats[lemMaster[k].type]);
        else
            mvwprintw(sous_fenetre3, i, 0, "Lemming numéro %d, type:%s", k+1, etats[lemSlave[k].type]);

        k++;
    }
}

/*Détermine si la partie est finie
@return 0 si non finie, 1 si Victoire, -1 si défaite*/
int partie_finie(lemming lemMaster[5], lemming lemSlave[5])
{
    int i, lemSlave_morts =1, lemMaster_morts = 1;

    for (i = 0; i < 5; i++)
    {
        if (lemSlave[i].type != TYPE_MORT)
            lemSlave_morts = 0;

        if (lemMaster[i].type != TYPE_MORT)
            lemMaster_morts = 0;
    }

    if(lemSlave_morts)
            return 1;
    else if(lemMaster_morts)
            return -1;
    else
        return 0;
}

/*Affiche l'écran de victoire sur le labyrinthe du gagnant*/
void ecran_gagner(WINDOW* sous_fenetre2, int nb_lignes, int nb_colonnes)
{
	int i, j;
	wattron(sous_fenetre2, COLOR_PAIR(6));
	for(i = 0; i < nb_lignes; i++)
	{
		for(j = 0; j < nb_colonnes; j++)
		{
            mvwaddch(sous_fenetre2, i, j, ' ');
		}
	}

	mvwprintw(sous_fenetre2, nb_lignes/2, nb_colonnes/4, "GAGNE :)");
	mvwprintw(sous_fenetre2, nb_lignes/2+1, nb_colonnes/15, "F2 pour quitter");
	wattroff(sous_fenetre2, COLOR_PAIR(6));
	wrefresh(sous_fenetre2);
}

/*Affiche l'écran de défaite sur le labyrinthe du perdant*/
void ecran_perdu(WINDOW *sous_fenetre2, int nb_lignes, int nb_colonnes)
{
	int i, j;
	wattron(sous_fenetre2, COLOR_PAIR(4));
	for(i = 0; i < nb_lignes; i++)
	{
		for(j = 0; j < nb_colonnes; j++)
		{
            mvwaddch(sous_fenetre2, i, j, ' ');
		}
	}

	mvwprintw(sous_fenetre2, nb_lignes/2, nb_colonnes/4, "PERDU :(");
	mvwprintw(sous_fenetre2, nb_lignes/2+1, nb_colonnes/15, "F2 pour quitter");
	wattroff(sous_fenetre2, COLOR_PAIR(4));
	wrefresh(sous_fenetre2);
}