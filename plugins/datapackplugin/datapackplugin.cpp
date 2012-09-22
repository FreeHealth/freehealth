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
 ***************************************************************************/
#include "datapackplugin.h"
#include "datapackpreference.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/dialogs/pluginaboutpage.h>

#include <utils/log.h>
#include <utils/global.h>
#include <datapackutils/datapackcore.h>
#include <datapackutils/iservermanager.h>
#include <datapackutils/widgets/serverpackeditor.h>

#include <QtPlugin>
#include <QDialog>
#include <QHBoxLayout>
#include <QAction>

#include <QDebug>

using namespace DataPackPlugin;
using namespace DataPackPlugin::Internal;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }
static inline QString defaultServerFile() {return settings()->path(Core::ISettings::DataPackApplicationPath) + "/defaultservers.txt";}

DataPackPluginIPlugin::DataPackPluginIPlugin()
{
    setObjectName("DataPackPlugin");
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
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
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

    if (!user())
        return;
    if (!user()->hasCurrentUser())
        return;


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
    core.registerPathTag(Core::Constants::TAG_USER_DOCUMENT_PATH, settings()->path(Core::ISettings::UserDocumentsPath));
    core.init();

    // Send the server manager configuration from the settings of the user/application
    QString xmlConfig;
#ifdef FREEMEDFORMS
    if (user())
        xmlConfig = user()->value(Core::IUser::DataPackConfig).toString();
#endif

#ifdef FREEACCOUNT
    if (user())
        xmlConfig = user()->value(Core::IUser::DataPackConfig).toString();
#endif

#ifdef FREEDIAMS
    xmlConfig = QString(QByteArray::fromBase64(settings()->value("datapack/server/config").toByteArray()));
#endif

    if (xmlConfig.isEmpty()) {
        // read default servers
        QString content = Utils::readTextFile(defaultServerFile(), Utils::DontWarnUser);
        if (!content.isEmpty()) {
            LOG(tr("Trying to set the default datapack servers using file %1").arg(defaultServerFile()));
            foreach(const QString &line, content.split("\n")) {
                if (line.startsWith("--") || (line.startsWith("//")))
                    continue;
                QStringList values = line.split(";");
                QString serverUrl;
                DataPack::Server::UrlStyle urlStyle;
                if (values.count()==2) {
                    serverUrl = values.at(0);
                    urlStyle = DataPack::Server::UrlStyle(values.at(1).toInt());
                    DataPack::Server server(serverUrl);
                    server.setUrlStyle(urlStyle);
                    if (!core.serverManager()->addServer(server))
                        LOG_ERROR(tr("Unable to add default server %1 (%2)").arg(serverUrl).arg(urlStyle));
                    else
                        LOG(tr("Adding default server %1 (%2)").arg(serverUrl).arg(urlStyle));
                }
            }
        }
    } else {
        if (!core.serverManager()->setGlobalConfiguration(xmlConfig))
            LOG_ERROR("Unable to set the datapack server manager configuration");
        // Always unsure that the freemedforms datapack server is available
        DataPack::Server http("http://packs.freemedforms.com");
        http.setUrlStyle(DataPack::Server::HttpPseudoSecuredAndZipped);
        core.serverManager()->addServer(http);
    }


    // TODO: Check for package update -> thread this

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void DataPackPluginIPlugin::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    // Core is fully intialized as well as all plugins
    // Add pack manager action to "Preferences" menu
    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context context(Core::Constants::C_GLOBAL);

    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Constants::M_CONFIGURATION);
    if (menu) {
        // Create local actions
        a = new QAction(this);
        a->setObjectName("aTogglePackManager");
        a->setIcon(theme()->icon(Constants::I_TOGGLEPACKMANAGER));
        cmd = actionManager()->registerAction(a, Core::Id(Constants::A_TOGGLE_PACKMANAGER), context);
        cmd->setTranslations(Constants::TOGGLEPACKMANAGER_TEXT, Constants::TOGGLEPACKMANAGER_TEXT, Constants::DATAPACKCONSTANTS_TR_CONTEXT);
        menu->addAction(cmd, Core::Id(Core::Constants::G_PREFERENCES));
        connect(a, SIGNAL(triggered()), this, SLOT(togglePackManager()));
    }

    // TODO: code here? DataPacks are checked.
    // DataPacks are checked

}

void DataPackPluginIPlugin::coreAboutToClose()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DataPackPluginIPlugin::coreAboutToClose";
    // Core is about to close
    // Core::user() is still available
    DataPack::DataPackCore &core = DataPack::DataPackCore::instance(this);
#ifdef FREEMEDFORMS
    if (user())
        user()->setValue(Core::IUser::DataPackConfig, core.serverManager()->xmlConfiguration());
#endif

#ifdef FREEACCOUNT
    if (user())
        user()->setValue(Core::IUser::DataPackConfig, core.serverManager()->xmlConfiguration());
#endif

#ifdef FREEDIAMS
    QByteArray s = QByteArray(core.serverManager()->xmlConfiguration().toUtf8()).toBase64();
    settings()->setValue("datapack/server/config", s);
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
    DataPack::ServerPackEditor *editor = new DataPack::ServerPackEditor(&dlg);
    lay->addWidget(editor);
    Utils::resizeAndCenter(&dlg);
    dlg.show();
    editor->refreshServerContent();
    dlg.exec();
}

ExtensionSystem::IPlugin::ShutdownFlag DataPackPluginIPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (m_prefPage) {
        removeObject(m_prefPage);
        delete m_prefPage;
        m_prefPage = 0;
    }

    // Core is about to close
    // Core::user() is still available
    DataPack::DataPackCore &core = DataPack::DataPackCore::instance(this);
#ifdef FREEMEDFORMS
    if (user())
        user()->setValue(Core::IUser::DataPackConfig, core.serverManager()->xmlConfiguration());
#endif

#ifdef FREEACCOUNT
    if (user())
        user()->setValue(Core::IUser::DataPackConfig, core.serverManager()->xmlConfiguration());
#endif

#ifdef FREEDIAMS
    QByteArray s = QByteArray(core.serverManager()->xmlConfiguration().toUtf8()).toBase64();
    settings()->setValue("datapack/server/config", s);
#endif


    return SynchronousShutdown;
}


Q_EXPORT_PLUGIN(DataPackPluginIPlugin)
