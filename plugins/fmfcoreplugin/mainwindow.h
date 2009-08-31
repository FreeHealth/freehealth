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
#ifndef FREEMEDFORMSMAINWINDOW_H
#define FREEMEDFORMSMAINWINDOW_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/imainwindow.h>

// include Qt headers
#include <QCloseEvent>
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

/**
 * \file mainwindow.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.12
 * \date 09 Aug 2009
*/

namespace Core {

class CORE_EXPORT MainWindow: public Core::IMainWindow
{
    Q_OBJECT
    enum { MaxRecentFiles = 10 };

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // IMainWindow Interface
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

    QStatusBar *statusBar();

    void afterTheShow();

    void createMenusAndActions();
    void createToolBars();
    void createStatusBar();
    void retranslateUi();

public Q_SLOTS: // Interface of MainWidowActionHandler
//    virtual bool newFile()     {return false;}
    bool openFile();
//    virtual bool saveFile()    {return false;}
//    virtual bool saveAsFile()  {return false;}
//    virtual bool print()       {return false;}
//
    bool applicationPreferences();
//    virtual bool configureMedintux()      {return false;}
//
//    virtual bool aboutApplication();
//    virtual bool applicationHelp();
    bool aboutQt();
    void aboutPlugins();


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

} // End FMFMainWindow

#endif  // FREEMEDFORMSMAINWINDOW_H
