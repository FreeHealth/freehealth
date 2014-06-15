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
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DATAPACK_PACKMANAGER_H
#define DATAPACK_PACKMANAGER_H

#include <datapackutils/ipackmanager.h>

#include <QVector>
#include <QList>

/**
 * \file packmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 06 Feb 2012
*/

namespace DataPack {
class IServerEngine;

namespace Internal {

class PackManager : public IPackManager
{
    Q_OBJECT
public:
    explicit PackManager(QObject *parent = 0);
    ~PackManager();

    bool init(const QVector<DataPack::IServerEngine*> &engines);
    void checkInstalledPacks();

    QList<Pack> installedPack(bool forceRefresh = false);
    bool isDataPackInstalled(const Pack &pack);
    bool isDataPackInstalled(const QString &packUid, const QString &packVersion = QString::null);

    bool isPackInPersistentCache(const Pack &pack);

    bool downloadPack(const Pack &pack, QProgressBar *bar);
    bool checkCachedPackFileIntegrity(const Pack &pack);
    bool installDownloadedPack(const Pack &pack, QProgressBar *bar = 0);
    bool removePack(const Pack &pack);

private Q_SLOTS:
    void packDownloadDone(const DataPack::Pack &pack, const DataPack::ServerEngineStatus &status);

private:
    QList<Pack> m_InstalledPacks;
    QVector<DataPack::IServerEngine*> m_Engines;
    QStringList m_Msg, m_Errors;
};

} // namespace Internal
} // namespace DataPack

#endif // DATAPACK_PACKMANAGER_H
