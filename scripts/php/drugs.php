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

    $localTest = true;

    $userName = $_GET['user'];
    $type = $_GET['type'];
    $subtype = $_GET['subtype'];
    $messageBase64 = $_GET['msg'];
    $messageBase64 = str_replace(' ', '+', $messageBase64);
    $message = base64_decode($messageBase64);
    $message = str_replace("{{~ServerDate~}}", date('l jS \of F Y h:i:s A'), $message);

    // create file name to receive the new message
    if ($localTest)
        $fileName = getcwd() . '/../perso_bkup/data/pages/en/drugs/proposals/' . $type;
    else
        $fileName = getcwd() . '/../dokuwiki/data/pages/en/drugs/proposals/' . $type . '/' . $subtype . '.txt';

    // Ensure path is available
    mkdir($fileName, 0777, true);

    // file does not already exists -> create it
    $fileName .= '/' . $subtype . '.txt';
    if (!file_exists($fileName)) {
        touch($fileName);
    }

    if (is_writable($fileName)) {
        // open the file (append)
        if (!$handle = fopen($fileName, 'a')) {
             echo "Unable to open file: $fileName";
             exit;
        }

        // write message content
        if (fwrite($handle, $message) === FALSE) {
            echo "Unable to write in file: $fileName";
            exit;
        }

        // Echo success & close file
        echo "Message stored in file: $fileName";
        fclose($handle);
    } else {
        echo "File $fileName is not accessible in write mode";
    }

    // Create mail content
    if ($localTest) {
        $to      = 'eric.maeker@gmail.com';
        $dokuwikiUrl = 'http://localhost/~eric/perso_bkup/doku.php?id=en/drugs/proposals/' . $type . '/' . $subtype;
    } else {
        $to      = 'freemedforms-dev@googlegroups.com';
        $dokuwikiUrl = 'http://www.freemedforms.com/en/drugs/proposals/' . $type . '/' . $subtype;
    }
    $subject = '[Drug Proposal] ' . $type . ' / ' . $subtype;
    $mail    = "Please refer to the following URL:";
    $mail   .= "\r\n";
    $mail   .= "    " . $dokuwikiUrl;
    $mail   .= "\r\n";
    $mail   .= "\r\n";
    $mail   .= "The FreeMedForms team, http://freemedforms.com";
    $headers = 'From: eric.maeker@gmail.com' . "\r\n" .
    'Reply-To: freemedforms-dev@googlegroups.com' . "\r\n" .
    'X-Mailer: PHP/' . phpversion();

    if (mail($to, $subject, $mail, $headers)) {
        echo '<br><br>Proposals URL:<br>     ' . $dokuwikiUrl;
    } else {
        echo '<br><br>Proposal error';
    }
?>

