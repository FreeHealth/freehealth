/*

lireChemins.c

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


void lireChemins(char *cheminApicrypt, char *cheminApitemp)
{
  char home[300];

  strcpy(home, getenv("HOME"));

  FILE* fichier1 = NULL;
  fichier1 = fopen("apiKmail.ini", "r+");

  if (fichier1 != NULL)
  {
    fgets(cheminApicrypt, 300, fichier1);
    cheminApicrypt[strlen(cheminApicrypt)-1]='\0';
    fgets(cheminApitemp, 300, fichier1);
    cheminApitemp[strlen(cheminApitemp)-1]='\0';
    fclose(fichier1);
  }
  else // si le fichier n'existe pas on le crée
  {
    FILE* fichier1 = NULL;
    fichier1 = fopen("apiKmail.ini", "w+");

    if (fichier1 != NULL)
    {
      sprintf(cheminApitemp,"%s/MedinTux/Programmes/apitux/bin/apitemp/", home);
      sprintf(cheminApicrypt,"%s/apicrypt/", home);
      fprintf(fichier1, "%s\n", cheminApicrypt);
      fprintf(fichier1, "%s\n", cheminApitemp);
      fclose(fichier1);
    }
    else
    {
      // On affiche un message d'erreur
      perror("Impossible d'ouvrir ou de créer le fichier apiKmail.ini ");
      printf("errno = %d.\n", errno);
      exit(1);
    }
  }

}
