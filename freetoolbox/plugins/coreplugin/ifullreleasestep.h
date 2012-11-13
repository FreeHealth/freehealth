/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
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
 * \version 0.8.0
 * \date 10 Nov 2012
*/

namespace Core {

/*!
 * \brief IFullReleaseStep provides a generic interface for generating data for FreeToolBox.
 *
 * It is a plugin extension point that is called during the FreeToolBox IFullReleaseStep action.
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
        DataPackProcessing,
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

    /*!
     * \brief Implementations of this method can process the registration of datapacks.
     * \returns \e true if successful, else \e false.
     * \sa DataPackPlugin::DataPackCore::registerDataPack()
     */
    virtual bool registerDataPack() = 0;

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

#endif // IFULLRELEASESTEP_H
