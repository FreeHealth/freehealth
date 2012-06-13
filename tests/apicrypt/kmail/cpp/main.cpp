/********************************************************************************************
 *                                                                                          *
 * ApiKmailC                                                                                *
 *                                                                                          *
 ********************************************************************************************/

# include <iostream>
# include <fstream>
# include <string>
# include <cstdlib> // librairie pour getenv()
# include "lireChemins.h"

using namespace std;

int main()
{
    string * cheminApicrypt(0);
    string * cheminApitemp(0);
    cheminApicrypt = new string;
    
    lireChemins(*cheminApicrypt, *cheminApitemp);
    cout << *cheminApicrypt << endl;
    cout << *cheminApitemp << endl;
    
    delete cheminApicrypt;
    cheminApicrypt = 0;
    delete cheminApitemp;
    cheminApitemp = 0;
}
