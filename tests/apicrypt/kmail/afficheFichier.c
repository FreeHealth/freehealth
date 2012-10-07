/*

afficheFichier.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "afficheFichier.h"


void afficheFichier(char *nomFichierTxt)
{
  // Affiche le contenu du fichier décrypté :
  int caractere;

  FILE* fichier3 = NULL;
  fichier3 = fopen(nomFichierTxt, "r+");

  if (fichier3 != NULL)
  {
    caractere = fgetc(fichier3);
    while ( caractere != EOF )
    {
      printf("%c", caractere);
      caractere = fgetc(fichier3); // On lit le caractère suivant
    }

    fclose(fichier3);
  }
  else
  {
    // On affiche un message d'erreur
    printf("Impossible d'ouvrir ou de créer le fichier %s ", nomFichierTxt);
    printf("errno = %d.\n", errno);
    exit(1);
  }
}
