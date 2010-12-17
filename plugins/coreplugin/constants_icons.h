/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef CORE_CONSTANTS_ICONS_H
#define CORE_CONSTANTS_ICONS_H

/**
 * \file constants_icons.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 13 June 2010
*/

namespace Core {

namespace Constants {

const char * const  FREEMEDFORMS_SPLASHSCREEN = "freemedforms.png";
const char * const  FREEDIAMS_SPLASHSCREEN    = "freediams.png";
const char * const  FREEACCOUNT_SPLASHSCREEN  = "freeaccount.png";
const char * const  FREEICD_SPLASHSCREEN      = "freeicd.png";
const char * const  FREEGIR_SPLASHSCREEN      = "freegir.png";
const char * const  FREETOOLBOX_SPLASHSCREEN  = "freetoolbox.png";

// application icons
const char * const  ICONFREEMEDFORMS   = "freemedforms.png";
const char * const  ICONFREEDIAMS      = "freediams.png";
const char * const  ICONFREEACCOUNT    = "freeaccount.png";
const char * const  ICONFREEICD        = "freeicd.png";
const char * const  ICONFREEGIR        = "freegir.png";
const char * const  ICONFREETOOLBOX    = "freetoolbox.png";

// menu File
const char * const  ICONFILENEW =      "filenew.png";
const char * const  ICONOPEN =         "fileopen.png";
const char * const  ICONSAVE =         "filesave.png";
const char * const  ICONSAVEAS =       "filesaveas.png";
const char * const  ICONPRINT =        "fileprint.png";
const char * const  ICONPRINTPREVIEW = "fileprintpreview.png";
const char * const  ICONQUIT =         "exit.png";
const char * const  ICONEXIT =         "exit.png";
const char * const  ICONTEMPLATES =    "templates.png";
const char * const  ICONCLOSELIGHT =   "closebuttonlight.png";
const char * const  ICONCLOSEDARK =    "closebuttondark.png";
const char * const  ICONADDLIGHT =     "addbuttonlight.png";
const char * const  ICONADDDARK =      "addbuttondark.png";
const char * const  ICONVALIDATELIGHT= "validatebuttonlight.png";
const char * const  ICONVALIDATEDARK = "validatebuttondark.png";
const char * const  ICONMOVEUPLIGHT=   "arrowuplight.png";
const char * const  ICONMOVEDOWNLIGHT= "arrowdownlight.png";
const char * const  ICONPROCESS=       "run.png";
const char * const  ICONSPINNER=       "spinner.gif";

// menu Edit
const char * const  ICONEDIT =         "edit.png";
const char * const  ICONUNDO =         "undo.png";
const char * const  ICONREDO =         "redo.png";
const char * const  ICONCOPY =         "editcopy.png";
const char * const  ICONCUT =          "editcut.png";
const char * const  ICONPASTE =        "editpaste.png";
const char * const  ICONCLEAR =        "editclear.png";
const char * const  ICONADD =          "add.png";
const char * const  ICONREMOVE =       "remove.png";
const char * const  ICONMOVEUP =       "1uparrow.png";
const char * const  ICONMOVEDOWN =     "1downarrow.png";
const char * const  ICONSORT =         "sort.png";
const char * const  ICONSELECTALL =    "edit-select-all.png";
const char * const  ICONDESELECT =     "";
const char * const  ICONFIND =         "find.png";
const char * const  ICONOK =           "ok.png";
const char * const  ICONERROR =        "warning.png";
const char * const  ICONWARNING =      "warning.png";
const char * const  ICONFORBIDDEN =    "deconseille.png";

// menu Navigation
const char * const  ICONNEXT =         "next.png";
const char * const  ICONPREVIOUS =     "previous.png";
const char * const  ICONHOME =         "gohome.png";
const char * const  ICONLOCK =         "lock.png";
const char * const  ICONUNLOCK =       "unlock.png";
const char * const  ICONONELEFTARROW = "1leftarrow.png";
const char * const  ICONTWOLEFTARROW = "2leftarrow.png";
const char * const  ICONONERIGHTARROW = "1rightarrow.png";
const char * const  ICONTWORIGHTARROW = "2rightarrow.png";

// menu View
const char * const  ICONEYES =         "eyes.png";
const char * const  ICONVIEWSIDE =     "view_sidetree.png";
const char * const  ICONFULLSCREEN =   "view-fullscreen.png";

// menu TextFormat
const char * const  ICONBOLD =          "format-text-bold.png";
const char * const  ICONUNDERLINE =     "format-text-underline.png";
const char * const  ICONITALIC =        "format-text-italic.png";
const char * const  ICONSTRIKE =        "format-text-strikethrough.png";
const char * const  ICONFONTFORMAT =    "format-fonts.png";
const char * const  ICONFONTBIGGER =    "font-bigger.png";
const char * const  ICONFONTSMALLER =   "font-smaller.png";

const char * const  ICONALIGNLEFT =     "format-justify-left.png";
const char * const  ICONALIGNRIGHT =    "format-justify-right.png";
const char * const  ICONALIGNCENTER =   "format-justify-center.png";
const char * const  ICONALIGNJUSTIFY =  "format-justify-fill.png";

// menu Table
const char * const  ICONTABLE =             "table.png";
const char * const  ICONTABLEPROPERTIES =   "table-properties.png";
const char * const  ICONTABLEADDROW =       "table-add-row.png";
const char * const  ICONTABLEADDCOLUMN =    "table-add-col.png";
const char * const  ICONTABLEREMOVEROW =    "table-remove-row.png";
const char * const  ICONTABLEREMOVECOLUMN = "table-remove-col.png";
const char * const  ICONTABLEMERGECELLS =   "table-merge-cells.png";
const char * const  ICONTABLESPLITCELLS =   "table-split-cells.png";

// menu Help
const char * const  ICONSEARCH =       "search.png";
const char * const  ICONPREFERENCES =  "configure.png";
const char * const  ICONHELP =         "help.png";
const char * const  ICONABOUT =        "help.png";
const char * const  ICONHELPTEXT =     "helptext.png";
const char * const  ICONDEBUGHELPER =  "";

// menu Extras
const char * const  ICONCRASHRECOVER = "crashrecoverer.png";
const char * const  ICONSOFTWAREUPDATEAVAILABLE = "updateavailable.png";
const char * const  ICONMEDINTUX    =  "medintux.png";
const char * const  ICONPENCIL    =  "pencil.png";

// date and time Icons
const char * const  ICONDATE        =  "date.png";

// menu Users --> MediumSize
const char * const  ICONUSER          = "user.png";
const char * const  ICONPERSONAL      = "personal.png";
const char * const  ICONUSERMANAGER   = "usermanager.png";
const char * const  ICONEDITUSER      = "edituser.png";
const char * const  ICONCLEARUSER     = "editclear.png";
const char * const  ICONDELETEUSER    = "deleteuser.png";
const char * const  ICONNEWUSER       = "adduser.png";

// patients icons --> MediumSize / BigSize
const char * const  ICONPATIENTS      = "patientsearch.png";
const char * const  ICONMALE          = "male.png";
const char * const  ICONFEMALE        = "female.png";
const char * const  ICONHERMAPHRODISM = "";
const char * const  ICONPATIENTFILES  = "patientfiles.png";


}  // end Constants
} // end Core

#endif // CORE_CONSTANTS_ICONS_H
