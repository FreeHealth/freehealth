/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "coreplugin.h"
#include "coreimpl.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <fmfcoreplugin/appaboutpage.h>
#include <coreplugin/dialogs/commonaboutpages.h>
#include <coreplugin/dialogs/commondebugpages.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/translators.h>
#include <coreplugin/dialogs/applicationgeneralpreferences.h>

#include <QtCore/QtPlugin>

using namespace Core::Internal;

CorePlugin::CorePlugin() : m_CoreImpl(0), prefPage(0)
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

    // add preferences page
    prefPage = new ApplicationGeneralPreferencesPage(this);
    prefPage->checkSettingsValidity();
    addObject(prefPage);

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
