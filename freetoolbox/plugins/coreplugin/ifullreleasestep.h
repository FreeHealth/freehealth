#ifndef IFULLRELEASESTEP_H
#define IFULLRELEASESTEP_H

#include <coreplugin/core_exporter.h>
#include <QObject>
QT_BEGIN_NAMESPACE
class QProgressBar;
QT_END_NAMESPACE

/**
 * \file ifullreleasestep.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 20 Jan 2012
*/

namespace Core {

/*!
 * \brief IFullReleaseStep provides a generic interface for generating data for FreeMedForms.
 *
 * It is a plugin extension point that is called during the FreeToolBox FullReleaseStep action.
 * Classes that implement this interface can provide a data source and storage for arbitrary data
 * that can be used then in FreeMedForms or one of its sub-applications.
 *
 * Generally first from all plugins the \sa createDir() function is called to create the temp directories.
 * Then the
 */
class CORE_EXPORT IFullReleaseStep : public QObject
{
    Q_OBJECT

public:
    enum Steps {
        InteractionDatabase = 0,
        DrugsDatabase,
        IcdDatabase,
        PimDatabase,
        PregnancyDatabase,
        ZipCodes,
        TmpCleaning
    };

    explicit IFullReleaseStep(QObject *parent = 0) : QObject(parent) {}
    /*! Returns the (meaningful) unique identifier of this step.*/
    virtual QString id() const = 0;

    /*!
     * \brief Represents the step number in the list of steps. The steps are
     * called in ascending order taken from this method. */
    virtual Steps stepNumber() const = 0;

    /*!
     * \brief Creates a temporary directory.
     * \returns \e true if successful, else \e false. */
    virtual bool createDir() = 0;

    /*!
     * \brief Removes the temporary directory and files.
     * \returns \e true if successful, else \e false. */
    virtual bool cleanFiles() = 0;

    /*!
     * \brief Downloads some files from a server.
     *
     * Optionally updates the QProgressBar \e bar.
     * \returns \e true if successful, else \e false. */
    virtual bool downloadFiles(QProgressBar *bar = 0) = 0;

    /*!
     * \brief Handles all the logic of the release step after downloadinf files.
     * \returns \e true if successful, else \e false. */
    virtual bool process() = 0;

    /*! Callback function, should provide a message that describes the current process. */
    virtual QString processMessage() const = 0;

    /*!
     * \brief Implementations of this method can process the data after downloading.
     *
     * The default implementation emits \e postProcessDownloadFinished() This should be done
     * in inherited implementations too.
     * \returns \e true if successful, else \e false. */
    virtual bool postProcessDownload() {Q_EMIT postProcessDownloadFinished(); return true;}

    /*! \brief Contains a list of error strings if something went wrong. */
    virtual QStringList errors() const = 0;

    /*! Provides a comparison of two steps, used by the sort algorythm. */
    static bool lessThan(const IFullReleaseStep *s1, const IFullReleaseStep *s2) {return s1->stepNumber() < s2->stepNumber();}

Q_SIGNALS:
    void downloadFinished();
    void processFinished();
    void postProcessDownloadFinished();

    // Progress dialog management when threaded
    void progressLabelChanged(const QString &label);
    void progress(int done);
    void progressRangeChanged(qint64 min, qint64 max);
};

} //  End namespace Core

#endif // IFULLRELEASESTEP_H.
