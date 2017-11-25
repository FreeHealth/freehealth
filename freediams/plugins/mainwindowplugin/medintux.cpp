/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "medintux.h"
#include "mainwindow.h"

#include <utils/global.h>
#include <utils/log.h>
#include <medintuxutils/configuration.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

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
              "--exchange-out=\"$ToAbsPath../../tmp/DIexch.txt\","
              "--patientname=\"{{NOM PATIENT}}\","
              "--patientfirstname=\"{{PRENOM PATIENT}}\","
              "--gender={{SEXE PATIENT}},"
              "--weight=\"{{POIDS PATIENT}} kg\","
              "--dateofbirth=\"{{DATE NAISSANCE PATIENT}}\")}}\\\n"
              "{{::import (../../tmp/DIexch.txt , insert)}}\\\n";

    const char * const MEDINTUX_LISTFILE_CONTENT =
            "<html><head>\n<meta name=\"qrichtext\" content=\"1\" /></head>\n"
            "<body style=\"font-size:10pt;font-family:Arial\">"
            "<p>"
            "{{SAVE CURRENT DOC=../../tmp/DIexch.txt}}"
            "{{::exe_process (WaitEnd ,%1,"
            "--medintux,"
            "--exchange-out=\"$ToAbsPath../../tmp/DIexch.txt\","
            "--patientname=\"{{NOM PATIENT}}\","
            "--patientfirstname=\"{{PRENOM PATIENT}}\","
            "--gender={{SEXE PATIENT}},"
            "--weight=\"{{POIDS PATIENT}} kg\","
            "--dateofbirth=\"{{DATE NAISSANCE PATIENT}}\")}}"
            "{{::import (../../tmp/DIexch.txt , replace )}}"
            "{{::delete_file (../../tmp/DIexch.txt)}}"
            "{{::Intitule ( Ordonnance FreeDiams , change )}}"
            "</p>"
            "</body></html>";


    void configureMedinTux()
    {
        QString finalMsg;
        bool error = false;
        bool ok = Utils::okCancelMessageBox(
                QCoreApplication::translate("diMedinTux", "Automatic configuration for MedinTux usage"),
                QCoreApplication::translate("diMedinTux", "This assistant will create a new entry into the users' menu of DrTux.\n"
                                            "You can create a new prescription by clicking this menu.\n"
                                            "Please take care that if DrTux' location changes, "
                                            "or if FreeDiams' location changes, you must "
                                            "reconfigure DrTux with this assistant."),
                QCoreApplication::translate("diMedinTux",
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
                                                    QCoreApplication::translate("diMedinTux", "Please select Manager's binary path"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (dir.isEmpty()) {
            finalMsg += QCoreApplication::translate("diMedinTux", "Unable to find Manager binary path.");
            error = true;
        } else {
            conf->setManagerBinaryPath(dir);
        }

        QDir binDir(conf->drtuxBinaryPath());
        // Create DrTux User's Menu for freediams
        if (!binDir.cd("Ressources")) {
            Utils::Log::addError("diCore", QCoreApplication::translate("diMedinTux", "Unable to configure MedinTux : Resources path does not exists. Path to DrTux binary : %1").arg(binDir.path()),
                                  __FILE__, __LINE__);
            finalMsg += QCoreApplication::translate("diMedinTux", "Unable to configure MedinTux : Resources path does not exists. Path to DrTux binary : %1").arg(binDir.path()) + "\n";
            error = true;
        } else if (!binDir.cd("Menus")) {
            if (!binDir.mkdir( "Menus" ) ) {
                Utils::Log::addError("diCore", QCoreApplication::translate("diMedinTux", "Unable to configure MedinTux : Menus path does not exists and can not be created. Path to DrTux binary : %1").arg(binDir.path()),
                                      __FILE__, __LINE__);
                finalMsg += QCoreApplication::translate("diMedinTux", "Unable to configure MedinTux : Menus path does not exists and can not be created. Path to DrTux binary : %1").arg(binDir.path()) + "\n";
                error = true;
            }
            binDir.cd("Menus");
        }

        // delete old drugsinteractions path
        // TODO: this part should be obsolete */
        if (binDir.exists("DrugsInteractions")) {
            QString error;
            if (!Utils::removeDir(binDir.absolutePath() + "/DrugsInteractions", &error))
                Utils::Log::addError("diCore",QCoreApplication::translate("diMedinTux", "Can not deleted old DrugsInteractions menu entry %1").arg(error),
                                     __FILE__, __LINE__);
        }
        /** end todo */

        if (!binDir.cd("FreeDiams")) {
            if (!binDir.mkdir("FreeDiams")) {
                Utils::Log::addError( "diCore", QCoreApplication::translate("diMedinTux", "Unable to configure MedinTux: FreeDiams menu does not exists and can not be created. Path to DrTux binary : %1").arg(binDir.path()),
                                      __FILE__, __LINE__);
                finalMsg += QCoreApplication::translate("diMedinTux", "Unable to configure MedinTux: FreeDiams menu does not exists and can not be created. Path to DrTux binary : %1").arg(binDir.path());
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

        if (!conf->addUserMenu(MEDINTUX_ACTION_SHORTITLE,
                               QCoreApplication::translate( "diMedinTux", MEDINTUX_ACTION_TITLE),
                               script,
                               Core::ICore::instance()->theme()->iconFullPath(Core::Constants::ICONFREEDIAMS),
                               MEDINTUX_ACTION_KEYSEQUENCE)) {
            LOG_FOR("diMedinTux", QString("Unable to add user menu.") );
            finalMsg += QCoreApplication::translate("diMedinTux", "Unable to configure MedinTux user menu.") + "\n";
            error = true;
        } else {
            finalMsg += QCoreApplication::translate("diMedinTux", "MedinTux user's menu correctly updated.") + "\n";
        }


        // Create list menu for DrTux
        // 1. get manager.ini --> glossary path / default list for Prescription
        QString defaultList;
        defaultList = conf->menuListsPath(MedinTux::Configuration::PrescriptionDefaultList);
        if (defaultList.isEmpty())
            defaultList = conf->glossaryPath(MedinTux::Configuration::PrescriptionGlossary);

        if (defaultList.isEmpty()) {
            defaultList = QFileDialog::getExistingDirectory(qApp->activeWindow(),
                                                            QCoreApplication::translate("diMedinTux", "Select path to the Prescription Menu."),
                                                            QDir::cleanPath(QFileInfo(conf->managerIniFileName()).absolutePath() + "/../../"));
            finalMsg += QCoreApplication::translate("diMedinTux", "Can not find menu list directory. Asking user.") + "\n";
        }

        // 2. get inside the dir (check it)
        if (!defaultList.isEmpty()) {
            finalMsg += QCoreApplication::translate("diMedinTux", "Using %1 as prescription menu directory.").arg(defaultList) + "\n";
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
                finalMsg += QCoreApplication::translate("diMedinTux", "Prescription menu correctly updated.") + "\n";
            }

            // delete old drugsinteractions entry
            if (QDir(defaultList).exists("Editer avec DrugsInteractions.html"))
                QDir(defaultList).remove("Editer avec DrugsInteractions.html");

        } else {
            LOG_FOR("diMedinTux",
                    QCoreApplication::translate("diMedinTux", "Unable to find the prescription list directory.")
                    .arg(defaultList));
            finalMsg += QCoreApplication::translate("diMedinTux", "Unable to find the prescription list directory.");
            error = true;
        }

        // 4. delete MedinTux cache
        if (conf->deleteListCache()) {
            finalMsg += QCoreApplication::translate("diMedinTux", "MedinTux cache has been correctly cleared.");
        } else {
            finalMsg += QCoreApplication::translate("diMedinTux", "Unable to clear the MedinTux cache.\nYou have to do this by hand.");
            //        error = true;
        }

        if (!error) {
            Utils::informativeMessageBox(QCoreApplication::translate("diMedinTux", "DrTux is now fully configured for %1.").arg(qApp->applicationName()),
                                         QCoreApplication::translate("diMedinTux", "You can run %1 using DrTux' user menu.").arg(qApp->applicationName()),
                                         QCoreApplication::translate("diMedinTux", "If you encounter difficulties with DrTux integration, please refer to documentation of both applications.\n%1").arg(finalMsg));
        } else {
            Utils::warningMessageBox(QCoreApplication::translate("diMedinTux", "DrTux is not correctly configured."),
                                     QCoreApplication::translate("diMedinTux", "This assistant encountered errors during its process."),
                                     QCoreApplication::translate("diMedinTux", "This is the log of the processes \n%1").arg(finalMsg));
        }
    }

}  // End Internal
}  // End MainWin

