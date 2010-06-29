/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include <fdmainwindowplugin/medintux.h>
#include <fdmainwindowplugin/mainwindow.h>

#include <utils/global.h>
#include <utils/log.h>
#include <medintuxutils/configuration.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>

#include <drugsbaseplugin/constants.h>

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QFileDialog>

namespace MainWin {
namespace Internal {

    const char * const MEDINTUX_MENU_FILENAME      = "definition.txt";
    const char * const MEDINTUX_LIST_FILENAME      = "Editer avec FreeDiams.html";

    const char * const MEDINTUX_ACTION_SHORTITLE   = "FreeDiams";
    const char * const MEDINTUX_ACTION_TITLE       = QT_TRANSLATE_NOOP("diMedintux", "Prescribe with FreeDiams");
    const char * const MEDINTUX_ACTION_KEYSEQUENCE = "Ctrl+D+I";
    const char * const MEDINTUX_ACTION_SCRIPT
            = "{{::Nouveau Document (Prescription)}}\\\n"
              "{{::Intitule ( Ordonnance FreeDiams , change )}}\\\n"
              "{{::delete_file (../../tmp/DIexch.txt)}}\\\n"
              "{{::exe_process (WaitEnd,%1,"
              "--medintux,"
              "--exchange=\"$ToAbsPath../../tmp/DIexch.txt\","
              "--patientname=\"{{NOM PATIENT}}\","
              "--patientsurname=\"{{PRENOM PATIENT}}\","
              "--gender={{SEXE PATIENT}},"
              "--weight=\"{{POIDS PATIENT}} Kg\","
              "--dateofbirth=\"{{DATE NAISSANCE PATIENT}}\")}}\\\n"
              "{{::import (../../tmp/DIexch.txt , insert)}}\\\n";

    const char * const MEDINTUX_LISTFILE_CONTENT =
            "<html><head>\n<meta name=\"qrichtext\" content=\"1\" /></head>\n"
            "<body style=\"font-size:10pt;font-family:Arial\">"
            "<p>"
            "{{SAVE CURRENT DOC=../../tmp/DIexch.txt}}"
            "{{::exe_process (WaitEnd ,%1,"
            "--medintux,"
            "--exchange=\"$ToAbsPath../../tmp/DIexch.txt\","
            "--patientname=\"{{NOM PATIENT}}\","
            "--patientsurname=\"{{PRENOM PATIENT}}\","
            "--gender={{SEXE PATIENT}},"
            "--weight=\"{{POIDS PATIENT}} Kg\","
            "--dateofbirth=\"{{DATE NAISSANCE PATIENT}}\")}}"
            "{{::import (../../tmp/DIexch.txt , replace )}}"
            "{{::delete_file (../../tmp/DIexch.txt)}}"
            "{{::Intitule ( Ordonnance FreeDiams , change )}}"
            "</p>"
            "</body></html>";


    const void configureMedinTux()
    {
        QString finalMsg;
        bool error = false;
        bool ok = Utils::okCancelMessageBox(
                QCoreApplication::translate( "diMedinTux", "Automatic configuration for MedinTux usage"),
                QCoreApplication::translate( "diMedinTux", "This assistant will create a new entrie into the users' menu of DrTux.\n"
                                             "You can create a new prescription by clicking this menu.\n"
                                             "Please take care that if DrTux' location changes, "
                                             "or if FreeDiams' location changes, you must "
                                             "reconfigure DrTux with this assitant."),
                QCoreApplication::translate( "diMedinTux",
                                             "This assistant will try to find your MedinTux path. If it doesn't, you will be asked "
                                             "about this place. Then it will configure a new user's menu for DrTux and create "
                                             "a new list in your prescription's default list.\n"
                                             "Please refer to the documentation.")
                );
        if (!ok)
            return ;

        // Ask about the Glossary path of medintux
        MedinTux::Configuration *conf = MedinTux::Configuration::instance();
        QString dir = conf->findManagerBinaryPath();

        if (dir.isEmpty())
            dir = QFileDialog::getExistingDirectory(Core::ICore::instance()->mainWindow(),
                                                    QCoreApplication::translate( "diMedinTux", "Please select Manager's binary path"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (dir.isEmpty())
            error = true;
        else
            conf->setManagerBinaryPath(dir);

        QDir binDir(conf->drtuxBinaryPath());
        // Create DrTux User's Menu for freediams
        if (!binDir.cd("Ressources")) {
            Utils::Log::addError( "diCore", QCoreApplication::translate( "diMedinTux", "Unable to configure MedinTux : Resources path does not exists. Path to DrTux binary : %1").arg(binDir.path() ) );
            error = true;
        } else if (!binDir.cd("Menus")) {
            if (!binDir.mkdir( "Menus" ) ) {
                Utils::Log::addError( "diCore", QCoreApplication::translate( "diMedinTux", "Unable to configure MedinTux : Menus path does not exists and can not be created. Path to DrTux binary : %1").arg(binDir.path() ) );
                error = true;
            }
            binDir.cd("Menus");
        }

        // delete old drugsinteractions path
        /** \todo this part should be obsolete */
        if (binDir.exists("DrugsInteractions")) {
            QString error;
            if (!Utils::removeDir(binDir.absolutePath() + "/DrugsInteractions", &error))
                Utils::Log::addError("diCore",QCoreApplication::translate("diMedinTux", "Can not deleted old DrugsInteractions menu entry %1").arg(error));
        }
        /** end todo */

        if (!binDir.cd("FreeDiams")) {
            if (!binDir.mkdir("FreeDiams")) {
                Utils::Log::addError( "diCore", QCoreApplication::translate( "diMedinTux", "Unable to configure MedinTux : DrugsInteractions menu does not exists and can not be created. Path to DrTux binary : %1").arg(binDir.path() ) );
                error = true;
            }
            binDir.cd("FreeDiams");
        }

        // Add action to Menu
        QString script;
        if (Utils::isRunningOnMac()) {
            QString tmp = qApp->applicationFilePath();
            tmp = tmp.left( tmp.indexOf(".app/Contents/MacOS/") );
            script = QString(MEDINTUX_ACTION_SCRIPT).arg(tmp);
        } else if (Utils::isRunningOnWin()) {
            // removes '.exe' from application's file dir path
            QString z = QFileInfo(qApp->applicationFilePath()).baseName();
            z.prepend(qApp->applicationDirPath() + QDir::separator());
            script = QString(MEDINTUX_ACTION_SCRIPT).arg(QDir::cleanPath(z));
        } else {
            script = QString(MEDINTUX_ACTION_SCRIPT).arg(qApp->applicationFilePath());
        }

        if (!conf->addUserMenu( MEDINTUX_ACTION_SHORTITLE,
                                QCoreApplication::translate( "diMedinTux", MEDINTUX_ACTION_TITLE),
                                script,
                                Core::ICore::instance()->theme()->iconFullPath(DrugsDB::Constants::I_SEARCHMOLS),
                                MEDINTUX_ACTION_KEYSEQUENCE ) ) {
            Utils::Log::addMessage( "diMedinTux", QString("Unable to add user menu.") );
            finalMsg += "*** User Menu = WRONG\n";
            error = true;
        } else
            finalMsg += "User Menu = OK\n";


        // Create list menu for DrTux
        // 1. get manager.ini --> glossary path / default list for Prescription
        QString defaultList = conf->defaultListsPath(MedinTux::Configuration::PrescriptionDefaultList);

        // 2. get inside the dir (check it)
        if (!defaultList.isEmpty()) {
            script.clear();

            if (Utils::isRunningOnMac()) {
                QString tmp = qApp->applicationFilePath();
                tmp = tmp.left( tmp.indexOf(".app/Contents/MacOS/") );
                script = QString(MEDINTUX_LISTFILE_CONTENT).arg(tmp);
            } else {
                script = QString(MEDINTUX_LISTFILE_CONTENT).arg(qApp->applicationFilePath());
            }

            if (Utils::saveStringToFile(script,
                                        QString("%1/%2")
                                        .arg(defaultList,MEDINTUX_LIST_FILENAME),
                                        Utils::Overwrite, Utils::DontWarnUser )) {
                finalMsg += "Prescription List = OK\n";
            }

            // delete old drugsinteractions entry
            if (QDir(defaultList).exists("Editer avec DrugsInteractions.html"))
                QDir(defaultList).remove("Editer avec DrugsInteractions.html");

        } else {
            Utils::Log::addMessage( "diMedinTux", QString("Unable to find default prescription list : %1.")
                               .arg(defaultList) );
            finalMsg += "*** Prescription List = WRONG\n";
            error = true;
        }

        // 4. delete MedinTux cache
        if (conf->deleteListCache()) {
            finalMsg += "Cache Deletion = OK\n";
        } else {
            finalMsg += "*** Cache Deletion = need to be done by hand\n";
            //        error = true;
        }

        if (!error)
            Utils::informativeMessageBox( QCoreApplication::translate( "diMedinTux", "DrTux is now fully configured for %1." ).arg(qApp->applicationName()),
                                             QCoreApplication::translate( "diMedinTux", "You can run %1 using DrTux User's Menu." ).arg(qApp->applicationName()),
                                             QCoreApplication::translate( "diMedinTux", "If you encounter difficulties with DrTux integration, please refer to documentation of both applications.\n%1" ).arg(finalMsg) );
        else
            Utils::warningMessageBox( QCoreApplication::translate( "diMedinTux", "DrTux is not correctly configured." ),
                                         QCoreApplication::translate( "diMedinTux", "This assitant encountered errors during its process." ),
                                         QCoreApplication::translate( "diMedinTux", "This is the log of the processes \n%1" ).arg(finalMsg) );
    }

}  // End Internal
}  // End MainWin

