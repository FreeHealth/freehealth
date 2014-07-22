/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developers : Eric Maeker                                          *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "packservercreator.h"
#include <datapackutils/pack.h>
#include <datapackutils/servercontent.h>
#include <datapackutils/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QString>
#include <QDir>
#include <QFile>
#include <QDomDocument>

using namespace DataPack;
using namespace Trans::ConstantTranslations;

namespace {

static QString getVendor(const QString &serverUid)
{
    if (serverUid == Constants::SERVER_COMMUNITY_FREE)
        return "community";
    else if (serverUid == Constants::SERVER_COMMUNITY_NONFREE)
        return "community";
    else if (serverUid == Constants::SERVER_ASSO_FREE)
        return "asso";
    else if (serverUid == Constants::SERVER_ASSO_NONFREE)
        return "asso";
    return serverUid;
}
} // anonymous namespace

PackServerCreator::PackServerCreator()
{}

PackServerCreator::~PackServerCreator()
{}

/** Add a new queue to this server */
bool PackServerCreator::addPackCreationQueue(const PackCreationQueue &queue)
{
    _queue += queue;
    return true;
}

/**
 * Define the server description files to use (\e descrAbsFileName)
 * for any server \e serverUid. New data will not erase already recorded data.
 */
bool PackServerCreator::setServerDescriptionFilePath(const QString &serverUid, const QString &descrAbsFileName)
{
    if (_serverUid_DescrFile.keys().contains(serverUid))
        return false;
   _serverUid_DescrFile.insert(serverUid, descrAbsFileName);
    return true;
}

/**
 * You must ensure that the server output path is empty to avoid conflicts.
 * \sa createZippedContent()
 */
bool PackServerCreator::createServer(const QString &serverAbsPath) const
{
    // Queue is empty -> error
    if (_queue.isEmpty()) {
        LOG_ERROR_FOR("PackServerCreator", "No Pack selected/found for server creation");
        return false;
    }

    // Create server output path.
    // Structure of server path is
    // "free"
    //   +- <vendor> ("community" or "asso")
    //        +- <version>  (serverRootPath)
    //             +- serverconf.zip
    //             +- <PackName>
    //                   +- pack-files.{xml,zip}
    //   +- <vendor> ("community" or "asso")
    //        +- <version>  (serverRootPath)
    //             +- serverconf.zip
    //             +- <PackName>
    //                   +- pack-files.{xml,zip}
    // "nonfree"
    //   +- <vendor> ("community" or "asso")
    //        +- <version>  (serverRootPath)
    //             +- serverconf.zip
    //             +- <PackName>
    //                   +- pack-files.{xml,zip}
    //   +- <vendor> ("community" or "asso")
    //        +- <version>  (serverRootPath)
    //             +- serverconf.zip
    //             +- <PackName>
    //                   +- pack-files.{xml,zip}

    // TEST
    QString queueXmlFile = serverAbsPath + "/queue.xml";
    if (!_queue.saveToXmlFile(queueXmlFile, false)) {
        LOG_ERROR_FOR("PackServerCreator", QString("Unable to save queue: %1").arg(queueXmlFile));
        return false;
    } else {
        LOG_FOR("PackServerCreator", QString("File Created: %1").arg(queueXmlFile));
    }
    // END TEST

    // Create the internal queue zipcontent and update the Pack description files
    QList<*Pack> packs;
    QHash<QString, *ServerContent> serverContent;

    foreach(const RequestedPackCreation &request, _queue.queue()) {
        Pack *pack = new Pack;
        if (!pack->fromXmlFile(request.descriptionFilePath)) {
            LOG_ERROR_FOR("PackServerCreator", QString("Pack description file can not be read: %1").arg(request.descriptionFilePath));
            return false;
        }
        PackDescription descr = pack->description();

        QString serverRootPath = QString("%1/%2/%3/%4")
                .arg(serverAbsPath)
                .arg(request.serverUid.contains("nonfree", Qt::CaseInsensitive)?"nonfree":"free")
                .arg(getVendor(request.serverUid))
                .arg(qApp->applicationVersion());
        QString packPath = QString("%1/%2")
                .arg(serverRootPath)
                .arg(descr.data(PackDescription::Uuid).toString());
        QString zipFile = QString("%1/%2.zip")
                .arg(packPath)
                .arg(descr.data(PackDescription::Uuid).toString());

        if (!_queue.createZippedContent(request, zipFile)) {
            LOG_ERROR_FOR("PackServerCreator", QString("Unable to create server zipped content"));
            return false;
        }

        // Update Pack description
        descr.setData(PackDescription::FreeMedFormsCompatVersion, qApp->applicationVersion());
        descr.setData(PackDescription::FreeDiamsCompatVersion, qApp->applicationVersion());
        descr.setData(PackDescription::FreeAccountCompatVersion, qApp->applicationVersion());
        descr.setData(PackDescription::LastModificationDate, QDateTime::currentDateTime().toString(Qt::ISODate));
        descr.setData(PackDescription::Size, QFileInfo(zipFile).size());
        descr.setData(PackDescription::Md5, Utils::fileMd5(zipFile));
        descr.setData(PackDescription::Sha1, Utils::fileSha1(zipFile));
        pack->setPackDescription(descr);

        // Save Pack description inside the server tree next to the zip file
        QString packDescriptionOutputFileName = QString("%1/%2").arg(packPath).arg(Constants::PACKDESCRIPTION_FILENAME);
        if (!Utils::saveStringToFile(pack->toXml(), packDescriptionOutputFileName, Utils::Overwrite, Utils::DontWarnUser)) {
            LOG_ERROR_FOR("PackServerCreator", QString("Unable to save Pack description file: %1").arg(packDescriptionOutputFileName));
            return false;
        }

        // Update server config file
        QDir serverdir(serverRootPath);
        ServerContent *content = serverContent.value(request.serverUid);
        if (!content) {
            content = new ServerContent;
            serverContent.insert(request.serverUid, content);
        }
        content->addPackRelativeFileName(serverdir.relativeFilePath(packDescriptionOutputFileName));

        // Store the pack into the internal list
        packs << pack;
    }

    //    ServerDescription descr;
    //    descr.fromXmlFile(server.originalDescriptionFileAbsolutePath());
    //    descr.setData(DataPack::ServerDescription::LastModificationDate, QDate::currentDate());
    //    if (server.autoVersion()) {
    //        descr.setData(DataPack::ServerDescription::Version, qApp->applicationVersion());
    //        descr.setData(DataPack::ServerDescription::FreeMedFormsCompatVersion, qApp->applicationVersion());
    //        descr.setData(DataPack::ServerDescription::FreeDiamsCompatVersion, qApp->applicationVersion());
    //        descr.setData(DataPack::ServerDescription::FreeAccountCompatVersion, qApp->applicationVersion());
    //    }
    //    // Find final tag of the server description
    //    QString xml = descr.toXml();
    //    int start = xml.indexOf("</DataPackServer>");
    //    xml.insert(start, serverContent);
    //    if (!Utils::saveStringToFile(xml, server.outputServerAbsolutePath() + "/server.conf.xml", Utils::Overwrite, Utils::DontWarnUser)) {
    //        LOG_ERROR("Unable to save server configuration file");
    //        return false;
    //    }

    //    // Zip XML files
    //    // Create a tmp server only with XML files
    //    QString tmp = settings()->path(Core::ISettings::ApplicationTempPath) + QDir::separator() + QUuid::createUuid().toString().remove("-").remove("{").remove("}");
    //    Utils::checkDir(tmp, true, objectName());
    //    QDir serverdir(server.outputServerAbsolutePath());
    //    // Get files from versionned server
    //    QFileInfoList list = Utils::getFiles(serverPath, "*.xml");
    //    bool serverFileIncluded = false;
    //    foreach(const QFileInfo &info, list) {
    //        // compute server relative path
    //        QString tmpFile = tmp + QDir::separator() + serverdir.relativeFilePath(info.absoluteFilePath());
    //        Utils::checkDir(QFileInfo(tmpFile).absolutePath(), true, objectName());
    //        QFile(info.absoluteFilePath()).copy(tmpFile);
    //        if (info.baseName()=="server.conf.xml")
    //            serverFileIncluded = true;
    //    }
    //    if (!serverFileIncluded) {
    //        // compute server relative path
    //        QString tmpFile = tmp + "/server.conf.xml";
    //        QFile(server.outputServerAbsolutePath() + "/server.conf.xml").copy(tmpFile);
    //    }
    //    if (!JlCompress::compressDir(server.outputServerAbsolutePath() + "/serverconf.zip", tmp))
    //        LOG_ERROR("Unable to zip config files");
    //    Utils::removeDirRecursively(tmp, 0);

    //    // TODO: should we clean the zipped xml files?
    //    return true;
    //}

    qDeleteAll(serverContent);
    serverContent.clear();
    qDeleteAll(packs);
    packs.clear();
    return true;
}
