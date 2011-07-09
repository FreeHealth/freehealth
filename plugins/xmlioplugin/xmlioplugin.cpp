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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
  \namespace XmlForms
  \brief Namespace reserved for the XML Forms' IO plugin.
*/

#include "xmlioplugin.h"
#include "xmlformio.h"
#include "xmlformcontentreader.h"
#include "xmliobase.h"

#include <coreplugin/dialogs/pluginaboutpage.h>

#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace XmlForms;

XmlFormIOPlugin::XmlFormIOPlugin() :
        ExtensionSystem::IPlugin(),
        m_XmlReader(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating XmlIOPlugin";
    // create XML reader singleton
    m_XmlReader = Internal::XmlFormContentReader::instance();
    addAutoReleasedObject(new XmlFormIO(this));
}

XmlFormIOPlugin::~XmlFormIOPlugin()
{
    qWarning() << "XmlFormIOPlugin::~XmlFormIOPlugin()";
    // delete XmlFormContentReader singleton
    if (m_XmlReader)
        delete m_XmlReader;
    m_XmlReader = 0;
}

bool XmlFormIOPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "XmlIOPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    return true;
}

void XmlFormIOPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "XmlIOPlugin::extensionsInitialized";

    // initialize database
    Internal::XmlIOBase::instance();

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}


Q_EXPORT_PLUGIN(XmlFormIOPlugin)
