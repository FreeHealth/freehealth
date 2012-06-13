/*

filenstr.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filenstr.h"


int filenstr(const char *chaine, FILE *fichier, int occurence, char *ligne) /* fonction qui trouve la position d'une chaine dans un fichier et copie la ligne où la chaine est trouvée */

{
  int caractere, position, longueurChaine, continuer, i, n, u;
  caractere = position = i = n = u = 0;
  continuer = 1;

  longueurChaine = strlen(chaine); /* Calcul la longueur de la chaine à trouver */
  fseek(fichier, position, SEEK_SET);  // On met le pointeur de fichier au début

  /* -------- Boucle principale : lecture des caractères un à un -------- */
  while ( continuer ) /* On continue tant que la variable continuer est vraie (donc différent de zéro) */
  {
    caractere = fgetc(fichier); /* On lit le caractère suivant */
    while ( continuer && (caractere == chaine[i]) && caractere != EOF )
    {
      i++;
      if ( i == longueurChaine )  /* vrai si on a lu toute la chaine */
      {
        n++;
        if ( n == occurence )
        {
          continuer = 0;
          fseek(fichier, position, SEEK_SET);
          break;
        }
        else if ( caractere == EOF )
        {
          continuer = 0;
          printf("%s\n", chaine);
          break;
        }
        else
        {
          fseek(fichier, position + 1, SEEK_SET);  /* On positionne le pointeur sur la position suivante */
          i = 0;
          break;
        } 
      }
      caractere = fgetc(fichier); /* On lit le caractère suivant */
    }
    if (i != 0) 
    {
      i = 0;
      fseek(fichier, position + 1, SEEK_SET);
    }
    if ( caractere == EOF && position != 0 )
    {
      continuer = 0;
      printf("%s\n", chaine);
      ligne = NULL;
      break;
    }
    position++;
  }

  if (position != 0) position = position -1;

  /* Copie la première chaine du fichier à partir de la position trouvée précédemment */
  fseek(fichier, position, SEEK_SET);  /* On positionne le pointeur sur position */
  if (ligne != NULL) // si la chaine ligne est égale à NULL on n'y met pas le contenu de la ligne du fichier
  {
    fgets(ligne, 300, fichier); /* On copie la ligne */
    printf("%s", ligne);
  }

}
