#include "files.h"

/*
*Génère le chemin complet du fichier
*@param nomFichier le nom du fichier spécifié par l'utilisateur
*@return le nom du fichier accolé à "data/"
*/
char* get_chemin_fichier(char * nomFichier)
{
	char* chemin;
	int tailleNomFichier = strlen(nomFichier) + strlen("data/");
    chemin = (char*) malloc(tailleNomFichier * sizeof(char));
	if(chemin == NULL)
	{
		printf("Erreur malloc.\n");
		exit(EXIT_FAILURE);
	}
    sprintf(chemin, "%s%s", "data/", nomFichier);
	
	return chemin;
}

/*
*Permet d'effectuer les opérations nécessaires avec le fichier permettant le bon fonctionnement
*de l'éditeur
*@return Le descripteur de fichier du fichier final
*/
int creer_fichier(char* nomFichier, unsigned char nb_lignes, unsigned char nb_colonnes)
{
	int fd, i, j,r;
    unsigned char **board;
	char * nomDefinitif;

	nomDefinitif = (char*) malloc(sizeof(unsigned char) * strlen(nomFichier) + strlen("data/")); 
	strcpy(nomDefinitif, get_chemin_fichier(nomFichier));

	if((fd = open(nomDefinitif, O_RDONLY, S_IRUSR|S_IWUSR)) == -1)
	{
		if((fd = open(nomDefinitif, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR)) == -1)
		{
			perror("Erreur création du fichier ");
			exit(EXIT_FAILURE);
		}

		board = (unsigned char **) malloc(sizeof(unsigned char*) * nb_lignes);
		for(i = 0; i < nb_lignes; i++)
		{
			board[i] = (unsigned char*) malloc(sizeof(unsigned char)* nb_colonnes);
		}

		for(i = 0; i < nb_lignes; i++)
		{
			for(j = 0; j < nb_colonnes; j++)
			{
				r = rand()%6;
				if(r == 0)
					board[i][j] = 1;
				else
					board[i][j] = 0;
			}
		}

		set_board(fd, board, nb_lignes, nb_colonnes);

		printf("Le fichier de l'éditeur n'existait pas et a été créé\n");
		for(i = 0; i < nb_lignes; i++)
		{
			free(board[i]);
		}
		free(board);
	}

    return fd;
}

int ouvrir_fichier(char* nom)
{
	int fd;
	if((fd = open(nom, O_RDWR, S_IRUSR|S_IWUSR)) == -1)
	{
		perror("Erreur ");
		exit(EXIT_FAILURE);
	}

	return fd;
}


/*
*Change le plateau du labyrinthe
*/
void set_board(int fd,unsigned char **board, unsigned char nb_ligne, unsigned char nb_colonne)
{
	int i,j;

	if(lseek(fd, 0L, SEEK_SET) == -1)
	{
		perror("Error lseek");
		exit(EXIT_FAILURE);
	}

	if(write(fd, &nb_colonne, sizeof(unsigned char)) == -1) 
        {
            perror("Erreur write setboard ");
            exit(EXIT_FAILURE);
        }

	if(write(fd, &nb_ligne, sizeof(unsigned char)) == -1) 
        {
            perror("Erreur write setboard ");
            exit(EXIT_FAILURE);
        }

	for(i = 0; i < nb_ligne; i++)
    {
        for(j = 0; j < nb_colonne; j++)
        {
            if(write(fd, &(board[i][j]), sizeof(unsigned char)) == -1) 
			{
				perror("Erreur write setboard ");
				exit(EXIT_FAILURE);
			}
        }
    }
}


unsigned char get_largeur(int fd)
{
	unsigned char h;

	if(lseek(fd, 0L, SEEK_SET) == -1)
	{
		perror("Error lseek");
		exit(EXIT_FAILURE);
	}

	if(read(fd, &h, sizeof(unsigned char)) == -1)
	{
		perror("Erreur lecture hauteur ");
        exit(EXIT_FAILURE);
	}

	return h;
}

unsigned char get_hauteur(int fd)
{
	unsigned char l;

	if(lseek(fd, sizeof(unsigned char), SEEK_SET) == -1)
	{
		perror("Error lseek");
		exit(EXIT_FAILURE);
	}

	if(read(fd, &l, sizeof(unsigned char)) == -1)
	{
		perror("Erreur lecture largeur ");
        exit(EXIT_FAILURE);
	}

	return l;
}

/*
*Change la valeur des octets designant les positions
*@param fd le descripteur de fichier
*@param posY la position en Y
@param posX la position en X
*/
void set_pos(int fd, unsigned char posY, unsigned char posX)
{
	int taille_octets = sizeof(int) + sizeof(unsigned char) + (sizeof(unsigned char) *450) + sizeof(unsigned char);

	if(lseek(fd, taille_octets, SEEK_SET) == -1)
	{
		perror("Error lseek");
		exit(EXIT_FAILURE);
	}

	if(write(fd, &posY, sizeof(unsigned char)) == -1) 
	{
		perror("Erreur write ");
		exit(EXIT_FAILURE);
	}

    if(write(fd, &posX, sizeof(unsigned char)) == -1) 
	{
		perror("Erreur write ");
		exit(EXIT_FAILURE);
	}
}

/*
*Retourne la valeur de la position en Y du joueur (dans la sauvegarde)
*/
unsigned char get_posY(int fd)
{
	int taille_octets = sizeof(int) + sizeof(unsigned char) + (sizeof(unsigned char) *450) + sizeof(unsigned char);
	int pos;

	if(lseek(fd, taille_octets, SEEK_SET) == -1)
	{
		perror("Error lseek");
		exit(EXIT_FAILURE);
	}

	if(read(fd, &pos, sizeof(unsigned char)) == -1)
	{
		perror("Erreur lecture posY ");
        exit(EXIT_FAILURE);
	}

	return pos;
}

/*
*Retourne la valeur de la position en X du joueur (dans la sauvegarde)
*/
unsigned char get_posX(int fd)
{
	int taille_octets = sizeof(int) + sizeof(unsigned char) + (sizeof(unsigned char) *450) + sizeof(unsigned char) + sizeof(unsigned char);
	int pos;

	if(lseek(fd, taille_octets, SEEK_SET) == -1)
	{
		perror("Error lseek");
		exit(EXIT_FAILURE);
	}

	if(read(fd, &pos, sizeof(unsigned char)) == -1)
	{
		perror("Erreur lecture posX ");
        exit(EXIT_FAILURE);
	}

	return pos;
}

/*
*Retourne un mur selon ses coordonnées
*@param fd le descripteur de fichier
*@param posY les coordonnées en Y
*@param posX les coordonnées en X
*@return le caractère du mur
*/
unsigned char get_mur(int fd, int posY, int posX)
{
	unsigned char mur;
	int y, nb_car;

	y = 30 * posY;
	nb_car = y+posX;

	if(lseek(fd, sizeof(int)+ sizeof(unsigned char), SEEK_SET) ==-1)
	{
		perror("Error lseek");
		exit(EXIT_FAILURE);
	}


	if(lseek(fd, nb_car*sizeof(unsigned char), SEEK_CUR) == -1)
	{
		perror("Error lseek");
		exit(EXIT_FAILURE);
	}

	if(read(fd, &mur, sizeof(unsigned char)) == -1)
	{
		perror("Erreur lecture mur ");
        exit(EXIT_FAILURE);
	}

	return mur;
}

/*
*Change l'état d'un mur
*@param fd le descripteur de fichier
*@param posY les coordonnées en Y
*@param posX les coordonnées en X
*@return le caractère du mur
*/
void set_mur(int fd, int posY, int posX, unsigned char mur)
{
	int y, nb_car;

	y = 30 * posY;
	nb_car = y+posX;

	if(lseek(fd, 2*sizeof(unsigned char), SEEK_SET) == -1)
	{
		perror("Error lseek");
		exit(EXIT_FAILURE);
	}

	if(lseek(fd, nb_car*sizeof(unsigned char), SEEK_CUR) == -1)
	{
		perror("Error lseek");
		exit(EXIT_FAILURE);
	}

	if(write(fd, &mur, sizeof(unsigned char)) == -1)
	{
		perror("Erreur lecture mur ");
        exit(EXIT_FAILURE);
	}
}

/*
*Retourne le nom du labyritnhe à partir du fichier
*/
char* get_nom_labyrinthe(char* nomFichier)
{
	char s1[2] = "/";
	char s2[2] = ".";
	char *token;

	token = strtok(nomFichier, s1);
	token = strtok(NULL, s1);
	token = strtok(token, s2);

	return token;
}