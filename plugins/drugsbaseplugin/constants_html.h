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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSBASE_CONSTANTS_HTML_H
#define DRUGSBASE_CONSTANTS_HTML_H

/**
 * \file constants.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.6
 * \date 12 May 2013
*/

namespace DrugsDB {
namespace Constants {

const char * const S_DEF_ALD_PRE_HTML =
        "<table border=\"1\" width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" style=\"border-style:solid;\">\n"
        "  <tr>\n"
        "    <td width=\"100%\">\n"
        "      <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;font-family:Arial;font-size:10pt;color:black;\">Prescriptions relatives au traitement de l'affection de longue dur&eacute;e reconnue (liste ou hors liste)<br />(AFFECTION EXON&Eacute;RANTE)</span>\n"
        "      </p>\n"
        "    </td>\n"
        "  </tr>\n"
        "</table>\n";
const char * const S_DEF_ALD_POST_HTML =
        "<table border=\"1\" width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" style=\"border-style:solid;\">\n"
        "  <tr>\n"
        "    <td width=\"100%\">\n"
        "      <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;font-family:Arial;font-size:10pt;color:black;\">Prescriptions SANS RAPPORT avec l'affection de longue dur&eacute;e<br />(MALADIES INTERCURRENTES)</span>\n"
        "      </p>\n"
        "    </td>\n"
        "  </tr>\n"
        "</table>\n";

// MASKS
const char * const  PROTOCOL_AUTOMATIC_LABEL_MASK =
        #ifdef WITH_PAD
        "{{~Prescription.Protocol.Quantity.Full~}}"//  "[[Q_FROM]][-[Q_TO]] [[Q_SCHEME]]"
        "{{~Prescription.Protocol.DailyScheme.Repeated~}}{{ ~Prescription.Protocol.Meal~}}{{ ~Prescription.Protocol.Period.Full~}}"   // "[ [REPEATED_DAILY_SCHEME]][ [MEAL]][ [[PERIOD] / ][PERIOD_SCHEME]]"
        "{{; ~Prescription.Protocol.Duration.Full~}}{{~Prescription.Protocol.DailyScheme.Distributed~}}"   // "[; [D_FROM]][-[D_TO]][ [D_SCHEME]][; [DISTRIBUTED_DAILY_SCHEME]]"
        ;
#else
        "[[Q_FROM]][-[Q_TO]] [[Q_SCHEME]]"
        "[ [REPEATED_DAILY_SCHEME]][ [MEAL]][ [[PERIOD] / ][PERIOD_SCHEME]]"
        "[; [D_FROM]][-[D_TO]][ [D_SCHEME]][; [DISTRIBUTED_DAILY_SCHEME]]"
        ;
#endif

const char * const  ENCODEDHTML_FULLDOC_MEDINTUX =
        "<html>\n"
        "<head>\n"
        "  <meta name=\"qrichtext\" content=\"1\" />\n"
        "  <meta name=\"charset\" content=\"UTF-8\" />\n"
        "  <meta name=\"generator\" content=\"{GENERATOR}\" />\n"
        "  <title>FreeDiams: Prescription</title>\n"
        "  <style type=\"text/css\">\n"
        "    A:link {text-decoration: none}\n"
        "    A:visited {text-decoration: none}\n"
        "    A:active {text-decoration: none}\n"
        "    A:hover {text-decoration: none;}\n"
        "  </style>\n"
        "</head>\n"
        "<body style=\"font-family:Arial;font-size:12pt\">\n\n"
        "<a style=\"text-decoration:none\" href=\"{ENCODEDPRESCRIPTION}\">\n"
        "{PRESCRIPTION}\n"
        "</a>\n\n"
        "</body>\n"
        "</html>\n";

const char * const  ENCODEDHTML_FULLDOC_NON_MEDINTUX =
        "<html>\n"
        "<head>\n"
        "  <meta name=\"qrichtext\" content=\"1\" />\n"
        "  <meta name=\"charset\" content=\"UTF-8\" />\n"
        "  <meta name=\"generator\" content=\"{GENERATOR}\" />\n"
        "  <meta name=\"prescription\" content=\"{ENCODEDPRESCRIPTION}\" />\n"
        "  <title>FreeDiams: Prescription</title>\n"
        "  <style type=\"text/css\">\n"
        "    A:link {text-decoration: none}\n"
        "    A:visited {text-decoration: none}\n"
        "    A:active {text-decoration: none}\n"
        "    A:hover {text-decoration: none;}\n"
        "  </style>\n"
        "</head>\n"
        "<body style=\"font-family:Arial;font-size:12pt\">\n\n"
        "{PRESCRIPTION}\n"
        "</body>\n"
        "</html>\n";

const char * const  ENCODEDHTML_FULLPRESCRIPTION_MEDINTUX =
        "<table border=0>\n"
        " <tr>\n"
        "   <td>\n"
        "     <ol>\n"
        "{FULLPRESCRIPTION}\n"
        "     </ol>\n"
        "   </td>\n"
        " </tr>\n"
        "</table>\n"
        ;

const char * const  ENCODEDHTML_FULLPRESCRIPTION_NON_MEDINTUX =
        "<ol style=\"font-size:10pt;font-weight:bold;\">\n"
        "{FULLPRESCRIPTION}\n"
        "</ol>\n";

const char * const  ENCODEDHTML_FREEDIAMSTAG = "FreeDiamsEncodedPrescription:";          /*!< \brief Default tag prepend to serialized prescription when saving. */

} // namespace Constants
} // namespace DrugsDB

#endif // CONSTANTS_HTML_H
