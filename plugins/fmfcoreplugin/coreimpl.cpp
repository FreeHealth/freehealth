/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#include "coreimpl.h"

#include <coreplugin/settings_p.h>
#include <coreplugin/isettings.h>
#include <coreplugin/theme.h>
#include <coreplugin/translators.h>
#include <coreplugin/actionmanager/actionmanager_p.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/contextmanager/contextmanager_p.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/formmanager.h>

#include <fmfcoreplugin/mainwindow.h>



#include <translationutils/constanttranslations.h>

#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

namespace Core {
namespace Internal {
static CoreImpl *m_instance = 0;
} // namespace Internal
} // namespace Core


using namespace Core;
using namespace Core::Internal;


ICore* ICore::instance()
{
    return m_instance;
}

// instance is created by Core::CorePlugin()
CoreImpl::CoreImpl(QObject *parent) : ICore(parent)
{
    m_UID = new UniqueIDManager();
    m_Settings = new SettingsPrivate(this);
    m_Theme = new ThemePrivate(this);
    m_Theme->setThemeRootPath(m_Settings->path(ISettings::ThemeRootPath));
    m_MainWindow = new MainWindow();
    m_ActionManager = new ActionManagerPrivate(m_MainWindow);
    m_ContextManager = new ContextManagerPrivate(m_MainWindow);
    m_FormManager = new FormManager(this);

    // add translators
    m_Translators = new Translators(this);
    m_Translators->setPathToTranslations(m_Settings->path(ISettings::TranslationsPath));
    // Qt
    m_Translators->addNewTranslator("qt");
    // Core Needed Libs
    m_Translators->addNewTranslator(Trans::Constants::CONSTANTS_TRANSLATOR_NAME);

    m_instance = this;
}

CoreImpl::~CoreImpl()
{
    delete m_MainWindow;
    delete m_UID;
}
//QStringList CoreImpl::showNewItemDialog(const QString &title,
//                                        const QList<IWizard *> &wizards,
//                                        const QString &defaultLocation)
//{
//    return m_mainwindow->showNewItemDialog(title, wizards, defaultLocation);
//}
//
//bool CoreImpl::showOptionsDialog(const QString &group, const QString &page, QWidget *parent)
//{
//    return m_mainwindow->showOptionsDialog(group, page, parent);
//}


ActionManager *CoreImpl::actionManager() const
{
    return m_ActionManager;
}

ContextManager *CoreImpl::contextManager() const
{
    return m_ContextManager;
}

UniqueIDManager *CoreImpl::uniqueIDManager() const
{
    return m_UID;
}

ITheme *CoreImpl::theme() const
{
    return m_Theme;
}

Translators *CoreImpl::translators() const
{
    return m_Translators;
}

ISettings *CoreImpl::settings() const
{
    return m_Settings;
}


IMainWindow *CoreImpl::mainWindow() const
{
    return m_MainWindow;
}

FormManager *CoreImpl::formManager() const
{
    return m_FormManager;
}


bool CoreImpl::initialize(const QStringList &arguments, QString *errorString)
{
    return m_MainWindow->initialize(arguments, errorString);
}

void CoreImpl::extensionsInitialized()
{
    m_MainWindow->extensionsInitialized();
}

