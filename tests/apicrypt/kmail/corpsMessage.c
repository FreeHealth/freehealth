/*

corpsMessage.c

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <errno.h>
#include "corpsMessage.h"


int corpsMessage(char *nomFichierSource, char *nomFichierCorps)
{
  int numero = 0;
  int i;
  long longueurFichier = 0;
  char ligne1[300];


  FILE* fichier1 = NULL;
  fichier1 = fopen(nomFichierSource, "r+");

  fseek(fichier1, 0, SEEK_END);
  longueurFichier = ftell(fichier1);

  rewind(fichier1); /* se positionne au début du fichier */

  FILE* fichier2 = NULL;
  fichier2 = fopen(nomFichierCorps, "w+");

  while (fgets(ligne1, 300, fichier1) != NULL)
  {
    //rechercher chaine dans ligne
    if (strcmp(ligne1, "\n") == 0) // si chaine trouvée
    { 
      while (fgets(ligne1, 300, fichier1) != NULL) fputs(ligne1, fichier2); // Copie la ligne dans fichier2
    }
  }
  fclose(fichier1);
  fclose(fichier2);
  return 0;
}
