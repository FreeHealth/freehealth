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
#include "genericdatapackcreator.h"
#include "datapackquery.h"
#include "datapackcore.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>
#include <datapackutils/servercreation/packcreationqueue.h>

#include <QTimer>

// TODO: create a specific page for "Automatic Pack Creation"

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DataPackPlugin::DataPackCore *datapackCore() {return DataPackPlugin::DataPackCore::instance();}

GenericDataPackCreator::GenericDataPackCreator(QObject *parent) :
    Core::IFullReleaseStep(parent)
{
    setObjectName("GenericDataPackCreator");
}

GenericDataPackCreator::Steps GenericDataPackCreator::stepNumber() const
{
    return Extras;
}

bool GenericDataPackCreator::createTemporaryStorage()
{
    return true;
}

bool GenericDataPackCreator::cleanTemporaryStorage()
{
    return true;
}

bool GenericDataPackCreator::startProcessing(ProcessTiming timing, SubProcess subProcess)
{
    bool ok = true;
    _currentTiming = timing;
    _currentSubProcess = subProcess;
    if (subProcess == DataPackSubProcess && timing == Process) {
        ok = createTemporaryStorage() && registerDataPack();
    }
    QTimer::singleShot(1, this, SLOT(onSubProcessFinished()));
    return ok;
}

void GenericDataPackCreator::onSubProcessFinished()
{
    Q_EMIT subProcessFinished(_currentTiming, _currentSubProcess);
}

bool GenericDataPackCreator::registerDataPack()
{
    // Read all queue, prepare and append all datapacks to the servers

    // Get the default XML file from the git path
    QString descriptionFile = QDir::cleanPath(QString("%1/%2/%3")
            .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
            .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
            .arg("auto_datapack/description.xml"));
    DataPack::PackCreationQueue packQueue;
    packQueue.fromXmlFile(descriptionFile);
    foreach(const DataPack::RequestedPackCreation &request, packQueue.queue()) {
        // Prepare pack
        QString zipFileName = QFileInfo(request.descriptionFilePath).absolutePath()+"/pack.zip";
        packQueue.createZippedContent(request, zipFileName);

        DataPackQuery query;
        query.setDescriptionFileAbsolutePath(request.descriptionFilePath);
        query.setOriginalContentFileAbsolutePath(zipFileName);

        if (!datapackCore()->registerDataPack(query, request.serverUid)) {
            LOG_ERROR("Datapack not registered: " + query.descriptionFileAbsolutePath());
            addError(Process, DataPackSubProcess, QString("Datapack not registered: " + query.descriptionFileAbsolutePath()));
        }
    }
    return true;
}
