/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \namespace Gir
  \brief Namespace reserved for the AGGIR plugins (a french only plugin that measure physical dependence).
*/

#include "aggirplugin.h"
#include "girwidget.h"

#include <coreplugin/dialogs/pluginaboutpage.h>

#include <utils/log.h>

#include <QtCore/QtPlugin>

#include <QDebug>

using namespace Gir;
using namespace Gir::Internal;

GirPlugin::GirPlugin() : m_Factory(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating GirPlugin";
}

GirPlugin::~GirPlugin()
{
    if (m_Factory){
        removeObject(m_Factory);
        delete m_Factory;
        m_Factory = 0;
    }
}

bool GirPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "GirPlugin::initialize";

    Q_UNUSED(arguments);
    Q_UNUSED(errorString);
    m_Factory = new GirWidgetFactory(this);
    addObject(m_Factory);
    return true;
}

void GirPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "GirPlugin::extensionsInitialized";

        addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}


Q_EXPORT_PLUGIN(GirPlugin)
