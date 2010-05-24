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
#ifndef MAINWINDOWACTIONS_H
#define MAINWINDOWACTIONS_H

#include <coreplugin/core_exporter.h>

#include <QFlags>

/**
 * \file mainwindowactions.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.6
 * \date 13 Aug 2009
*/

namespace Core {

/** Use this class to pass informations on the desired actions to be created by the
   Core::IMainWindow
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
        A_FileNew    = 0x01,
        A_FileOpen   = 0x02,
        A_FileSave   = 0x04,
        A_FileSaveAs = 0x08,
        A_FileClose  = 0x10,
        A_FilePrint  = 0x20,
        A_FilePrintPreview = 0x40,
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
        A_FormsAbout   = 0x20,
        A_CheckUpdate  = 0x40
    };
    Q_DECLARE_FLAGS( HelpActions, HelpAction );

    enum TemplatesAction {
        A_Templates_New          = 0x01,
        A_Templates_Manager      = 0x02,
        A_Templates_ToogleViewer = 0x04
    };
    Q_DECLARE_FLAGS( TemplatesActions, TemplatesAction );

    enum PatientsAction {
        A_Patients_New            = 0x01,
        A_Patients_ViewIdentity   = 0x02,
        A_Patients_Remove         = 0x04
    };
    Q_DECLARE_FLAGS( PatientsActions, PatientsAction );

    MainWindowActions() : file(0), config(0), help(0), templates(0), edit(false) {}
    ~MainWindowActions() {}

    void setFileActions(int actions) { file = actions; }
    void createEditActions(bool yesOrNO) { edit = yesOrNO; }
    void setConfigurationActions(int actions) { config = actions; }
    void setHelpActions(int actions) { help = actions; }
    void setTemplatesActions(int actions) { templates = actions; }
    void setPatientsActions(int actions) {patients = actions;}

    int fileActions() const { return file; }
    bool editActionsToCreate() const { return edit; }
    int configurationActions() const { return config; }
    int helpActions() const { return help; }
    int templatesActions() const { return templates; }
    int patientsActions() const {return patients;}

private:
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
