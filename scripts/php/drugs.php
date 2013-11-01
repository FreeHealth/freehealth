<?php
/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
    // This file is part of the FreeDDIManager sub-project
    // With FreeDDIManager, users can send there updates to the FreeMedForms server.
    // These updates can involve:
    // - ATC codes
    // - Drug interactors (interacting classes or molecules)
    // - Drug-Drug interactions
    // - Potentially inappropriate medication for elderly (PIMs)
    // - Drug-Pregnancy interactions
    // - Drug-Breast feeding interactions
    //
    // FreeDDIManager formats the proposed update into a wiki usable text.
    // This text can be later downloaded and integrated to the FreeMedForms drugs database.
    //
    // Arguments:
    //    - type: type of proposed update:
    //        - new
    //        - update
    //    - subtype:
    //        - atc: ATC code
    //        - interactor: drug interactor
    //        - ddi: Drug-drug interaction
    //        - pim: PIM
    //    - msg: Base64 encoded message content
    //
    // Dokuwiki structure:
    // All proposals are stored as a wiki page in
    //      dokuwiki-data-path/en/proposals/$subtype/$type.txt
    //
    // This file reside in the following server path
    //      freemedforms/scripts/drugs.php
    // URL
    //      http://scripts.freemedforms.com/drugs.php
    //
    // See also: freeddimanager/plugins/ddiplugin/server/updatemanager.{h,cpp}

    $userName = $_POST['user'];
    $type = $_POST['type'];
    $subtype = $_POST['subtype'];
    $messageBase64 = $_POST['msg'];
    $messageBase64 = str_replace(' ', '+', $messageBase64);
    $message = base64_decode($messageBase64);
    $message = str_replace("{{~ServerDate~}}", date('l jS \of F Y h:i:s A'), $message);

    // create file name to receive the new message
    $fileName = getcwd() . '/../data/pages/en/proposals/' . $type . '/' . $subtype . '.txt';

    // file does not already exists -> create it
    if (!file_exists($fileName)) {
        touch($fileName);
    }

    // save the bug file to dokuwiki bugreport namespace
    $fp = fopen($fileName , "a");
    fwrite($fp , $message);
    fclose ($fp);

    $dokuwikiUrl = 'http://www.freemedforms.com/en/bugreport/bugreport' . $today;
    if (mail('eric.maeker@gmail.com', '[FreeMedForms - Bug report]', $dokuwikiUrl)) {
        echo 'Debug report ' . $dokuwikiUrl;
    } else {
        echo 'ERREUR';
    }
?>

