#ifndef SAXHANDLER_H
#define SAXHANDLER_H

#include <QXmlDefaultHandler>
#include "patient.h"

class SaxHandler : public QXmlDefaultHandler
{
public:
    bool readFile(const QString &fileName);
    SaxHandler(Patient* patienttest, QFile* filetest);


protected:
    bool startElement(const QString &namespaceURI,
                      const QString &localName,
                      const QString &qName,
                      const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI,
                    const QString &localName,
                    const QString & qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);

public:
      Patient* patientptr;
      QFile* fileptr;


};

#endif
