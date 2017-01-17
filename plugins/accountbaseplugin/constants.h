/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNTBASE_CONSTANTS_H
#define ACCOUNTBASE_CONSTANTS_H

#include <QFlags>
#include <QHash>
#include <QStringList>

/**
 * \file constants.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.1.0
 * \date 28 Mar 2010
 * \brief Some constants for Accountancy database and model management
 * \ingroup freeaccount account
*/
namespace AccountDB {
namespace Constants {

    const char * const DB_ACCOUNTANCY           = "account";
    const char * const DEFAULT_ACCOUNTANCY_USER = "AccountancyDefaultUser";
    const char * const DB_VERSION_NUMBER        = "0.3";// "0.2"; "0.1";

    //DatapackMPModel
    const char * const DATAPACK_ACCOUNTANCY    = "account";
    const char * const DATAPACK_DB = "mp_datapack";
    const char * const DATAPACK_ACCOUNTANCY_FILENAME    = "mp_datapack.db";

    // Database abstraction

    enum Tables {
        Table_MedicalProcedure = 0,
        Table_BankDetails,
        Table_Deposit,
        Table_Account,
        Table_Acts,
        Table_Assets,
        Table_AssetsRates,
        Table_Movement,
        Table_AvailableMovement,
        Table_Insurance,
        Table_Sites,
        Table_Users,
        Table_Percent,
        Table_Rules,
        Table_DistanceRules,
        Table_Thesaurus,
        Table_Session,
        Table_Banking,
        Table_NameIndex,
        Table_VERSION
    };

    enum Tables_datapack
    {
        Table_DatapackMP = 0
        };

    enum TableMedicalProcedure_Fields {
        MP_ID = 0,
        MP_UID,
        MP_USER_UID,
        MP_INSURANCE_UID,
        MP_NAME,
        MP_ABSTRACT,
        MP_TYPE,
        MP_AMOUNT,
        MP_REIMBOURSEMENT,
        MP_DATE,
        MP_OTHERS,
        MP_COUNTRY,
        MP_MaxParam
    };
    enum TableBankDetails_Fields {
        BANKDETAILS_ID = 0,
        BANKDETAILS_USER_UID,
        BANKDETAILS_LABEL,
        BANKDETAILS_OWNER,
        BANKDETAILS_OWNERADRESS,
        BANKDETAILS_ACCOUNTNUMBER,
        BANKDETAILS_IBAN,
        BANKDETAILS_BALANCE,
        BANKDETAILS_BALANCEDATE,
        BANKDETAILS_COMMENT,
        BANKDETAILS_DEFAULT,
        BANKDETAILS_MaxParam
    };
    enum TableDep_Fields {
        DEPOSIT_ID = 0,
        DEPOSIT_USER_UID,
        DEPOSIT_ACCOUNT_ID,
        DEPOSIT_TYPE,
        DEPOSIT_DATE,
        DEPOSIT_PERTIODSTART,
        DEPOSIT_PERIODEND,
        DEPOSIT_CONTENT,
        DEPOSIT_COMMENT,
        DEPOSIT_ISVALID,
        DEPOSIT_MaxParam
    };
    enum TableAcc_Fields {
        ACCOUNT_ID = 0,
        ACCOUNT_UID,
        ACCOUNT_USER_UID,
        ACCOUNT_PATIENT_UID,
        ACCOUNT_PATIENT_NAME,
        ACCOUNT_SITE_ID,
        ACCOUNT_INSURANCE_ID,
        ACCOUNT_DATE,
        ACCOUNT_MEDICALPROCEDURE_XML,
        ACCOUNT_MEDICALPROCEDURE_TEXT,
        ACCOUNT_COMMENT,
        ACCOUNT_CASHAMOUNT,
        ACCOUNT_CHEQUEAMOUNT,
        ACCOUNT_VISAAMOUNT,
        ACCOUNT_INSURANCEAMOUNT,
        ACCOUNT_OTHERAMOUNT,
        ACCOUNT_DUEAMOUNT,
        ACCOUNT_DUEBY,
        ACCOUNT_ISVALID,
        ACCOUNT_TRACE,
        ACCOUNT_MaxParam
    };

    enum TableActs_Fields {
        ACTS_ID,
        ACTS_UID,
        ACTS_USER_UID,
        ACTS_PATIENT_UID,
        ACTS_PATIENT_NAME,
        ACTS_SITE_ID,
        ACTS_INSURANCE_ID,
        ACTS_DATE,
        ACTS_MEDICALPROCEDURE_TEXT,
        ACTS_COMMENT,
        ACTS_TRACE,
        ACTS_MaxParam
    };
    enum TableAssets_Fields {
        ASSETS_ID = 0,
        ASSETS_USER_UID,
        ASSETS_ACCOUNT_ID,
        ASSETS_LABEL,
        ASSETS_DATE,
        ASSETS_DURATION,
        ASSETS_MODE,
        ASSETS_VALUE,
        ASSETS_TAXEACTS,
        ASSETS_YEARLY_RESULT,
        ASSETS_RESIDUAL_VALUE,
        ASSETS_YEARS,
        ASSETS_RATE,
        ASSETS_MOVEMENT,
        ASSETS_COMMENT,
        ASSETS_TRACE,
        ASSETS_MaxParam
    };

    enum TableAssetsRates_Fields {
        ASSETSRATES_ID = 0,
        ASSETSRATES_USER_UID,
        ASSETSRATES_NAME,
        ASSETSRATES_YEARS,
        ASSETSRATES_RATES,
        ASSETSRATES_DATE,
        ASSETSRATES_MaxParam
    };
    enum TableMov_Fields {
        MOV_ID = 0,
        MOV_AV_MOVEMENT_ID,
        MOV_USER_UID,
        MOV_ACCOUNT_ID,
        MOV_TYPE,
        MOV_LABEL,
        MOV_DATE,
        MOV_DATEOFVALUE,
        MOV_AMOUNT,
        MOV_COMMENT,
        MOV_VALIDITY,
        MOV_TRACE,
        MOV_ISVALID,
        MOV_DETAILS,
        MOV_MaxParam
    };
    enum TableAvMov_Fields {
        AVAILMOV_ID = 0,
        AVAILMOV_PARENT,
        AVAILMOV_TYPE,
        AVAILMOV_LABEL,
        AVAILMOV_CODE,
        AVAILMOV_COMMENT,
        AVAILMOV_DEDUCTIBILITY,
        AVAILMOV_MaxParam
    };
    enum TableInsurance_Fields {
        INSURANCE_ID = 0,
        INSURANCE_UID,
        INSURANCE_NAME,
        INSURANCE_ADRESS,
        INSURANCE_CITY,
        INSURANCE_ZIPCODE,
        INSURANCE_COUNTRY,
        INSURANCE_TEL,
        INSURANCE_FAX,
        INSURANCE_MAIL,
        INSURANCE_CONTACT,
        INSURANCE_PREF,
        INSURANCE_MaxParam
    };
    enum TableSites_Fields {
        SITES_ID = 0,
        SITES_UID,
        SITES_NAME,
        SITES_ADRESS,
        SITES_CITY,
        SITES_ZIPCODE,
        SITES_COUNTRY,
        SITES_TEL,
        SITES_FAX,
        SITES_MAIL,
        SITES_CONTACT,
        SITES_PREF,
        SITES_MaxParam
    };
    enum TableUsers_Fields {
        USER_ID = 0,
        USER_UID,
        USER_LOG,
        USER_PASS,
        USER_EMR_UID,
        USER_MaxParam
    };
    enum TablePercent_Fields {
        PERCENT_ID = 0,
        PERCENT_UID,
        PERCENT_USER_UID,
        PERCENT_TYPE,
        PERCENT_VALUES,
        PERCENT_MaxParam
    };
    enum TableRules_Fields {
        RULES_ID = 0,
        RULES_UID,
        RULES_TYPE,
        RULES_VALUES,
        RULES_PREF,
        RULES_MaxParam
    };
    enum TableDistanceRules_Fields {
        DISTRULES_ID = 0,
        DISTRULES_UID,
        DISTRULES_TYPE,
        DISTRULES_VALUES,
        DISTRULES_MIN_KM,
        DISTRULES_PREF,
        DISTRULES_MaxParam
    };

    enum TableThesaurus_Fields {
        THESAURUS_ID = 0,
        THESAURUS_UID,
        THESAURUS_USERUID,
        THESAURUS_VALUES,
        THESAURUS_PREF,
        THESAURUS_MaxParam
    };

    enum fieldsVERSION {
        VERSION_ACTUAL = 0
    };

    //assets
    enum Modes {
        LINEAR_MODE = 1,
        DECREASING_MODE,
        MODES_MaxParam
    };

    enum TypeOfMovements {
        SPENDING = 0,
        RECEIVING,
        TYPEOFMOVEMENTS_MaxParam
    };

    // Menu and action for ActionManager
    const char * const  ACCOUNTCONSTANTS_TR_CONTEXT = "Account";
    const char * const  M_PLUGINS_DRUGS         =  "Acc.menuDrugs";
    const char * const  M_PLUGINS_SEARCH        =  "Acc.menuSearch";
    const char * const  M_PLUGINS_MODES         =  "Acc.menuModes";

    const char * const  G_PLUGINS_DRUGS         =  "Acc.groupDrugs";
    const char * const  G_PLUGINS_SEARCH        =  "Acc.groupSearch";
    const char * const  G_PLUGINS_MODES         =  "Acc.groupModes";

    const char * const  C_DRUGS_PLUGINS         =  "Acc.ContextDrugs";
    const char * const  C_DRUGS_SEARCH          =  "Acc.ContextSearch";

    const char* const   A_ACCOUNT_BOOK          = "a.Acc.AccBook";
    const char* const   A_REVENUES              = "a.Acc.Revenues";
    const char* const   A_EXPENSES              = "a.Acc.Expenses";

    const char* const   ACCOUNTBOOK_TEXT        = QT_TRANSLATE_NOOP("Account", "Account book");
    const char* const   REVENUES_TEXT           = QT_TRANSLATE_NOOP("Account", "Revenues");
    const char* const   EXPENSES_TEXT           = QT_TRANSLATE_NOOP("Account", "Expenses");

    // Payment methods
    const char* const   CASH                    = QT_TRANSLATE_NOOP("Account", "Cash");
    const char* const   CHEQUES                 = QT_TRANSLATE_NOOP("Account", "Cheques");
    const char* const   CREDITCARD              = QT_TRANSLATE_NOOP("Account", "Credit card");
    const char* const   MONEY                   = QT_TRANSLATE_NOOP("Account", "Money");
    const char* const   DUE                     = QT_TRANSLATE_NOOP("Account", "Due");
    const char* const   INSURANCE               = QT_TRANSLATE_NOOP("Account", "Insurance");
    const char* const   OTHER                   = QT_TRANSLATE_NOOP("Account", "Other");

    // Settings names
    const char * const  S_GROUP                  = "Account";                              /*!< \brief Key for settings. */
    const char * const  S_CONFIGURED             = "Account/Configured";                   /*!< \brief Key for settings. */
    const char * const  S_VIEWFONT               = "Account/view/Font";                    /*!< \brief Key for settings. */
    const char * const  S_VIEWFONTSIZE           = "Account/view/FontSize";                /*!< \brief Key for settings. */
    const char * const  S_INTERACTIONVIEW_ZOOM   = "Account/view/InteractionViewer/Zoom";  /*!< \brief Key for settings. */
    const char * const  S_DRUGFONT               = "Account/print/drug/Font";              /*!< \brief Key for settings. */
    const char * const  S_PRESCRIPTIONFONT       = "Account/print/prescription/Font";      /*!< \brief Key for settings. */

    const char * const  S_USER                   = "Account/user/Name";                    /*!< \brief Key for settings. */
    const char * const  S_PASSWORD               = "Account/user/Pass";                    /*!< \brief Key for settings. */
    const char * const  S_SERIAL                 = "Account/user/SN";                      /*!< \brief Key for settings. */
    const char * const  S_USER_HEADER            = "Account/user/htmlHeader";                 /*!< \brief Key for settings. */
    const char * const  S_USER_FOOTER            = "Account/user/htmlFooter";                 /*!< \brief Key for settings. */
    const char * const  S_USER_WATERMARK         = "Account/user/htmlWatermark";                 /*!< \brief Key for settings. */

    const char * const  S_HISTORYSIZE            = "Account/historySize";                  /*!< \brief Key for settings. */
    const char * const  S_DRUGHISTORY            = "Account/drugsHistory";                 /*!< \brief Key for settings. */

    const char * const  S_USERRECORDEDFORMS      = "Account/userRecordedForms";            /*!< \brief Key for settings. */

    const char * const  S_STATEPREFIX            = "Account/";


}  // End namespace Constants
}  // End namespace AccountDB

#endif // ACCOUNTBASE_CONSTANTS_H
