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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ICORE_H
#define ICORE_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>
#include <QString>

QT_BEGIN_NAMESPACE
class QSplashScreen;
class QPixmap;
class QWidget;
QT_END_NAMESPACE

/**
 * \file icore.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 24 Jan 2013
*/

namespace Utils {
class UpdateChecker;
}

namespace Core {
class ActionManager;
class ContextManager;
class Translators;
class ITheme;
class ISettings;
class IMainWindow;
class ModeManager;
class ICommandLine;
class IPatient;
class IUser;
class FileManager;
class IPadTools;
class IScriptManager;
}

namespace Core {

class CORE_EXPORT ICore : public QObject
{
    Q_OBJECT

public:
    ICore(QObject *parent);
    virtual ~ICore();

    static ICore *instance();

    // Some information
    virtual bool updatePreferences() const {return m_UpdatePreferences;}

    // Dialogs
    virtual bool alphaDialog() const;
    virtual bool applicationConfigurationDialog() const { return true; }

    // Action manager, ContextManager, ModeManager, FileManager
    virtual ActionManager *actionManager() const = 0;
    virtual ContextManager *contextManager() const = 0;
    virtual ModeManager *modeManager() const {return 0;}
    virtual FileManager *fileManager() const = 0;

    // Themes, settings, translations
    virtual ITheme *theme() const = 0;
    virtual Translators *translators() const = 0;
    virtual ISettings *settings() const = 0;

    virtual ICommandLine *commandLine() const = 0;
    virtual Utils::UpdateChecker *updateChecker() const = 0;

    virtual void setMainWindow(IMainWindow *) = 0;
    virtual IMainWindow *mainWindow() const = 0;

    virtual void setPadTools(IPadTools *) = 0;
    virtual IPadTools *padTools() const = 0;

    // Patient's data wrapper
    virtual IPatient *patient() const = 0;
    virtual void setPatient(IPatient *patient) = 0;

    virtual IUser *user() const = 0;
    virtual void setUser(IUser *user) = 0;

    virtual void setScriptManager(IScriptManager *) = 0;
    virtual IScriptManager *scriptManager() const = 0;



//    virtual MessageManager *messageManager() const = 0;
//    virtual EditorManager *editorManager() const = 0;
//    virtual ProgressManager *progressManager() const = 0;
//    virtual ScriptManager *scriptManager() const = 0;
//    virtual VariableManager *variableManager() const = 0;
//    virtual VCSManager *vcsManager() const = 0;
//    virtual MimeDatabase *mimeDatabase() const = 0;
//
//    virtual QSettings *settings() const = 0;
//    virtual SettingsDatabase *settingsDatabase() const = 0;
//    virtual QPrinter *printer() const = 0;
//
//    virtual QString resourcePath() const = 0;

    void coreIsAboutToClose() { Q_EMIT coreAboutToClose(); }
    void requestSaveSettings() { Q_EMIT saveSettingsRequested(); }
    void databaseServerLoginChanged() { Q_EMIT databaseServerChanged(); }
    void requestFirstRunDatabaseCreation() { Q_EMIT firstRunDatabaseCreation(); }

Q_SIGNALS:
    void coreAboutToOpen();
    void coreOpened();
    void saveSettingsRequested();
    void optionsDialogRequested();
    void coreAboutToClose();

    // Databases management
    void firstRunDatabaseCreation();
    void databaseServerChanged();

protected:
    bool m_UpdatePreferences;
};

} // namespace Core

#endif // ICORE_H
