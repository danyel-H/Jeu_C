/**
 * @author Hocquigny/De Sousa
 * @version 18/01/2019
 **/

#include <stdlib.h>     /* Pour EXIT_SUCCESS */
#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h> /* Pour pause */
#include <signal.h> /* Pour signal */

#include "ncurses.h"
#include "files.h"
#include "string.h"
#include "game.h"
#include "sockets.h"

#define LIGNES_MAX 25
#define COLONNES_MAX 40

lemming lemMaster[5], lemSlave[5];
unsigned char nb_lignes, nb_colonnes, *board;

/*
Arguments :
1 : nom de fichier
2 : Adresse IP du serveur
3 : Port serveur
4 : Port TCP 
*/

void handler_mouvement(int signum)
{
    int i;

    for (i = 0; i < 5; i++)
    {
        /*Mouvement aléatoire des lemmings du Master*/
        switch (rand() % 4)
        {
        case 0:
            if(get_case(board, nb_colonnes, lemMaster[i].x+1, lemMaster[i].y) == 1) {break;};/*On vérifie qu'il n'y a pas de mur*/
            if(lemMaster[i].type == TYPE_TERRAIN)
                lemMaster[i].x = (lemMaster[i].x == nb_colonnes - 1) ? lemMaster[i].x : lemMaster[i].x + 1;
            break;
        case 1:
            if(get_case(board, nb_colonnes, lemMaster[i].x-1, lemMaster[i].y) == 1) {break;};
            if(lemMaster[i].type == TYPE_TERRAIN)
                lemMaster[i].x = (lemMaster[i].x == 0) ? lemMaster[i].x : lemMaster[i].x - 1;
            break;
        case 2:
            if(get_case(board, nb_colonnes, lemMaster[i].x, lemMaster[i].y+1) == 1) {break;};
            if(lemMaster[i].type == TYPE_TERRAIN)
                lemMaster[i].y = (lemMaster[i].y == nb_lignes - 1) ? lemMaster[i].y : lemMaster[i].y + 1;
            break;
        case 3:
            if(get_case(board, nb_colonnes, lemMaster[i].x, lemMaster[i].y-1) == 1) {break;};
            if(lemMaster[i].type == TYPE_TERRAIN)
                lemMaster[i].y = (lemMaster[i].y == 0) ? lemMaster[i].y : lemMaster[i].y - 1;
            break;
        }
    }
    for (i = 0; i < 5; i++)
    {
        /*Mouvement aléatoire des lemmings du Slave*/
        switch (rand() % 4)
        {
        case 0:
            if(get_case(board, nb_colonnes, lemSlave[i].x+1, lemSlave[i].y) == 1) {break;};
            if(lemSlave[i].type == TYPE_TERRAIN)
                lemSlave[i].x = (lemSlave[i].x == nb_colonnes - 1) ? lemSlave[i].x : lemSlave[i].x + 1;
            break;
        case 1:
            if(get_case(board, nb_colonnes, lemSlave[i].x-1, lemSlave[i].y) == 1) {break;};
            if(lemSlave[i].type == TYPE_TERRAIN)
                lemSlave[i].x = (lemSlave[i].x == 0) ? lemSlave[i].x : lemSlave[i].x - 1;
            break;
        case 2:
            if(get_case(board, nb_colonnes, lemSlave[i].x, lemSlave[i].y+1) == 1) {break;};
            if(lemSlave[i].type == TYPE_TERRAIN)
                lemSlave[i].y = (lemSlave[i].y == nb_lignes - 1) ? lemSlave[i].y : lemSlave[i].y + 1;
            break;
        case 3:
            if(get_case(board, nb_colonnes, lemSlave[i].x, lemSlave[i].y-1) == 1) {break;};
            if(lemSlave[i].type == TYPE_TERRAIN)
                lemSlave[i].y = (lemSlave[i].y == 0) ? lemSlave[i].y : lemSlave[i].y - 1;
            break;
        }
    }
    alarm(5);
}

int main(int argc, char *argv[]) 
{
    int ch, sockfd, fd,i, flags, lemming_choisi=0,k,l,j, isOver = 0;
    unsigned short port_tcp;
    unsigned char type;
    struct sockaddr_in adresseServeur;
	socklen_t taille_s = sizeof(adresseServeur);
    WINDOW* fenetre;
    WINDOW* fenetre2;
    WINDOW* fenetre3;
    WINDOW* sous_fenetre;
    WINDOW* sous_fenetre2;
    WINDOW* sous_fenetre3;
    int sourisX, sourisY, bouton;
    char *nomFichier;
    struct sigaction action;

    if(argc == 5)
    {
        nomFichier = get_chemin_fichier(argv[1]);        
        fd = ouvrir_fichier(nomFichier);
        nb_lignes = get_hauteur(fd);
        nb_colonnes = get_largeur(fd);
        port_tcp = atoi(argv[4]);
        board = get_board(fd);

        srand(time(NULL));

        /* Positionnement du gestionnaire pour SIGALRM */
        action.sa_handler = handler_mouvement;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        if (sigaction(SIGALRM, &action, NULL) == -1)
        {
            perror("Erreur lors du positionnement ");
            exit(EXIT_FAILURE);
        }

        printf("Attente de connexion\n");

        /* Creation de la socket */
        if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
            perror("Erreur lors de la creation de la socket ");
            exit(EXIT_FAILURE);
        }
            
        /* Creation de l'adresse du serveur */
        memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
        adresseServeur.sin_family = AF_INET;
        adresseServeur.sin_port = htons(atoi(argv[3]));
        if(inet_pton(AF_INET, argv[2], &adresseServeur.sin_addr) != 1) {
            perror("Erreur lors de la conversion de l'adresse ");
            exit(EXIT_FAILURE);
        }
        
        /* Initialisation de ncurses */
        ncurses_initialiser();
        ncurses_souris();
        ncurses_couleurs();
        bkgd(COLOR_PAIR(1));

        refresh();

        /* Vérification des dimensions du terminal */
        if((COLS < nb_colonnes+10) || (LINES < nb_lignes+10))
        {
            ncurses_stopper();
            fprintf(stderr, 
                    "Les dimensions du terminal sont insufisantes");
            exit(EXIT_FAILURE);
        } 

        type = CONNEXION_SERVEUR_MASTER;

        envoyer_udp(sockfd, type, sizeof(unsigned char), adresseServeur);
        if (sendto(sockfd, &port_tcp, sizeof(unsigned short), 0, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) == -1)
        {
            perror("Erreur lors de l'envoi du message ");
            exit(EXIT_FAILURE);
        }

        /* Fermeture de la socket */
        if(close(sockfd) == -1) {
            perror("Erreur lors de la fermeture de la socket ");
            exit(EXIT_FAILURE);
        }


        /*Attente de connexion*/
        /* Creation de la socket */
        if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
            perror("Erreur lors de la creation de la socket ");
            exit(EXIT_FAILURE);
        }
        
        /* Creation de l'adresse du serveur */
        memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
        adresseServeur.sin_family = AF_INET;
        adresseServeur.sin_addr.s_addr = htonl(INADDR_ANY);
        adresseServeur.sin_port = htons(atoi(argv[4]));
        
        /* Nommage de la socket */
        if(bind(sockfd, (struct sockaddr*)&adresseServeur, sizeof(struct sockaddr_in)) == -1) {
            perror("Erreur lors du nommage de la socket ");
            exit(EXIT_FAILURE);
        }
        
        /* Mise en mode passif de la socket */
        if(listen(sockfd, 1) == -1) {
            perror("Erreur lors de la mise en mode passif ");
            exit(EXIT_FAILURE);
        }
        
        /* Attente d'une connexion */
        printf("Serveur : attente de connexion...\n");
        if((sockfd = accept(sockfd, (struct sockaddr*)&adresseServeur, &taille_s)) == -1) {
            perror("Erreur lors de la demande de connexion ");
            exit(EXIT_FAILURE);
        }

        /*Envoi du plateau*/
        type = ENVOI_CARTE;

        /* Envoi de la valeur 3*/
        envoyer_tcp(sockfd, type, sizeof(unsigned char));

        /* Envoi du nombre de ligne*/
        envoyer_tcp(sockfd, nb_lignes, sizeof(unsigned char));

        /* Envoi du nombre de colonne*/
        envoyer_tcp(sockfd, nb_colonnes, sizeof(unsigned char));

        /* Envoi du plateau*/
        if(write(sockfd, board, sizeof(unsigned char)*(nb_lignes*nb_colonnes)) == -1) {
            perror("Erreur lors de l'envoi du message ");
            exit(EXIT_FAILURE);
        }
        
        /*Attente de reception d'un OK du Slave*/
        do{
            type = lire_tcp(sockfd);
        } while (type != 4);

        printf("Message reçu : %d\n", type);



        /* Creation de la fenêtre */
        /*Fenêtre d'infos*/
        fenetre = newwin(8, COLS , 0, 0);
        box(fenetre, 0, 0);
        sous_fenetre = subwin(fenetre, 7 - 1, COLS - 1,  0+ 1,  0+ 1);

        /*Fenêtre du labyrinthe*/
        fenetre2 = newwin(nb_lignes+2, nb_colonnes+2, 8, 0);
        box(fenetre2, 0, 0);
        sous_fenetre2 = subwin(fenetre2, nb_lignes, nb_colonnes,  8+ 1,  0+ 1);

        /*Fenêtre des infos du joueur*/
        fenetre3 = newwin(14, COLS - COLONNES_MAX - 2, 8, COLONNES_MAX + 2);
        box(fenetre3, 0, 0);
        sous_fenetre3 = subwin(fenetre3, 12, COLS - COLONNES_MAX - 4, 8 + 1, COLONNES_MAX + 3);

        scrollok(sous_fenetre, TRUE);
        scrollok(sous_fenetre2, FALSE);
        scrollok(sous_fenetre3, FALSE);

        mvwprintw(fenetre, 0, 1, "Infos");
        mvwprintw(fenetre2, 0, 1, "Jeu");
        mvwprintw(fenetre3, 0, 1, "Légende");

        generer_labyrinthe_jeu(sous_fenetre2, board, nb_colonnes, nb_lignes);
        afficher_infos_labyrinthe(sous_fenetre, sous_fenetre2, sous_fenetre3,board);

        /* Création d'un cadre */
        wrefresh(fenetre);
        wrefresh(fenetre2);
        wrefresh(fenetre3);

        for(i = 0; i < 5; i++){
            lemSlave[i].type = TYPE_RETIRE;
            lemMaster[i].type = TYPE_RETIRE;
        }

        flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

        alarm(5);
        timeout(200);
        while((ch = getch()) != KEY_F(2)) 
        { 
            /*Envoi périodique des lemmings*/
            type = ACTUALISATION;
            envoyer_tcp(sockfd, type, sizeof(unsigned char));
            for(i = 0; i < 5; i++)
            {
                envoyer_tcp(sockfd, lemMaster[i].type, sizeof(unsigned char));
                if(lemMaster[i].type == TYPE_TERRAIN || lemMaster[i].type == TYPE_BLOQUE) 
                {
                    envoyer_tcp(sockfd, lemMaster[i].x, sizeof(unsigned char));
                    envoyer_tcp(sockfd, lemMaster[i].y, sizeof(unsigned char));
                }
                else 
                {
                    type = 0; /*Utilisation de la variable type pour transmettre des coordonnées placées à 0*/
                    envoyer_tcp(sockfd, type, sizeof(unsigned char));
                    envoyer_tcp(sockfd, type, sizeof(unsigned char));
                }
            }
            for(i = 0; i < 5; i++)
            {
                envoyer_tcp(sockfd, lemSlave[i].type, sizeof(unsigned char));
                    /*printf("Je viens d'envoyer le type qui est de %d", lemSlave[i].type);*/
                if(lemSlave[i].type == TYPE_TERRAIN || lemSlave[i].type == TYPE_BLOQUE) 
                {
                    envoyer_tcp(sockfd, lemSlave[i].x, sizeof(unsigned char));
                    envoyer_tcp(sockfd, lemSlave[i].y, sizeof(unsigned char));
                }
                else 
                {
                    type = 0; /*Utilisation de la variable type pour transmettre des coordonnées placées à 0*/
                    envoyer_tcp(sockfd, type, sizeof(unsigned char));
                    envoyer_tcp(sockfd, type, sizeof(unsigned char));
                }
            }
            afficher_infos_lemmings(sous_fenetre3, lemMaster, lemSlave, 0);

            type = lire_tcp(sockfd);
            if(type == AJOUT_LEMMING || type == RETRAIT_LEMMING || type == BLOCAGE_LEMMING ) /*Ajout ou suppression ou blocage de lemming*/
            {
                if(type == AJOUT_LEMMING)
                {
                    i = lire_tcp(sockfd);
                    lemSlave[i].x = lire_tcp(sockfd);
                    lemSlave[i].y = lire_tcp(sockfd);
                    lemSlave[i].type = TYPE_TERRAIN;
                    lemSlave[i].nb_retrait++;
                }
                else if (type == RETRAIT_LEMMING)
                {
                    i = lire_tcp(sockfd);
                    lemSlave[i].type = TYPE_RETIRE;
                }
                else
                {
                    i = lire_tcp(sockfd);
                    lemSlave[i].type = TYPE_BLOQUE;
                }
            }
            else if(type == EXPLOSION_LEMMING) /*Le slave a fait exploser un Lemming*/
            {
                i = lire_tcp(sockfd);
                lemSlave[i].type = TYPE_MORT;
                explosion_lemming(sous_fenetre2, board,lemSlave[i].x, lemSlave[i].y);
                for (k = lemSlave[i].x - 2; k < lemSlave[i].x + 3; k++)/*Parcours de la zone de l'explosion*/
                {
                    for (l = lemSlave[i].y - 2; l < lemSlave[i].y + 3; l++)
                    {
                        for (j = 0; j < 5; j++)/*Parcours de tous les Lemmings*/
                        {
                            if((lemMaster[j].x == k && lemMaster[j].y == l))
                            {
                                lemMaster[j].type = TYPE_MORT;
                                lemSlave[i].type = TYPE_RETIRE;
                            }
                            else if((lemSlave[j].x == k && lemSlave[j].y == l))
                                lemSlave[j].type = TYPE_MORT;
                        }
                    }
                }
            }
    
            /*On regarde si le jeu est terminé*/
            if((isOver = (partie_finie(lemMaster, lemSlave))))
            {
                type = MORT;
                envoyer_tcp(sockfd, type, sizeof(unsigned char));
                if (isOver == -1)
                {
                    type = 1; /*Joueur 2 = SLAVE*/
                    envoyer_tcp(sockfd, type, sizeof(unsigned char));
                    ecran_perdu(sous_fenetre2, nb_lignes, nb_colonnes);
                }
                else if(isOver == 1)
                {
                    type = 2; /*Joueur 1 = MASTER*/
                    envoyer_tcp(sockfd, type, sizeof(unsigned char));
                    ecran_gagner(sous_fenetre2, nb_lignes, nb_colonnes);
                }
                break;
            }

            generer_labyrinthe_jeu(sous_fenetre2, board, nb_colonnes, nb_lignes);
            actualiser_plateau(sous_fenetre2, lemMaster, lemSlave);

            if((ch == KEY_MOUSE) && (souris_getpos(&sourisX, &sourisY, &bouton) == OK))
            {
                /*L'utilisateur clique sur le labyrinthe*/
                   if(wmouse_trafo(sous_fenetre2, &sourisY, &sourisX, 0))
                    {
                        if (bouton & BUTTON1_CLICKED)/*Ajout et retrait de lemming*/
                        {
                            if (get_case(board, nb_colonnes, sourisX, sourisY) == 0)
                            {
                                if (!lemMaster[lemming_choisi].type)
                                {
                                    lemMaster[lemming_choisi].type = TYPE_TERRAIN;
                                    lemMaster[lemming_choisi].x = sourisX;
                                    lemMaster[lemming_choisi].y = sourisY;
                                    lemMaster[lemming_choisi].action_recente = 1; /*Afin de ne pas retirer le lemming après l'avoir mis*/
                                }
                            }

                            for (i = 0; i < 5; i++)/*On regarde si l'utilisateur a cliqué sur un lemming déjà placé*/
                            {
                                if (lemMaster[i].type == TYPE_TERRAIN && (lemMaster[i].x == sourisX && lemMaster[i].y == sourisY) && 
                                lemMaster[i].nb_retrait < 4 && !lemMaster[i].action_recente)
                                {
                                    lemMaster[i].type = TYPE_RETIRE;
                                    lemMaster[i].nb_retrait++;
                                }
                            }

                            lemMaster[lemming_choisi].action_recente = 0;
                        }
                        else if (bouton & BUTTON3_CLICKED)/*Cas de l'explosion*/
                        {
                            for (i = 0; i < 5; i++) /*Parcours des lemmings*/
                            {
                                if ((lemMaster[i].type == TYPE_TERRAIN || lemMaster[i].type == TYPE_BLOQUE) && 
                                (lemMaster[i].x == sourisX && lemMaster[i].y == sourisY))
                                {
                                    lemMaster[i].type = TYPE_MORT;
                                    explosion_lemming(sous_fenetre2, board, lemMaster[i].x, lemMaster[i].y);

                                    for (k = lemMaster[i].x - 2; k < lemMaster[i].x + 3; k++) /*Parcours de la zone de l'explosion*/
                                    {
                                        for (l = lemMaster[i].y - 2; l < lemMaster[i].y + 3; l++)
                                        {
                                            for (j = 0; j < 5; j++) /*Parcours de tous les Lemmings afin de vérifier lesquels sont touchés*/
                                            {
                                                if ((lemSlave[j].x == k && lemSlave[j].y == l))
                                                {
                                                    lemSlave[j].type = TYPE_MORT;
                                                    lemMaster[i].type = TYPE_RETIRE;
                                                }
                                                else if ((lemMaster[j].x == k && lemMaster[j].y == l))
                                                    lemMaster[j].type = TYPE_MORT;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (bouton & BUTTON1_DOUBLE_CLICKED) /*Blocage de Lemming*/
                        {
                            for (i = 0; i < 5; i++) /*On regarde si l'utilisateur a cliqué sur un Lemming*/
                            {
                                if (lemMaster[i].type == TYPE_TERRAIN && lemMaster[i].x == sourisX && lemMaster[i].y == sourisY)
                                {
                                    lemMaster[i].type = TYPE_BLOQUE;
                                }
                            }
                        }
                    }
            }
            else if(ch >= LEMMING_1 && ch <= LEMMING_5)
            {
                lemming_choisi = ch - 49;
                mvwprintw(sous_fenetre3,12,0,"Lemming choisi : %d", lemming_choisi+1);
            }   

            wrefresh(sous_fenetre2);
            wrefresh(sous_fenetre3);
            wrefresh(fenetre3);
        }

        if (isOver)
        {
            while ((ch = getch()) != KEY_F(2)){}
        }

        /* Suppression de la fenêtre */
        delwin(fenetre);
        delwin(fenetre2);
        delwin(fenetre3);

        /* Arrêt de ncurses */
        ncurses_stopper();


        /* Fermeture du fichier */
        if(close(fd) == -1) 
        {
            perror("Erreur close ");
            exit(EXIT_FAILURE);
        }
        if (close(sockfd) == -1)
        {
            perror("Erreur close ");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("Vous n'avez pas spécifié le bon nombre d'arguments\n");
        printf("Pour rappel, le bon format est ./client_master nomMap ip_serveur port_serveur port_tcp\n");
        printf("Le port tcp est libre de votre choix, Cependant, il est préferable d'en choisir un > 1000");
        exit(EXIT_FAILURE);
    }

    /* Fermeture de la socket */
    if(close(sockfd) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}