#include "SystemV.h"

/*
Initialise un tableau de semaphore
@param cle la clé du tableau
@param val le nombre de semaphores
@param flags les flags potentiels
*/
int init_tableau(int cle, int val, int flags)
{
    int semid;

    /* Creation du tableau de semaphore */
    if((semid = semget((key_t)cle, val,flags)) == -1) 
    {
        perror("Erreur lors de la creation du tableau de semaphores ");
        exit(EXIT_FAILURE);
    }

    return semid;
}

/*
Retourne la valeur d'un semaphore
@param semid l'id du semaphore
@param sema l'id du tableau de semaphores
*/
int get_val(int semid, int sema)
{
    int val;
    /* Initialisation des semaphores */
    if((val = semctl(sema, semid , GETVAL)) == -1) 
    {
        perror("Erreur lors de l'initialisation des semaphores ");
        exit(EXIT_FAILURE);
    }

	return val;
} 

/*Initialise un semaphore 
@param semid id du semaphore
@param val valeur que prendra le semaphore
@param sema le tableau de semaphore*/
void init_sema(int semid, int val, int sema)
{
    /* Initialisation des semaphores */
    if(semctl(sema, semid, SETVAL, val) == -1) 
    {
        perror("Erreur lors de l'initialisation des semaphores ");
        exit(EXIT_FAILURE);
    }
}

/*Fais une opération sur un semaphore
@param id id du semaphore
@param operation la valeur à mettre dans sem_op
@param sema l'id du tableau de semaphore
@param flags les potentiels flags
Renvoie -1 si un Puis-je ne peut se faire (en cas de IPC_NOWAIT)*/
int op_sema(int id, int operation, int sema, int flags)
{
    int retour = 1;
    struct sembuf op;
    op.sem_num = id;
    op.sem_op = operation;
    op.sem_flg = flags;
    if(semop(sema, &op, 1) == -1) 
    {
        if(errno == EAGAIN)
        {
            retour = -1;
        }
        else 
        {
            perror("Erreur lors de l'operation sur le semaphore, numéro de sema ");
            exit(EXIT_FAILURE);
        }
    }

    return retour;
}

/*
Supprime un tableau de semaphore selon son id
*/
void supprimer_sema(int semid)
{
    /* Suppression du tableau de semaphores */
    if(semctl(semid, 0, IPC_RMID) == -1) 
    {
        perror("Erreur lors de la suppresion du tableau de semaphores ");
        exit(EXIT_FAILURE);
    }
}

/*
Créer un segment de mémoire partagée
@param cle la clé du smp
@param taille la taille en octets
@param options les potentielles options
*/
int creer_smp(int cle, int taille, int options)
{
    int shmid;
    if((shmid = shmget((key_t)cle, taille, options)) == -1)
    {
        perror("Erreur dans la création/récupération du smp ");
        exit(EXIT_FAILURE);
    }

    return shmid;
}

/*
Attache un segment de mémoire partagée via son id
Renvoie l'adresse du smp
*/
void* attacher_smp(int shmid)
{
    void* adresse;
    if((adresse = shmat(shmid, NULL, 0)) == (void*)-1)
    {
        perror("Erreur lors de l'attachement du segment de memoire partagee \n");
        exit(EXIT_FAILURE);
    }

    return adresse;
}

/*
Créé une file de message
@param cle la clé de la FdM
@param flags les potentiels flags
retourne l'id de la file*/
int creer_file(int cle, int flags)
{
  int msqid;
  if((msqid = msgget((key_t)cle, flags)) == -1)
  {
    if(errno == EEXIST)
    {
      fprintf(stderr, "File (cle=%d) existante\n", cle);
    }
    else
    {
      perror("Erreur lors de la creation ");
      exit(EXIT_FAILURE);
    }
  }

  return msqid;
}

/*Supprime une file de message selon son id*/
void supprimer_file(int msqid)
{
    /* Suppression de la file */
      if(msgctl(msqid, IPC_RMID, 0) == -1) 
      {
        perror("Erreur lors de la suppression de la file ");
        exit(EXIT_FAILURE);
      }
}

/*Envoie un message dans une file de message
@param msqid l'id de la file
@param r la requête à envoyer*/
void envoyer_requete(int msqid, requete_t r)
{
    if(msgsnd(msqid, &r, sizeof(requete_t) - sizeof(long), 0) == -1)
    {
    perror("Erreur lors de l'envoi de la requete ");
    exit(EXIT_FAILURE);
    }
}

/*
Permet d'attendre la réception d'une requête 
@param msqid l'id de la file
@param m la structure utilisée pour reçevoir la requête
@param type le type à attendre
*/
requete_t receive_requete(int msqid, requete_t m, int type)
{
    if(msgrcv(msqid, &m, sizeof(requete_t) - sizeof(long), type, 0) == -1) 
    {
      perror("Erreur lors de la reception d'une requete ");
      exit(EXIT_FAILURE);
    }
    return m;
}

/*Supprime un smp selon son id*/
void supprimer_smp(int shmid)
{
    /* Suppression du segment de memoire partagee */
    if(shmctl(shmid, IPC_RMID, 0) == -1) 
    {
      perror("Erreur lors de la suppression du segment de memoire partagee ");
      exit(EXIT_FAILURE);
    } 
}

/*
*Change l'état d'un mur
*@param adresse l'adresse du smp
*@param posY les coordonnées en Y
*@param posX les coordonnées en X
*/
void set_mur_ipc(void* adresse, int posY, int posX, unsigned char mur)
{
	int y, nb_car;
    unsigned char *debut;
    debut = adresse;

	y = 30 * posY;
	nb_car = y+posX;

	debut += nb_car;
    *debut = mur;
}

/*Renvoie un caractère du smp selon sa position dans le labyrinthe*/
unsigned char get_mur_ipc(void* adresse, int posY, int posX)
{
    unsigned char mur, *debut;
	int y, nb_car;
    debut = adresse;

    if(posY <0 || posX < 0)
    {
        mur = 0;
    }
    else
    {
        y = 30 * posY;
        nb_car = y+posX;

        mur = debut[nb_car];
    }
    
	return mur;
}

/*Renvoie la position du personnage en Y*/
unsigned char get_posY_ipc(void* adresse)
{
    int i,j, k = 0;
	unsigned char board[15][30], y = 16;
	unsigned char *debut;

    debut = adresse;
    for(i = 0; i < 15; i++)
    {
        for(j = 0; j < 30; j++)
        {
            board[i][j] = debut[k];
            k++;
            if(board[i][j] ==  10)
            {
                y = i;
            }
        }
    }
    return y;
}

/*Renvoie la position du personnage en X*/
unsigned char get_posX_ipc(void* adresse)
{
    int i,j, k = 0;
	unsigned char board[15][30], x = 31;
	unsigned char *debut;

    debut = adresse;
    for(i = 0; i < 15; i++)
    {
        for(j = 0; j < 30; j++)
        {
            board[i][j] = debut[k];
            k++;
            if(board[i][j] ==  10)
            {
                x = j;
            }
        }
    }
    return x;
}


/*Retourne le nombre de murs qui sont invisibles*/
int get_nb_murs_caches(void* adresse)
{
	int i,j,k = 0,nbMurs = 0;
	unsigned char board[15][30];
    unsigned char *debut;
    debut = adresse;


	for(i =0; i < 15; i++)
        {
            for(j = 0; j < 30; j++)
            {
                board[i][j] = debut[k];
                k++;
                if(board[i][j] == 1)
                {
                    nbMurs++;
                }
            }
        }

    return nbMurs;
}

/*Retourne le nombre de murs découverts*/
int get_nb_murs_decouverts(void* adresse)
{
	int i,j,k = 0,nbMurs = 0;
	unsigned char board[15][30];
    unsigned char *debut;
    debut = adresse;


	for(i =0; i < 15; i++)
        {
            for(j = 0; j < 30; j++)
            {
                board[i][j] = debut[k];
                k++;
                if(board[i][j] == 2) /* Mur Invisible*/
                {
                    nbMurs++;
                }
            }
        }

    return nbMurs;
}

/*Retourne le nombre de vies du personnage*/
unsigned char get_vie_ipc(void* adresse)
{
    unsigned char *debut, vies;
    debut = adresse;

    vies = debut[450];

    return vies;
}

/*Fixe le nombre de vies du personnages*/
void set_vie_ipc(void* adresse, unsigned char nb_vies)
{
    unsigned char *debut;
    debut = adresse;

    debut[450] = nb_vies;
}