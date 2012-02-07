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
 *   Main Developpers :                                                    *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "packmanager.h"
#include "datapackcore.h"
#include "iserverengine.h"
#include "servermanager.h"

#include <utils/global.h>
#include <utils/log.h>

#include <QDir>
#include <QProgressBar>

using namespace DataPack;
using namespace DataPack::Internal;

static inline DataPack::DataPackCore &core() {return DataPack::DataPackCore::instance();}
static inline DataPack::Internal::ServerManager *serverManager() {return qobject_cast<DataPack::Internal::ServerManager *>(DataPack::DataPackCore::instance().serverManager());}

PackManager::PackManager(QObject *parent) :
    IPackManager(parent)
{
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

bool PackManager::downloadPack(const Pack &pack, QProgressBar *bar)
{
    Q_ASSERT(!m_Engines.isEmpty());
    m_Msg.clear();
    m_Errors.clear();

    Server server = serverManager()->getServerForPack(pack);

    if (server.isNull()) {
        LOG_ERROR(tr("No server found for pack %1 (%2)").arg(pack.uuid()).arg(pack.version()));
        m_Errors << tr("No server found for pack %1 (%2)").arg(pack.uuid()).arg(pack.version());
        if (bar) {
            bar->setRange(0,1);
            bar->setValue(1);
        }
        return false;
    }

    // Pack not already downloaded ?
    if (!isPackInPersistentCache(pack)) {
        if (bar) {
            bar->setRange(0,1);
            bar->setValue(1);
        }
        return true;
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
            connect(engine, SIGNAL(queueDowloaded()), this, SLOT(packDownloadDone()));
            engine->startDownloadQueue();
        }
    }
    if (!downloading) {
        m_Errors << tr("Nothing to download");
        return false;
    }
}

void PackManager::packDownloadDone()
{
    // All engine finished ?
    for(int i=0; i<m_Engines.count(); ++i) {
        if (m_Engines.at(i)->downloadQueueCount()>0) {
            return;
        }
    }

    // Check MD5 of downloaded files
//    QList<Pack> dld, toInstall;
//    dld << m_InstallPacks;
//    dld << m_UpdatePacks;
//    for(int i=0; i < dld.count(); ++i) {
//        const Pack &p = dld.at(i);
//        QByteArray downloadedMd5 = Utils::md5(p.persistentlyCachedZipFileName());
//        if (downloadedMd5 != p.md5ControlChecksum()) {
//            m_Error = true;
//            m_Msg << tr("Downloaded file is corrupted. Please retry to download the pack: %1.").arg(p.name());
//        } else {
//            toInstall << p;
//        }
//    }

//    LOG(QString("Requested packs are downloaded in %1").arg(core().persistentCachePath()));
//    QStringList logPacks;
//    for(int i=0; i < toInstall.count(); ++i) {
//        const Pack &p = toInstall.at(i);
//        logPacks << QString("%1 (%2, %3)")
//                    .arg(p.name())
//                    .arg(p.uuid())
//                    .arg(p.version());
//    }
//    LOG(QString("Requested packs: %1").arg(logPacks.join("; ")));

//    // Copy/Unzip all packs to datapackInstallPath according to PackDescription::InstallToPath
//    for(int i=0; i < toInstall.count(); ++i) {
//        const Pack &p = toInstall.at(i);
//        const QString pathTo = p.unzipPackToPath();
//        QDir to(pathTo);
//        if (!to.exists()) {
//            to.mkpath(pathTo);
//        }

//        /** \todo manage updating packs */

//        // Unzip pack to the install path
//        bool error = false;
//        if (!QuaZipTools::unzipFile(p.persistentlyCachedZipFileName(), pathTo)) {
//            LOG_ERROR(tr("Unable to unzip pack file %1 to %2").arg(p.persistentlyCachedZipFileName()).arg(pathTo));
//            m_Error = true;
//            m_Msg << tr("Unable to unzip pack file %1 to %2").arg(p.persistentlyCachedZipFileName()).arg(pathTo);
//            error = true;
//        }
//        // Add the pack description for future analysis (update, remove...)
//        if (!QFile::copy(p.persistentlyCachedXmlConfigFileName(), p.installedXmlConfigFileName())) {
//            LOG_ERROR(tr("Unable to copy pack description file"));
//            m_Error = true;
//            m_Msg << tr("Unable to copy pack description file");
//            error = true;
//        }
//        if (error) {
//            m_Msg << tr("An error was detected during installation of %1.").arg(p.name());
//        } else {
//            m_Msg << tr("Pack %1 was correctly installed.").arg(p.name());
//        }
//    }
}

bool PackManager::installDownloadedPack(const Pack &pack)
{}
bool PackManager::removePack(const Pack &pack)
{}

