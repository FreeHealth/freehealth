/*

fonctions.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include "fonctions.h"

  /* **************************** filenstr ********************************* */

  /* fonction qui trouve la position d'une chaine 
     dans un fichier et copie la ligne où 
     la chaine est trouvée                        */

int filenstr(const char *chaine, FILE *fichier, int occurence, char *ligne)

{
  int caractere, position, longueurChaine, continuer, i, n, u;
  caractere = position = i = n = u = 0;
  continuer = 1;

  longueurChaine = strlen(chaine);      /* Calcul la longueur de la chaine à trouver */
  fseek(fichier, position, SEEK_SET);  /* On met le pointeur de fichier au début     */

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

  /* ************************* afficheFichier ****************************** */

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

  /* **************************** csplit ********************************* */

int csplit(const char *chaine, char *nomFichier1, char *nomFichier2, const char *prefixe)
{
  int nombreChaines = 0;
  char ligne1[300];


  sprintf(nomFichier2, "%s%03d.txt", prefixe, nombreChaines); /* Ecrit dans la chaine nomFichier2
                                                                     le nom formaté du fichier suivant   */

  FILE* fichier1 = NULL;
  fichier1 = fopen(nomFichier1, "r+");     /* Ouverture de fichier1 en lecture et écriture  */

  FILE* fichier2 = NULL;
  fichier2 = fopen(nomFichier2, "w+");     /* On création de fichier2 */

  while (fgets(ligne1, 300, fichier1) != NULL)      /* lecture du fichier ligne par ligne          */
  {
    if (strstr(ligne1, chaine))                     /* si chaine trouvée dans ligne                */
    {
      if (strcmp(chaine,"****FINFICHIER****") == 0) /* Si chaine  identique à "****FINFICHIER****" */
        break;                                      /* on arrête la boucle while
                                                       Notez que la ligne n'est pas copiée         */

      fputs(ligne1, fichier2);                      /* Copie la ligne dans fichier2                */ 
      fclose(fichier2);                             /* fermeture de fichier2 et incrémentation de nombreChaines */
      nombreChaines++;
      sprintf(nomFichier2, "%s%03d.txt", prefixe, nombreChaines); /* Ecrit dans la chaine nomFichier2
                                                                     le nom formaté du fichier suivant   */
      fichier2 = fopen(nomFichier2, "w+");                        /* On crée le nouveau fichier          */
    }
    else
    {
      fputs(ligne1, fichier2);                       /* Copie la ligne dans fichier2 */
    }
  }
  fclose(fichier1);
  fclose(fichier2);

  return nombreChaines;
}

  /* ************************* moveFichiers ****************************** */

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
//        if (mkdir(cheminApitemp, 0775) != 0)
//          {
            printf("Impossible de créer le répertoire %s!\n", cheminApitemp);
            exit(1);
//          }
      }

// calcul le nombre de fichiers "resutext" contenu dans le dossier cheminApitemp
  while ((fichierLu = readdir(rep)) != NULL)
  {
    if (strcmp(fichierLu->d_name, ".") != 0 && /* Si le fichier lu n'est pas . */
        strcmp(fichierLu->d_name, "..") != 0 && /*  Et n'est pas .. non plus */
        strncmp(fichierLu->d_name, "resutext", 8) == 0)  /* strncmp compare les n premiers octets au plus 
                                                            de deux chaînes en utilisant l'ordre lexicographique */
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

  /* ************************* lireChemins ****************************** */

void lireChemins(char *cheminApicrypt, char *cheminApitemp)
{
  char home[300];

  strcpy(home, getenv("HOME"));

  FILE* fichier1 = NULL;
  fichier1 = fopen("apikmail.ini", "r+");

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
    fichier1 = fopen("apikmail.ini", "w+");

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
      perror("Impossible d'ouvrir ou de créer le fichier apikmail.ini ");
      printf("errno = %d.\n", errno);
      exit(1);
    }
  }
}
