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
#ifndef CORE_CONSTANTS_ICONS_H
#define CORE_CONSTANTS_ICONS_H

namespace Core {

namespace Constants {

const char * const  FREEMEDFORMS_SPLASHSCREEN = "freemedforms.png";
const char * const  FREEDIAMS_SPLASHSCREEN = "freediams2.png";
const char * const  FREEACCOUNT_SPLASHSCREEN = "freeaccount.png";

// application icons
const char * const  ICONFREEDIAMS = "freediams.png";
const char * const  ICONFREEMEDFORMS = "freemedforms.png";
const char * const  ICONFREEACCOUNT = "freeaccount.png";

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
