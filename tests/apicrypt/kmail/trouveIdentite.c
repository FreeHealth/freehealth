/*

trouveIdentite.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trouveIdentite.h"


int trouveIdentite(const char *ligne, char *identite) /* fonction qui extrait l'identité du destinataire dans ligne */

{
  char finLigne[300];
  int position, longueurChaine;;

  longueurChaine = strlen(ligne); /* Calcul la longueur de la chaine à trouver */
  sscanf (ligne, "To: %s", identite); /* lecture formattée depuis une chaîne de caractères */
  identite = strtok(identite, "@"); /* Scinde une chaine selon une synthaxe particulière (ici le @) */
//  printf("identite = %s\n",identite);

}
