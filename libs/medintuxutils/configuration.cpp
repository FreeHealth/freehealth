/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#include "medintuxutils/configuration.h"

#include <utils/log.h>
#include <utils/global.h>

#include <QtGui>

using namespace MedinTux;

Configuration *Configuration::m_Instance = 0;

Configuration *Configuration::instance()
{
    if (!m_Instance)
        m_Instance = new MedinTux::Configuration(qApp);
    return m_Instance;
}

Configuration::Configuration( QObject *parent ) :
        QObject(parent), d(0)
{
    Utils::functionNotAvailableMessageBox( tr("Medintux Configurator") );
}

Configuration::~Configuration()
{
}

bool Configuration::applicationIsAMedintuxPlugins() const
{
    return false;
}

QString Configuration::medintuxPluginIniFileName() const
{
    return QString();
}

QString Configuration::medintuxPluginIni(const QString &rubrik, const QString &key ) const
{
    Q_UNUSED(rubrik);
    Q_UNUSED(key);
    return QString();
}

QString Configuration::medintuxPluginInformation( PluginsParameters info ) const
{
    Q_UNUSED(info);
    return QString();
}


QString Configuration::findManagerBinaryPath() const
{
    return QString();
}

QString Configuration::askUserForManagerBinary() const
{
    return QString();
}

bool Configuration::setManagerBinaryPath(const QString &absPath)
{
    Q_UNUSED(absPath);
    return false;
}

QString Configuration::managerIniFileName() const
{
    return QString();
}

QVariant Configuration::managerIni( const QString &rubrik, const QString &key ) const
{
    Q_UNUSED(rubrik);
    Q_UNUSED(key);
    return QVariant();
}

QString Configuration::drtuxIniFileName() const
{
    return QString();
}

QVariant Configuration::drtuxIni( const QString &rubrik, const QString &key ) const
{
    Q_UNUSED(rubrik);
    Q_UNUSED(key);
    return QVariant();
}

QString Configuration::glossaryPath() const
{
    return QString();
}

QString Configuration::glossaryPath(GlossaryFor rubrik) const
{
    Q_UNUSED(rubrik);
    return QString();
}

QString Configuration::defaultListsPath(DefaultList rubrik) const
{
    Q_UNUSED(rubrik);
    return QString();
}

bool Configuration::deleteListCache() const
{
    return false;
}

QString Configuration::drtuxBinaryPath() const
{
    return QString();
}

QString Configuration::drtuxUserMenuPath() const
{
    return QString();
}

QString Configuration::drtuxResourcesPath() const
{
    return QString();
}
