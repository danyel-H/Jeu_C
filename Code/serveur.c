/**
 * @author Hocquigny/De Sousa
 * @version 18/01/2019
 **/

#include "sockets.h"
#include "ncurses.h"
#include "files.h"
#include "string.h"
#include "game.h"

#define LIGNES_MAX 25
#define COLONNES_MAX 40

/*
Arguments :
1 : nom de fichier
2 : Adresse IP du serveur
3 : Port serveur
4 : Port TCP 
*/

int main(int argc, char *argv[]) 
{
    int sockfd, n = 0;
    unsigned char type_message;
    unsigned short port_temp;
  	struct sockaddr_in adresseServeur, adresseClient;
  	socklen_t taille_s = sizeof(struct sockaddr_in);
    file_attente *fileMaster, *fileSlave;

    if(argc != 2)
    {
        printf("pas assez d'arguments");
        exit(EXIT_FAILURE);
    }

    fileMaster = (file_attente*) malloc(sizeof(file_attente));
    fileSlave = (file_attente*) malloc(sizeof(file_attente));


    /* Attente de la requete du client */
    printf("Serveur en attente du message du client.\n");

    while(1)
    {
        /* Creation de la socket */
        if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
            perror("Erreur lors de la creation de la socket ");
            exit(EXIT_FAILURE);
        }
        
        /* Creation de l'adresse du serveur */
        memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
        adresseServeur.sin_family = AF_INET;
        adresseServeur.sin_port = htons(atoi(argv[1]));
        adresseServeur.sin_addr.s_addr = htonl(INADDR_ANY);
        
        /* Nommage de la socket */
        if(bind(sockfd, (struct sockaddr*)&adresseServeur, sizeof(struct sockaddr_in)) == -1) {
            perror("Erreur lors du nommage de la socket ");
            exit(EXIT_FAILURE);
        }
    
        if(recvfrom(sockfd, &type_message, sizeof(unsigned char), 0,  (struct sockaddr*)&adresseClient, &taille_s) == -1) {
            perror("Erreur lors de la reception du message ");
            exit(EXIT_FAILURE);
        }
        
        printf("Client : message recu : %d.\n", type_message);
        if(type_message == 1)
        {
            /*Code*/
            if(recvfrom(sockfd, &port_temp, sizeof(unsigned short), 0,  (struct sockaddr*)&adresseClient, &taille_s) == -1) {
                perror("Erreur lors de la reception du message ");
                exit(EXIT_FAILURE);
            }
            push_back(adresseClient, fileMaster, port_temp);
            afficher_f(fileMaster);
        }
        else if(type_message == 2)
        {
            /*Code*/
            push_back(adresseClient, fileSlave, 0);

            n++;
            if(!(n%3))
            {
                printf("Oui");
                decaler(fileSlave);
            }
            afficher_f(fileSlave);

        }
        
        
        /* Fermeture de la socket */
        if(close(sockfd) == -1) {
            perror("Erreur lors de la fermeture de la socket ");
            exit(EXIT_FAILURE);
        }

        if(fileMaster->debut != NULL && fileSlave->debut != NULL)
        { 
            /* Creation de la socket */
            if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
                perror("Erreur lors de la creation de la socket ");
                exit(EXIT_FAILURE);
            }

            if(sendto(sockfd, inet_ntoa(fileMaster->debut->adresseClient.sin_addr), sizeof(unsigned char)*16, 0, (struct sockaddr*)&(fileSlave->debut->adresseClient), sizeof(struct sockaddr_in)) == -1) {
                perror("Erreur lors de l'envoi du message ");
                exit(EXIT_FAILURE);
            }	

            if(sendto(sockfd, &(fileMaster->debut->port_tcp), sizeof(unsigned short), 0, (struct sockaddr*)&(fileSlave->debut->adresseClient), sizeof(struct sockaddr_in)) == -1) {
                perror("Erreur lors de l'envoi du message ");
                exit(EXIT_FAILURE);
            }	

            decaler(fileMaster);
            decaler(fileSlave);
        }
    }
    printf("Serveur termine.\n");
    free(fileMaster);
    free(fileSlave);

    return EXIT_SUCCESS;
}