/*

fonctions.h

*/

#ifndef DEF_FONCTIONS
#define DEF_FONCTIONS

int filenstr(const char *chaine, FILE *fichier, int occurence, char *ligne);
void afficheFichier(char *nomFichierTxt);
int csplit(const char *chaine, char *nomFichier1, char *nomFichier2, const char *prefixe);
void moveFichiers(int nbFichier, char *cheminApicrypt, char *cheminApitemp);
void lireChemins(char *cheminApicrypt, char *cheminApitemp);

#endif
