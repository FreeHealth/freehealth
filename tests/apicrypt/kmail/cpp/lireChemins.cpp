# include <iostream>
# include <fstream>
# include <string>
# include <cstdlib> // librairie pour getenv()
# include "lireChemins.h"

using namespace std;

void lireChemins(string *cheminApicrypt, string *cheminApitemp)
{
    string const nomFichier("apiKmail.ini");
//    string cheminApicrypt;
//    string cheminApitemp;

    ifstream fichier(nomFichier.c_str());
    if (fichier)
    {
        string ligne;

        if(getline(fichier, ligne))
        {
            *cheminApicrypt = ligne;
        }
        if(getline(fichier, ligne))
        {
            *cheminApitemp = ligne;
        }
        cout << "cheminApicrypt = " << *cheminApicrypt << endl;
        cout << "cheminApitemp = " << *cheminApitemp << endl;
        cout << getenv("HOME") << endl;  
    }
    else
    {
        ofstream fichier(nomFichier.c_str());
        if (fichier) // si le fichier n'existe pas on le crée
        {
            fichier << getenv("HOME") << "/home/laurent/apicrypt/" << endl;
            fichier << getenv("HOME") << "/MedinTux/Programmes/apitux/bin/apitemp/" << endl;
        }
        else
        {
            cout << "Erreur : impossible d'ouvrir et/ou créer le fichier " << nomFichier << endl;
        }
    }

}
