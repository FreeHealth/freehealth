/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \namespace Form
  \brief Namespace reserved for the FormManager plugin.
  The Form namespace includes:
  - All UIs and engines related to the Patient's Form management
*/

#include "formmanagerplugin.h"
#include "formcore.h"
#include "formmanagermode.h"
#include "formmanager.h"
#include "formplaceholder.h"
#include "episodebase.h"
#include "episodemodel.h"
#include "formmanagerpreferencespage.h"
#include "firstrunformmanager.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>
#include <QWidget>
#include <QDebug>

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}

using namespace Form;
using namespace Internal;

FormManagerPlugin::FormManagerPlugin() :
    ExtensionSystem::IPlugin(),
    _core(0),
    _mode(0),
    m_FirstRun(0),
    m_PrefPageSelector(0),
    m_PrefPage(0)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "creating FormManagerPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_formmanager");

    // Create first run pages
    m_FirstRun = new Internal::FirstRunFormManagerConfigPage(this);
    addObject(m_FirstRun);

    // Create preferences pages
    m_PrefPageSelector = new Internal::FormPreferencesFileSelectorPage(this);
    m_PrefPage = new Internal::FormPreferencesPage(this);
    addAutoReleasedObject(m_PrefPage);
    addAutoReleasedObject(m_PrefPageSelector);

    // Create the form core
    _core = new Form::FormCore(this);
    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

FormManagerPlugin::~FormManagerPlugin()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
}

bool FormManagerPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "FormManagerPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    messageSplash(tr("Initializing form manager plugin..."));

    return _core->initialize();
}

void FormManagerPlugin::extensionsInitialized()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << "FormManagerPlugin::extensionsInitialized";

    // no user -> end
    if (!user())
        return;
    if (user()->uuid().isEmpty())
        return;

    messageSplash(tr("Initializing form manager plugin..."));

//    _core->initialize();

    // Initialize base and manager
    episodeBase()->initialize();
    formManager().checkFormUpdates();

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Add mode
    _mode = new FormManagerMode(this);
}

void FormManagerPlugin::postCoreInitialization()
{
    if (Utils::Log::debugPluginsCreation())
        qWarning() << Q_FUNC_INFO;


    // Check FirstRun Default Form
    const QString &uid = settings()->defaultForm();
//    qWarning() << uid;
    if (!uid.isEmpty()) {
        episodeBase()->setGenericPatientFormFile(uid);
        formManager().readPmhxCategories(uid);
        formManager().loadPatientFile();
        settings()->setDefaultForm("");
    } else {
        // reload patient file just to emit patientFormsLoaded
        formManager().readPmhxCategories("");
        formManager().loadPatientFile();
    }
}

ExtensionSystem::IPlugin::ShutdownFlag FormManagerPlugin::aboutToShutdown()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (m_FirstRun) {
        removeObject(m_FirstRun);
        delete m_FirstRun;
        m_FirstRun = 0;
    }
    delete _mode;
    delete _core;
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(FormManagerPlugin)
