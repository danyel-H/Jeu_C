#include <stdlib.h>     /* Pour EXIT_SUCCESS */
#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "ncurses.h"
#include "files.h"
#include "string.h"
#include "game.h"
#include "sockets.h"

#define LIGNES_MAX 25
#define COLONNES_MAX 40

int main(int argc, char* argv[])
{
    unsigned char type, nb_lignes, nb_colonnes, *board;
	int sockfd, fd, i, flags, lemming_choisi = 0,  ch, isOver = 0;
    struct sockaddr_in adresseServeur, adresseMaster;
    unsigned short port_tcp;
    char message[16];
    WINDOW* fenetre;
    WINDOW* fenetre2;
    WINDOW* fenetre3;
    WINDOW* sous_fenetre;
    WINDOW* sous_fenetre2;
    WINDOW* sous_fenetre3;
    int sourisX, sourisY, bouton;
    lemming lemMaster[5], lemSlave[5];

    for(i = 0; i < 5; i++){
        lemSlave[i].type = TYPE_RETIRE;
        lemMaster[i].type = TYPE_RETIRE;
    }

	if(argc < 2)
	{
		printf("Veuillez renseigner le bon nombre d'arguments\n");
        printf("Pour rappel, le format est ./client_slave ip_serveur port_serveur\n");
		exit(EXIT_FAILURE);
	}

    srand(time(NULL));

    /* Creation de la socket */
    if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Erreur lors de la creation de la socket ");
        exit(EXIT_FAILURE);
    }
		 
    /* Creation de l'adresse du serveur */
    memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
    adresseServeur.sin_family = AF_INET;
    adresseServeur.sin_port = htons(atoi(argv[2]));
    if(inet_pton(AF_INET, argv[1], &adresseServeur.sin_addr) != 1) {
        perror("Erreur lors de la conversion de l'adresse ");
        exit(EXIT_FAILURE);
    }

    type = CONNEXION_SERVEUR_SLAVE;
     
    envoyer_udp(sockfd, type, sizeof(unsigned char), adresseServeur);

    /*Récepetion de l'adresse IP du Master*/
    if(recvfrom(sockfd, &message, sizeof(char) * 16, 0, NULL, NULL) == -1) {
        perror("Erreur lors de la reception du message ");
        exit(EXIT_FAILURE);
    } 
	printf("\nClient : message recu : %s.\n", message);

    /*Réception du port TCP sur lequel attends le Master*/
    if(recvfrom(sockfd, &port_tcp, sizeof(unsigned short), 0, NULL, NULL) == -1) {
        perror("Erreur lors de la reception du message ");
        exit(EXIT_FAILURE);
    } 
	printf("Client : port recu : %d.\n", port_tcp);

    /* Fermeture de la socket */
    if(close(sockfd) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    /*Connexion au client Master associé*/
    /* Creation de la socket */
    if((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Erreur lors de la creation de la socket ");
        exit(EXIT_FAILURE);
    }
    
    /* Remplissage de la structure */
    memset(&adresseMaster, 0, sizeof(struct sockaddr_in));
    adresseMaster.sin_family = AF_INET;
    if(inet_pton(AF_INET, message, &adresseMaster.sin_addr.s_addr) != 1) {
        perror("Erreur lors de la conversion de l'adresse ");
        exit(EXIT_FAILURE);
    }
    adresseMaster.sin_port = htons(port_tcp);

    printf("Attente du connexion.....\n");

    /* Connexion au serveur */
    if(connect(fd, (struct sockaddr*)&adresseMaster, sizeof(adresseMaster)) == -1) {
        perror("Erreur lors de la connexion ");
        printf("erreur : %d\n",errno);
        exit(EXIT_FAILURE);
    }

    /* Attente du OK*/
    do{
        type = lire_tcp(fd);
    }
    while(type != ENVOI_CARTE);
    printf("Message reçu : %d\n", type);


    /* Lecture du nombre de ligne*/
    nb_lignes = lire_tcp(fd);

    /* Lecture du nombre de colonne*/
    nb_colonnes = lire_tcp(fd);

    board = (unsigned char *) malloc(sizeof(unsigned char) * (nb_lignes*nb_colonnes));


    /* Envoi du plateau*/
    if(read(fd, board, sizeof(unsigned char)*(nb_colonnes*nb_lignes)) == -1) {
        perror("Erreur lors de l'envoi du message ");
        exit(EXIT_FAILURE);
    }

    type = OK_CARTE;

    /*Envoi du ok*/
    if(write(fd, &type, sizeof(unsigned char)) == -1) {
        perror("Erreur lors de la réception du message ");
        exit(EXIT_FAILURE);
    }


    /* Creation de la fenêtre */
    /* Initialisation de ncurses */
    ncurses_initialiser();
    ncurses_souris();
    ncurses_couleurs();
    bkgd(COLOR_PAIR(1));

    /* Vérification des dimensions du terminal */
    if ((COLS < nb_colonnes + 10) || (LINES < nb_lignes + 10))
    {
        ncurses_stopper();
        fprintf(stderr,
                "Les dimensions du terminal sont insufisantes");
        exit(EXIT_FAILURE);
    }

    refresh();

    /*Fenêtre d'infos*/
    fenetre = newwin(8, COLS , 0, 0);
    box(fenetre, 0, 0);
    sous_fenetre = subwin(fenetre, 7 - 1, COLS - 1,  0+ 1,  0+ 1);

    /*Fenêtre du labyrinthe*/
    fenetre2 = newwin(nb_lignes+2, nb_colonnes+2, 8, 0);
    box(fenetre2, 0, 0);
    sous_fenetre2 = subwin(fenetre2, nb_lignes, nb_colonnes,  8+ 1,  0+ 1);

    /*Fenêtre des infos du joueur*/
    fenetre3 = newwin(14, COLS-COLONNES_MAX-2, 8, COLONNES_MAX+2);
    box(fenetre3, 0, 0);
    sous_fenetre3 = subwin(fenetre3, 12, COLS-COLONNES_MAX -4,  8+ 1,  COLONNES_MAX+3);

    scrollok(sous_fenetre, TRUE);
    scrollok(sous_fenetre2, FALSE);
    scrollok(sous_fenetre3, FALSE);

    mvwprintw(fenetre, 0, 1, "Infos");
    mvwprintw(fenetre2, 0, 1, "Jeu");
    mvwprintw(fenetre3, 0, 1, "Légende");

    generer_labyrinthe_jeu(sous_fenetre2, board, nb_colonnes, nb_lignes);
    afficher_infos_labyrinthe(sous_fenetre, sous_fenetre2, sous_fenetre3, board);

    sleep(1);/*On laisse le temps au Master de s'initialiser (Risque de bug)*/

    /* Création d'un cadre */
    wrefresh(fenetre);
    wrefresh(fenetre2);
    wrefresh(fenetre3);

    flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    timeout(200);
    while((ch = getch()) != KEY_F(2)) 
    { 
        /*Envoi périodique des lemmings*/
        type = lire_tcp(fd);
        if(type == ACTUALISATION) /* On met à jour les infos des 10 lemmings*/
        {
            for(i = 0; i < 5; i++){
                lemMaster[i].type = lire_tcp(fd);
                lemMaster[i].x = lire_tcp(fd);
                lemMaster[i].y = lire_tcp(fd);
            }
            for(i = 0; i < 5; i++){
                lemSlave[i].type = lire_tcp(fd);
                lemSlave[i].x = lire_tcp(fd);
                lemSlave[i].y = lire_tcp(fd);
                lemSlave[i].nb_retrait = 0;
            }
            afficher_infos_lemmings(sous_fenetre3, lemMaster, lemSlave, 1);
        }
        else if(type == MORT) /*On affiche l'écran de victoire ou de défaite*/
        {
            type = lire_tcp(fd);
            if (type == 2)
                ecran_perdu(sous_fenetre2, nb_lignes, nb_colonnes);
            else if(type == 1)
                ecran_gagner(sous_fenetre2, nb_lignes, nb_colonnes);
            

            isOver++;
            break;
        }

        generer_labyrinthe_jeu(sous_fenetre2, board, nb_colonnes, nb_lignes);
        actualiser_plateau(sous_fenetre2, lemMaster, lemSlave);

        if((ch == KEY_MOUSE) && (souris_getpos(&sourisX, &sourisY, &bouton) == OK)) 
        {
            /*L'utilisateur clique sur le labyrinthe*/
            if(wmouse_trafo(sous_fenetre2, &sourisY, &sourisX, 0))
            {
                if(bouton & BUTTON1_CLICKED) /*Ajout de lemming*/
                {
                    if (get_case(board, nb_colonnes, sourisX, sourisY) == 0)
                    {
                        if (!lemSlave[lemming_choisi].type)
                        {
                            type = AJOUT_LEMMING;
                            envoyer_tcp(fd, type, sizeof(unsigned char));
                            envoyer_tcp(fd, lemming_choisi, sizeof(unsigned char));
                            envoyer_tcp(fd, sourisX, sizeof(unsigned char));
                            envoyer_tcp(fd, sourisY, sizeof(unsigned char));
                            lemMaster[lemming_choisi].action_recente = 1; /*Afin de ne pas retirer le lemming après l'avoir mis*/
                        }
                    }

                    for (i = 0; i < 5; i++)/*On regarde si l'utilisateur a cliqué sur un lemming*/
                    {
                        if (lemSlave[i].type == TYPE_TERRAIN && lemSlave[i].x == sourisX && 
                        lemSlave[i].y == sourisY && lemSlave[i].nb_retrait < 4  && !lemMaster[i].action_recente)
                        {
                            type = RETRAIT_LEMMING;
                            envoyer_tcp(fd, type, sizeof(unsigned char));
                            envoyer_tcp(fd, i, sizeof(unsigned char));
                            lemSlave[i].nb_retrait++;
                        }
                    }
                    lemMaster[lemming_choisi].action_recente = 0;
                }
                else if (bouton & BUTTON3_CLICKED) /*Explosion de Lemming*/
                {
                    for (i = 0; i < 5; i++) /*On regarde si l'utilisateur a cliqué sur un Lemming*/
                    {
                        if ((lemSlave[i].type == TYPE_TERRAIN || lemSlave[i].type == TYPE_BLOQUE) && lemSlave[i].x == sourisX && lemSlave[i].y == sourisY)
                        {
                            type = EXPLOSION_LEMMING;
                            envoyer_tcp(fd, type, sizeof(unsigned char));
                            envoyer_tcp(fd, i, sizeof(unsigned char));
                            explosion_lemming(sous_fenetre2, board, sourisX, sourisY);
                        }
                    }
                }
                else if(bouton & BUTTON1_DOUBLE_CLICKED) /*Blocage de Lemming*/
                {
                    for (i = 0; i < 5; i++) /*On regarde si l'utilisateur a cliqué sur un Lemming*/
                    {
                        if (lemSlave[i].type == TYPE_TERRAIN && lemSlave[i].x == sourisX && lemSlave[i].y == sourisY)
                        {
                            type = BLOCAGE_LEMMING;
                            envoyer_tcp(fd, type, sizeof(unsigned char));
                            envoyer_tcp(fd, i, sizeof(unsigned char));
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

    }

    if (isOver)/*Permet de demander au joueur de confirmer sa victoire ou sa défaite*/
    {
        while ((ch = getch()) != KEY_F(2)){}
    }

    /* Suppression de la fenêtre */
    delwin(fenetre);
    delwin(fenetre2);
    delwin(fenetre3);

    /* Arrêt de ncurses */
    ncurses_stopper();

    /* Fermeture de la socket */
    if(close(fd) == -1) 
    {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }


    return EXIT_SUCCESS;
}