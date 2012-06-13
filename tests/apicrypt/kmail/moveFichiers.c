/*

moveFichiers.c

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include "moveFichiers.h"


void moveFichiers(int nbFichier, char *cheminApicrypt, char *cheminApitemp)
{

  int i = 0;
  int numero = 0;
  char nomFichierAvant[150];
  char nomFichierApres[150];

  DIR* rep = NULL;
  struct dirent* fichierLu = NULL;
  rep = opendir(cheminApitemp);
  if (rep == NULL)
      {
      printf("Le répertoire %s n'existe pas !\n", cheminApitemp);
      exit(1);
      }


  while ((fichierLu = readdir(rep)) != NULL)
  {
    if (strcmp(fichierLu->d_name, ".") != 0 && /* Si le fichier lu n'est pas . */
        strcmp(fichierLu->d_name, "..") != 0 && /*  Et n'est pas .. non plus */
        strncmp(fichierLu->d_name, "resutext", 8) == 0) /* strncmp compare les n premiers octets au plus de deux chaînes en utilisant l'ordre lexicographique */
    i++;
  }
  if (closedir(rep) == -1) {
      printf("closedir(rep) == -1\n"); // test
      exit(-1); 
      }
  while (numero < nbFichier)
  {
    sprintf(nomFichierAvant, "%stmp/resutext%03d.txt", cheminApicrypt, numero);
    sprintf(nomFichierApres, "%sresutext%03d.txt", cheminApitemp, (numero+i));

    rename(nomFichierAvant, nomFichierApres);
    numero++;
  }


}
