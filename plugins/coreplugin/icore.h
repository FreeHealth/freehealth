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
#ifndef ICORE_H
#define ICORE_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>
#include <QString>

class QSplashScreen;
class QPixmap;
class QWidget;

/**
 * \file icore.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 22 Aug 2010
*/


namespace Utils {
class UpdateChecker;
}

namespace Core {
class ActionManager;
class ContextManager;
class UniqueIDManager;
class Translators;
class ITheme;
class ISettings;
class IMainWindow;
class ModeManager;
class CommandLine;
class IPatient;
class IUser;
class FileManager;
class IPadTools;
}

namespace Core {

class CORE_EXPORT ICore : public QObject
{
    Q_OBJECT

public:
    ICore(QObject *parent) : QObject(parent), m_UpdatePreferences(false) {}
    virtual ~ICore() {}

    static ICore *instance();

    // Some informations
    virtual bool updatePreferences() const {return m_UpdatePreferences;}

    // Action manager, ContextManager, ModeManager, FileManager
    virtual ActionManager *actionManager() const = 0;
    virtual ContextManager *contextManager() const = 0;
    virtual UniqueIDManager *uniqueIDManager() const = 0;
    virtual ModeManager *modeManager() const {return 0;}
    virtual FileManager *fileManager() const = 0;

    // Themes, settings, translations
    virtual ITheme *theme() const = 0;
    virtual Translators *translators() const = 0;
    virtual ISettings *settings() const = 0;

    virtual CommandLine *commandLine() const = 0;
    virtual Utils::UpdateChecker *updateChecker() const = 0;

    virtual void setMainWindow(IMainWindow *) = 0;
    virtual IMainWindow *mainWindow() const = 0;

	virtual IPadTools *padTools() const { return 0; } // pad tools are not mandatory for everyone, so not pure virtual

    // Patient's datas wrapper
    virtual IPatient *patient() const = 0;
    virtual void setPatient(IPatient *patient) = 0;

    virtual IUser *user() const = 0;
    virtual void setUser(IUser *user) = 0;

//    virtual IFormManager *formManager() const = 0;



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

Q_SIGNALS:
    void coreAboutToOpen();
    void coreOpened();
    void saveSettingsRequested();
    void optionsDialogRequested();
    void coreAboutToClose();

    void databaseServerChanged();

protected:
    bool m_UpdatePreferences;
};

} // namespace Core

#endif // ICORE_H
