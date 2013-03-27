/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  Redistribution and use in source and binary forms, with or without     *
 *  modification, are permitted provided that the following conditions     *
 *  are met:                                                               *
 *   * Redistributions of source code must retain the above copyright      *
 *     notice, this list of conditions and the following disclaimer.       *
 *   * Redistributions in binary form must reproduce the above copyright   *
 *     notice, this list of conditions and the following disclaimer        *
 *     in the documentation and/or other materials provided with the       *
 *     distribution.                                                       *
 *   * Neither the name of the FreeMedForms nor the names of its           *
 *     contributors may be used to endorse or promote products derived     *
 *     from this software without specific prior written permission.       *
 *                                                                         *
 *                                                                         *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    *
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      *
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  *
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT       *
 *  HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,        *
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;                           *
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       *
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT     *
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY  *
 *  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE            *
 *  POSSIBILITY OF SUCH DAMAGE.                                            *
 *                                                                         *
 *  You should have received a copy of the BSD 3 clause License            *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *      Laurent Masson <docteur.laurentmasson@gmail.com>                   *
 *      Pierre-Marie Desombre <pm.desombre@gmail.com>                      *
 *      Eric MAEKER, <eric.maeker@gmail.com>                               *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/***************************************************************************
 *                              ApiKmail                                   *
 ***************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "fonctions.h"


int main(int argc, char *argv[])
{
  int numero = 0;
  char c;
  char nomFichier[150];
  char nomFichierTxt[150];
  char nomFichier1[150];
  char nomFichier2[150];
  char prefixe[50];
  char decrypter[300];
  char ligne[300];
  char cheminApicrypt[300];
  char cheminApitemp[300];

  lireChemins(cheminApicrypt, cheminApitemp);

  sprintf(nomFichier, "%smail", cheminApicrypt); // Ecrit dans la chaine nomFichier le nom formaté du fichier mail

  FILE* fichier2 = NULL;
  fichier2 = fopen(nomFichier, "w+");

  if (fichier2 != NULL)
  {
    /* utilisation des flux standards
       d'entrées-sorties : stdin et stdout */
    while ((c = getchar()) != EOF)
    {
      fputc(c, fichier2); // copie le mail d'origine (flux envoyé par Kmail) dans nomFichier
    }

    filenstr("Date:", fichier2, 1, ligne); // Trouve la ligne contenant de la 1ère occurence de la chaine
    filenstr("Subject:", fichier2, 1, ligne); // Trouve la ligne contenant de la 1ère occurence de la chaine
    filenstr("To:", fichier2, 1, ligne); // Trouve la ligne contenant de la 1ère occurence de la chaine
    filenstr("From:", fichier2, 1, ligne); // Trouve la ligne contenant de la 1ère occurence de la chaine
    printf("Content-Type: text/plain; charset=ISO-8859-1; format=flowed\nContent-Transfer-Encoding: 7bit\n\n");

    fclose(fichier2);

    sprintf(decrypter, "%sapiuncrypt -s %s -k %s", cheminApicrypt, nomFichier, cheminApicrypt);
    sprintf(nomFichierTxt, "%s.txt", nomFichier);

    system(decrypter);                         /* décrypte et copie dans nomFichier.txt */

    remove(nomFichier);                        /* efface le fichier nomFichier */

    sprintf(nomFichier1, "%smail.txt", cheminApicrypt);
    sprintf(nomFichier2, "%smailtext000.txt", cheminApicrypt);
    sprintf(prefixe, "%smailtext", cheminApicrypt);

    csplit("****FINFICHIER****", nomFichier1, nomFichier2, prefixe); /* copie le fichier nomFichier1 jusqu'à la ligne
                                                                        précédant la chaine "****FINFICHIER****"      */

//    if (mkdir(cheminApitemp, 0775) != 0) printf("problème dossier !\n");

    sprintf(nomFichier1, "%smailtext000.txt", cheminApicrypt);
    sprintf(nomFichier2, "%stmp/resutext000.txt", cheminApicrypt);
    sprintf(prefixe, "%stmp/resutext", cheminApicrypt);

    numero = csplit("****FIN****", nomFichier1, nomFichier2, prefixe); /* découpe le fichier en sections
                                                                          déterminées par des lignes contenant
                                                                          la chaine et renvoie le nombre d'occurences
                                                                          trouvées                                   */

    if (numero == 0) numero = 1;     /* si l'occurence "****FIN****" n'est pas trouvée
                                        il y a quand même un fichier à copier
                                        d'où "numero = 1"                              */

    moveFichiers(numero, cheminApicrypt, cheminApitemp); // renome et déplace les fichiers de tmp vers apitemp

    afficheFichier(nomFichierTxt); // Affiche le contenu du fichier décrypté

    remove(nomFichier1);   // efface le fichier nomFichier2
    remove(nomFichier2);   // efface le fichier nomFichier2
    remove(nomFichierTxt); // efface le fichier nomFichierTxt
  }
  return 0;
}
