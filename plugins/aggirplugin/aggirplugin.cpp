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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \namespace Gir
  \brief Namespace reserved for the AGGIR plugins (a french only plugin that measure physical dependence).
*/

#include "aggirplugin.h"
#include "girwidget.h"

#include <coreplugin/dialogs/pluginaboutpage.h>

#include <utils/log.h>

#include <QtPlugin>

#include <QDebug>

using namespace Gir;
using namespace Gir::Internal;

GirPlugin::GirPlugin() : m_Factory(0)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating GirPlugin";
}

GirPlugin::~GirPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
}

bool GirPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "GirPlugin::initialize";

    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    m_Factory = new GirWidgetFactory(this);
    addObject(m_Factory);
    return true;
}

void GirPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "GirPlugin::extensionsInitialized";

        addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}

ExtensionSystem::IPlugin::ShutdownFlag GirPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (m_Factory){
        removeObject(m_Factory);
        delete m_Factory;
        m_Factory = 0;
    }
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(GirPlugin)
