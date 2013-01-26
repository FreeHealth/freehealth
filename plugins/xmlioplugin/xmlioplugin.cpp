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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
  \namespace XmlForms
  \brief Namespace reserved for the XML Forms' IO plugin.
*/

#include "xmlioplugin.h"
#include "xmlformio.h"
#include "xmlformcontentreader.h"
#include "xmliobase.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/translators.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <utils/log.h>

#include <QAction>
#include <QtPlugin>
#include <QDebug>

using namespace XmlForms;
using namespace Internal;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }

XmlFormIOPlugin::XmlFormIOPlugin() :
    ExtensionSystem::IPlugin(),
    m_XmlReader(0),
    m_FormIo(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating XmlIOPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_xmlio");

    // create instance
    m_XmlReader = XmlFormContentReader::instance();
    new XmlIOBase(this);

    addObject(m_FormIo = new XmlFormIO(this));
}

XmlFormIOPlugin::~XmlFormIOPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
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

    // no user -> end
    if (!user())
        return;
    if (user()->uuid().isEmpty())
        return;

    // initialize database
    Internal::XmlIOBase::instance()->initialize();

    // add help menu action
    Core::Context globalcontext(Core::Constants::C_GLOBAL);
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Id(Core::Constants::M_HELP_DATABASES));
    QAction *a = new QAction(this);
    a->setObjectName("aXmlFormIOPlugin.showDatabaseInformation");
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Core::Id("aXmlFormIOPlugin.showDatabaseInformation"), globalcontext);
    cmd->setTranslations(Trans::Constants::XMLIO_DATABASE_INFORMATION);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Id(Core::Constants::G_HELP_DATABASES));
    }
    connect(a, SIGNAL(triggered()), this, SLOT(showDatabaseInformation()));

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}

ExtensionSystem::IPlugin::ShutdownFlag XmlFormIOPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    // delete XmlFormContentReader singleton
    if (m_XmlReader)
        delete m_XmlReader;
    m_XmlReader = 0;
    if (m_FormIo) {
        removeObject(m_FormIo);
    }
    return SynchronousShutdown;
}

void XmlFormIOPlugin::showDatabaseInformation()
{
    Utils::DatabaseInformationDialog dlg(this);
    dlg.setTitle(tkTr(Trans::Constants::XMLIO_DATABASE_INFORMATION));
    dlg.setDatabase(*Internal::XmlIOBase::instance());
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}

Q_EXPORT_PLUGIN(XmlFormIOPlugin)
