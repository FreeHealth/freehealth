/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "core.h"
#include "servermanager.h"

#include <QDir>

using namespace DataPack;

namespace  {
static DataPack::Core *m_instance = 0;
} // namespace anonymous

DataPack::Core* DataPack::Core::instance(QObject *parent)
{
    if (!m_instance)
        m_instance = new Core(parent);
    return m_instance;
}

namespace DataPack {
namespace Internal {
class CorePrivate
{
public:
    CorePrivate() : m_ServerManager(0) {}

public:
    ServerManager *m_ServerManager;
    QHash<int, QString> m_ThemePath;
    QString m_InstallPath, m_TmpCachePath, m_PersistentCachePath;
};
}  // End namespace Internal
}  // End namespace DataPack

Core::Core(QObject *parent) :
    QObject(parent),
    d(new Internal::CorePrivate)
{
    d->m_ServerManager = new Internal::ServerManager(this);
}

Core::~Core()
{
    if (d) {
        delete d;
        d = 0;
    }
}

bool Core::isInternetConnexionAvailable()
{
//    foreach(const QNetworkConfiguration &conf, QNetworkConfigurationManager().allConfigurations()) {
//        qWarning() << conf.bearerName() << conf.bearerTypeName() << conf.state() << conf.identifier() << conf.name();
//    }
    // TODO
    return true;
}

IServerManager *Core::serverManager() const
{
    return d->m_ServerManager;
}

void Core::setInstallPath(const QString &absPath)
{
    d->m_InstallPath = QDir::cleanPath(absPath);
    d->m_ServerManager->setInstallPath(d->m_InstallPath);
}

QString Core::installPath() const
{
    return d->m_InstallPath;
}

void Core::setPersistentCachePath(const QString &absPath)
{
    d->m_PersistentCachePath = QDir::cleanPath(absPath);
    d->m_ServerManager->setPersistentCachePath(d->m_PersistentCachePath);
}

QString Core::persistentCachePath() const
{
    return d->m_PersistentCachePath;
}

void Core::setTemporaryCachePath(const QString &absPath)
{
    d->m_TmpCachePath = QDir::cleanPath(absPath);
    d->m_ServerManager->setTemporaryCachePath(d->m_TmpCachePath);
}

QString Core::temporaryCachePath() const
{
    return d->m_TmpCachePath;
}

void Core::setThemePath(ThemePath path, const QString &absPath)
{
    d->m_ThemePath.insert(path, QDir::cleanPath(absPath));
}

QString Core::icon(const QString &name, ThemePath path)
{
    return QString("%1/%2").arg(d->m_ThemePath.value(path)).arg(name);
}

