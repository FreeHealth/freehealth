# include <iostream>
# include <fstream>
# include <string>
# include <cstdlib> // library for getenv()
# include "fonctions.h"

using namespace std;

void readFileIni(string *pathApicrypt, string *pathTemp)
{
    string const fileName("apiKmail.ini");
//    string pathApicrypt;
//    string pathTemp;

    ifstream file(fileName.c_str());
    if (file)
    {
        string line;

        if(getline(file, line))
        {
            *pathApicrypt = line;
        }
        if(getline(file, line))
        {
            *pathTemp = line;
        }
        cout << "pathApicrypt = " << *pathApicrypt << endl;
        cout << "pathTemp = " << *pathTemp << endl;
        cout << getenv("HOME") << endl;  
    }
    else
    {
        ofstream file(fileName.c_str());
        if (file) // if file don't exist, it will created
        {
            file << getenv("HOME") << "/home/laurent/apicrypt/" << endl;
            file << getenv("HOME") << "/MedinTux/Programmes/apitux/bin/apitemp/" << endl;
        }
        else
        {
            cout << "Erreur : impossible d'ouvrir et/ou créer le file " << fileName << endl;
        }
    }

}
