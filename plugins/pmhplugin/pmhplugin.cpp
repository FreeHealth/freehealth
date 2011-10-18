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
  \namespace PMH
  \brief Namespace reserved for the Patients' PMHx plugin.
  The PMH namespace includes:
  - All database accesses to the PMHx database
  - A creator wizard
  - A viewer
*/

#include "pmhplugin.h"
#include "pmhmode.h"
#include "pmhbase.h"
#include "pmhcore.h"
#include "pmhpreferencespage.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace PMH;
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user()  { return Core::ICore::instance()->user(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

PmhPlugin::PmhPlugin() :
    mode(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating PmhPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("pmhplugin");

    m_PrefPage = new Internal::PmhPreferencesPage(this);
    addAutoReleasedObject(m_PrefPage);
}

PmhPlugin::~PmhPlugin()
{
    qWarning() << "PmhPlugin::~PmhPlugin()";
    delete PmhCore::instance();
}

bool PmhPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PmhPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    messageSplash(tr("Initializing PMHx database plugin..."));

    return true;
}

void PmhPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PmhPlugin::extensionsInitialized";

    // no user -> end
    if (!user())
        return;
    if (user()->uuid().isEmpty())
        return;

    messageSplash(tr("Initializing PMHx database plugin..."));

    // Initialize Base
    Internal::PmhBase::instance();

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    m_PrefPage->checkSettingsValidity();

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void PmhPlugin::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    PmhCore::instance(this);
    mode = new Internal::PmhMode(this);
}


Q_EXPORT_PLUGIN(PmhPlugin)
