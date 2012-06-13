/*

numeroFichierMail.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "numeroFichierMail.h"

#define CHEMIN_APICRYPT "/home/laurent/apicrypt/"


int numeroFichierMail()
{
  // Calcul le numéro du fichier mail
  char numeroTxt[5];
  int numero;

  FILE* fichier1 = NULL;
  fichier1 = fopen(CHEMIN_APICRYPT"affiche.ini", "r+");

  if (fichier1 != NULL)
  {
    fgets(numeroTxt, 5, fichier1);
    numero = atoi(numeroTxt); // convertit une chaine en int
    rewind(fichier1);
    numero++;
    sprintf(numeroTxt,"%d",numero);
    fputs(numeroTxt, fichier1);
    fclose(fichier1);
    return numero;
  }
  else
  {
    FILE* fichier1 = NULL;
    fichier1 = fopen(CHEMIN_APICRYPT"affiche.ini", "w+");

    if (fichier1 != NULL)
    {
      numero = 1;
      sprintf(numeroTxt,"%d",numero);
      fputs(numeroTxt, fichier1);
      fclose(fichier1);
      return numero;
    }
    else
    {
      // On affiche un message d'erreur
      perror("Impossible d'ouvrir ou de créer le fichier affiche.ini ");
      printf("errno = %d.\n", errno);
      exit(1);
    }
  }
}
