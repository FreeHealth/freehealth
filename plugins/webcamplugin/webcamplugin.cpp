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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>    *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

#include <opencv2/highgui/highgui.hpp>

#include "webcamplugin.h"
#include "webcamconstants.h"
#include "webcamphotoprovider.h"
#include "webcampreferences.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/translators.h>

#include <extensionsystem/pluginmanager.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <QtPlugin>
#include <QDebug>

using namespace Webcam;
using namespace Internal;

static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

WebcamPlugin::WebcamPlugin() :
    ExtensionSystem::IPlugin(),
    m_prefPage(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating Webcam";
    setObjectName("WebcamPlugin");

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_webcam");
    
    // Add here the Core::IFirstConfigurationPage objects to the pluginmanager object pool
    
//    // All preferences pages must be created in this part (before user connection)
//    m_prefPage = new WebcamPreferencesPage(this);
//    // And included in the QObject pool
//    addObject(m_prefPage);
    
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
    connect(Core::ICore::instance(), SIGNAL(coreAboutToClose()), this, SLOT(coreAboutToClose()));
}

WebcamPlugin::~WebcamPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool WebcamPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    
    if (Utils::Log::warnPluginsCreation()) {
        qDebug() << "WebcamPlugin::initialize";
    }


    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // connect to other plugins' signals
    // "In the initialize method, a plugin can be sure that the plugins it
    //  depends on have initialized their members."


    // FreeMedForms:
    // Initialize database here
    // Initialize the drugs engines
    // Add your Form::IFormWidgetFactory here to the plugin manager object pool
    
    // No user is logged in until here
    
    messageSplash(tr("Initializing Webcam..."));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    detectDevices();

    //    Core::ActionManager *am = Core::ICore::instance()->actionManager();
    //
    //    QAction *action = new QAction(tr("Webcam action"), this);
    //    Core::Command *cmd = am->registerAction(action, Constants::ACTION_ID,
    //                         Core::Context(Core::Constants::C_GLOBAL));
    //    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Shift+Meta+A")));
    //    connect(action, SIGNAL(triggered()), this, SLOT(triggerAction()));
    //
    //    Core::ActionContainer *menu = am->createMenu(Constants::CUSTOM_MENU_ID);
    //    menu->menu()->setTitle(tr("Webcam"));
    //    menu->addAction(cmd);
    //    am->actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);
    
    return true;
}

void WebcamPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation()) {
        qDebug() << "Webcam::extensionsInitialized";
    }
    
    // Retrieve other objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
    
    // If you want to stop the plugin initialization if there are no identified user
    // Just uncomment the following code
    //    // no user -> stop here
    //    if (!user())
    //        return;
    //    if (user()->uuid().isEmpty())
    //        return;
    
    messageSplash(tr("Initializing Webcam..."));
    
    // At this point, user is connected
    
    // Add here e.g. the DataPackPlugin::IDataPackListener objects to the pluginmanager object pool
    
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}

void WebcamPlugin::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    // Core is fully intialized as well as all plugins
    // DataPacks are checked
}

ExtensionSystem::IPlugin::ShutdownFlag WebcamPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool

//    if (m_prefPage) {
//        removeObject(m_prefPage);
//        delete(m_prefPage);
//        m_prefPage = 0;
//    }
    return SynchronousShutdown;
}

/**
 * Detect connected webcam and create a Core::IPhotoProvider object for each device found.
 * Created objects are sent to the plugin manager object pool.
 */
void WebcamPlugin::detectDevices()
{
    int nbDevice = 0;
    for(int deviceId = 0; deviceId<10; deviceId++) {
        cv::VideoCapture cap(deviceId);
        cv::Mat frame;
        if (cap.isOpened()) {
            cap.read(frame);
            if (!frame.empty()) {
                ++nbDevice;
                // add WebcamPhotoProvider object to the static list of providers
                bool alreadyThere = false;
                foreach(WebcamPhotoProvider *provider, WebcamPhotoProvider::getProviders()) {
                    if (provider->deviceId() == deviceId)
                        alreadyThere = true;
                }
                if (!alreadyThere)
                    addAutoReleasedObject(new WebcamPhotoProvider(deviceId, this));
            }
        }
    }
    LOG(QString("Found %1 webcam device").arg(nbDevice));
}

void WebcamPlugin::coreAboutToClose()
{
    // Core is about to close
    // ICore::user() is still available
}

Q_EXPORT_PLUGIN(WebcamPlugin)
