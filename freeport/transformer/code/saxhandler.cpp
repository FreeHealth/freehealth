#include <QtGui>
#include <iostream>
#include <QDebug>
#include "saxhandler.h"
#include "patient.h"
#include "xmlwriterblock.h"

SaxHandler::SaxHandler(Patient* patienttest, QFile* filetest)
{
    patientptr = patienttest;
    fileptr = filetest;
}


bool SaxHandler::readFile(const QString &fileName)
{
    QFile file(fileName);
    if ( !file.exists()){
        qDebug() << "The file"<< file.fileName() << "doesn't exist.";
        return -1;
    }
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    return reader.parse(inputSource);
}

    static QString test ="";
    QString chr;
    static QString control ="";


bool SaxHandler::startElement(const QString & /* namespaceURI */,
                              const QString & /* localName */,
                              const QString &qName,
                              const QXmlAttributes &attributes)
{

    control = "start";
    test = qName;
    return true;
}


bool SaxHandler::characters(const QString &str)
{

    chr = str;



    //Fill in the data
        if (control == "start"){
    if (test == "NCEPatient") patientptr->setindex(str); else
    if (test == "PatientNom") patientptr->setnom(str); else
    if (test == "PatientPrenom") patientptr->setprenom(str); else
    if (test == "PatientDateNaissance") patientptr->setdatedenaissance(str); else
    if (test == "AssureAdresse1") patientptr->setadresse(str); else
    if (test == "AssureCodePostal") patientptr->setcodepostal(str); else
    if (test == "AssureVille") patientptr->setville(str); else
    if (test == "AntecedentTexte1") patientptr->settextelibre(str); else
    if (test == "PatientTelephone1") patientptr->settelephone1(str); else
    if (test == "PatientTelephone2") patientptr->settelephone2(str);
}
      return true;
}

bool SaxHandler::endElement(const QString & /* namespaceURI */,
                            const QString & /* localName */,
                            const QString & qName)
{

    //qDebug() <<"End element : "<< qName;
    if (qName =="OMNIS_FPatient")
    {
        QString test1= patientptr->nom;
        //qDebug() <<" Tentative d'ecriture de patient : " << test1;
        xmlwriterblock patientblock(fileptr, patientptr);
    }
    control = "end";
    return true;
}

bool SaxHandler::fatalError(const QXmlParseException &exception)
{
    std::cerr << "Parse error at line " << exception.lineNumber()
              << ", " << "column " << exception.columnNumber() << ": "
              << qPrintable(exception.message()) << std::endl;
    return false;
}
