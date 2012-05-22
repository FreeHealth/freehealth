#include <QFile>
#include "xmlwriter.h"
#include "patient.h"
#include "xmlwriterblock.h"



xmlwriterblock::xmlwriterblock(QFile* file , Patient* blockpatient)
{

    XmlWriter xw (file);
    xw.setAutoNewLine( true );

    const QString index = blockpatient->index;
    const QString prenom = blockpatient->prenom;
    const QString nomjeunefille = blockpatient->nomdejeunefille;
    const QString nom = blockpatient->nom;
    const QString datedenaissance = blockpatient->datedenaissance;
    const QString datededeces= blockpatient->datededeces;

    const QString adresse = blockpatient->adresse;
    const QString ville = blockpatient->ville;
    const QString region = blockpatient->region;
    const QString codepostal = blockpatient->codepostal;
    const QString pays = blockpatient->pays;

    const QString telephone1 = blockpatient->telephone1;
    const QString telephone2 = blockpatient->telephone2;
    const QString telephone3 = blockpatient->telephone3;
    const QString courriel = blockpatient->courriel;
    const QString telecopie = blockpatient->telecopie;

    const QString textelibre = blockpatient->textelibre;
    const QString marque = blockpatient->marque;
    const QString codeinternat = blockpatient->codeinternat;
    const QString datededebut = blockpatient->datededebut;
    const QString datedefin = blockpatient->datedefin;
    const QString formatage = blockpatient->formatage;
    const QString indexdeconfiance = blockpatient->indexdeconfiance;


    xw.newLine();
    xw.writeRaw("<!-- Patient block -->");
    xw.newLine();
    AttrMap attrPatient;
    attrPatient.insert( "emrUid", index);
    attrPatient.insert( "firstname" , prenom);
    attrPatient.insert( "birthname" , nom);
    attrPatient.insert( "secondname", nomjeunefille);
    attrPatient.insert( "dob", datedenaissance);
    attrPatient.insert( "dod", datededeces);
    xw.writeOpenTag("Patient", attrPatient);
    xw.writeRaw("<!-- Contact -->");
    xw.newLine();
    AttrMap attrAdress;
    attrAdress.insert( "ad", adresse);
    attrAdress.insert( "city", ville);
    attrAdress.insert( "county", region);
    attrAdress.insert( "zip" , codepostal);
    attrAdress.insert( "state" , pays);
    xw.writeOpenTag( "Address", attrAdress);
    AttrMap attrContact;
    attrContact.insert( "tel1", telephone1);
    attrContact.insert( "tel2", telephone2);
    attrContact.insert( "tel3" , telephone3);
    attrContact.insert( "fax" , telecopie);
    attrContact.insert("mail" , courriel);
    xw.writeOpenTag( "Contact", attrContact);
    xw.writeRaw( "<!-- PMHx -->");
    xw.newLine();
    xw.writeOpenTag("PMHx");
    xw.writeOpenTag("PMH_Item");
    xw.writeRaw("<!-- use FreeText or Encoded not both -->");
    xw.newLine();
    xw.writeTaggedString( "FreeText", textelibre, AttrMap() );
    AttrMap attrEncoded;
    attrEncoded.insert("label", marque);
    attrEncoded.insert( "icd10" , codeinternat);
    xw.writeOpenTag( "Encoded" , attrEncoded);
    xw.writeTaggedString( "DateStart", datededebut, AttrMap("format", formatage) );
    xw.writeTaggedString( "DateEnd", datedefin, AttrMap("format", formatage) );
    xw.writeTaggedString( "ConfidenceIndex", indexdeconfiance, AttrMap() );
    xw.writeCloseTag("PMH_Item");
    xw.writeCloseTag("PMHx");
    xw.writeRaw("<!-- Populate Form items -->");
    xw.newLine();
    xw.writeCloseTag("Patient");
    xw.newLine();

}

