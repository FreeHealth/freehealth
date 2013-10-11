/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include <quazip/JlCompress.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QDomDocument>
#include <QFileInfo>
#include <QTimer>

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

/** --- XML Architecture ---
  <datapack description="" server="">
    <content type=""></content>
  </datapack>
 */

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DataPackPlugin::DataPackCore *datapackCore() {return DataPackPlugin::DataPackCore::instance();}

namespace {
const char * const XML_ROOT_TAG = "Automatic_DataPack";
const char * const XML_DATAPACK_TAG = "datapack";
const char * const XML_DATAPACK_CONTENT_TAG = "content";
const char * const XML_DATAPACK_DESCRIPTION_ATTRIB = "description";
const char * const XML_DATAPACK_SERVER_ATTRIB = "server";
const char * const XML_CONTENT_TYPE_ATTRIB = "type";

const char * const XML_TYPE_FILE_ZIPPED = "file_zipped";
const char * const XML_TYPE_FILE_NONZIPPED = "file_nonzipped";
const char * const XML_TYPE_DIR = "dir";
}

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
    // Get XML file from the git path
    QString descriptionFile = QString("%1/%2/%3")
            .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
            .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
            .arg("auto_datapack/description.xml");
    QString descriptionFilePath = QFileInfo(descriptionFile).absolutePath() + QDir::separator();
    QDomDocument doc;
    int line = 0;
    int col = 0;
    QString error;
    if (!doc.setContent(Utils::readTextFile(descriptionFile, Utils::DontWarnUser), &error, &line, &col)) {
        LOG_ERROR(tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(error).arg(line).arg(col));
        addError(Process, DataPackSubProcess, QString("Unable to read file. %1;%2: %3").arg(line).arg(col).arg(error));
        return false;
    }

    // Read XML
    QDomElement root = doc.documentElement();
    if (root.tagName().compare(::XML_ROOT_TAG, Qt::CaseInsensitive) != 0) {
        LOG_ERROR("Wrong root tag: " + root.tagName() + "; awaiting Automatic_DataPack");
        addError(Process, DataPackSubProcess, QString("Wrong root tag: " + root.tagName() + "; awaiting Automatic_DataPack"));
        return false;
    }

    QDomElement packElement = root.firstChildElement(::XML_DATAPACK_TAG);
    while (!packElement.isNull()) {
        // Get pack description filename
        QString packDescr = packElement.attribute(::XML_DATAPACK_DESCRIPTION_ATTRIB);
        QFileInfo packDescrFile(packDescr);
        if (packDescrFile.isRelative())
            packDescrFile.setFile(descriptionFilePath + packDescr);
        if (!packDescrFile.exists()) {
            LOG_ERROR("Pack does not exists: " + packDescrFile.absoluteFilePath());
            addError(Process, DataPackSubProcess, QString("Pack does not exists: " + packDescrFile.absoluteFilePath()));
            packElement = packElement.nextSiblingElement(::XML_DATAPACK_TAG);
            continue;
        }

        // Get server
        QString server = packElement.attribute(::XML_DATAPACK_SERVER_ATTRIB);

        // Prepare pack
        DataPackQuery query;
        query.setDescriptionFileAbsolutePath(packDescrFile.absoluteFilePath());

        // Get datapack content
        QDomElement content = packElement.firstChildElement(::XML_DATAPACK_CONTENT_TAG);
        if (!content.isNull()) {
            const QString &type = content.attribute(::XML_CONTENT_TYPE_ATTRIB);
            if (type.compare(::XML_TYPE_DIR, Qt::CaseInsensitive) == 0) {
                // Zip dir content
                QString dirPath = content.text();
                if (QDir(dirPath).isRelative())
                    dirPath.prepend(descriptionFilePath);
                if (!JlCompress::compressDir(packDescrFile.absolutePath()+"/pack.zip", dirPath, true)) {
                    LOG_ERROR("Unable to compress dir: "+dirPath);
                    addError(Process, DataPackSubProcess, QString("Unable to compress dir: "+dirPath));
                }
                // Include zipped file into the query
                query.setOriginalContentFileAbsolutePath(packDescrFile.absolutePath()+"/pack.zip");
            } else if (type.compare(::XML_TYPE_FILE_NONZIPPED, Qt::CaseInsensitive) == 0) {
                // Zip file
                if (QFileInfo(content.text()).isRelative())
                    query.setOriginalContentFileAbsolutePath(descriptionFilePath + content.text());
                else
                    query.setOriginalContentFileAbsolutePath(content.text());
                query.setZipOriginalFile(true);
            } else {
                // Just include the zipped file to the query
                if (QFileInfo(content.text()).isRelative())
                    query.setOriginalContentFileAbsolutePath(descriptionFilePath + content.text());
                else
                    query.setOriginalContentFileAbsolutePath(content.text());
            }
        }

        if (!datapackCore()->registerDataPack(query, server)) {
            LOG_ERROR("Datapack not registered: " + query.descriptionFileAbsolutePath());
            addError(Process, DataPackSubProcess, QString("Datapack not registered: " + query.descriptionFileAbsolutePath()));
        }

        // Next sibling
        packElement = packElement.nextSiblingElement(::XML_DATAPACK_TAG);
    }
    return true;
}
