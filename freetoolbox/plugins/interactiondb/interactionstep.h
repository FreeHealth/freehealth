#ifndef INTERACTIONSTEP_H
#define INTERACTIONSTEP_H

#include <coreplugin/ifullreleasestep.h>
#include <QStringList>
#include <QVector>

namespace IAMDb {

class InteractionStep : public Core::IFullReleaseStep
{
    Q_OBJECT
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

public Q_SLOTS:
    void downloadNextSource();

private:
    QStringList m_Errors;
    bool m_UseProgressDialog;
    int m_ActiveDownloadId;
    QVector<int> m_SourceToDownload;
};

}  //  End namespace IAMDb

#endif // INTERACTIONSTEP_H
