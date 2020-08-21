#include "sockets.h"

/*Insère une adresse dans la file d'attente*/
void push_back(struct sockaddr_in a, file_attente *f, unsigned short port_tcp)
{
    struct cellule *c1;
    c1 = (struct cellule*) malloc(sizeof(struct cellule));
    c1->adresseClient = a;
    if(port_tcp)
    {
        c1->port_tcp = port_tcp;
    }
    if(&(f->debut->adresseClient) == (struct sockaddr_in*) NULL)
    {
        f->debut = c1;
    }
    else
    {
        f->fin->queue = c1;
    }

    c1->queue = (struct cellule*)NULL;
    f->fin = c1;
}

/*Permet de libérer les deux adresses du Master et du Slave afin de faire avancer la file d'attente*/
void decaler(file_attente *f)
{
    struct cellule *c1;
    c1 = (struct cellule*) malloc(sizeof(struct cellule));
    *c1 = *(f->debut);
    free(f->debut);
    f->debut = c1->queue;
    free(c1);
}

/*Affiche toutes les adresses stockées dans la file d'attente*/
void afficher_f(file_attente *l)
{
    struct cellule *c;
    c = l->debut;
    printf("Adresse : %s, le port : %d\n",inet_ntoa(c->adresseClient.sin_addr), c->port_tcp);
    while((c->queue))
    {
        c = c->queue;
        printf("Adresse : %s, le port : %d\n",inet_ntoa(c->adresseClient.sin_addr), c->port_tcp);
    }
}
 
 /*Permet d'envoyer un unsigned char en TCP*/
void envoyer_tcp(int sockfd, unsigned char valeur, size_t taille)
{
    if(write(sockfd, &valeur, taille) == -1) {
            perror("Erreur lors de l'envoi du message TCP ");
            exit(EXIT_FAILURE);
        }
}

/*Permet de reçevoir un unsigned char*/
unsigned char lire_tcp(int sockfd)
{
    unsigned char valeur;
    if(read(sockfd, &valeur, sizeof(unsigned char)) == -1) {
        if(errno != EAGAIN)
        {
            perror("Erreur lors de la reception du message TCP ");
            exit(EXIT_FAILURE);
        }
    }

    return valeur;
}

/*Permet d'envoyer un unsigned char en UDP*/
void envoyer_udp(int sockfd, unsigned char valeur, size_t taille, struct sockaddr_in adresseServeur)
{
    if (sendto(sockfd, &valeur, taille, 0, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Erreur lors de l'envoi du message UDP ");
        exit(EXIT_FAILURE);
    }
}

/*Permet de reçevoir un unsigned char en UDP*/
unsigned char lire_udp(int sockfd)
{
    unsigned char valeur;
    if (recvfrom(sockfd, &valeur, sizeof(unsigned char), 0, NULL, NULL) == -1)
    {
        perror("Erreur lors de la reception du message UDP ");
        exit(EXIT_FAILURE);
    }

    return valeur;
}