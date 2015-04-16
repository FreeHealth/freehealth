/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2015 by Eric MAEKER, MD                                      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREEMEDFORMS_H
#define FREEMEDFORMS_H

#include <coreplugin/actionmanager/mainwindowactionhandler.h>


//namespace Core {
//namespace Internal {
//class MainWindowActionHandler;
//} // end Internal
//} // end Core

// include toolkit headers
//#include <tkMainWindow.h>

// include freemedforms headers
//#include <mfExporter.h>
//#include <mfRecovererThread.h>
//#include <mfObject.h>
//#include <mfMainWidget.h>
//class mfSettings;

//include usertoolkit headers
//class tkUserManager;

// include Qt headers
#include <QCloseEvent>
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

/**
 * \file mainwindow.h
 * \author Eric MAEKER <eric.maeker@gmail.com>, Guillaume Denry
 * \version 0.0.12
 * \date 09 Aug 2009
*/

namespace Core {

class CORE_EXPORT MainWindow: public Core::Internal::MainWindowActionHandler
{
//     friend class mfCore;
//     friend class mfSettings;
     Q_OBJECT
     enum { MaxRecentFiles = 10 };

public:
     QStatusBar *statusBar();

//public Q_SLOTS:
//     bool updateFound() { return tkMainWindow::updateFound(); }

     MainWindow(QWidget *parent = 0);
     bool initialize();
     bool extensionInitialized();
     ~MainWindow();
//     void closeEvent( QCloseEvent *event );

     void afterTheShow();

     void createActions();
     void createMenusAndActions();
     void createToolBars();
     void createStatusBar();
     void retranslateUi();

     void aboutPlugins();

 private Q_SLOTS:
     bool aboutQt();

//private Q_SLOTS:
//     // AutoConnect via tkActionManager
//     bool newFile();
//     bool openFile();
//     bool saveFile();
//     bool saveAsFile();
//     bool print();
//
//     bool aboutApplication();
//     bool debugDialog();
//     bool aboutThisForm();
//
//     bool applicationHelp() { return tkMainWindow::applicationHelp(); }
////     void switchLanguage(QAction * action) { tkMainWindow::switchLanguage(action); }
//
//     void on_actionHelpTextToggler_triggered(); // helpTextHideShow();
//     void on_actionInterpretor_triggered();  // showInterpretation();
//     void userManagerRequested();  // showUserManager();
//
//     void refreshToolbarMenu();
//
//     void documentWasModified();
//     void openRecentFile();
//     void refreshWholeUi();
//     void formEditor();
//     void toolbarToggled(bool state);

     // DEBUGGING FOCUS
//     void warnFocus(QWidget *old, QWidget *now) {qWarning() << old << now ; }
//     // END
//
//     void saveFormData(); // TEMPORARY, JUST FOR TESTS OF FORM DATA ROUTINES
//     void loadFormData(); // TEMPORARY, JUST FOR TESTS OF FORM DATA ROUTINES
//
//private:
//     void timerEvent(QTimerEvent *event);
//     void changeEvent( QEvent * event );
//     void showWidget();
//     void readSettings();
//     void writeSettings();
//     bool maybeSave();
//     void loadFile( const QString &fileName );
//     bool saveFile( const QString &fileName );
//     void setCurrentFile( const QString &fileName );
//     void updateRecentFileActions();
//     QString strippedName( const QString &fullFileName );
//     void createLanguageMenu(QMenu *menu);


public:
     QToolBar *fileToolBar;
     QToolBar *editToolBar;

     QAction *recentFilesAct[MaxRecentFiles];
     QAction *aUserManager;

     QStringList recentFiles;

//     mfObject *        m_pRootObject;
//     mfMainWidget *    m_MainWidget;
     QString           m_CurrentFile;
     bool              m_HelpTextShow;
     uint              m_AutomaticSaveInterval;   /*!< Interval between each automatic save in SECONDS */
     int               m_TimerId;
//     mfRecovererThread thread;
     bool              m_OpenLastOpenedForm;
     QByteArray        windowState;
//     QPointer<tkUserManager> m_UserManager;
};

} // End Core

#endif
