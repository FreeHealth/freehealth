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

#include <utils/global.h>

#include <QDir>

using namespace DataPack;
using namespace DataPack::Internal;

static inline DataPack::DataPackCore &core() {return DataPack::DataPackCore::instance();}

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

bool PackManager::downloadPack(const Pack &pack)
{}
bool PackManager::installDownloadedPack(const Pack &pack)
{}
bool PackManager::removePack(const Pack &pack)
{}

