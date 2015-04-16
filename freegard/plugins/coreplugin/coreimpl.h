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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEGUARD_COREIMPL_H
#define FREEGUARD_COREIMPL_H

#include <coreplugin/icore.h>

/**
 * \file coreimpl.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 14 Oct 2014
*/

namespace Core {
namespace Internal {
    class ThemePrivate;
    class ActionManagerPrivate;
    class ContextManagerPrivate;
    class SettingsPrivate;
    class CommandLine;
}  // End Internal
}  // End Core

namespace Core {
namespace Internal {

class CoreImpl : public Core::ICore
{
    Q_OBJECT
public:
    CoreImpl(QObject *parent);
    ~CoreImpl();

    static CoreImpl *instance() { return static_cast<CoreImpl *>(ICore::instance()); }

    ActionManager *actionManager() const;
    ContextManager *contextManager() const;
    ModeManager *modeManager() const;

    ITheme *theme() const;
    Translators *translators() const;

    ISettings *settings() const;

    IMainWindow *mainWindow() const;
    void setMainWindow(IMainWindow *);

    void setPadTools(IPadTools *pad) {m_PadTools=pad;}
    IPadTools *padTools() const {return m_PadTools;}

    FileManager *fileManager() const;

    // initialization
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    ICommandLine *commandLine() const;

    Utils::UpdateChecker *updateChecker() const;

    virtual IPatient *patient() const {return m_Patient;}
    virtual void setPatient(IPatient *) {}  // Created and managed by CoreImpl

    virtual IUser *user() const {return m_User;}
    virtual void setUser(IUser *user) {m_User=user;}

    virtual void setScriptManager(IScriptManager *) {}
    virtual IScriptManager *scriptManager() const {return 0;}

private:
    IMainWindow *m_MainWindow;
    ActionManagerPrivate *m_ActionManager;
    ContextManagerPrivate *m_ContextManager;
    ModeManager *m_ModeManager;
    ThemePrivate *m_Theme;
    Translators *m_Translators;
    SettingsPrivate *m_Settings;
    CommandLine *m_CommandLine;
    Utils::UpdateChecker *m_UpdateChecker;
    Core::FileManager *m_FileManager;
    Core::IUser *m_User;
    Core::IPatient *m_Patient;
    IPadTools *m_PadTools;
};

} // namespace Internal
} // namespace Core

#endif // FREEGUARD_COREIMPL_H
