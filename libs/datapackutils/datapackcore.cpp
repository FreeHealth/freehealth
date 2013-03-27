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
  \class DataPack::DataPackCore
  Central place for the management of DataPacks. This core mainly contains paths and single objects like
  the unique DataPack::IServerManager to use.
*/

#include "datapackcore.h"
#include "servermanager.h"
#include "packmanager.h"
#include "serverengines/localserverengine.h"
#include "serverengines/httpserverengine.h"

#include <utils/log.h>
#include <utils/global.h>

#include <QDir>
#include <QNetworkProxy>

#include <quazip/global.h>

using namespace DataPack;

namespace  {
static DataPack::DataPackCore *m_instance = 0;
} // namespace anonymous

/** Creates and return the singleton of the core */
DataPack::DataPackCore &DataPack::DataPackCore::instance(QObject *parent)
{
    if (!m_instance)
        m_instance = new DataPackCore(parent);
    return *m_instance;
}

namespace DataPack {
namespace Internal {
class DataPackCorePrivate
{
public:
    DataPackCorePrivate() :
        m_ServerManager(0),
        m_PackManager(0),
        m_LocalEngine(0),
        m_HttpEngine(0)
    {}

public:
    ServerManager *m_ServerManager;
    PackManager *m_PackManager;
    LocalServerEngine *m_LocalEngine;
    HttpServerEngine *m_HttpEngine;
    QVector<IServerEngine *> m_Engines;

    QHash<int, QString> m_ThemePath;
    QString m_InstallPath, m_TmpCachePath, m_PersistentCachePath;
    QNetworkProxy m_Proxy;

    QHash<QString, QString> m_Tag_Path;
};
}  // End namespace Internal
}  // End namespace DataPack

/** Constructor */
DataPackCore::DataPackCore(QObject *parent) :
    QObject(parent),
    d(new Internal::DataPackCorePrivate)
{
    d->m_ServerManager = new Internal::ServerManager(this);
    d->m_PackManager = new Internal::PackManager(this);

    //    qWarning() << QuaZipTools::zipFileNameContent("/Users/eric/Desktop/Programmation/freemedforms/global_resources/datapacks/http_zip/forms/geriatrics/geriatric_pack01.zip").join("\n");
//    Pack p;
//    p.fromXmlFile("/Users/eric/Desktop/Programmation/freemedforms/global_resources/datapacks/http_zip/forms/geriatrics/packdescription.xml");
//    p.setInstalledFiles(QuaZipTools::zipFileNameContent("/Users/eric/Desktop/Programmation/freemedforms/global_resources/datapacks/http_zip/forms/geriatrics/geriatric_pack01.zip"));
//    qWarning() << p.toXml();
}

/** Destructor */
DataPackCore::~DataPackCore()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void DataPackCore::init()
{
    // Avoid infinite looping when using core::instance in servermanager/serverengine constructors
    d->m_LocalEngine = new Internal::LocalServerEngine(this);
    d->m_HttpEngine = new Internal::HttpServerEngine(this);
    d->m_Engines << d->m_LocalEngine << d->m_HttpEngine;
    d->m_ServerManager->init(d->m_Engines);
    d->m_PackManager->init(d->m_Engines);
}

/** Test the internet connection and return the state of availability of it. This is just a wrapper of the Utils::testInternetConnection(). */
bool DataPackCore::isInternetConnectionAvailable()
{
    return !Utils::testInternetConnection().isEmpty();
}

/** Untill application can not fully detect the system proxy, you have to define the proxy to use in all the internet access done by the DataPack lib. */
void DataPackCore::setNetworkProxy(const QNetworkProxy &proxy)
{
    d->m_Proxy = proxy;
}

/** Return the proxy to use in all the internet access done by the DataPack lib. */
const QNetworkProxy &DataPackCore::networkProxy() const
{
    return d->m_Proxy;
}

/** Return the single DataPack::IServerManager in use in the lib. */
IServerManager *DataPackCore::serverManager() const
{
    return d->m_ServerManager;
}

IPackManager *DataPackCore::packManager() const
{
    return d->m_PackManager;
}

bool DataPackCore::stopJobsAndClearQueues() const
{
    bool ok = true;
    for(int i=0; i < d->m_Engines.count(); ++i) {
        IServerEngine *e = d->m_Engines[i];
        if (!e->stopJobsAndClearQueue()) {
            LOG_ERROR("Unable to stop job && clear queue for engine: "+ e->objectName());
            ok = false;
        }
    }
    return ok;
}

/** Define the path where to install datapacks. */
void DataPackCore::setInstallPath(const QString &absPath)
{
    d->m_InstallPath = QDir::cleanPath(absPath);
    QDir test(d->m_InstallPath);
    if (!test.exists()) {
        if (!test.mkpath(test.absolutePath()))
            LOG_ERROR(QString("Unable to create DataPack::InstallDir %1").arg(d->m_InstallPath));
    }
}

/** Return the path where to install datapacks. */
QString DataPackCore::installPath() const
{
    return d->m_InstallPath;
}

/** Define the path where to cache datapacks. This path must not be cleaned and should contain all downloaded files. */
void DataPackCore::setPersistentCachePath(const QString &absPath)
{
    d->m_PersistentCachePath = QDir::cleanPath(absPath);
    QDir test(d->m_PersistentCachePath);
    if (!test.exists()) {
        if (!test.mkpath(test.absolutePath()))
            LOG_ERROR(QString("Unable to create DataPack::PersistentCache %1").arg(d->m_PersistentCachePath));
    }
}

/** Return the path where to cache datapacks. This path must not be cleaned and should contain all downloaded files. */
QString DataPackCore::persistentCachePath() const
{
    return d->m_PersistentCachePath;
}

/** Define the path to use a volatile cache. */
void DataPackCore::setTemporaryCachePath(const QString &absPath)
{
    d->m_TmpCachePath = QDir::cleanPath(absPath);
    QDir test(d->m_TmpCachePath);
    if (!test.exists()) {
        if (!test.mkpath(test.absolutePath()))
            LOG_ERROR(QString("Unable to create DataPack::TempCache %1").arg(d->m_TmpCachePath));
    }
}

/** Return the path to use a volatile cache. */
QString DataPackCore::temporaryCachePath() const
{
    return d->m_TmpCachePath;
}

/** Define the theme path. */
void DataPackCore::setThemePath(ThemePath path, const QString &absPath)
{
    QDir test(absPath);
    if (!test.exists())
        LOG_ERROR(QString("Theme path does not exist %1").arg(test.absolutePath()));
    d->m_ThemePath.insert(path, QDir::cleanPath(absPath));
}

/** Return the theme path. */
QString DataPackCore::icon(const QString &name, ThemePath path)
{
    return QString("%1/%2").arg(d->m_ThemePath.value(path)).arg(name);
}

/** Register a path tag. It is preferable that the \e tag starts and ends with __. \e absPath must be an absolute path. */
void DataPackCore::registerPathTag(const QString &tag, const QString &absPath)
{
    d->m_Tag_Path.insert(tag, QDir::cleanPath(absPath) + QDir::separator());
}

/** Test \e path for existing tag. */
bool DataPackCore::containsPathTag(const QString &path)
{
    foreach(const QString &t, d->m_Tag_Path.keys()) {
        if (path.contains(t, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

/** Replace tags in \e path and return the computed new path. */
QString DataPackCore::replacePathTag(const QString &path)
{
    QString tmp = path;
    foreach(const QString &t, d->m_Tag_Path.keys()) {
        if (tmp.contains(t, Qt::CaseInsensitive))
            tmp = tmp.replace(t, d->m_Tag_Path.value(t));
    }
    return QDir::cleanPath(tmp);
}
