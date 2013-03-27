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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class DataPack::PackManager::PackManager
 * \internal
 */

#include "packmanager.h"
#include "datapackcore.h"
#include "iserverengine.h"
#include "servermanager.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_filepathxml.h>
#include <quazip/global.h>

#include <QDir>
#include <QProgressBar>
#include <QFile>

using namespace DataPack;
using namespace DataPack::Internal;
using namespace Trans::ConstantTranslations;

static inline DataPack::DataPackCore &core() {return DataPack::DataPackCore::instance();}
static inline DataPack::Internal::ServerManager *serverManager() {return qobject_cast<DataPack::Internal::ServerManager *>(DataPack::DataPackCore::instance().serverManager());}

PackManager::PackManager(QObject *parent) :
    IPackManager(parent)
{
    setObjectName("PackManager");
}

PackManager::~PackManager()
{}

bool PackManager::init(const QVector<IServerEngine*> &engines)
{
    m_Engines = engines;
    return true;
}

void PackManager::checkInstalledPacks()
{
    if (!m_InstalledPacks.isEmpty())
        return;
    // Scan the install dir for packconfig.xml files and read them
    foreach(const QFileInfo &info, Utils::getFiles(QDir(core().installPath()), "packconfig.xml")) {
        Pack p;
        p.fromXmlFile(info.absoluteFilePath());
        if (p.isValid())
            m_InstalledPacks.append(p);
    }
}

bool PackManager::isDataPackInstalled(const Pack &pack)
{
    return isDataPackInstalled(pack.uuid(), pack.version());
}

bool PackManager::isDataPackInstalled(const QString &packUid, const QString &packVersion)
{
    Q_UNUSED(packUid);
    Q_UNUSED(packVersion);
    checkInstalledPacks();
    bool checkVersion = !packVersion.isEmpty();
    foreach(const Pack &p, m_InstalledPacks) {
        if (p.uuid().compare(packUid, Qt::CaseInsensitive)==0) {
            if (checkVersion) {
                return (p.version()==packVersion);
            }
            return true;
        }
    }
    return false;
}

bool PackManager::isPackInPersistentCache(const Pack &pack)
{
    QFileInfo info(core().persistentCachePath() + QDir::separator() + pack.uuid() + QDir::separator() + QFileInfo(pack.serverFileName()).fileName());
    if (info.exists()) {
        // Test local version of the pack
        Pack cached;
        cached.fromXmlFile(core().persistentCachePath() + QDir::separator() + pack.uuid() + QDir::separator() + "packconfig.xml");
        return (cached.version() == pack.version());
    }
    return false;
}

QList<Pack> PackManager::installedPack(bool forceRefresh)
{
    if (forceRefresh)
        m_InstalledPacks.clear();
    checkInstalledPacks();
    return m_InstalledPacks;
}

//FIXME: *bar must not be directly accessed, not thread safe & bad design
//--> use signal/slot mechanism
bool PackManager::downloadPack(const Pack &pack, QProgressBar *bar)
{
    Q_ASSERT(!m_Engines.isEmpty());
    m_Msg.clear();
    m_Errors.clear();

    Server &server = serverManager()->getServerForPack(pack);

    if (server.isNull()) {
        LOG_ERROR(tr("No server found for pack %1 (%2)").arg(pack.uuid()).arg(pack.version()));
        m_Errors << tr("No server found for pack %1 (%2)").arg(pack.uuid()).arg(pack.version());
        if (bar) {
            bar->setRange(0,1);
            bar->setValue(1);
        }
        return false;
    }

    // Pack already downloaded ?
    if (isPackInPersistentCache(pack)) {
//        qWarning() << "IN CACHE" << pack.name() << pack.version();
        if (checkCachedPackFileIntegrity(pack)) {
            LOG("Pack already downloaded. Using the cached pack: " + pack.persistentlyCachedZipFileName()) ;
            if (bar) {
                bar->setRange(0,1);
                bar->setValue(1);
            }
            ServerEngineStatus status;
            status.downloadCorrectlyFinished = true;
            status.hasError = false;
            status.isSuccessful = true;
            Q_EMIT packDownloaded(pack, status);
            return true;
        } else {
            QString error;
            Utils::removeDirRecursively(pack.persistentlyCachedZipFileName(), &error);
        }
    }

    // Download the pack from this server
    for(int j=0; j < m_Engines.count(); ++j) {
        DataPack::IServerEngine *engine = m_Engines.at(j);
        if (engine->managesServer(server)) {
            // Create the label/progress for the pack
            DataPack::ServerEngineQuery query;
            query.downloadPackFile = true;
            query.pack = &pack;
            query.server = &server;
            query.progressBar = bar;
            engine->addToDownloadQueue(query);
            LOG(tr("Adding %1 to %2 download queue").arg(pack.uuid()).arg(server.uuid()));
            m_Msg << tr("Adding %1 to %2 download queue.").arg(pack.uuid()).arg(server.uuid());
        }
    }

    // Start download
    bool downloading = false;
    for(int i = 0; i < m_Engines.count(); ++i) {
        DataPack::IServerEngine *engine = m_Engines.at(i);
        if (engine->downloadQueueCount() > 0) {
            downloading = true;
            connect(engine, SIGNAL(packDownloaded(DataPack::Pack, DataPack::ServerEngineStatus)),
                    this, SLOT(packDownloadDone(DataPack::Pack, DataPack::ServerEngineStatus)));
            engine->startDownloadQueue();
        }
    }
    if (!downloading) {
        m_Errors << tr("Nothing to download");
        return false;
    }
    return true;
}

void PackManager::packDownloadDone(const DataPack::Pack &pack, const DataPack::ServerEngineStatus &status)
{
    ServerEngineStatus s = status;
    // Check status

    // Check file
    if (!checkCachedPackFileIntegrity(pack)) {
        LOG_ERROR(tr("Pack file corrupted (%1)").arg(pack.persistentlyCachedZipFileName()));
        m_Errors << tr("Pack file corrupted (%1)").arg(pack.persistentlyCachedZipFileName());
        s.hasError = true;
        s.isSuccessful = false;
        s.errorMessages << tr("Pack file corrupted (%1)").arg(pack.persistentlyCachedZipFileName());
        Q_EMIT packDownloaded(pack, s);
        return;
    }

    LOG(QString("Requested pack is downloaded: %1").arg(pack.persistentlyCachedZipFileName()));
    Q_EMIT packDownloaded(pack, status);
}

bool PackManager::checkCachedPackFileIntegrity(const Pack &pack)
{
    QByteArray downloadedMd5 = Utils::fileMd5(pack.persistentlyCachedZipFileName());
    return (downloadedMd5 == pack.md5ControlChecksum());
}

bool PackManager::installDownloadedPack(const Pack &pack, QProgressBar *bar)
{
//    WARN_FUNC << pack.uuid();
    const QString &pathTo = pack.unzipPackToPath();
    if (!Utils::checkDir(pack.unzipPackToPath(), true, "PackManager::installDownloadedPack"))
        return false;

    // TODO: manage updating packs

    // Unzip pack to the install path
    bool error = false;
    if (!QuaZipTools::unzipFile(pack.persistentlyCachedZipFileName(), pathTo, bar)) {
        LOG_ERROR(tr("Unable to unzip pack file %1 to %2").arg(pack.persistentlyCachedZipFileName()).arg(pathTo));
        m_Errors << tr("Unable to unzip pack file %1 to %2").arg(pack.persistentlyCachedZipFileName()).arg(pathTo);
        error = true;
    }

    // Add the pack description for future analysis (update, remove...)
    QFile testExisting(pack.installedXmlConfigFileName());
    if (testExisting.exists()) {
        if (!testExisting.remove()) {
            LOG_ERROR(QString("Unable to remove old pack configuration file: %1").arg(pack.installedXmlConfigFileName()));
            return false;
        }
    }

    // Log installed files && save pack configuration
    QStringList instFiles = QuaZipTools::zipFileNameContent(pack.persistentlyCachedZipFileName());
    Pack p;
    p.fromXmlFile(pack.persistentlyCachedXmlConfigFileName());
    p.setInstalledFiles(instFiles);
    Utils::checkDir(QFileInfo(p.installedXmlConfigFileName()).absolutePath(), true, "PackManager::installDownloadedPack");
    Utils::saveStringToFile(p.toXml(), p.installedXmlConfigFileName());

    if (error)
        m_Errors << tr("An error was detected during installation of %1.").arg(pack.name());
    else
        m_Msg << tr("Pack %1 was installed successfully.").arg(pack.name());

    Q_EMIT packInstalled(pack);

    // Recheck installed packs
    m_InstalledPacks.clear();
    checkInstalledPacks();
    return error;
}

bool PackManager::removePack(const Pack &pack)
{
    LOG("Pack removal requested: " + pack.uuid());
    // Remove the zipPath used for the pack
    QFileInfo zipPath(pack.unzipPackToPath());
    if (!zipPath.exists()) {
        LOG_ERROR(tr("Unable to remove pack %1, unzip path does not exist (%2)").arg(pack.name()).arg(pack.unzipPackToPath()));
        m_Errors << tr("Unable to remove pack %1, unzip path does not exist (%2)").arg(pack.name()).arg(pack.unzipPackToPath());
        return false;
    }
    Pack instPackFile;
    instPackFile.fromXmlFile(pack.installedXmlConfigFileName());
    QStringList instFiles = instPackFile.installedFiles();
    instFiles << instPackFile.installedXmlConfigFileName();
    foreach(QString file, instFiles) {
        if (!QFile::remove(file)) {
            LOG_ERROR(QString("Unable to remove file %1").arg(file));
        }
    }

    m_Msg << tr("Pack %1 successfully removed.").arg(pack.name()); // TODO: successfully -> successfully
    LOG(tr("Pack %1 successfully removed.").arg(pack.name())); // TODO: successfully -> successfully
    Q_EMIT packRemoved(pack);
    return true;
}

