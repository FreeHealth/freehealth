CC=gcc
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=apikmail

all: $(EXEC)

apikmail: main.o fonctions.o
	$(CC) main.o fonctions.o -o apikmail $(LDFLAGS)

fonctions.o: fonctions.c
	$(CC) -c fonctions.c -o fonctions.o $(LDFLAGS)

main.o: main.c fonctions.h
	$(CC) -c main.c -o main.o $(LDFLAGS)

clean:
	rm *.o

mrproper: clean
	rm $(EXEC)
