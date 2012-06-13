/*

csplit.c

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include "csplit.h"


int csplit(const char *chaine, char *nomFichier1, char *nomFichier2, char *prefixe)
{
  int numero = 0;
  int i;
  long longueurFichier = 0;
  char ligne1[300];


  FILE* fichier1 = NULL;
  fichier1 = fopen(nomFichier1, "r+");

  fseek(fichier1, 0, SEEK_END);
  longueurFichier = ftell(fichier1);

  rewind(fichier1);

  FILE* fichier2 = NULL;
  fichier2 = fopen(nomFichier2, "w+");

  while (fgets(ligne1, 300, fichier1) != NULL)
  {
    //rechercher chaine dans ligne
    if (strstr(ligne1, chaine)) // si chaine trouvée
    { 
      numero++;
      sprintf(nomFichier2, "%s%03d.txt", prefixe, numero); // Ecrit dans la chaine nomFichier le nom formaté du fichier suivant
      if (longueurFichier != ftell(fichier1))
      {
        fclose(fichier2);
        fichier2 = fopen(nomFichier2, "w+");
      }
    }
    else
    {
      fputs(ligne1, fichier2); // Copie la ligne dans fichier2
    }
  }
  fclose(fichier1);
  fclose(fichier2);
  return numero;
}
