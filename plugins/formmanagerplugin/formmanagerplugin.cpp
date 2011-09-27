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
  \namespace Form
  \brief Namespace reserved for the FormManager plugin.
  The Form namespace includes:
  - All UIs and engines related to the Patient's Form management
*/

#include "formmanagerplugin.h"
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

using namespace Form;
using namespace Internal;

static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}


FormManagerPlugin::FormManagerPlugin() :
        mode(0), m_FirstRun(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating FormManagerPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("formmanagerplugin");

    m_FirstRun = new Internal::FirstRunFormManagerConfigPage(this);
    addObject(m_FirstRun);
}

FormManagerPlugin::~FormManagerPlugin()
{
    qWarning() << "FormManagerPlugin::~FormManagerPlugin()";
    if (m_FirstRun) {
        removeObject(m_FirstRun);
        delete m_FirstRun;
        m_FirstRun = 0;
    }
    delete FormManager::instance();
}

bool FormManagerPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "FormManagerPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    messageSplash(tr("Initializing form manager plugin..."));

    return true;
}

void FormManagerPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "FormManagerPlugin::extensionsInitialized";

    // no user -> end
    if (!user())
        return;
    if (user()->uuid().isEmpty())
        return;

    messageSplash(tr("Initializing form manager plugin..."));

    // Initialize patient base and manager
    episodeBase();
    FormManager::instance();

    // Check FirstRun Default Form
    const QString &uid = settings()->defaultForm();
    if (!uid.isEmpty()) {
        episodeBase()->setGenericPatientFormFile(uid);
        FormManager::instance()->readPmhxCategories(uid);
        settings()->setDefaultForm("");
    }

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    Internal::FormManagerPreferencesPage *prefPage = new Internal::FormManagerPreferencesPage(this);
    prefPage->checkSettingsValidity();
    addAutoReleasedObject(prefPage);

    // Add mode
    mode = new FormManagerMode(this);
}


Q_EXPORT_PLUGIN(FormManagerPlugin)
