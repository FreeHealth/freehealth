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
#ifndef CONSTANTTRANSLATIONS_MENU_H
#define CONSTANTTRANSLATIONS_MENU_H

#include <qglobal.h>

namespace Trans {
namespace Constants {

// Used for the language menu
const char * const  ENGLISH             = QT_TRANSLATE_NOOP("tkConstants", "English");

// Menus translations
const char * const  M_FILE_TEXT              = QT_TRANSLATE_NOOP("tkConstants", "&File");
const char * const  M_FILE_RECENTFILES_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Recent files");
const char * const  M_PATIENT_NAVIGATION_TEXT= QT_TRANSLATE_NOOP("tkConstants", "&Patient navigation");
const char * const  M_EDIT_TEXT              = QT_TRANSLATE_NOOP("tkConstants", "&Edit");
const char * const  M_EDIT_SELECT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "&Selection");
const char * const  M_EDIT_LIST_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "&List");
const char * const  M_EDIT_TEMPLATES_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Templates");
const char * const  M_FORMAT_TEXT            = QT_TRANSLATE_NOOP("tkConstants", "&Format");
const char * const  M_FORMAT_FONT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Font");
const char * const  M_FORMAT_PARAGRAPH_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Paragraph");
const char * const  M_FORMAT_TABLE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "&Table");
const char * const  M_FORMAT_IMAGE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "&Image");
const char * const  M_TOOLS_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "T&ools");
const char * const  M_PLUGINS_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Pl&ugins");
const char * const  M_CONFIGURATION_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "&Configuration");
const char * const  M_TOOLBARS_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "&Toolbars");
const char * const  M_LANGUAGES_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Lan&guages");
const char * const  M_INTERPRETER_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Interpreter");
const char * const  M_ABOUT_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "&Help");
const char * const  ABOUTDATABASES_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "About databases");
const char * const  M_UPDATE_TEXT            = QT_TRANSLATE_NOOP("tkConstants", "Update available");

// Actions translations
const char * const  FILENEW_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "&New");
const char * const  FILEOPEN_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "&Open...");
const char * const  FILESAVE_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "&Save");
const char * const  FILESAVEAS_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Save &as...");
const char * const  FILEPRINT_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "&Print...");
const char * const  PRINTPREVIEW_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Print pre&view");
const char * const  FILEEXIT_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "&Exit");
const char * const  SEARCHMENU_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Sea&rch...");

const char * const  PATIENTNEW_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "New patient");
const char * const  PATIENTREMOVE_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Remove patient");
const char * const  PATIENTVIEWIDENTITY_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "View patient identity");
const char * const  IDENTITY_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Identity");

const char * const  OPENPRESCRIPTION_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Open a prescription");
const char * const  SAVEPRESCRIPTION_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Save a prescription");
const char * const  PRINTPRESCRIPTION_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Print a prescription");

const char * const  CLOSE_TEXT            = QT_TRANSLATE_NOOP("tkConstants", "Close");
const char * const  EDITCOPY_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Copy");
const char * const  EDITCUT_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Cut");
const char * const  EDITPASTE_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Paste");
const char * const  EDITUNDO_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Undo");
const char * const  EDITREDO_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Redo");
const char * const  EDITSELECTALL_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Select all");
const char * const  EDITDESESLECT_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Deselect");
const char * const  NEXT_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Next");
const char * const  PREVIOUS_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Previous");
const char * const  HOME_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Home");
const char * const  LISTADD_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Add to list");
const char * const  LISTREMOVE_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Remove from list");
const char * const  LISTMOVEUP_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Move item up");
const char * const  LISTMOVEDOWN_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Move item down");
const char * const  LISTCLEAR_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Clear");
const char * const  LISTSORT_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Sort");
const char * const  WEBSITE_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Web site");

const char * const  TEMPLATES             = QT_TRANSLATE_NOOP("tkConstants", "Templates");
const char * const  CREATETEMPLATE_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Create a new template");
const char * const  TEMPLATES_TOGGLEVIEW_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Toggle template view");
const char * const  ADDCATEGORY_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Add a category");
const char * const  ADD_TEXT              = QT_TRANSLATE_NOOP("tkConstants", "Add");
const char * const  REMOVE_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Remove");
const char * const  LOCK_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Lock");
const char * const  UNLOCK_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Unlock");
const char * const  LOCKED_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Locked");
const char * const  UNLOCKED_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Unlocked");
const char * const  TEMPLATES_DATABASE_INFORMATION_TEXT         = QT_TRANSLATE_NOOP("tkConstants", "Templates database information");
const char * const  REMOVE_EPISODE        = QT_TRANSLATE_NOOP("tkConstants", "Remove episode");
const char * const  RENEW_EPISODE         = QT_TRANSLATE_NOOP("tkConstants", "Renew episode");

const char * const  PMHX                  = QT_TRANSLATE_NOOP("tkConstants", "PMHx");
const char * const  PASTMEDICALHISTORY    = QT_TRANSLATE_NOOP("tkConstants", "Past medical history");
const char * const  PMHX_PREFERENCES      = QT_TRANSLATE_NOOP("tkConstants", "PMHx preferences");

const char * const  FORMATBOLD_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Bold");
const char * const  FORMATITALIC_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Italic");
const char * const  FORMATUNDERLINE_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Underline");
const char * const  FORMATSTRIKE_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Strike through");
const char * const  FORMATBIGGER_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Bigger");
const char * const  FORMATSMALLER_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Smaller");
const char * const  FORMATFIXEDWIDTHFONT_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Fixed width");
const char * const  FORMATTYPEWRITER_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Typewriter");
const char * const  FORMATFONT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Font format");
const char * const  FORMATFONTCOLOR_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Color");
const char * const  FORMATTABLE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Table format");
const char * const  FORMATIMAGE_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Image format");
const char * const  ALIGNCENTER_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Center");
const char * const  ALIGNLEFT_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "Left");
const char * const  ALIGNRIGHT_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Right");
const char * const  ALIGNJUSTIFY_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Justify");

const char * const  TABLE_ADDTABLE_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "Add table");
const char * const  TABLE_PROPERTIES_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Table properties");
const char * const  TABLE_ADDROW_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Add row");
const char * const  TABLE_REMOVEROW_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Remove row");
const char * const  TABLE_ADDCOLUMN_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Add colunm");
const char * const  TABLE_REMOVECOLUMN_TEXT = QT_TRANSLATE_NOOP("tkConstants", "Remove column");
const char * const  TABLE_MERGECELLS_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Merge cells");
const char * const  TABLE_SPLITCELLS_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Split cells");

const char * const  ABOUT_TEXT            = QT_TRANSLATE_NOOP("tkConstants", "&About...");
const char * const  MAC_ABOUT_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "About");
const char * const  ABOUTPLUGINS_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "About &plugins...");
const char * const  MAC_ABOUTPLUGINS_TEXT     = QT_TRANSLATE_NOOP("tkConstants", "About plugins");
const char * const  ABOUTQT_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "About &Qt...");
const char * const  ABOUTFORM_TEXT        = QT_TRANSLATE_NOOP("tkConstants", "About this &form...");
const char * const  APPLICATIONHELP_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "&Help");
const char * const  DEBUGHELPER_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Debugging tools");
const char * const  HELPTEXTTOGGLER_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Display/Hide form's help text");
const char * const  HELP_TEXT             = QT_TRANSLATE_NOOP("tkConstants", "Help");
const char * const  INTERPRETOR_GENERAL_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Interpreter");
const char * const  USERMANAGER_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "&User manager");
const char * const  PREFERENCES_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Preferences");
const char * const  APPCONFIGURATOR_TEXT  = QT_TRANSLATE_NOOP("tkConstants", "Application global configurator");
const char * const  CONFIGMEDINTUX_TEXT   = QT_TRANSLATE_NOOP("tkConstants", "Configure MedinTux");
const char * const  VIEWUPDATE_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "View update information");
const char * const  PRINT_PREVIEW_TEXT    = QT_TRANSLATE_NOOP("tkConstants", "Print preview");
const char * const  TRANSLATORS_TEXT      = QT_TRANSLATE_NOOP("tkConstants", "Translators");

const char * const  FULLSCREEN_TEXT       = QT_TRANSLATE_NOOP("tkConstants", "Toggle full screen");
const char * const  ZOOMIN_TEXT           = QT_TRANSLATE_NOOP("tkConstants", "Zoom in");
const char * const  ZOOMOUT_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Zoom out");
const char * const  TAKE_SCREENSHOT       = QT_TRANSLATE_NOOP("tkConstants", "Take screenshot");
const char * const  SCREENSHOTS           = QT_TRANSLATE_NOOP("tkConstants", "Screenshots");
const char * const  NAVIGATION            = QT_TRANSLATE_NOOP("tkConstants", "Navigation");
const char * const  DEFAULTS              = QT_TRANSLATE_NOOP("tkConstants", "Defaults");

const char * const  PMHMENU_TEXT          = QT_TRANSLATE_NOOP("tkConstants", "Past medical history");

const char * const  PLUGINS_CATEGORY = QT_TRANSLATE_NOOP("tkConstants", "Plugins");

} // end  ConstantTranslations
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_MENU_H
