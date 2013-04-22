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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class PMH::Internal::PmhContentExporter
 * \brief short description of class
 *
 * Long description of class
 * \sa PMH::
 */

#include "pmhcontentexporter.h"
#include "pmhcore.h"
#include "pmhcategorymodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>

#include <QTime>
#include <QDateTime>

#include <QDebug>

// TODO: check user rights+++++

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline PMH::PmhCategoryModel *pmhCategoryModel() {return PMH::PmhCore::instance()->pmhCategoryModel();}

namespace PMH {
namespace Internal {
class PmhContentExporterPrivate
{
public:
    PmhContentExporterPrivate(PmhContentExporter *parent) :
        q(parent)
    {
    }
    
    ~PmhContentExporterPrivate()
    {
    }
    
public:
    // Put your data here
    
private:
    PmhContentExporter *q;
};
} // namespace Internal
} // end namespace PMH


/*! Constructor of the PMH::Internal::PmhContentExporter class */
PmhContentExporter::PmhContentExporter(QObject *parent) :
    Core::IPatientDataExporter(parent),
    d(new PmhContentExporterPrivate(this))
{
    setObjectName("PmhContentExporter");
    setExporterTypes(PmhxExporter);
}

/*! Destructor of the PMH::Internal::PmhContentExporter class */
PmhContentExporter::~PmhContentExporter()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PmhContentExporter::initialize()
{
    return true;
}

bool PmhContentExporter::isBusy() const
{
    return false;
}

Core::PatientDataExtraction *PmhContentExporter::startExportationJob(const Core::PatientDataExporterJob &job)
{
    Core::PatientDataExtraction *result = 0;
    // Patient to export must be the current patient
    if (job.patientUids().count()==0)
        return result;

    if (job.patientUids().at(0).isEmpty())
        return result;

    // Patient to extract must be the current one
    if (job.patientUids().at(0) != patient()->uuid()) {
        LOG_ERROR("Extracting wrong patient");
        return result;
    }

    // Log chrono & emits progress signals
    QTime chr;
    chr.start();
    QString msg = tr("Start PMHx exportation for patient %1")
            .arg(patient()->uuid());
    LOG(msg);
    Q_EMIT extractionProgressRangeChanged(0, 1);
    Q_EMIT extractionProgressValueChanged(0);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Extract form episodes & generate HTML output file
    result = new Core::PatientDataExtraction;
    QString overview = pmhCategoryModel()->synthesis();
    QString fileName = QString("%1/%2_%3.html")
            .arg(job.outputAbsolutePath())
            .arg("pmhx_")
            .arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"));
    if (!Utils::saveStringToFile(overview, fileName, Utils::Overwrite, Utils::DontWarnUser)) {
        LOG_ERROR(tkTr(Trans::Constants::FILE_1_CAN_NOT_BE_CREATED).arg(fileName));
        result->addErrorMessage(tkTr(Trans::Constants::FILE_1_CAN_NOT_BE_CREATED).arg(fileName));
    } else {
        result->setMasterAbsoluteFilePath(fileName);
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    msg = tr("Exportation done in %1 ms").arg(chr.elapsed());
    Q_EMIT extractionProgressMessageChanged(msg);
    LOG(msg);

    return result;
}
