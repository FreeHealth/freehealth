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
#ifndef ICORE_H
#define ICORE_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>
class QSplashScreen;
class QPixmap;
class QWidget;
class QString;

/**
 * \file icore.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.3.0
 * \date 28 Jan 2010
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
class FormManager;
class CommandLine;
class Patient;
class FileManager;
}

namespace Core {

class CORE_EXPORT ICore : public QObject
{
    Q_OBJECT

public:
    ICore(QObject *parent) : QObject(parent) {}
    virtual ~ICore() {}

    static ICore *instance();


    // Splash screen functions
    virtual void createSplashScreen(const QPixmap &pix) = 0;
    virtual void finishSplashScreen(QWidget *w) = 0;
    virtual void messageSplashScreen(const QString &msg) = 0;
    virtual QSplashScreen *splashScreen() = 0;


    virtual ActionManager *actionManager() const = 0;
    virtual ContextManager *contextManager() const = 0;
    virtual UniqueIDManager *uniqueIDManager() const = 0;

    virtual ITheme *theme() const = 0;
    virtual Translators *translators() const = 0;

    virtual ISettings *settings() const = 0;

    virtual IMainWindow *mainWindow() const = 0;

    virtual CommandLine *commandLine() const = 0;

    virtual Utils::UpdateChecker *updateChecker() const = 0;

    virtual void setMainWindow(IMainWindow *) = 0;

    // Use this with precaution (only used by FreeDiams)
    virtual Patient *patient() const {return 0;}

    virtual FormManager *formManager() const = 0;

    virtual FileManager *fileManager() const = 0;

//    virtual MessageManager *messageManager() const = 0;
//    virtual EditorManager *editorManager() const = 0;
//    virtual ProgressManager *progressManager() const = 0;
//    virtual ScriptManager *scriptManager() const = 0;
//    virtual VariableManager *variableManager() const = 0;
//    virtual VCSManager *vcsManager() const = 0;
//    virtual ModeManager *modeManager() const = 0;
//    virtual MimeDatabase *mimeDatabase() const = 0;
//
//    virtual QSettings *settings() const = 0;
//    virtual SettingsDatabase *settingsDatabase() const = 0;
//    virtual QPrinter *printer() const = 0;
//
//    virtual QString resourcePath() const = 0;

    void coreIsAboutToClose() { Q_EMIT coreAboutToClose(); }
    void requestSaveSettings() { Q_EMIT saveSettingsRequested(); }

Q_SIGNALS:
    void coreAboutToOpen();
    void coreOpened();
    void saveSettingsRequested();
    void optionsDialogRequested();
    void coreAboutToClose();
};

} // namespace Core

#endif // ICORE_H
