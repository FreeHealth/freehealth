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
 ***************************************************************************/
#ifndef FREEACCOUNT_COREIMPL_H
#define FREEACCOUNT_COREIMPL_H

#include <coreplugin/icore.h>

/**
 * \file coreimpl.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 12 Mar 2010
*/


namespace Core {
namespace Internal {
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

    // Splash screen functions
    void createSplashScreen(const QPixmap &pix);
    void finishSplashScreen(QWidget *w);
    void messageSplashScreen(const QString &msg);
    QSplashScreen *splashScreen();

    ActionManager *actionManager() const;
    ContextManager *contextManager() const;
    UniqueIDManager *uniqueIDManager() const;

    ITheme *theme() const;
    Translators *translators() const;

    ISettings *settings() const;

    IMainWindow *mainWindow() const;
    void setMainWindow(IMainWindow *);

    FileManager *fileManager() const;

    // initialization
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    CommandLine *commandLine() const;

    Utils::UpdateChecker *updateChecker() const;

    virtual IPatient *patient() const {return m_Patient;}
    virtual void setPatient(IPatient *) {}  // Created and managed by CoreImpl

    virtual IUser *user() const {return m_User;}
    virtual void setUser(IUser *user) {m_User=user;}

private:
    QSplashScreen *m_Splash;
    IMainWindow *m_MainWindow;
    ActionManagerPrivate *m_ActionManager;
    ContextManagerPrivate *m_ContextManager;
    UniqueIDManager *m_UID;
    ThemePrivate *m_Theme;
    Translators *m_Translators;
    SettingsPrivate *m_Settings;
    CommandLine *m_CommandLine;
    Utils::UpdateChecker *m_UpdateChecker;
    Core::FileManager *m_FileManager;
    Core::IUser *m_User;
    Core::IPatient *m_Patient;
};

} // namespace Internal
} // namespace Core

#endif // FREEACCOUNT_COREIMPL_H
