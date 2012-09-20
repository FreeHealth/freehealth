/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MAINWINDOWACTIONHANDLER_H
#define MAINWINDOWACTIONHANDLER_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QMainWindow>
#include <QAction>

/**
 * \file MainWindowActionHandler.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 06 Nov 2011
*/

namespace Utils {
class UpdateChecker;
}

namespace Core {
    class MainWindowActions;
    class ActionContainer;

namespace Internal {

class CORE_EXPORT MainWindowActionHandler : public QMainWindow
{
    Q_OBJECT
public:
    MainWindowActionHandler(QWidget *parent = 0);
    ~MainWindowActionHandler();

    void createGeneralMenu();
    void connectGeneralActions();

    Core::ActionContainer *menubarContainer(bool createIfNotExist = false);

    void createFileMenu();
    void createFileNewSubMenu();
    void connectFileActions();

    void createPatientMenu();
    void createPatientsActions(int actions);
    void connectPatientActions();

    void createEditMenu();
    void createEditActions();
    void connectEditActions();

    void createPluginsMenu();

    void createConfigurationMenu();
    void connectConfigurationActions();

    void createHelpMenu();
    void connectHelpActions();

    void createTemplatesMenu();

    void createActions(const Core::MainWindowActions &actions);
    void createGeneralActions(const int actions);
    void createFileActions(int actions);
    void createConfigurationActions(int actions);
    void createHelpActions(int actions);
    void createTemplatesActions(int actions);

    void createFormatMenu();
    void createUpdateMenu();

    void createFormatActions();
    void createUpdateActions();

public Q_SLOTS: // useful slots that interactions with actions and menus
    virtual bool updateFound();
    virtual void updateCheckerEnd(bool error);


public Q_SLOTS: // Slots automatically connected to actions is requiered
    virtual bool newFile()      {return false;}
    virtual bool openFile()     {return false;}
    virtual bool saveFile()     {return false;}
    virtual bool saveAsFile()   {return false;}
    virtual bool print()        {return false;}
    virtual bool printPreview() {return false;}

    virtual bool createNewPatient() {return false;}
    virtual bool viewPatientIdentity() {return false;}
    virtual bool removePatient() {return false;}

    virtual bool applicationPreferences();
    virtual bool applicationConfiguratorWizard() {return false;}
    virtual bool configureMedintux()      {return false;}

    virtual bool aboutApplication();
    virtual bool aboutPlugins();
    virtual bool applicationHelp();
    virtual bool aboutQt()          {return false;}
    virtual bool debugDialog();
    virtual bool checkUpdate();
    virtual void goToAppWebSite();

public Q_SLOTS:
    virtual void switchLanguage(QAction * action);


protected:
    QAction *aGeneralNew, *aGeneralOpen, *aGeneralSave, *aGeneralSaveAs, *aGeneralPrint, *aGeneralPrintPreview, *aGeneralQuit;
    QAction *aGeneralPatientNew, *aGeneralPatientViewIdentity, *aGeneralPatientRemove;
    QAction *aGeneralUndo, *aGeneralRedo, *aGeneralCut, *aGeneralCopy, *aGeneralPaste, *aGeneralSelectAll;
    QAction *aGeneralAppPrefs, *aGeneralAppConfigurator, *aGeneralPlugsPrefs, *aGeneralMedinTux;
    QAction *aGeneralAppAbout, *aGeneralPlugsAbout, *aGeneralAppHelp, *aGeneralQtAbout, *aGeneralDebugDialog;
    QAction *aGeneralCheckUpdate, *aGeneralUpdateAvailable;

    QAction *aNew;
    QAction *aOpen;
    QAction *aSave;
    QAction *aSaveAs;
    QAction *aPrint;
    QAction *aPrintPreview;
    QAction *aQuit;

    QAction *aPatientNew;
    QAction *aPatientViewIdentity;
    QAction *aPatientRemove;

    QAction *aUndo;
    QAction *aRedo;
    QAction *aCut;
    QAction *aCopy;
    QAction *aPaste;
    QAction *aSelectAll;
    QAction *aSearch;

    QAction *aAppPrefs;
    QAction *aAppConfigurator;
    QAction *aPlugsPrefs;
    QAction *aMedinTux;
    QActionGroup *aLanguageGroup;

    QAction *aAppAbout;
    QAction *aAppGoToWebSite;
    QAction *aPlugsAbout;
    QAction *aAppHelp;
    QAction *aQtAbout;
    QAction *aDebugDialog;

    QAction *aCheckUpdate;
    QAction *aUpdateAvailable;

    Utils::UpdateChecker *m_UpdateChecker;

};

}  // end Internal
}  // end Core

#endif // MAINWINDOWACTIONHANDLER_H
