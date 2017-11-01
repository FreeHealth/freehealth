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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ICD_INTERNAL_ICD10STEP_H
#define ICD_INTERNAL_ICD10STEP_H

#include <coreplugin/ifullreleasestep.h>

/**
 * \file icd10step.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 25 Nov 2012
*/

namespace Icd10 {
namespace Internal {
class Icd10StepPrivate;

class Icd10Step : public Core::IFullReleaseStep
{
    Q_OBJECT
public:
    explicit Icd10Step(QObject *parent = 0);
    ~Icd10Step();
    bool initialize();

    // Core::IFullReleaseStep Interface
    QString id() const {return objectName();}
    Steps stepNumber() const {return IcdDatabase;}
    bool createTemporaryStorage();
    bool cleanTemporaryStorage();
    bool startProcessing(ProcessTiming timing, SubProcess subProcess);

    bool startDownload();
    bool postDownloadProcessing();
    bool process();
    bool createDatabase();
    bool populateDatabaseWithRawSources();
    QString processMessage() const;

    bool registerDataPack();

    QStringList errors() const;

private Q_SLOTS:
    void onSubProcessFinished();

private:
    Internal::Icd10StepPrivate *d;
};

} // namespace Internal
} // namespace Icd10

#endif // ICD_INTERNAL_ICD10STEP_H

