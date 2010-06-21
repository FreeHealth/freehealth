/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
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
#ifndef COREIMPL_H
#define COREIMPL_H

#include <coreplugin/icore.h>
#include <fdcoreplugin/patient.h>
#include <coreplugin/ipatient.h>

/**
 * \file coreimpl.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 12 Mar 2010
*/


namespace MedinTux {
class Configuration;
}

namespace Core {
    class Patient;

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

    
//    FormManager *formManager() const;
    FileManager *fileManager() const;

    // initialization
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    CommandLine *commandLine() const;

    Utils::UpdateChecker *updateChecker() const;

    // FreeDiams specific
    MedinTux::Configuration *medintuxConfiguration() const;

    // Patient's datas wrapper
    IPatient *patient() const {return m_Patient;}
    void setPatient(IPatient *) {}

    IUser *user() const {return m_User;}
    void setUser(IUser *user) {m_User=user;}


private:
    QSplashScreen *m_Splash;
    IMainWindow *m_MainWindow;
    ActionManagerPrivate *m_ActionManager;
    ContextManagerPrivate *m_ContextManager;
    UniqueIDManager *m_UID;
    ThemePrivate *m_Theme;
    Translators *m_Translators;
    SettingsPrivate *m_Settings;
//    FormManager *m_FormManager;
    CommandLine *m_CommandLine;
    mutable MedinTux::Configuration *m_MedinTux;
    Patient *m_Patient;
    IUser *m_User;
    Utils::UpdateChecker *m_UpdateChecker;
    Core::FileManager *m_FileManager;
};

} // namespace Internal
} // namespace Core

#endif // COREIMPL_H
