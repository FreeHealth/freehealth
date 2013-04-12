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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MAINWINDOWACTIONS_H
#define MAINWINDOWACTIONS_H

#include <coreplugin/core_exporter.h>

#include <QFlags>

/**
 * \file mainwindowactions.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 10 Apr 2013
*/

namespace Core {

/** Use this class to pass information on the desired actions to be created by the
   Core::IMainWindow.

   The general menu accept all actions.

   \code
       class MyMainWindow : public IMainWindow { .... }

       void MyMainWindow::createActionsInHandler()
       {
           Core::MainWindowActions actions;
           actions.setFileActions( Core::MainWindowActions::A_FileNew );
           actions.setHelpActions( Core::MainWindowActions::A_AppAbout );
           IMainWindow::createActions(actions);
       }
   \endcode
*/
class CORE_EXPORT MainWindowActions
{
public:
    enum FileAction {
        A_FileNew                = 0x00000001,
        A_FileOpen               = 0x00000002,
        A_FileSave               = 0x00000004,
        A_FileSaveAs             = 0x00000008,
        A_FileClose              = 0x00000010,
        A_FilePrint              = 0x00000020,
        A_FilePrintPreview       = 0x00000040,
        A_FileQuit               = 0x00000080
    };
    enum ConfigurationAction {
        A_AppPreferences         = 0x00000100,
        A_PluginsPreferences     = 0x00000200,
        A_ConfigureMedinTux      = 0x00000400,
        A_LanguageChange          = 0x00000800,
        A_AppConfigurator        = 0x00001000
    };
    enum HelpAction {
        A_AppAbout               = 0x00002000,
        A_AppHelp                = 0x00004000,
        A_QtAbout                = 0x00008000,
        A_DebugDialog            = 0x00010000,
        A_PluginsAbout           = 0x00020000,
        A_FormsAbout             = 0x00040000,
        A_CheckUpdate            = 0x00080000,
        A_AppGoToWebSite         = 0x00100000
    };
    enum TemplatesAction {
        A_Templates_New          = 0x00200000,
        A_Templates_Manager      = 0x00400000,
        A_Templates_ToogleViewer = 0x00800000
    };
    enum PatientsAction {
        A_Patients_New           = 0x08000000,
        A_Patients_ViewIdentity  = 0x10000000,
        A_Patients_Remove        = 0x20000000
    };

    Q_DECLARE_FLAGS( FileActions, FileAction )
    Q_DECLARE_FLAGS( ConfigurationActions, ConfigurationAction )
    Q_DECLARE_FLAGS( HelpActions, HelpAction )
    Q_DECLARE_FLAGS( TemplatesActions, TemplatesAction )
    Q_DECLARE_FLAGS( PatientsActions, PatientsAction )

    MainWindowActions() :
        general(0),
        file(0),
        config(0),
        help(0),
        templates(0),
        patients(0),
        edit(false)
    {
    }

    ~MainWindowActions() {}

    void setGeneralActions(int actions) { general = actions; }
    void setFileActions(int actions) { file = actions; }
    void createEditActions(bool yesOrNO) { edit = yesOrNO; }
    void setConfigurationActions(int actions) { config = actions; }
    void setHelpActions(int actions) { help = actions; }
    void setTemplatesActions(int actions) { templates = actions; }
    void setPatientsActions(int actions) {patients = actions;}

    int generalActions() const { return general; }
    int fileActions() const { return file; }
    bool editActionsToCreate() const { return edit; }
    int configurationActions() const { return config; }
    int helpActions() const { return help; }
    int templatesActions() const { return templates; }
    int patientsActions() const {return patients;}

private:
    int general;
    int file;
    int config;
    int help;
    int templates;
    int patients;
    bool edit;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( MainWindowActions::FileActions )
Q_DECLARE_OPERATORS_FOR_FLAGS( MainWindowActions::ConfigurationActions )
Q_DECLARE_OPERATORS_FOR_FLAGS( MainWindowActions::HelpActions )
Q_DECLARE_OPERATORS_FOR_FLAGS( MainWindowActions::PatientsActions )

}  // end Core

#endif // MAINWINDOWACTIONS_H
