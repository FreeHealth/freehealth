/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>
#include <QWidget>
#include <QDebug>


using namespace Form;
using namespace Internal;

static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}


FormManagerPlugin::FormManagerPlugin() :
        mode(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating FormManagerPlugin";
}

FormManagerPlugin::~FormManagerPlugin()
{
    qWarning() << "FormManagerPlugin::~FormManagerPlugin()";
    if (mode) {
        removeObject(mode);
        delete mode;
        mode = 0;
    }
}

bool FormManagerPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "FormManagerPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("formmanagerplugin");

    // Initialize patient base
    episodeBase();
    if (!episodeBase()->isInitialized())
        return false;

    // Add options page
    addAutoReleasedObject(new Internal::FormManagerPreferencesPage(this));

    return true;
}

void FormManagerPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "FormManagerPlugin::extensionsInitialized";

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Add mode
    FormManager::instance();
    mode = new FormManagerMode(this);
    mode->setWidget(FormManager::instance()->formPlaceHolder());
    addObject(mode);
}


Q_EXPORT_PLUGIN(FormManagerPlugin)
