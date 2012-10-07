#ifndef SAXHANDLER_H
#define SAXHANDLER_H

#include <QXmlDefaultHandler>
#include "patient.h"

class SaxHandler : public QXmlDefaultHandler
{
public:
    bool readFile();
    SaxHandler(Patient* patienttest, QFile* filetest);


public:
      Patient* patientptr;
      QFile* fileptr;

};

#endif
