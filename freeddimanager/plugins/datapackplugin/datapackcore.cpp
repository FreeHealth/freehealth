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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DataPackPlugin::DataPackCore
 * Central place where user can register datapack to servers, get server content and description,
 * recreate/update server. \n
 * Servers references (DataPackPlugin::DataPackServerQuery) are created internally.
 */

#include "datapackcore.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/waitforsignal.h>
#include <translationutils/constants.h>
#include <datapackutils/packdescription.h>
#include <datapackutils/serverdescription.h>
#include <extensionsystem/pluginmanager.h>
#include <datapackutils/constants.h>

#include <quazip/JlCompress.h>

#include <QFile>
#include <QDir>
#include <QUuid>

#include <QDebug>

using namespace DataPackPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  {return Core::ICore::instance()->settings();}
static inline ExtensionSystem::PluginManager *pluginManager()  {return ExtensionSystem::PluginManager::instance();}

DataPackCore *DataPackCore::_instance = 0;

namespace DataPackPlugin {
namespace Internal {
class DataPackCorePrivate
{
public:
    DataPackCorePrivate(DataPackCore *parent) :
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~DataPackCorePrivate()
    {
//        qDeleteAll(_serverDatapacks);
//        qDeleteAll(_servers);
//        qDeleteAll(_serverDescription);
    }

//    // Return the server output path
//    QString serverOutputAbsPath()
//    {
//        return QDir::cleanPath(settings()->value(Core::Constants::S_DATAPACK_SERVER_OUTPUT_PATH).toString());
//    }

//    // Return the path to the description files (inside global_resources)
//    QString serverDescriptionFileAbsPath()
//    {
//        return QString("%1/%2/servers")
//                .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
//                .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES);
//    }

//    QString normalizedApplicationVersion()
//    {
//        QString version = qApp->applicationVersion();
//        if (version.contains("~"))
//            version = version.left(version.indexOf("~"));
//        if (version.contains("-"))
//            version = version.left(version.indexOf("-"));
//        return version;
//    }

//    // Create the default servers
//    bool createServers()
//    {
//        if (_servers.count() > 0)
//            return true;

//        DataPackServerQuery *freeCommunityServer = new DataPackServerQuery;
//        DataPackServerQuery *nonFreeCommunityServer = new DataPackServerQuery;
//        DataPackServerQuery *freeFrenchAssoServer = new DataPackServerQuery;
//        DataPackServerQuery *nonFreeFrenchAssoServer = new DataPackServerQuery;

//        const QString &version = normalizedApplicationVersion();

//        freeCommunityServer->setServerInternalUuid(Core::Constants::SERVER_COMMUNITY_FREE);
//        freeCommunityServer->setOriginalDescriptionFileAbsolutePath(serverDescriptionFileAbsPath() + "/free/community/server.conf.xml");
//        freeCommunityServer->setOutputServerAbsolutePath(serverOutputAbsPath() + "/free/community/" + version);

//        nonFreeCommunityServer->setServerInternalUuid(Core::Constants::SERVER_COMMUNITY_NONFREE);
//        nonFreeCommunityServer->setOriginalDescriptionFileAbsolutePath(serverDescriptionFileAbsPath() + "/nonfree/community/server.conf.xml");
//        nonFreeCommunityServer->setOutputServerAbsolutePath(serverOutputAbsPath() + "/nonfree/community/" + version);

//        freeFrenchAssoServer->setServerInternalUuid(Core::Constants::SERVER_ASSO_FREE);
//        freeFrenchAssoServer->setOriginalDescriptionFileAbsolutePath(serverDescriptionFileAbsPath() + "/free/asso/server.conf.xml");
//        freeFrenchAssoServer->setOutputServerAbsolutePath(serverOutputAbsPath() + "/free/asso/" + version);

//        nonFreeFrenchAssoServer->setServerInternalUuid(Core::Constants::SERVER_ASSO_NONFREE);
//        nonFreeFrenchAssoServer->setOriginalDescriptionFileAbsolutePath(serverDescriptionFileAbsPath() + "/nonfree/asso/server.conf.xml");
//        nonFreeFrenchAssoServer->setOutputServerAbsolutePath(serverOutputAbsPath() + "/nonfree/asso/" + version);

//        _servers.append(freeCommunityServer);
//        _servers.append(nonFreeCommunityServer);
//        _servers.append(freeFrenchAssoServer);
//        _servers.append(nonFreeFrenchAssoServer);

//        return true;
//    }

//    // Return the server corresponding to the uid or the nullServer
//    DataPackServerQuery *server(const QString &serverUid)
//    {
//        for(int i=0; i < _servers.count(); ++i) {
//            if (_servers.at(i)->serverInternalUuid() == serverUid)
//                return _servers.at(i);
//        }
//        return &_nullServer;
//    }

//    // return the server description or a null server description if the serverUid does not exist
//    DataPack::ServerDescription *serverDescription(const QString &serverUid)
//    {
//        DataPack::ServerDescription *descr = _serverDescription.value(serverUid, 0);
//        // in cache
//        if (descr)
//            return descr;
//        // create it
//        DataPackServerQuery *server = this->server(serverUid);
//        if (!server->isValid())
//            return &_nullServerDescription;
//        descr = new DataPack::ServerDescription;
//        descr->fromXmlFile(server->originalDescriptionFileAbsolutePath());
//        _serverDescription.insert(serverUid, descr);
//        return descr;
//    }

//public:
//    QMultiHash<QString, DataPackQuery *> _serverDatapacks;
//    QVector<DataPackServerQuery *> _servers;
//    QHash<QString, DataPack::ServerDescription *> _serverDescription;
//    DataPackServerQuery _nullServer;
//    DataPack::ServerDescription _nullServerDescription;

private:
    DataPackCore *q;
};
} // namespace Internal
} // namespace DataPackPlugin

/*! Constructor of the DataPackPlugin::DataPackCore class */
DataPackCore::DataPackCore(QObject *parent) :
    QObject(parent),
    d(new DataPackCorePrivate(this))
{
    setObjectName("DataPackCore");
    _instance = this;
}

/*! Destructor of the DataPackPlugin::DataPackCore class */
DataPackCore::~DataPackCore()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DataPackCore::initialize()
{
    // nothing to do
    return true;
}

///**
// * Request all Core::IFullReleaseStep to register their datapacks.
// * You can then get the registered datapacks using serverRegisteredDatapacks().\n
// * Servers uids are stored in constants: Core::Constants::SERVER_*
// */
//bool DataPackCore::refreshServerDataPacks(const QString &serverUid)
//{
//    // remove all recorded datapack from cache
//    d->_serverDatapacks.remove(serverUid);

//    // ask all steps to register datapacks
//    QList<Core::IFullReleaseStep *> steps = pluginManager()->getObjects<Core::IFullReleaseStep>();
//    foreach(Core::IFullReleaseStep *step, steps) {
//        // TODO: remove ugly Utils::waitForSignal()
//        qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
//        step->startProcessing(Core::IFullReleaseStep::PreProcess, Core::IFullReleaseStep::DataPackSubProcess);
//        Utils::waitForSignal(step, SIGNAL(subProcessFinished(Core::IFullReleaseStep::ProcessTiming, Core::IFullReleaseStep::SubProcess)), 600000); // wait 10 minutes

//        step->startProcessing(Core::IFullReleaseStep::Process, Core::IFullReleaseStep::DataPackSubProcess);
//        Utils::waitForSignal(step, SIGNAL(subProcessFinished(Core::IFullReleaseStep::ProcessTiming, Core::IFullReleaseStep::SubProcess)), 600000); // wait 10 minutes

//        step->startProcessing(Core::IFullReleaseStep::PostProcess, Core::IFullReleaseStep::DataPackSubProcess);
//        Utils::waitForSignal(step, SIGNAL(subProcessFinished(Core::IFullReleaseStep::ProcessTiming, Core::IFullReleaseStep::SubProcess)), 600000); // wait 10 minutes
//        QApplication::restoreOverrideCursor();
//    }
//    return true;
//}

///**
// * Register a DataPackQuery inside a server. You can register multiple DataPackQuery for one server.
// * All queries will be processed and added to the server. \n
// * Servers uids are stored in constants: Core::Constants::SERVER_*
// */
//bool DataPackCore::registerDataPack(const DataPackQuery &query, const QString &serverUid)
//{
//    // avoid duplicates
//    const QList<DataPackQuery*> &queries = d->_serverDatapacks.values(serverUid);
//    foreach(DataPackQuery *serverQuery, queries) {
//        if (*serverQuery==query)
//            return true;
//    }

//    // add the query to the server
//    DataPackQuery *newquery = new DataPackQuery(query);
//    d->_serverDatapacks.insertMulti(serverUid, newquery);
//    return true;
//}

///**
// * Return all the registered datapacks for one server identified by its \e serverUid \n
// * Servers uids are stored in constants: Core::Constants::SERVER_*
// */
//QList<DataPackQuery> DataPackCore::serverRegisteredDatapacks(const QString &serverUid) const
//{
//    QList<DataPackQuery> toReturn;
//    const QList<DataPackQuery*> &queries = d->_serverDatapacks.values(serverUid);
//    foreach(DataPackQuery *serverQuery, queries)
//       toReturn << *serverQuery;
//    return toReturn;
//}

///**
// * Create and return the default datapack server. \n
// * Servers uids are stored in constants: Core::Constants::SERVER_*
// */
//QList<DataPackServerQuery> DataPackCore::servers() const
//{
//    QList<DataPackServerQuery> toReturn;
//    if (!d->createServers())
//        return toReturn;
//    for(int i=0; i < d->_servers.count(); ++i)
//        toReturn << *d->_servers.at(i);
//    return toReturn;
//}

///**
// * Return the server related to the \e serverUid or a null server if the uid is not supported. \n
// * Servers uids are stored in constants: Core::Constants::SERVER_*
// */
//const DataPackServerQuery &DataPackCore::server(const QString &serverUid) const
//{
//    return *d->server(serverUid);
//}

//const DataPack::ServerDescription &DataPackCore::serverDescription(const QString &serverUid)
//{
//    return *d->serverDescription(serverUid);
//}

///**
// * Update the datapack server files. This will call Core::IFullReleaseStep::registerDatapack() on
// * all Core::IFullReleaseStep objects before the server processing. \n
// * Servers uids are stored in constants: Core::Constants::SERVER_*
// */
////bool DataPackCore::updateServer(const QString &serverUid)
////{
////}

///**
// * Create the datapack server files. This will call Core::IFullReleaseStep::registerDatapack() on
// * all Core::IFullReleaseStep objects before the server processing. \n
// * Servers uids are stored in constants: Core::Constants::SERVER_*
// * \sa Core::Constants::SERVER_COMMUNITY_FREE
// */
//bool DataPackCore::createServer(const QString &serverUid)
//{
//    // TODO: remove magic numbers here (filenames...)
//    refreshServerDataPacks(serverUid);
//    const DataPackServerQuery &server = this->server(serverUid);
//    QList<DataPackQuery> queries = serverRegisteredDatapacks(serverUid);

//    // Check server path
//    QString serverPath = server.outputServerAbsolutePath();
////    if (server.autoVersion())
////        serverPath.append(QDir::separator() + qApp->applicationVersion() + QDir::separator());
//    if (!Utils::checkDir(serverPath, true, "DataPackCore"))
//        return false;

//    // Manage server datapacks
//    QString serverContent;
//    for(int i = 0; i < queries.count(); ++i) {
//        const DataPackQuery &query = queries.at(i);
//        if (!query.isValid())
//            continue;

//        // Read datapack description file and check datapacks path
//        DataPack::PackDescription descr;
//        descr.fromXmlFile(query.descriptionFileAbsolutePath());

//        QString path = serverPath + QDir::separator() + descr.data(DataPack::PackDescription::AbsFileName).toString();
//        if (!Utils::checkDir(QFileInfo(path).absolutePath(), true, "DataPackCore"))
//            continue;

//        // Install datapack content file inside server path
//        if (query.zipOriginalFile()) {
//            LOG("Trying to compress: " + query.originalContentFileAbsolutePath());
//            if (!JlCompress::compressFile(path, query.originalContentFileAbsolutePath())) {
//                LOG_ERROR("Unable to compress file");
//            }
//            LOG("Compressed: " + path);
//        } else {
//            if (!QFile::copy(query.originalContentFileAbsolutePath(), path)) {
//                LOG_ERROR("Copying datapack content to the server");
//                continue;
//            }
//        }

//        // Update & install datapack description file next to the content file
//        descr.setData(DataPack::PackDescription::LastModificationDate, QDate::currentDate());
//        descr.setData(DataPack::PackDescription::Size, QFileInfo(path).size());
//        descr.setData(DataPack::PackDescription::Md5, Utils::fileMd5(path));
//        descr.setData(DataPack::PackDescription::Sha1, Utils::fileSha1(path));
//        if (query.autoVersion()) {
//            // Normalized version only used for path
//            const QString &version = d->normalizedApplicationVersion();
//            QString versionnedFileName = descr.data(DataPack::ServerDescription::AbsFileName).toString();
//            if (versionnedFileName.startsWith(version))
//                versionnedFileName.replace(version, ".");
//            // For Description use the qApp->applicationVersion()
//            descr.setData(DataPack::ServerDescription::AbsFileName, versionnedFileName);
//            descr.setData(DataPack::ServerDescription::Version, qApp->applicationVersion());
//            descr.setData(DataPack::PackDescription::EHRCompatVersion, qApp->applicationVersion());
//            descr.setData(DataPack::PackDescription::FreeDiamsCompatVersion, qApp->applicationVersion());
//            descr.setData(DataPack::PackDescription::FreeAccountCompatVersion, qApp->applicationVersion());
//        }

//        QString outputPackDescriptionFile = QString("%1/%2").arg(QFileInfo(path).absolutePath()).arg(DataPack::Constants::PACKDESCRIPTION_FILENAME);
//        if (!Utils::saveStringToFile(descr.toXml(), outputPackDescriptionFile, Utils::Overwrite, Utils::DontWarnUser)) {
//            LOG_ERROR("Unable to add the pack description file: " + outputPackDescriptionFile);
//            continue;
//        }

//        // Update server config file
//        QDir serverdir(server.outputServerAbsolutePath());
//        serverContent += QString("  <Pack serverFileName=\"%1\"/>\n")
//                .arg(serverdir.relativeFilePath(outputPackDescriptionFile));
//    }

//    // Save server config file
////    <ServerContents>
////      <Pack serverFileName="./icd10/packdescription.xml"/>
////    </ServerContents>
//    serverContent.append("</ServerContents>\n");
//    serverContent.prepend("<ServerContents>\n");
//    DataPack::ServerDescription descr;
//    descr.fromXmlFile(server.originalDescriptionFileAbsolutePath());
//    descr.setData(DataPack::ServerDescription::LastModificationDate, QDate::currentDate());
//    if (server.autoVersion()) {
//        descr.setData(DataPack::ServerDescription::Version, qApp->applicationVersion());
//        descr.setData(DataPack::ServerDescription::EHRCompatVersion, qApp->applicationVersion());
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
