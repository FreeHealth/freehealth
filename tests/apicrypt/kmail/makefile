all: apiKmailC

apiKmailC: main.o filenstr.o afficheFichier.o lireChemins.o trouveIdentite.o corpsMessage.o
	gcc main.o filenstr.o afficheFichier.o lireChemins.o trouveIdentite.o corpsMessage.o -o apiKmailC

filenstr.o: filenstr.c
	gcc -c filenstr.c -o filenstr.o

afficheFichier.o: afficheFichier.c
	gcc -c afficheFichier.c -o afficheFichier.o

lireChemins.o: lireChemins.c
	gcc -c lireChemins.c -o lireChemins.o

trouveIdentite.o: trouveIdentite.c
	gcc -c trouveIdentite.c -o trouveIdentite.o

corpsMessage.o: corpsMessage.c
	gcc -c corpsMessage.c -o corpsMessage.o

main.o: main.c filenstr.h afficheFichier.h lireChemins.h trouveIdentite.h corpsMessage.h
	gcc -c main.c -o main.o
