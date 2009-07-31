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
#ifndef TKMAINWINDOW_H
#define TKMAINWINDOW_H

#include <QMainWindow>
#include <QAction>

/**
 * \file tkMainWindow.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.5
 * \date 22 July 2009
*/

class tkMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum FileAction {
        A_FileNew    = 0x01,
        A_FileOpen   = 0x02,
        A_FileSave   = 0x04,
        A_FileSaveAs = 0x08,
        A_FileClose  = 0x10,
        A_FilePrint  = 0x11,
        A_FileQuit   = 0x88
    };
    Q_DECLARE_FLAGS( FileActions, FileAction );

    enum ConfigurationAction {
        A_AppPreferences     = 0x01,
        A_PluginsPreferences = 0x02,
        A_ConfigureMedinTux  = 0x04,
        A_LangageChange      = 0x08
    };
    Q_DECLARE_FLAGS( ConfigurationActions, ConfigurationAction );

    enum HelpAction {
        A_AppAbout     = 0x01,
        A_AppHelp      = 0x02,
        A_QtAbout      = 0x04,
        A_DebugDialog  = 0x08,
        A_PluginsAbout = 0x10,
        A_FormsAbout   = 0x20
    };
    Q_DECLARE_FLAGS( HelpActions, HelpAction );

    tkMainWindow(QWidget *parent = 0);
    ~tkMainWindow();

    void createFileMenu();
    void createFileActions(FileActions actions);
    void connectFileActions();

    void createEditMenu();
    void createEditActions();
    void connectEditActions();

    void createPluginsMenu();

    void createConfigurationMenu();
    void createConfigurationActions(ConfigurationActions actions);
    void connectConfigurationActions();

    void createHelpMenu();
    void createHelpActions(HelpActions actions);
    void connectHelpActions();

    void createFormatMenu();
    void createUpdateMenu();

    void createFormatActions();
    void createUpdateActions();

public Q_SLOTS: // Usefull slots that interactions with actions and menus
    virtual bool updateFound();


public Q_SLOTS: // Slots automatically to actions is requiered
    virtual bool newFile()     {return false;}
    virtual bool openFile()    {return false;}
    virtual bool saveFile()    {return false;}
    virtual bool saveAsFile()  {return false;}
    virtual bool print()       {return false;}

    virtual bool applicationPreferences() {return false;}
    virtual bool configureMedintux()      {return false;}

    virtual bool aboutApplication() {return false;}
    virtual bool applicationHelp();
    virtual bool aboutQt()          {return false;}
    virtual bool debugDialog()      {return false;}

public Q_SLOTS:
    virtual void switchLanguage(QAction * action);


protected:
    QAction *aNew;
    QAction *aOpen;
    QAction *aSave;
    QAction *aSaveAs;
    QAction *aPrint;
    QAction *aQuit;

    QAction *aUndo;
    QAction *aRedo;
    QAction *aCut;
    QAction *aCopy;
    QAction *aPaste;
    QAction *aSelectAll;

    QAction *aAppPrefs;
    QAction *aPlugsPrefs;
    QAction *aMedinTux;
    QActionGroup *aLanguageGroup;

    QAction *aAppAbout;
    QAction *aAppHelp;
    QAction *aQtAbout;
    QAction *aDebugDialog;

    QAction *aUpdateAvailable;

};
Q_DECLARE_OPERATORS_FOR_FLAGS( tkMainWindow::FileActions )
Q_DECLARE_OPERATORS_FOR_FLAGS( tkMainWindow::ConfigurationActions )
Q_DECLARE_OPERATORS_FOR_FLAGS( tkMainWindow::HelpActions )

#endif // TKMAINWINDOW_H
