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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "interactionplugin.h"
#include "drugdruginteractioncore.h"
#include "afssapsintegrator.h"
#include "interactionsdatabasepage.h"
#include "interactioneditorpage.h"
#include "interactoreditorpage.h"
//#include "cytochromep450interactionspage.h"

#include <coreplugin/dialogs/pluginaboutpage.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace IAMDb::Internal;

InteractionPlugin::InteractionPlugin() :
    m_DDIPage(0),
    m_InteractorsPage(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "Creating InteractionPlugin";
}

InteractionPlugin::~InteractionPlugin()
{
    qWarning() << "InteractionPlugin::~InteractionPlugin()";
    if (m_DDIPage) {
        removeObject(m_DDIPage);
        delete m_DDIPage;
        m_DDIPage = 0;
    }
    if (m_InteractorsPage) {
        removeObject(m_InteractorsPage);
        delete m_InteractorsPage;
        m_InteractorsPage = 0;
    }
}

bool InteractionPlugin::initialize(const QStringList &/*arguments*/, QString */*errorMessage*/)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "InteractionPlugin::initialize";

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Create the core object
    DrugDrugInteractionCore::instance();

//    addAutoReleasedObject(new AfssapsIntegratorPage(this));
//    addAutoReleasedObject(new AfssapsClassTreePage(this));
    addObject(m_DDIPage = new InteractionEditorPage(this));
    addObject(m_InteractorsPage = new InteractorEditorPage(this));
    addAutoReleasedObject(new InteractionsDatabaseCreatorPage(this));
//    addAutoReleasedObject(new CytochromeP450InteractionsPage(this));

    return true;
}

void InteractionPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "InteractionPlugin::extensionsInitialized";

//    Core::ICore::instance()->translators()->addNewTranslator("freetoolbox-interactionplugin");
}


Q_EXPORT_PLUGIN(InteractionPlugin)
