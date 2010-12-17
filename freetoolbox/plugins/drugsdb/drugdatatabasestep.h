#ifndef DRUGDATATABASESTEP_H
#define DRUGDATATABASESTEP_H

#include <coreplugin/ifullreleasestep.h>

#include <QStringList>


class DrugDatatabaseStep : Core::IFullReleaseStep
{
public:
    DrugDatatabaseStep(QObject *parent = 0);
    Steps stepNumber() const;

    bool createDir();
    bool cleanFiles();
    bool downloadFiles();
    bool process();

    QStringList errors() const {return m_Errors;}

private:
    QStringList m_Errors;
};

#endif // DRUGDATATABASESTEP_H
