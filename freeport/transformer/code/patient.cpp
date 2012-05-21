#include "patient.h"
#include <QString>
#include <QDebug>
#include <iostream>

Patient::Patient(void)
{
    this->adresse="";
    this->codeinternat="";
    this->codepostal="";
    this->courriel="";
    this->datededebut="";
    this->datededeces="";
    this->datedefin="";
    this->datedenaissance="";
    this->formatage="";
    this->index="";
    this->indexdeconfiance="";
    this->marque="";
    this->nom="";
    this->nomdejeunefille="";
    this->pays="";
    this->prenom="";
    this->region="";
    this->telecopie="";
    this->telephone1="";
    this->telephone2="";
    this->telephone3="";
    this->textelibre="";
    this->ville="";
}

    //Setter
void Patient::setindex(const QString &s)
{

    this->index = s;

}

void Patient::setprenom(const QString &s)
{
    this->prenom = s;
}
void Patient::setnomdejeunefille(const QString &s)
{
    this->nomdejeunefille = s;
}
void Patient::setnom(const QString &s)
{
    this->nom = s;
}
void Patient::setdatedenaissance(const QString &s)
{
    this->datedenaissance = s;
}
void Patient::setdatededeces(const QString &s)
{
    this->datededeces = s;
}
void Patient::setadresse(const QString &s)
{
    this->adresse = s;
}
void Patient::setville(const QString &s)
{
    this->ville = s;
}
void Patient::setregion(const QString &s)
{
    this->region = s;
}
void Patient::setcodepostal(const QString &s)
{
    this->codepostal = s;
}
void Patient::setpays(const QString &s)
{
    this->pays = s;
}
void Patient::settelephone1(const QString &s)
{
    this->telephone1 = s;
}
void Patient::settelephone2(const QString &s)
{
    this->telephone2 = s;
}
void Patient::setcourriel(const QString &s)
{
    this->courriel = s;
}
void Patient::settelecopie(const QString &s)
{
    this->telecopie = s;
}
void Patient::settextelibre(const QString &s)
{
    this->textelibre = s;
}
void Patient::setmarque(const QString &s)
{
    this->marque = s;
}
void Patient::setformatage(const QString &s)
{
    this->formatage = s;
}
void Patient::setcodeinternat(const QString &s)
{
    this->codeinternat = s;
}
void Patient::setdatededebut(const QString &s)
{
    this->datededebut = s;
}
void Patient::setdatedefin(const QString &s)
{
    this->datedefin = s;
}
void Patient::setindexdeconfiance(const QString &s)
{
    this->indexdeconfiance = s;
}

    //Getter


void Patient::showPatientFile(Patient &patient)
{
    //Patient block
        QString x1 = patient.index ;
        QString x2 = patient.prenom ;
        QString x3 = patient.nomdejeunefille;
        QString x4 = patient.nom ;
        QString x5 = patient.datedenaissance ;
        QString x6 = patient.datededeces;

    //Patient contact
        QString a = patient.adresse;
        QString b = patient.ville ;
        QString c = patient.region ;
        QString d = patient.codepostal ;
        QString e = patient.pays ;

        QString f = patient.telephone1 ;
        QString g = patient.telephone2 ;
        QString h = patient.telephone3 ;
        QString i = patient.courriel ;
        QString j = patient.telecopie ;

   //patient PMHx
        QString k = patient.textelibre;
        QString l = patient.marque ;
        QString m = patient.codeinternat;
        QString n = patient.datededebut ;
        QString o = patient.datedefin ;
        QString p = patient.formatage ;
        QString q = patient.indexdeconfiance ;

        qDebug()
                  //Patient block
                     << "index : " << x1 << endl
                     << "prenom : " << x2 << endl
                     << "nomdejeunefille : " << x3 <<endl
                     << "nom : " << x4 << endl
                     << "datedenaissance : " << x5 << endl
                     << "datededeces : " << x6 << endl

                  //Patient contact
                     << "adresse : " << a << endl
                     << "ville : "  << b << endl
                     << "region : " << c << endl
                     << "codepostal : " << d << endl
                     << "pays : " << e << endl

                     << "telephone1 : " << f << endl
                     << "telephone2 : " << g << endl
                     << "telephone3 : " << h << endl
                     << "courriel : " << i << endl
                     << "telecopie : " << j << endl

                 //patient PMHx
                     << "textelibre : " << k << endl
                     << "marque : " << l << endl
                     << "codeinternat : " << m << endl
                     << "datededebut : " << n << endl
                     << "datedefin : " << o << endl
                     << "formatage : " << p << endl
                     << "indexdeconfiance : " << q << endl;}



