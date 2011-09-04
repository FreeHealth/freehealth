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
#include "coreplugin.h"
#include "coreimpl.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <fmfcoreplugin/appaboutpage.h>
#include <coreplugin/isettings.h>
#include <coreplugin/dialogs/commonaboutpages.h>
#include <coreplugin/dialogs/commondebugpages.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/applicationgeneralpreferences.h>

#include <QtCore/QtPlugin>

using namespace Core::Internal;

CorePlugin::CorePlugin() :
        m_CoreImpl(0), prefPage(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating CorePlugin";
    m_CoreImpl =  new CoreImpl(this);
}

CorePlugin::~CorePlugin()
{
    qWarning() << "CorePlugin::~CorePlugin()";
    if (prefPage) {
        removeObject(prefPage);
        delete prefPage; prefPage=0;
    }
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "CorePlugin::initialize";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("utils");
    Core::ICore::instance()->translators()->addNewTranslator("translationutils");
    Core::ICore::instance()->translators()->addNewTranslator("medicalutils");
    Core::ICore::instance()->translators()->addNewTranslator("fmfcoreplugin");

    return m_CoreImpl->initialize(arguments,errorMessage);
//    Q_UNUSED(arguments);
//    const bool success = m_mainWindow->init(errorMessage);
//    if (success) {
//        m_welcomeMode = new WelcomeMode;
//        addObject(m_welcomeMode);
//
//        EditorManager *editorManager = m_mainWindow->editorManager();
//        m_editMode = new EditMode(editorManager);
//        addObject(m_editMode);
//    }
//    return success;
}

void CorePlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "CorePlugin::extensionsInitialized";

    m_CoreImpl->extensionsInitialized();

    // add about pages
    this->addAutoReleasedObject(new AppAboutPage(this));
    this->addAutoReleasedObject(new TeamAboutPage(this));
    this->addAutoReleasedObject(new LicenseAboutPage(this));
    this->addAutoReleasedObject(new BuildAboutPage(this));

    // add debugging pages
    this->addAutoReleasedObject(new LogErrorDebugPage(this));
    this->addAutoReleasedObject(new LogMessageDebugPage(this));
    this->addAutoReleasedObject(new SettingDebugPage(this));

    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // add preferences page
    prefPage = new ApplicationGeneralPreferencesPage(this);
    prefPage->checkSettingsValidity();
    addObject(prefPage);
    m_CoreImpl->settings()->sync();
}

void CorePlugin::remoteArgument(const QString& arg)
{
    Q_UNUSED(arg);
    // An empty argument is sent to trigger activation
    // of the window via QtSingleApplication. It should be
    // the last of a sequence.
//    if (arg.isEmpty()) {
//        m_mainWindow->activateWindow();
//    } else {
//        m_mainWindow->openFiles(QStringList(arg));
//    }
}

Q_EXPORT_PLUGIN(CorePlugin)
