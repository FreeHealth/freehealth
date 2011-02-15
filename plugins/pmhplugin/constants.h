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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>
#include <QString>

namespace PMH {
namespace Constants {

    // Translate context
    const char * const  MH_CONSTANTS_TR_CONTEXT = "PMHx";

    // Database
    const char * const  DB_NAME            = "pmh";
    const char * const  DB_FILENAME        = "pmh.db";
    const char * const  DB_ACTUALVERSION   = "0.1";

    // Help pages
    const char * const  H_PMH_CREATOR_PAGE  = "";

    // Tables
    enum Tables {
      Table_MASTER = 0,
      Table_EPISODE,
      Table_ICD,
      Table_CATEGORIES,
      Table_CATEGORY_LABEL,
      Table_VERSION
    };

    // Fields
    enum MasterFields {
        MASTER_ID = 0,
        MASTER_PATIENT_UID,
        MASTER_USER_UID,
        MASTER_CATEGORY_ID,
        MASTER_EPISODE_ID,
        MASTER_CONTACTS_ID,
        MASTER_LABEL,
        MASTER_TYPE,
        MASTER_STATE,
        MASTER_CONFINDEX,
        MASTER_ISVALID,
        MASTER_PRIVATE,
        MASTER_COMMENT,
        MASTER_TRACE_ID,
        MASTER_MaxParam
    };

    enum EpisodeFields {
        EPISODE_ID = 0,
        EPISODE_MASTER_ID,
        EPISODE_LABEL,
        EPISODE_DATE_START,
        EPISODE_DATE_END,
        EPISODE_CONF_INDEX,
        EPISODE_ICD_CODES,
        EPISODE_ISVALID,
        EPISODE_COMMENT,
        EPISODE_TRACE_ID,
        EPISODE_MaxParam
    };

    enum IcdFields {
        ICD_ID = 0,
        ICD_CONTENT,
        ICD_MaxParam
    };

    enum PmhCatFields {
        CATEGORY_ID = 0, CATEGORY_PARENT, CATEGORY_LABEL_ID, CATEGORY_ISRISKFACTOR,
        CATEGORY_ISCHONICDISEASE, CATEGORY_SORT_ID, CATEGORY_ISVALID, CATEGORY_THEMEDICON,
        CATEGORY_MaxParam
    };

    enum PmhCatLabelFields {
        CATEGORYLABEL_ID = 0, CATEGORYLABEL_LABEL_ID, CATEGORYLABEL_LANG, CATEGORYLABEL_VALUE,
        CATEGORYLABEL_ISVALID,
        CATEGORYLABEL_MaxParam
    };

    enum VersionFields {
        VERSION_TEXT = 0
    };


    // Types and Statuts of Medical History
    enum MHType {
        NoTypeDefined                       = 0,
        ChronicDisease,
        ChronicDiseaseWithoutAcuteEpisodes,
        AcuteDisease,
        RiskFactor
    };

    enum MHStatus {
        NoStatusDefined = 0,
        IsActive,
        IsInRemission,
        IsQuiescent,
        IsCured
    };

    QString typeToString(int type);
    QStringList availableTypes();
    int stringToType(const QString &type);

    QString statusToString(int state);
    QStringList availableStatus();
    int stringToStatus(const QString &status);


    // Settings
    const char * const  S_BACKGROUND_CATEGORIES    = "PMHx/ModelBackground/Categories";
    const char * const  S_BACKGROUND_TEMPLATES     = "PMHx/ModelBackground/PMH";
    const char * const  S_FOREGROUND_CATEGORIES    = "PMHx/ModelForeground/Categories";
    const char * const  S_FOREGROUND_TEMPLATES     = "PMHx/ModelForeground/PMH";

    // Contexts, menus and actions
    const char * const  C_PMH_PLUGINS = "PmhContext";
    const char * const  M_PMH         = "mPmh";
    const char * const  G_PMH         = "gPmh";
    const char * const  G_PMH_NEW     = "gPmhNew";
    const char * const  G_PMH_EDITION = "gPmhEdit";

    const char * const  A_PMH_NEW               = "aNewPmh";
    const char * const  A_PMH_REMOVE            = "aRemovePmh";
    const char * const  A_PMH_NEWCATEGORY       = "aNewCatgeroyPmh";
    const char * const  A_PMH_REMOVECATEGORY    = "aRemoveCatgeroyPmh";
    const char * const  A_PMH_CATEGORYMANAGER   = "aPmhCatMan";


    // Translations
    const char * const  PMHCONSTANTS_TR_CONTEXT = "PMH";
    const char * const  CREATEPMH_TEXT          = QT_TRANSLATE_NOOP("PMH", "Create a new PMHx");
    const char * const  REMOVEPMH_TEXT          = QT_TRANSLATE_NOOP("PMH", "Remove PMH");
    const char * const  CREATECATEGORY_TEXT     = QT_TRANSLATE_NOOP("PMH", "Create a new category");
    const char * const  REMOVECATEGORY_TEXT     = QT_TRANSLATE_NOOP("PMH", "Remove category");
    const char * const  CATEGORYMANAGER_TEXT    = QT_TRANSLATE_NOOP("PMH", "PMHx category manager");


}  // End namespace Constants
}  // End namespace History


#endif // CONSTANTS_H
