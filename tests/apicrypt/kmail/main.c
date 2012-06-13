/********************************************************************************************
 *                                                                                          *
 * Affiche fichier texte envoyé par pipe au programme                                       *
 *                                                                                          *
 * Copyright (C) 2012, Laurent Masson                                                       *
 *                                                                                          *
 * Version : 04/04/2012 00:01:35                                                            *
 *                                                                                          *
 ********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iconv.h>
#include "filenstr.h"
#include "afficheFichier.h"
#include "trouveIdentite.h"
#include "lireChemins.h"
#include "corpsMessage.h"


int main(int argc, char *argv[])
{
  char c;
  char nomFichierSource[150];
  char nomFichierCorps[150];
  char nomFichierCrypte[150];
  char crypter[600];
  char ligne[300];
  char cheminApicrypt[300];
  char cheminApitemp[300];
  char utilisateur[]="Laurent.MASSON";
  char destinataires[300];

  lireChemins(cheminApicrypt, cheminApitemp);

  sprintf(nomFichierSource, "%smail", cheminApicrypt); // fichier source
  sprintf(nomFichierCorps, "%smailcorps", cheminApicrypt); // fichier source
  sprintf(nomFichierCrypte, "%smail.txt", cheminApicrypt);  // fichier arrivée

  FILE* fichier2 = NULL;
  fichier2 = fopen(nomFichierSource, "w+");

  if (fichier2 != NULL)
  {
    /* utilisation des flux standards
       d'entrées-sorties : stdin et stdout */
    while ((c = getchar()) != EOF)
    {
      fputc(c, fichier2); // copie le mail d'origine (flux entrée) dans nomFichierSource
    }

    filenstr("Date:", fichier2, 1, ligne); // Trouve la ligne contenant de la 1ère occurence de la chaine
    filenstr("Subject:", fichier2, 1, ligne); // Trouve la ligne contenant de la 1ère occurence de la chaine
    filenstr("To:", fichier2, 1, ligne); // Trouve la ligne contenant de la 1ère occurence de la chaine
    trouveIdentite(ligne, destinataires); // fonction qui extrait l'identité du destinataire
    filenstr("From:", fichier2, 1, ligne); // Trouve la ligne contenant de la 1ère occurence de la chaine
    printf("Content-Type: text/plain; charset=ISO-8859-1; format=flowed\nContent-Transfer-Encoding: 7bit\n\n");

    fclose(fichier2);

    corpsMessage(nomFichierSource, nomFichierCorps);

/*    iconv_t iconv_open(const char *tocode, const char *fromcode);
    size_t iconv(iconv_t cd,
             char **inbuf, size_t *inbytesleft,
             char **outbuf, size_t *outbytesleft);
*/

    sprintf(crypter, "%sapicrypt -s %s -o %s -u %s -k %s -d %s", cheminApicrypt, nomFichierCorps, nomFichierCrypte, utilisateur, cheminApicrypt, destinataires);

    system(crypter); // décrypte et copie dans nomFichierCrypte

    remove(nomFichierSource); // efface le fichier nomFichierSource
    remove(nomFichierCorps); // efface le fichier nomFichierCorps

    afficheFichier(nomFichierCrypte); // Affiche le contenu du fichier décrypté

  }
  return 0;
}
