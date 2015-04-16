/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef DATAPACK_CORE_H
#define DATAPACK_CORE_H

#include <datapackutils/datapack_exporter.h>
#include <QObject>
QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkProxy;
QT_END_NAMESPACE

/**
 * \file datapackcore.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.2
 * \date 06 Feb 2012
 * Needs Qt >= 4.7
*/

namespace DataPack {
class IServerManager;
class IPackManager;
namespace Internal {
class DataPackCorePrivate;
}


class DATAPACK_EXPORT DataPackCore : public QObject
{
    Q_OBJECT
    explicit DataPackCore(QObject *parent = 0);

public:
    enum ThemePath {
        SmallPixmaps = 0,
        MediumPixmaps,
        BigPixmaps
    };

    static DataPackCore &instance(QObject *parent = 0);
    ~DataPackCore();
    void init();

    // Manage path
    void setInstallPath(const QString &absPath);
    QString installPath() const;

    void setPersistentCachePath(const QString &absPath);
    QString persistentCachePath() const;

    void setTemporaryCachePath(const QString &absPath);
    QString temporaryCachePath() const;

    void setThemePath(ThemePath path, const QString &absPath);
    QString icon(const QString &name, ThemePath path = SmallPixmaps);

    // Manage path tags
    void registerPathTag(const QString &tag, const QString &absPath);
    bool containsPathTag(const QString &path);
    QString replacePathTag(const QString &path);

    // Configuration of internet connection
    bool isInternetConnectionAvailable();
    void setNetworkProxy(const QNetworkProxy &proxy);
    const QNetworkProxy &networkProxy() const;

    // Managers
    IServerManager *serverManager() const;
    IPackManager *packManager() const;

public Q_SLOTS:
    bool stopJobsAndClearQueues() const;

private:
    Internal::DataPackCorePrivate *d;
};

}  // End namespace DataPack

#endif // DATAPACK_CORE_H
