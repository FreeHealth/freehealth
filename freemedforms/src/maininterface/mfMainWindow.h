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
 *       Guillaume DENRY <guillaume.denry@gmail.com>                       *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef FREEMEDFORMS_H
#define FREEMEDFORMS_H

// include freemedforms headers
#include <mfExporter.h>
#include <mfRecovererThread.h>
#include <mfObject.h>
#include <mfMainWidget.h>
class mfSettings;

//include usertoolkit headers
class tkUserManager;

// include Qt headers
#include <QMainWindow>
#include <QCloseEvent>
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

/**
 * \file mfMainWindow.h
 * \author Eric MAEKER <eric.maeker@free.fr>, Guillaume Denry
 * \version 0.0.7
 * \date 07 Jun 2009
*/

class Q_FMF_EXPORT mfMainWindow: public QMainWindow
{
     friend class mfCore;
     friend class mfSettings;
     Q_OBJECT
     enum { MaxRecentFiles = 10 };

public:
     QStatusBar *statusBar();

protected:
     mfMainWindow( QWidget * parent = 0 );
     ~mfMainWindow();
     void closeEvent( QCloseEvent *event );

private Q_SLOTS:
     // AutoConnect via tkActionManager
     void on_actionFileNew_triggered();      // newFile();
     void on_actionFileOpen_triggered();     // open();
     bool on_actionFileSave_triggered();     // save();
     bool on_actionFileSaveAs_triggered();   // saveAs();
     bool on_actionFilePrint_triggered();    // print();
     void on_actionFileExit_triggered()      { close(); }
     void on_actionAbout_triggered();        // about();
     void on_actionHelpTextToggler_triggered(); // helpTextHideShow();
     void on_actionDebugHelper_triggered();  // displayDebugDialog();
     void on_actionAboutThisForm_triggered(); // aboutThisForm();
     void on_actionInterpretor_triggered();  // showInterpretation();
     void on_actionUserManager_triggered();  // showUserManager();

     void on_menuToolbars_aboutToShow();     // refreshToolbarMenu();

     void documentWasModified();
     void openRecentFile();
     void switchLanguage( QAction * action );
     void refreshWholeUi();
     void formEditor();
     void toolbarToggled(bool state);
     void afterTheShow();

     // DEBUGGING FOCUS
     void warnFocus(QWidget *old, QWidget *now) {qWarning() << old << now ; }
     // END

     void saveFormData(); // TEMPORARY, JUST FOR TESTS OF FORM DATA ROUTINES
     void loadFormData(); // TEMPORARY, JUST FOR TESTS OF FORM DATA ROUTINES

private:
//     void addToMenu( QMenu * menu, QAction * action, QAction * before = 0 );

     void timerEvent(QTimerEvent *event);
     void changeEvent( QEvent * event );
     void showWidget();
     void createActions();
     void createMenusAndActions();
     void createToolBars();
     void createStatusBar();
     void readSettings();
     void writeSettings();
     bool maybeSave();
     void loadFile( const QString &fileName );
     bool saveFile( const QString &fileName );
     void setCurrentFile( const QString &fileName );
     void updateRecentFileActions();
     QString strippedName( const QString &fullFileName );
     void retranslateUi();
//     void createToolsMenu();
     void createLanguageMenu(QMenu *menu);
//     void createConfigurationMenu();

     QMenu *fileMenu;
     QMenu *editMenu;
     QMenu *toolbarMenu;
     QMenu *interpretMenu;
     QMenu *configMenu;
     QMenu *languageMenu;
     QMenu *helpMenu;

     QToolBar *fileToolBar;
     QToolBar *editToolBar;
     QActionGroup *languageActionGroup;

     QAction *recentFilesAct[MaxRecentFiles],
      *cutAct, *copyAct, *pasteAct,
      *previousAct, *nextAct;

     QStringList recentFiles;

     mfObject *        m_pRootObject;
     mfMainWidget *    m_MainWidget;
     QString           m_CurrentFile;
     bool              m_HelpTextShow;
     uint              m_AutomaticSaveInterval;   /*!< Interval between each automatic save in SECONDS */
     int               m_TimerId;
     mfRecovererThread thread;
     bool              m_OpenLastOpenedForm;
     QByteArray        windowState;
     QPointer<tkUserManager> m_UserManager;
};

#endif
