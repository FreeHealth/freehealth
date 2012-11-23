#ifndef XMLWRITERBLOCK_H
#define XMLWRITERBLOCK_H

#include <QString>
#include <QFile>
#include "Patient.h"
#include "xmlwriter.h"

class xmlwriterblock {

public :
    xmlwriterblock(QFile* file , Patient* blockpatient);

};


#endif // XMLWRITERBLOCK_H
