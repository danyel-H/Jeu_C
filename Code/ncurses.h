#ifndef _NCURSES_
#define _NCURSES_

/**
 * Initialisation de ncurses.
 */
void ncurses_initialiser();

/**
 * Fin de ncurses.
 */
void ncurses_stopper();

/**
 * Initialisation des couleurs.
 */
void ncurses_couleurs();

void ncurses_souris(); 
int souris_getpos(int *, int *, int *);

#endif
