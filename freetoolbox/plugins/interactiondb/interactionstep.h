#ifndef INTERACTIONSTEP_H
#define INTERACTIONSTEP_H

#include <coreplugin/ifullreleasestep.h>
#include <QStringList>


namespace IAMDb {

class InteractionStep : public Core::IFullReleaseStep
{
public:
    InteractionStep(QObject *parent = 0);

    QString id() const {return "InteractionStep";}
    Steps stepNumber() const {return Core::IFullReleaseStep::InteractionDatabase;}

    bool createDir();
    bool cleanFiles();
    bool downloadFiles();
    bool process();
    QString processMessage() const {return tr("Drug-Drug Interactions database creation");}

    QStringList errors() const {return m_Errors;}

private:
    QStringList m_Errors;
    bool m_UseProgressDialog;
};

}  //  End namespace IAMDb

#endif // INTERACTIONSTEP_H
