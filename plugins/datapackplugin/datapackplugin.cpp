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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "datapackplugin.h"
#include "datapackpreference.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/dialogs/pluginaboutpage.h>

#include <utils/log.h>
#include <utils/global.h>
#include <datapackutils/datapackcore.h>
#include <datapackutils/iservermanager.h>
#include <datapackutils/widgets/serverpackeditor.h>

#include <QtCore/QtPlugin>
#include <QDialog>
#include <QHBoxLayout>
#include <QDebug>

using namespace DataPackPlugin;
using namespace DataPackPlugin::Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

DataPackPluginIPlugin::DataPackPluginIPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating DataPackPluginIPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("datapackutils");
    Core::ICore::instance()->translators()->addNewTranslator("datapackplugin");

    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool

    // All preferences pages must be created in this part (before user connection)
    m_prefPage = new Internal::DataPackPreferencePage(this);
    addObject(m_prefPage);

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
}

DataPackPluginIPlugin::~DataPackPluginIPlugin()
{    
    if (m_prefPage) {
        removeObject(m_prefPage);
        delete m_prefPage;
        m_prefPage = 0;
    }
}

bool DataPackPluginIPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DataPackPluginIPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // No user connected here

    // Initialize database here
    // Initialize the drugs engines
    // Add your Form::IFormWidgetFactory here to the plugin manager object pool

    return true;
}

void DataPackPluginIPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DataPackPluginIPlugin::extensionsInitialized";

    messageSplash(tr("Initializing DataPackPlugin..."));

    // At this point, user is connected
    // Create && configure the DataPack lib core
    DataPack::DataPackCore &core = DataPack::DataPackCore::instance(this);
    core.setInstallPath(settings()->dataPackInstallPath());
    core.setPersistentCachePath(settings()->dataPackPersitentTempPath());
    core.setTemporaryCachePath(settings()->path(Core::ISettings::ApplicationTempPath));
    core.setThemePath(DataPack::DataPackCore::SmallPixmaps, settings()->path(Core::ISettings::SmallPixmapPath));
    core.setThemePath(DataPack::DataPackCore::MediumPixmaps, settings()->path(Core::ISettings::MediumPixmapPath));
    core.setThemePath(DataPack::DataPackCore::BigPixmaps, settings()->path(Core::ISettings::BigPixmapPath));
    core.init();

    // Send the server manager configuration from the settings of the user/application
    QString xmlConfig;
#ifdef FREEMEDFORMS
    xmlConfig = "";
#else
#  ifdef FREEDIAMS
    xmlConfig = QString(QByteArray::fromBase64(settings()->value("datapack/server/config").toByteArray()));
#  endif
#endif
    if (!core.serverManager()->setGlobalConfiguration(xmlConfig))
        LOG_ERROR("Unable to set the datapack server manager configuration");


    /** \todo Check for package update -> thread this */


    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void DataPackPluginIPlugin::postCoreInitialization()
{
    // Core is fully intialized as well as all plugins
    // Add a pack manager action
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();

    QAction *a = 0;
    Core::Command *cmd = 0;

    QList<int> context = QList<int>() << Core::Constants::C_GLOBAL_ID;

    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Constants::M_CONFIGURATION);
    if (menu) {
        // Create local actions
        a = new QAction(this);
        a->setObjectName("aTogglePackManager");
        a->setIcon(theme()->icon(Constants::I_TOGGLEPACKMANAGER));
        cmd = actionManager()->registerAction(a, Constants::A_TOGGLE_PACKMANAGER, context);
        cmd->setTranslations(Constants::TOGGLEPACKMANAGER_TEXT, Constants::TOGGLEPACKMANAGER_TEXT, Constants::DATAPACKCONSTANTS_TR_CONTEXT);
        menu->addAction(cmd, Core::Constants::G_PREFERENCES);
        connect(a, SIGNAL(triggered()), this, SLOT(togglePackManager()));
    }

    /** \todo code here ??? DataPacks are checked. */
    // DataPacks are checked

}

void DataPackPluginIPlugin::coreAboutToClose()
{
    // Core is about to close
    // Core::user() is still available
#ifdef FREEMEDFORMS
    ;
#else
#  ifdef FREEDIAMS
    QByteArray s = QByteArray(DataPack::DataPackCore::instance().serverManager()->xmlConfiguration().toUtf8()).toBase64();
    settings()->setValue("datapack/server/config", s);
#  endif
#endif
}

void DataPackPluginIPlugin::togglePackManager()
{
    QDialog dlg;
    dlg.setWindowTitle(QApplication::translate(Constants::DATAPACKCONSTANTS_TR_CONTEXT, Constants::TOGGLEPACKMANAGER_TEXT));
    dlg.setWindowIcon(theme()->icon(Constants::I_TOGGLEPACKMANAGER));
    dlg.setModal(true);
    QHBoxLayout *lay = new QHBoxLayout(&dlg);
    dlg.setLayout(lay);
    lay->setMargin(0);
    lay->addWidget(new DataPack::ServerPackEditor(&dlg));
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}


Q_EXPORT_PLUGIN(DataPackPluginIPlugin)
