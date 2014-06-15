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
#ifndef DATAPACK_IPACKMANAGER_H
#define DATAPACK_IPACKMANAGER_H

#include <datapackutils/datapack_exporter.h>
#include <datapackutils/serverenginestatus.h>
#include <datapackutils/pack.h>
#include <QObject>
QT_BEGIN_NAMESPACE
class QProgressBar;
QT_END_NAMESPACE

/**
 * \file ipackmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 06 Feb 2012
*/

namespace DataPack {

class DATAPACK_EXPORT IPackManager : public QObject
{
    Q_OBJECT
public:
    IPackManager(QObject *parent = 0) : QObject(parent) {}
    virtual ~IPackManager() {}

    virtual QList<Pack> installedPack(bool forceRefresh = false) = 0;
    virtual bool isDataPackInstalled(const Pack &pack) = 0;
    virtual bool isDataPackInstalled(const QString &packUid, const QString &packVersion = QString::null) = 0;

    virtual bool isPackInPersistentCache(const Pack &pack) = 0;

    // TODO: remove QProgressBar direct access and use sig/slot instead?
    virtual bool downloadPack(const Pack &pack, QProgressBar *bar) = 0;
    virtual bool checkCachedPackFileIntegrity(const Pack &pack) = 0;
    virtual bool installDownloadedPack(const Pack &pack, QProgressBar *bar = 0) = 0;
    virtual bool removePack(const Pack &pack) = 0;

    // TODO: add signal including the ServerEngineStatus
Q_SIGNALS:
    void packDownloaded(const DataPack::Pack &pack, const DataPack::ServerEngineStatus &status);
    void packInstalled(const DataPack::Pack &pack);
    void packRemoved(const DataPack::Pack &pack);
};

}  // End namespace DataPack

#endif // DATAPACK_IPACKMANAGER_H
