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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
  \namespace PadTools
  \brief Namespace reserved for the PadTools plugin.
  The PadTools namespace includes:
  - Token engine
  - PadDocument creator
  - PadWriter
*/

#include "padtoolsplugin.h"
#include "padtoolsimpl.h"
//#include "padwriter.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace PadTools;

PadToolsPlugin::PadToolsPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating PadToolsPlugin";
    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("padtoolsplugin");
}

PadToolsPlugin::~PadToolsPlugin()
{
}

bool PadToolsPlugin::initialize(const QStringList &arguments, QString *errorString)
{
	qDebug("PadToolsPlugin::initialize");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PadToolsPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

	addAutoReleasedObject(new PadToolsImpl());

    return true;
}

void PadToolsPlugin::extensionsInitialized()
{
	qDebug("PadToolsPlugin::extensionsInitialized");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PadToolsPlugin::extensionsInitialized";

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

//    mainWindow()->setCentralWidget(new PadWriter);
}


Q_EXPORT_PLUGIN(PadToolsPlugin)
