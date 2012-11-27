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
    virtual QString id() const = 0;
    virtual Steps stepNumber() const = 0;

    virtual bool createTemporaryStorage() = 0;
    virtual bool cleanTemporaryStorage() = 0;

    virtual bool startDownload(QProgressBar *bar = 0) = 0;
    virtual bool postDownloadProcessing() {Q_EMIT postDownloadProcessingFinished(); return true;}
    virtual bool process() = 0;

    virtual bool registerDataPack() = 0;

    virtual QString processMessage() const = 0;
    virtual QStringList errors() const = 0;

    static bool lessThan(const IFullReleaseStep *s1, const IFullReleaseStep *s2) {return s1->stepNumber() < s2->stepNumber();}

Q_SIGNALS:
    void downloadFinished();
    void postDownloadProcessingFinished();
    void processFinished();

    // Progress dialog management when threaded
    void progressLabelChanged(const QString &label);
    void progress(int done);
    void progressRangeChanged(qint64 min, qint64 max);
};

} //  End namespace Core

#endif // IFULLRELEASESTEP_H
