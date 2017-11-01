/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IFULLRELEASESTEP_H
#define IFULLRELEASESTEP_H

#include <coreplugin/core_exporter.h>
#include <QObject>

/**
 * \file ifullreleasestep.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 10 Nov 2012
*/

namespace Core {
namespace Internal {
// You shouldn't directly access this member.
// Use The IFullReleaseStep::hasError(), IFullReleaseStep::errorMessage()
struct SubProcessError {
    QString errorMsg;
    int timing, subProcess;
};
}

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
        Extras,
        DataPackProcessing,
        TmpCleaning
    };

    enum ProcessTiming {
        PreProcess = 0,
        Process,
        PostProcess
    };

    enum SubProcess {
        Initialization = 0,
        Main,
        DataPackSubProcess,
        Final
    };

    explicit IFullReleaseStep(QObject *parent = 0) : QObject(parent) {}
    virtual ~IFullReleaseStep() {}

    virtual QString id() const = 0;
    virtual Steps stepNumber() const = 0;

    virtual bool createTemporaryStorage() = 0;
    virtual bool cleanTemporaryStorage() = 0;

    virtual bool startProcessing(ProcessTiming timing, SubProcess subProcess) = 0;

    // Error management
    virtual void clearErrors();
    virtual void addError(ProcessTiming timing, SubProcess subProcess, const QString &message);
    virtual bool hasError() const;
    virtual bool hasError(ProcessTiming timing, SubProcess subProcess) const;
    virtual QString errorMessage(ProcessTiming timing, SubProcess subProcess) const;

    // OBSOLETE
//    virtual bool startDownload() = 0;
//    virtual bool postDownloadProcessing();
//    virtual bool process() = 0;

//    virtual bool registerDataPack() = 0;

//    virtual QString processMessage() const = 0;
//    virtual QStringList errors() const = 0;
    // END OBSOLETE

    static bool lessThan(const IFullReleaseStep *s1, const IFullReleaseStep *s2) {return s1->stepNumber() < s2->stepNumber();}

Q_SIGNALS:
    void subProcessFinished(Core::IFullReleaseStep::ProcessTiming timing, Core::IFullReleaseStep::SubProcess subProcess);

    void downloadFinished();
    void postDownloadProcessingFinished();
    void processFinished();

    // Progress dialog management when threaded
    void progressLabelChanged(const QString &label);
    void progress(int value);
    void progressRangeChanged(int min, int max);

private:
    QList<Internal::SubProcessError> _errors;
};

} //  End namespace Core

#endif // IFULLRELEASESTEP_H
