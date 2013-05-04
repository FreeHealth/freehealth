/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEMEDFORMS_COREIMPL_H
#define FREEMEDFORMS_COREIMPL_H

#include <coreplugin/icore.h>

#include <QPixmap>
#include <QSplashScreen>

namespace Utils {
class UpdateChecker;
}

namespace Core {
    class MainWindow;
    class ActionManager;
    class ISettings;
    class IMainWindow;
    class CommandLine;
    class ApplicationAutoLock;

namespace Internal {
    class ThemePrivate;
    class ActionManagerPrivate;
    class ContextManagerPrivate;
    class SettingsPrivate;
}
}

namespace Core {
namespace Internal {

class CoreImpl : public Core::ICore
{
    Q_OBJECT
public:
    CoreImpl(QObject *parent);
    ~CoreImpl();

    bool applicationConfigurationDialog() const;

    ActionManager *actionManager() const;
    ContextManager *contextManager() const;
    ITheme *theme() const;
    Translators *translators() const;
    ISettings *settings() const;

    IMainWindow *mainWindow() const;
    void setMainWindow(IMainWindow *window);

    void setPadTools(IPadTools *pad) {m_PadTools=pad;}
    IPadTools *padTools() const {return m_PadTools;}

//    FormManager *formManager() const;
    virtual ModeManager *modeManager() const;

    ICommandLine *commandLine() const;
    Utils::UpdateChecker *updateChecker() const;
    FileManager *fileManager() const;

    // Patient's data wrapper
    virtual IPatient *patient() const {return m_Patient;}
    virtual void setPatient(IPatient *patient) {m_Patient=patient;}

    // User's data wrapper
    virtual IUser *user() const {return m_User;}
    virtual void setUser(IUser *user) {m_User = user;}

    virtual void setScriptManager(IScriptManager *script) {m_Script=script;}
    virtual IScriptManager *scriptManager() const {return m_Script;}

    // initialization
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

private:
    IMainWindow *m_MainWindow;
    ActionManagerPrivate *m_ActionManager;
    ContextManagerPrivate *m_ContextManager;
    ThemePrivate *m_Theme;
    Translators *m_Translators;
    SettingsPrivate *m_Settings;
//    FormManager *m_FormManager;
    Utils::UpdateChecker *m_UpdateChecker;
    FileManager *m_FileManager;
    CommandLine *m_CommandLine;
    ModeManager *m_ModeManager;
    IPatient *m_Patient;
    IUser *m_User;
    IScriptManager *m_Script;
    IPadTools *m_PadTools;
    ApplicationAutoLock *m_AutoLock;
};

} // namespace Internal
} // namespace Core

#endif // COREIMPL_H
