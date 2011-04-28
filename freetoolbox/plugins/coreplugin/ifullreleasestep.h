#ifndef IFULLRELEASESTEP_H
#define IFULLRELEASESTEP_H

#include <coreplugin/core_exporter.h>
#include <QObject>

namespace Core {

class CORE_EXPORT IFullReleaseStep : public QObject
{
    Q_OBJECT

public:
    enum Steps {
        InteractionDatabase = 0,
        DrugsDatabase,
        IcdDatabase,
        PimDatabase,
        TmpCleaning
    };

    explicit IFullReleaseStep(QObject *parent = 0) : QObject(parent) {}
    virtual QString id() const = 0;
    virtual Steps stepNumber() const = 0;

    virtual bool createDir() = 0;
    virtual bool cleanFiles() = 0;
    virtual bool downloadFiles() = 0;
    virtual bool process() = 0;
    virtual QString processMessage() const = 0;

    virtual bool postProcessDownload() {Q_EMIT postProcessDownloadFinished();return true;}

    virtual QStringList errors() const = 0;

    static bool lessThan(const IFullReleaseStep *s1, const IFullReleaseStep *s2) {return s1->stepNumber() < s2->stepNumber();}

Q_SIGNALS:
    void downloadFinished();
    void processFinished();
    void postProcessDownloadFinished();

    // Progress dialog management when threaded
    void progressLabelChanged(const QString &label);
    void progress(int done);
    void progressRangeChanged(int min, int max);

public Q_SLOTS:

};

} //  End namespace Core

#endif // IFULLRELEASESTEP_H
