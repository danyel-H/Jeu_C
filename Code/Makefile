#
# CONFIGURATION GENERALE
#

EXEC = client_master client_slave creer_plateau serveur
OBJETS = ncurses.o game.o files.o SystemV.o sockets.o
NOM_PROJET = mini_projet

#
# SUFFIXES
#

.SUFFIXES: .c .o

#
# OBJETS
#

EXEC_O = $(EXEC:=.o)
OBJETS_O = $(OBJETS) $(EXEC_O)

#
# ARGUMENTS ET COMPILATEUR
#

CC = gcc
CCFLAGS_STD = -Wall -O3 -Werror -ansi -pedantic -D_GNU_SOURCE
CCFLAGS_DEBUG = -D _DEBUG_
CCFLAGS = $(CCFLAGS_STD)
CCLIBS = -lncurses

#
# REGLES
#

all: msg $(OBJETS) $(EXEC_O)
	@echo "Creation des executables..."
	@for i in $(EXEC); do \
	$(CC) -o $$i $$i.o $(OBJETS) $(CCLIBS); \
	done
	@rm -f $(OBJETS) $(EXEC_O)
	@echo "Termine."

msg:
	@echo "Creation des objets..."

debug: CCFLAGS = $(CCFLAGS_STD) $(CCFLAGS_DEBUG)
debug: all

#
# REGLES PAR DEFAUT
#

.c.o: .h
	@cd $(dir $<) && ${CC} ${CCFLAGS} -c $(notdir $<) -o $(notdir $@)

#
# REGLES GENERALES
#

clean:
	@echo "Suppresion des objets, des fichiers temporaires..."
	@rm -f $(OBJETS) $(EXEC_O)
	@rm -f *~ *#
	@rm -f $(EXEC)
	@rm -f dependances
	@echo "Termine."

depend:
	@echo "Creation des dependances..."
	@sed -e "/^# DEPENDANCES/,$$ d" makefile > dependances
	@echo "# DEPENDANCES" >> dependances
	@for i in $(OBJETS_O); do \
	$(CC) -MM -MT $$i $(CCFLAGS) `echo $$i | sed "s/\(.*\)\\.o$$/\1.c/"` >> dependances; \
	done
	@cat dependances > makefile
	@rm dependances
	@echo "Termine."

#
# CREATION ARCHIVE
#

ARCHIVE_FILES = *

archive: clean
	@echo "Creation de l'archive $(NOM_PROJET)$(shell date '+%y%m%d.tar.gz')..."
	@REP=`basename "$$PWD"`; cd .. && tar zcf $(NOM_PROJET)$(shell date '+%y%m%d.tar.gz') $(addprefix "$$REP"/,$(ARCHIVE_FILES))
	@echo "Termine."

# DEPENDANCES
ncurses.o: ncurses.c ncurses.h
game.o: game.c game.h ncurses.h files.h SystemV.h
creer_plateau.o : creer_plateau.c files.h
files.o: files.c files.h
SystemV.o: SystemV.c SystemV.h
sockets.o: sockets.c sockets.h
serveur.o: serveur.c sockets.h
client_master.o: client_master.c ncurses.h files.h SystemV.h 
client_slave.o: client_slave.c ncurses.h files.h SystemV.h 

