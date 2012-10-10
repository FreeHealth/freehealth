#ifndef PATIENT_H
#define PATIENT_H
#include <QString>

class Patient
{
public :

    //Patient block
        QString index ;
        QString prenom  ;
        QString nomdejeunefille ;
        QString nom  ;
        QString datedenaissance  ;
        QString datededeces ;

    //Patient contact
        QString adresse ;
        QString ville  ;
        QString region ;
        QString codepostal ;
        QString pays  ;

        QString telephone1  ;
        QString telephone2  ;
        QString telephone3  ;
        QString courriel  ;
        QString telecopie  ;

   //patient PMHx
        QString textelibre ;
        QString marque  ;
        QString codeinternat ;
        QString datededebut  ;
        QString datedefin  ;
        QString formatage  ;
        QString indexdeconfiance  ;

public :
        Patient(void);

public :
    //setter
        void setindex(const QString &s);
        void setprenom(const QString &s);
        void setnomdejeunefille(const QString &s);
        void setnom(const QString &s);
        void setdatedenaissance(const QString &s);
        void setdatededeces(const QString &s);
        void setadresse(const QString &s);
        void setville(const QString &s);
        void setregion(const QString &s);
        void setcodepostal(const QString &s);
        void setpays(const QString &s);
        void settelephone1(const QString &s);
        void settelephone2(const QString &s);
        void setcourriel(const QString &s);
        void settelecopie(const QString &s);
        void settextelibre(const QString &s);
        void setmarque(const QString &s);
        void setformatage(const QString &s);
        void setcodeinternat(const QString &s);
        void setdatededebut(const QString &s);
        void setdatedefin(const QString &s);
        void setindexdeconfiance(const QString &s);

     //getter
        void showPatientFile (Patient &patient);


       };



#endif // PATIENT_H
