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
#ifndef COREIMPL_H
#define COREIMPL_H

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>

/**
 * \file coreimpl.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 03 Dec 2010
*/


namespace Core {
class Patient;

namespace Internal {
class CommandLine;
class ThemePrivate;
class ActionManagerPrivate;
class ContextManagerPrivate;
class SettingsPrivate;
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

    // Patient's datas wrapper
    IPatient *patient() const {return 0;}
    void setPatient(IPatient *) {}

    IUser *user() const {return m_User;}
    void setUser(IUser *user) {m_User = user;}

    virtual void setScriptManager(IScriptManager *script) {m_Script=script;}
    virtual IScriptManager *scriptManager() const {return m_Script;}

private:
    IMainWindow *m_MainWindow;
    ActionManagerPrivate *m_ActionManager;
    ContextManagerPrivate *m_ContextManager;
    ThemePrivate *m_Theme;
    Translators *m_Translators;
    SettingsPrivate *m_Settings;
    CommandLine *m_CommandLine;
    Patient *m_Patient;
    IUser *m_User;
    Utils::UpdateChecker *m_UpdateChecker;
    Core::FileManager *m_FileManager;
    IPadTools *m_PadTools;
    IScriptManager *m_Script;
};

} // namespace Internal
} // namespace Core

#endif // COREIMPL_H
