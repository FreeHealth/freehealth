/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "accountbase.h"
#include "constants.h"
#include "accountdata.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/icommandline.h>
#include <coreplugin/constants.h>

#ifdef FREEACCOUNT
#    include <coreplugin/commandlineparser.h>
#endif

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QFile>
#include <QDir>
#include <QMultiHash>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QSet>

using namespace AccountDB;
using namespace AccountDB::Internal;
using namespace Trans::ConstantTranslations;

enum Warn {WarnDebugMessage = true};

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }

static inline bool connectDatabase(QSqlDatabase &DB, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("AccountBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()), __FILE__, line);
            return false;
        }
    }
    return true;
}

namespace AccountDB {
namespace Internal {
/**
  \brief Private part of AccountBase
  \internal
*/
class AccountBasePrivate
{
public:
    AccountBasePrivate(AccountBase *base) : q(base), m_LogChrono(false), m_initialized(false) {}
    ~AccountBasePrivate()
    {
    }

public:
    AccountBase *q;
    bool m_LogChrono;
    bool m_initialized;
};
}  // End Internal
}  // End AccountDB


//--------------------------------------------------------------------------------------------------------
//--------------------------------- Initialization of static members -------------------------------------
//--------------------------------------------------------------------------------------------------------
AccountBase *AccountBase::m_Instance = 0;

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Initializing Database -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Returns the unique instance of AccountBase. If it does not exist, it is created */
AccountBase *AccountBase::instance()
{
    Q_ASSERT(m_Instance);
    return m_Instance;
}

/**
   \brief Constructor.
   \private
*/
AccountBase::AccountBase(QObject *parent) :
    QObject(parent), Utils::Database(),
    d(0)
{
    m_Instance = this;
    d = new AccountBasePrivate(this);
    setObjectName("AccountBase");

    using namespace AccountDB::Constants;
    addTable(Table_MedicalProcedure,  "medical_procedure");
    addTable(Table_BankDetails,       "bank_details");
    addTable(Table_Deposit,           "deposit");
    addTable(Table_Account,           "account");
    addTable(Table_Acts,              "acts");
    addTable(Table_Assets,            "assets");
    addTable(Table_AssetsRates,       "assets_rates");
    addTable(Table_Movement,          "movement");
    addTable(Table_AvailableMovement, "available_movement");
    addTable(Table_Insurance,         "insurance");
    addTable(Table_Sites,             "sites");
    addTable(Table_Users,             "users");
    addTable(Table_Percent,           "percentages");
    addTable(Table_Rules,             "rules");
    addTable(Table_DistanceRules,     "distance_rules");
    addTable(Table_Thesaurus,         "thesaurus");
//    addTable(Table_Session,           "session");
//    addTable(Table_Banking,           "banking");
//    addTable(Table_NameIndex,         "nameindex");

    addTable(Table_VERSION,         "VERSION");
    addField(Table_VERSION, VERSION_ACTUAL, "ACTUAL", FieldIsShortText);

    addField(Table_MedicalProcedure, MP_ID,             "MP_ID",          FieldIsUniquePrimaryKey);
    addField(Table_MedicalProcedure, MP_UID,            "MP_UUID",        FieldIsUUID);
    addField(Table_MedicalProcedure, MP_USER_UID,       "MP_USER_UID",    FieldIsUUID);
    addField(Table_MedicalProcedure, MP_INSURANCE_UID,  "MP_INSURANCE_UID",FieldIsUUID);
    addField(Table_MedicalProcedure, MP_NAME,           "NAME",           FieldIsShortText);
    addField(Table_MedicalProcedure, MP_ABSTRACT,       "ABSTRACT",       FieldIsLongText);
    addField(Table_MedicalProcedure, MP_TYPE,           "TYPE",           FieldIsShortText);
    addField(Table_MedicalProcedure, MP_AMOUNT,         "AMOUNT",         FieldIsReal);
    addField(Table_MedicalProcedure, MP_REIMBOURSEMENT, "REIMBOURSEMENT", FieldIsReal);
    addField(Table_MedicalProcedure, MP_DATE,           "DATE",           FieldIsDate);
    addField(Table_MedicalProcedure, MP_OTHERS,         "OTHERS",         FieldIsBlob);
    addField(Table_MedicalProcedure, MP_COUNTRY,        "COUNTRY",         FieldIsBlob);

//    "CREATE TABLE 	actes_disponibles ("  --> medical_procedure
//            "id_acte_dispo  int(10)  	UNSIGNED  	       		NOT NULL  	 auto_increment ,"
//            "nom_acte 	varchar(50) 	COLLATE utf8_unicode_ci 	NOT NULL  			,"
//            "desc_acte 	varchar(200) 	COLLATE utf8_unicode_ci 	NOT NULL  			,"
//            "type 		varchar(100) 	COLLATE utf8_unicode_ci 	NOT NULL 			,"
//            "montant_total 	double 						NOT NULL 			,"
//            "montant_tiers 	double 						NOT NULL 			,"
//            "date_effet	date						NOT NULL			,"
//            "PRIMARY KEY(id_acte_dispo));";

    addField(Table_BankDetails,  BANKDETAILS_ID,             "BD_ID",             FieldIsUniquePrimaryKey);
    // TODO: this field must be not null
    addField(Table_BankDetails,  BANKDETAILS_USER_UID,       "BD_USER_UID",       FieldIsUUID);
    addField(Table_BankDetails,  BANKDETAILS_LABEL,          "BD_LABEL",          FieldIsShortText);
    addField(Table_BankDetails,  BANKDETAILS_OWNER,          "BD_OWNER",          FieldIsShortText);
    addField(Table_BankDetails,  BANKDETAILS_OWNERADRESS,    "BD_OWNERADRESS",    FieldIsShortText);
    addField(Table_BankDetails,  BANKDETAILS_ACCOUNTNUMBER,  "BD_ACCNUMB",        FieldIsShortText);
    addField(Table_BankDetails,  BANKDETAILS_IBAN,           "BD_IBAN",           FieldIsShortText);
    addField(Table_BankDetails,  BANKDETAILS_BALANCE,        "BD_BALANCE",        FieldIsReal);
    addField(Table_BankDetails,  BANKDETAILS_BALANCEDATE,    "BD_BALANCEDATE",    FieldIsDate, "CURRENT_DATE");
    addField(Table_BankDetails,  BANKDETAILS_COMMENT,        "BD_COMMENT",        FieldIsLongText);
    addField(Table_BankDetails,  BANKDETAILS_DEFAULT,        "BD_ISDEFAULT",      FieldIsBoolean);
    /** Link owner to user db ? */

//    "CREATE TABLE 	comptes_bancaires ("  -->  bank_details
//                    "id_compte	      int(10)		UNSIGNED 			NOT NULL	auto_increment 	,"
//                    "id_usr 	      int(10) 		UNSIGNED 			NOT NULL 			,"
//                    "libelle 	      varchar(150) 	COLLATE utf8_unicode_ci  	NULL 				,"
//                    "titulaire 	      varchar(100) 	COLLATE utf8_unicode_ci 	NULL 				,"
//                    "nom_banque 	      varchar(50) 	COLLATE utf8_unicode_ci 	NULL 				,"
//                    "rib_code_banque      varchar(5) 	COLLATE utf8_unicode_ci 	NULL 				,"
//                    "rib_code_guichet     varchar(5) 	COLLATE utf8_unicode_ci  	NULL 				,"
//                    "rib_numcompte 	      varchar(11) 	COLLATE utf8_unicode_ci 	NULL 				,"
//                    "rib_cle 	      varchar(2) 	COLLATE utf8_unicode_ci 	NULL 				,"
//                    "solde_initial 	      double 						NULL				,"
//                    "remarque 	      blob 		 	 			NULL 				,"
//                    "PRIMARY KEY(id_compte));";

    addField(Table_Deposit,  DEPOSIT_ID,           "DEP_ID",          FieldIsUniquePrimaryKey);
    addField(Table_Deposit,  DEPOSIT_USER_UID,     "USER_UID",        FieldIsUUID);
    addField(Table_Deposit,  DEPOSIT_ACCOUNT_ID,   "DEP_ACCDOUNT_ID", FieldIsLongInteger);
    addField(Table_Deposit,  DEPOSIT_TYPE,         "TYPE",            FieldIsShortText);
    addField(Table_Deposit,  DEPOSIT_DATE,         "DATE",            FieldIsDate, "CURRENT_DATE");
    addField(Table_Deposit,  DEPOSIT_PERTIODSTART, "PERIOD_START",    FieldIsDate, "CURRENT_DATE");
    addField(Table_Deposit,  DEPOSIT_PERIODEND,    "PERIOD_END",      FieldIsDate, "CURRENT_DATE");
    addField(Table_Deposit,  DEPOSIT_CONTENT,      "CONTENT",         FieldIsBlob);
    addField(Table_Deposit,  DEPOSIT_COMMENT,      "COMMENT",         FieldIsLongText);
    addField(Table_Deposit,  DEPOSIT_ISVALID,      "ISVALID",         FieldIsBoolean);

//    "CREATE TABLE 	depots 	  ("  ---> deposit
//                  "id_depot  	int(11)  	 	  				NOT NULL  	 auto_increment  ,"
//                  "id_usr 	int(11) 						NOT NULL 			 ,"
//                  "id_compte 	int(10) 		UNSIGNED 			NOT NULL 			,"
//                  "type_depot 	varchar(10) 		COLLATE utf8_unicode_ci		NULL 				,"
//                  "date 		date 							NOT NULL  			,"
//                  "periode_deb 	date 							NULL 	         		,"
//                  "periode_fin 	date 							NULL  			        ,"
//                  "blob_depot 	blob 			 				NULL 			        ,"
//                  "remarque 	text                    COLLATE utf8_unicode_ci         NULL 				,"
//                  "valide 	tinyint(4) 						NULL 			        ,"
//                  "PRIMARY KEY(id_depot));";
/*        ACCOUNT_ID = 0,
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
        ACCOUNT_MaxParam*/
    addField(Table_Account,  ACCOUNT_ID,              "ACCOUNT_ID",     FieldIsUniquePrimaryKey);
    addField(Table_Account,  ACCOUNT_UID,             "ACCOUNT_UID",    FieldIsUUID);
    addField(Table_Account,  ACCOUNT_USER_UID,        "USER_UID",       FieldIsUUID);
    addField(Table_Account,  ACCOUNT_PATIENT_UID,     "PATIENT_UID",    FieldIsShortText);
    addField(Table_Account,  ACCOUNT_PATIENT_NAME,    "PATIENT_NAME",   FieldIsLongText);
    addField(Table_Account,  ACCOUNT_SITE_ID,         "SITE_ID",        FieldIsLongInteger);
    addField(Table_Account,  ACCOUNT_INSURANCE_ID,    "INSURANCE_ID",   FieldIsLongInteger);
    addField(Table_Account,  ACCOUNT_DATE,            "DATE",           FieldIsDate, "CURRENT_DATE");
    addField(Table_Account,  ACCOUNT_MEDICALPROCEDURE_XML,   "MP_XML",  FieldIsLongText);
    addField(Table_Account,  ACCOUNT_MEDICALPROCEDURE_TEXT,   "MP_TXT", FieldIsLongText);
    addField(Table_Account,  ACCOUNT_COMMENT,         "COMMENT",        FieldIsLongText);
    addField(Table_Account,  ACCOUNT_CASHAMOUNT,      "CASH",           FieldIsReal);
    addField(Table_Account,  ACCOUNT_CHEQUEAMOUNT,    "CHEQUE",         FieldIsReal);
    addField(Table_Account,  ACCOUNT_VISAAMOUNT,      "VISA",           FieldIsReal);
    addField(Table_Account,  ACCOUNT_INSURANCEAMOUNT, "BANKING",        FieldIsReal);
    addField(Table_Account,  ACCOUNT_OTHERAMOUNT,     "OTHER",          FieldIsReal);
    addField(Table_Account,  ACCOUNT_DUEAMOUNT,       "DUE",            FieldIsReal);
    addField(Table_Account,  ACCOUNT_DUEBY,           "DUE_BY",         FieldIsLongText);
    addField(Table_Account,  ACCOUNT_ISVALID,         "VALID",          FieldIsBoolean);
    addField(Table_Account,  ACCOUNT_TRACE,           "TRACE",          FieldIsBlob);


    addField(Table_Acts,  ACTS_ID,              "ACTS_ID",     FieldIsUniquePrimaryKey);
    addField(Table_Acts,  ACTS_UID,             "ACTS_UID",    FieldIsUUID);
    addField(Table_Acts,  ACTS_USER_UID,        "USER_UID",       FieldIsUUID);
    addField(Table_Acts,  ACTS_PATIENT_UID,     "PATIENT_UID",    FieldIsLongInteger);
    addField(Table_Acts,  ACTS_PATIENT_NAME,    "PATIENT_NAME",   FieldIsLongInteger);
    addField(Table_Acts,  ACTS_SITE_ID,         "SITE_ID",        FieldIsLongInteger);
    addField(Table_Acts,  ACTS_INSURANCE_ID,    "INSURANCE_ID",   FieldIsLongInteger);
    addField(Table_Acts,  ACTS_DATE,            "DATE",           FieldIsDate, "CURRENT_DATE");
    addField(Table_Acts,  ACTS_MEDICALPROCEDURE_TEXT,   "MP_TXT", FieldIsLongText);
    addField(Table_Acts,  ACTS_COMMENT,         "COMMENT",        FieldIsLongText);
    addField(Table_Acts,  ACTS_TRACE,           "TRACE",          FieldIsBlob);

//    "CREATE TABLE 	honoraires	("   --> amount
//                    "id_hono  	int(11)  	 	UNSIGNED  			NOT NULL	 auto_increment  ,"
//                    "id_usr 	int(11) 						NOT NULL			 ,"
//                    "id_drtux_usr 	int(11) 						NULL 				 ,"
//                    "patient 	varchar(75) 		COLLATE utf8_unicode_ci		NOT NULL 			,"
//                    "id_site        varchar(10)    	        COLLATE utf8_unicode_ci         NULL                            ,"
//                    "id_payeurs     varchar(36)             COLLATE utf8_unicode_ci         NULL                            ,"
//                    "GUID 		varchar(36) 		COLLATE utf8_unicode_ci		NULL 				,"
//                    "praticien 	varchar(75) 		COLLATE utf8_unicode_ci 	NOT NULL			,"
//                    "date 		date 							NOT NULL 			,"
//                    "acte 		blob 		                        		NULL 				,"
//                    "acteclair      varchar(12)             COLLATE utf8_unicode_ci         NULL                            ,"
//                    "remarque 	blob 			 	 			NULL 				,"
//                    "esp 		double 							NOT NULL 			,"
//                    "chq 		double 							NOT NULL 			,"
//                    "cb 		double 							NOT NULL			,"
//                    "daf 		double 							NOT NULL 			,"
//                    "autre 		double 							NOT NULL 			,"
//                    "du 		double 							NOT NULL 			,"
//                    "du_par		varchar(100) 		COLLATE utf8_unicode_ci		NULL 			        ,"
//                    "valide 	tinyint(1) 						NULL				,"
//                    "tracabilite 	blob 			 				NULL				,"
//                    "PRIMARY KEY(id_hono));";

    // Fr == Immobilisations
    addField(Table_Assets,  ASSETS_ID,             "ASSETS_ID",        FieldIsUniquePrimaryKey);
    addField(Table_Assets,  ASSETS_USER_UID,       "USER_UID",      FieldIsUUID);
    addField(Table_Assets,  ASSETS_ACCOUNT_ID,     "ACCOUNT_ID",    FieldIsLongInteger);
    addField(Table_Assets,  ASSETS_LABEL,          "LABEL",         FieldIsShortText);
    addField(Table_Assets,  ASSETS_DATE,           "DATE",          FieldIsDate, "CURRENT_DATE");
    addField(Table_Assets,  ASSETS_DURATION,       "DURATION",      FieldIsReal);
    addField(Table_Assets,  ASSETS_MODE,           "MODE",          FieldIsInteger);
    addField(Table_Assets,  ASSETS_VALUE,          "VALUE",         FieldIsReal);
    addField(Table_Assets,  ASSETS_TAXEACTS,       "TAX ACTS",      FieldIsReal);
    addField(Table_Assets,  ASSETS_YEARLY_RESULT,  "YEARLY VALUE",  FieldIsReal);
    addField(Table_Assets,  ASSETS_RESIDUAL_VALUE, "RESIDUAL VALUE",FieldIsReal);
    addField(Table_Assets,  ASSETS_YEARS,          "YEARS TO RUN",  FieldIsInteger);
    addField(Table_Assets,  ASSETS_RATE,           "RATE",          FieldIsReal);
    addField(Table_Assets,  ASSETS_MOVEMENT,       "MOVEMENT",      FieldIsBlob);
    addField(Table_Assets,  ASSETS_COMMENT,        "COMMENT",       FieldIsLongText);
    addField(Table_Assets,  ASSETS_TRACE,          "TRACE",         FieldIsBlob);

//    "CREATE TABLE 	immobilisations ("  --> assets
//                    "id_immob  	int(10)  	 	UNSIGNED  			NOT NULL  	 auto_increment  ,"
//                    "id_usr 	int(10) 		UNSIGNED 			NOT NULL 			,"
//                    "id_compte 	int(10) 		UNSIGNED 			NOT NULL 			,"
//                    "libelle 	varchar(150) 		COLLATE utf8_unicode_ci 	NULL 				,"
//                    "date_service 	date 							NOT NULL			,"
//                    "duree 		int(11) 						NOT NULL 			,"
//                    "mode 		tinyint(1) 						NOT NULL			,"
//                    "valeur 	bigint(20) 		UNSIGNED 			NOT NULL			,"
//                    "montant_tva 	double 							NULL 				,"
//                    "valeur_residuelle bigint(20) 						NOT NULL			,"
//                    "resultat 	blob 			 				NOT NULL			,"
//                    "mouvements 	blob 			 	 			NULL 				,"
//                    "remarque 	text 			COLLATE utf8_unicode_ci	        NULL 				,"
//                    "tracabilite 	blob 			 	 			NULL				,"
//                    "PRIMARY KEY(id_immob));";
    addField(Table_AssetsRates,  ASSETSRATES_ID,       "ASSETSRATES_ID", FieldIsUniquePrimaryKey);
    addField(Table_AssetsRates,  ASSETSRATES_USER_UID, "USER_UID",       FieldIsUUID);
    addField(Table_AssetsRates,  ASSETSRATES_NAME,     "NAME",           FieldIsShortText);
    addField(Table_AssetsRates,  ASSETSRATES_YEARS,    "YEARS_RANGE",    FieldIsBlob);
    addField(Table_AssetsRates,  ASSETSRATES_RATES,    "RATE",           FieldIsReal);
    addField(Table_AssetsRates,  ASSETSRATES_DATE,     "DATE",           FieldIsDate,"CURRENT_DATE");

//    addTable(Table_Movement, "movement");
    addField(Table_Movement,  MOV_ID,             "MOV_ID",     FieldIsUniquePrimaryKey);
    addField(Table_Movement,  MOV_AV_MOVEMENT_ID, "AV_MOV_ID",  FieldIsLongInteger);
    addField(Table_Movement,  MOV_USER_UID,       "USER_UID",   FieldIsUUID);
    addField(Table_Movement,  MOV_ACCOUNT_ID,     "ACCOUNT_ID", FieldIsLongInteger);
    addField(Table_Movement,  MOV_TYPE,           "TYPE",       FieldIsInteger);
    addField(Table_Movement,  MOV_LABEL,          "LABEL",      FieldIsShortText);
    addField(Table_Movement,  MOV_DATE,           "DATE",       FieldIsDate, "CURRENT_DATE");
    addField(Table_Movement,  MOV_DATEOFVALUE,    "DATEVALUE",  FieldIsDate, "CURRENT_DATE");
    addField(Table_Movement,  MOV_AMOUNT,         "ACTS",       FieldIsReal);
    addField(Table_Movement,  MOV_COMMENT,        "COMMENT",    FieldIsLongText);
    addField(Table_Movement,  MOV_VALIDITY,       "VALIDITY",   FieldIsInteger);
    addField(Table_Movement,  MOV_TRACE,          "TRACE",      FieldIsBlob);
    addField(Table_Movement,  MOV_ISVALID,        "ISVALID",    FieldIsBoolean);
    addField(Table_Movement,  MOV_DETAILS,        "DETAILS",    FieldIsShortText);

    //    "CREATE TABLE 	mouvements	("  --> movement
//                    "id_mouvement  int(10) 			UNSIGNED   			NOT NULL    	 auto_increment ,"
//                    "id_mvt_dispo  int(11)             					NOT NULL    			,"
//                    "id_usr        int(10) 			UNSIGNED      			NOT NULL                    	,"
//                    " id_compte    int(10) 			UNSIGNED    			NOT NULL 			,"
//                    " type         tinyint(3) 		UNSIGNED  			NOT NULL			,"
//                    " libelle      varchar(100)        	COLLATE utf8_unicode_ci 	NOT NULL			,"
//                    " date         date                					NOT NULL			,"
//                    " date_valeur  date                					NOT NULL 			,"
//                    " montant      double UNSIGNED     					NOT NULL			,"
//                    "remarque      blob                 					NULL  				,"
//                    " valide       tinyint(4)          					NOT NULL			,"
//                    " tracabilite  blob               					NULL   				,"
//                    " validation   tinyint(1)         					NULL				,"
//                    " detail       varchar(100)        	COLLATE utf8_unicode_ci 	NULL  				,"
//                    "PRIMARY KEY(id_mouvement));";

    addField(Table_AvailableMovement,  AVAILMOV_ID,           "AVAILMOV_ID",   FieldIsUniquePrimaryKey);
    addField(Table_AvailableMovement,  AVAILMOV_PARENT,       "PARENT",        FieldIsShortText);
    addField(Table_AvailableMovement,  AVAILMOV_TYPE,         "TYPE",          FieldIsInteger);
    addField(Table_AvailableMovement,  AVAILMOV_LABEL,        "LABEL",         FieldIsShortText);
    addField(Table_AvailableMovement,  AVAILMOV_CODE,         "CODE",         FieldIsShortText);
    addField(Table_AvailableMovement,  AVAILMOV_COMMENT,      "COMMENT",       FieldIsLongText);
    addField(Table_AvailableMovement,  AVAILMOV_DEDUCTIBILITY,"TAX_DEDUCTIBLE",FieldIsBoolean);
    /** Add user_uid, and uid */

//    "CREATE TABLE 	mouvements_disponibles ("  --> available_movement
//                    "id_mvt_dispo  	int(10)  	 	UNSIGNED  			NOT NULL  	 auto_increment  ,"
//                    "id_mvt_parent 	int(11) 					 	NULL 				,"
//                    "type 		tinyint(3) 		UNSIGNED 			NOT NULL  			,"
//                    "libelle 	varchar(100) 		COLLATE utf8_unicode_ci 	NOT NULL   			,"
//                    "remarque 	blob 							NULL 				,"
//                    "PRIMARY KEY(id_mvt_dispo));";

//    addTable(Table_Insurance, "insurance");
    addField(Table_Insurance,  INSURANCE_ID,          "INSURANCE_ID", FieldIsUniquePrimaryKey);
    addField(Table_Insurance,  INSURANCE_UID,         "INSURANCE_UID",   FieldIsLongInteger);
    addField(Table_Insurance,  INSURANCE_NAME,        "NAME",   FieldIsShortText);
    addField(Table_Insurance,  INSURANCE_ADRESS,      "ADRESS",   FieldIsLongText);
    addField(Table_Insurance,  INSURANCE_CITY,        "CITY",   FieldIsShortText);
    addField(Table_Insurance,  INSURANCE_ZIPCODE,     "ZIPCODE",   FieldIsShortText);
    addField(Table_Insurance,  INSURANCE_COUNTRY,     "COUNTRY",   FieldIsShortText);
    addField(Table_Insurance,  INSURANCE_TEL,         "TEL",   FieldIsShortText);
    addField(Table_Insurance,  INSURANCE_FAX,         "FAX",   FieldIsShortText);
    addField(Table_Insurance,  INSURANCE_MAIL,        "MAIL",   FieldIsShortText);
    addField(Table_Insurance,  INSURANCE_CONTACT,     "CONTACT",   FieldIsShortText);
    addField(Table_Insurance,  INSURANCE_PREF,        "PREFERRED",  FieldIsBoolean);

    /** Create an IDENTITY table */

//    "CREATE TABLE 	payeurs	("  --> insurance
//                    "ID_Primkey  	bigint(20)  	 	  				NOT NULL  	 auto_increment  ,"
//                    "id_payeurs 	bigint(20) 						NOT NULL 			,"
//                    "nom_payeur 	varchar(100) 		COLLATE utf8_unicode_ci 	NULL 				,"
//                    "adresse_payeur varchar(100) 		COLLATE utf8_unicode_ci 	NULL 				,"
//                    "ville_payeur 	varchar(100) 		COLLATE utf8_unicode_ci 	NULL 				,"
//                    "code_postal 	varchar(5) 		COLLATE utf8_unicode_ci 	NULL 				,"
//                    "telephone_payeur varchar(100) 		COLLATE utf8_unicode_ci 	NULL				,"
//                    "PRIMARY KEY(ID_Primkey));";

//    addTable(Table_Sites, "sites");
    addField(Table_Sites,  SITES_ID,      "SITE_ID", FieldIsUniquePrimaryKey);
    addField(Table_Sites,  SITES_UID,     "SITE_UID",   FieldIsLongInteger);
    addField(Table_Sites,  SITES_NAME,    "NAME",   FieldIsShortText);
    addField(Table_Sites,  SITES_ADRESS,  "ADRESS",   FieldIsLongText);
    addField(Table_Sites,  SITES_CITY,    "CITY",   FieldIsShortText);
    addField(Table_Sites,  SITES_ZIPCODE, "ZIPCODE",   FieldIsShortText);
    addField(Table_Sites,  SITES_COUNTRY, "COUNTRY",   FieldIsShortText);
    addField(Table_Sites,  SITES_TEL,     "TEL",   FieldIsShortText);
    addField(Table_Sites,  SITES_FAX,     "FAX",   FieldIsShortText);
    addField(Table_Sites,  SITES_MAIL,    "MAIL",   FieldIsShortText);
    addField(Table_Sites,  SITES_CONTACT, "CONTACT",   FieldIsShortText);
    addField(Table_Sites,  SITES_PREF,    "PREFERRED",  FieldIsBoolean);

//    "CREATE TABLE	sites ("  --> sites
//                    "ID_Primkey  	bigint(10)  	 	  				NOT NULL 	 auto_increment  ,"
//                    "id_site 	varchar(10) 		COLLATE utf8_unicode_ci 	NOT NULL			,"
//                    "site 		varchar(100) 		COLLATE utf8_unicode_ci 	NULL 				,"
//                    "adresse_site 	varchar(100) 		COLLATE utf8_unicode_ci  	NULL 				,"
//                    "cp_site 	varchar(5) 		COLLATE utf8_unicode_ci		NULL 				,"
//                    "ville_site 	varchar(100) 		COLLATE utf8_unicode_ci  	NULL 				,"
//                    "telsite 	varchar(50) 		COLLATE utf8_unicode_ci 	NULL 				,"
//                    "faxsite 	varchar(50) 		COLLATE utf8_unicode_ci 	NULL 				,"
//                    "mailsite 	varchar(100) 		COLLATE utf8_unicode_ci 	NULL				,"
//                    "PRIMARY KEY(ID_Primkey));";

//    addTable(Table_Users, "users");
    addField(Table_Users,  USER_ID,      "USER_ID",      FieldIsUniquePrimaryKey);
    addField(Table_Users,  USER_UID,     "USER_UID",     FieldIsUUID);
    addField(Table_Users,  USER_LOG,     "LOG",          FieldIsShortText);
    addField(Table_Users,  USER_PASS,    "PASS",         FieldIsShortText);
    addField(Table_Users,  USER_EMR_UID, "EMR_USER_UID", FieldIsShortText);
//    "CREATE TABLE	utilisateurs ("  --> users
//                    "id_usr  	int(10)  	 	UNSIGNED  			NOT NULL  	 auto_increment ,"
//                    "nom_usr 	varchar(150) 		COLLATE utf8_unicode_ci		NULL 				,"
//                    "login 	varchar(15) 			COLLATE utf8_unicode_ci		NOT NULL			,"
//                    "password 	varchar(50) 		COLLATE utf8_unicode_ci		NOT NULL			,"
//                    "id_drtux_usr 	int(11) 			 			NULL 				,"
//                    "PRIMARY KEY(id_usr));";

    addField(Table_Percent,  PERCENT_ID,      "PCT_ID",      FieldIsUniquePrimaryKey);
    addField(Table_Percent,  PERCENT_UID,     "PCT_UID",     FieldIsUUID);
    addField(Table_Percent,  PERCENT_USER_UID,"PCT_USER_UID",FieldIsUUID);
    addField(Table_Percent,  PERCENT_TYPE,    "TYPE",        FieldIsShortText);
    addField(Table_Percent,  PERCENT_VALUES,  "VALUE",       FieldIsReal);
//    "CREATE TABLE	pourcentages ("  --> pourcentages
//                            "id_pourcent	int(10)		UNSIGNED			NOT NULL	auto_increment ,"
//                            "type		varchar(100)	COLLATE utf8_unicode_ci		NULL				,"
//                            "pourcentage	varchar(6)	COLLATE utf8_unicode_ci		NOT NULL			,"
//                            "PRIMARY KEY(id_pourcent));";

    addField(Table_Rules,  RULES_ID,      "RULES_ID",      FieldIsUniquePrimaryKey);
    addField(Table_Rules,  RULES_UID,     "RULES_UID",     FieldIsUUID);
    addField(Table_Rules,  RULES_TYPE,    "TYPE",          FieldIsShortText);
    addField(Table_Rules,  RULES_VALUES,  "VALUES",        FieldIsShortText);
    addField(Table_Rules,  RULES_PREF,    "PREFERRED",      FieldIsBoolean);


    addField(Table_DistanceRules,  DISTRULES_ID,      "DISTRULES_ID",      FieldIsUniquePrimaryKey);
    addField(Table_DistanceRules,  DISTRULES_UID,     "DISTRULES_UID",     FieldIsUUID);
    addField(Table_DistanceRules,  DISTRULES_TYPE,    "TYPE",              FieldIsShortText);
    addField(Table_DistanceRules,  DISTRULES_VALUES,  "VALUES",            FieldIsShortText);
    addField(Table_DistanceRules,  DISTRULES_MIN_KM,  "MINIMUM_KILOMETERS",FieldIsShortText);
    addField(Table_DistanceRules,  DISTRULES_PREF,    "PREFERRED",          FieldIsBoolean);

    addField(Table_Thesaurus,  THESAURUS_ID,      "THESAURUS_ID",      FieldIsUniquePrimaryKey);
    addField(Table_Thesaurus,  THESAURUS_UID,     "THESAURUS_UID",     FieldIsUUID);
    addField(Table_Thesaurus,  THESAURUS_USERUID, "THESAURUS_USERUID", FieldIsUUID);
    addField(Table_Thesaurus,  THESAURUS_VALUES,  "VALUES",            FieldIsShortText);
    addField(Table_Thesaurus,  THESAURUS_PREF,    "PREFERRED",          FieldIsBoolean);

//    addTable(Table_Session, "session");
//    addField(Table_Session,  PAIE_ID,          "BANKING_ID",      FieldIsUniquePrimaryKey);
//    addField(Table_Session,  PAIE_ACCOUNT_UID, "_UID",     FieldIsUUID);
//    addField(Table_Session,  PAIE_DATE,        "",     FieldIs);
//    addField(Table_Session,  PAIE_INSURANCE,   "",     FieldIs);
//    addField(Table_Session,  PAIE_USER_UID,    "",     FieldIs);
//    addField(Table_Session,  PAIE_SITE_UID,    "",     FieldIs);
//    addField(Table_Session,  PAIE_MEDICPROC,   "",     FieldIs);
//    addField(Table_Session,  PAIE_CASHACTS,      "CASH",           FieldIsReal);
//    addField(Table_Session,  PAIE_CHEQUEACTS,    "CHEQUE",         FieldIsReal);
//    addField(Table_Session,  PAIE_VISAACTS,      "VISA",           FieldIsReal);
//    addField(Table_Session,  PAIE_INSURANCEACTS, "INSURANCE",      FieldIsReal);
//    addField(Table_Session,  PAIE_OTHERACTS,     "OTHER",          FieldIsReal);
//    addField(Table_Session,  PAIE_DUEACTS,       "DUE",            FieldIsReal);
//    addField(Table_Session,  PAIE_DUEBY,           "DUE_BY",         FieldIsShortText);
//    addField(Table_Session,  PAIE_ISVALID,         "ISVALID",        FieldIsBoolean);
//    addField(Table_Session,  PAIE_TRACE,           "TRACE",          FieldIsBlob);

//    "CREATE TABLE	seances  ("  --> paiement ?
//                          "id_paiement    int(11)			unsigned			NOT NULL     	auto_increment ,"
//                          "id_hono  	int(11)  	 	UNSIGNED  			NOT NULL	 		,"
//                          "date 		date 							NOT NULL 			,"
//                          "id_payeurs 	bigint(20) 						NOT NULL 			,"
//                          "id_usr         varchar(10)                                             NULL                            ,"
//                          "id_site        varchar(10)                                             NULL                            ,"
//                          "acte 		text                    COLLATE  utf8_unicode_ci	NULL 				,"
//                          "esp 		double 							NOT NULL 			,"
//                          "chq 		double 							NOT NULL 			,"
//                          "cb 		double 							NOT NULL			,"
//                          "daf 		double 							NOT NULL 			,"
//                          "autre 		double 							NOT NULL 			,"
//                          "du 		double 							NOT NULL 			,"
//                          "valide 	tinyint(1) 						NULL				,"
//                          "PRIMARY KEY(id_paiement));";

//    addTable(Table_Banking, "banking");
//    addField(Table_Banking,  BANKING_ID,      "BANKING_ID",      FieldIsUniquePrimaryKey);
//    addField(Table_Banking,  BANKING_UID,     "_UID",     FieldIsUUID);
//    "CREATE TABLE IF NOT EXISTS rapprochement_bancaire ("  --> banking_cycle ??
//                  "id_rap_bank   int(11) UNSIGNED  NOT NULL  auto_increment ,"
//                  "id_compte     int(11) UNSIGNED  null,"
//                  "solde         double NULL,"
//                  "PRIMARY KEY(id_rap_bank));";

//    addTable(Table_NameIndex, "nameindex");
//   "CREATE TABLE IF NOT EXISTS  nameindex ("  --> nameindex
//          "id_index         INTEGER PRIMARY KEY,"
//          "name             varchar(50)                NULL,"
//          "surname          varchar(50)               NULL,"
//          "guid             varchar(6)                NOT NULL);";

    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

/** \brief Destructor. */
AccountBase::~AccountBase()
{
    if (Utils::Log::debugPluginsCreation())
        WARN_FUNC;
    if (d)
        delete d;
    d=0;
}

bool AccountBase::initialize()
{
    // only one base can be initialized
    if (d->m_initialized)
        return true;

    // connect
    if (commandLine()->value(Core::ICommandLine::ClearUserDatabases).toBool()) {
        createConnection(Constants::DB_ACCOUNTANCY, Constants::DB_ACCOUNTANCY,
                         settings()->databaseConnector(),
                         Utils::Database::DeleteAndRecreateDatabase);
    } else {
        createConnection(Constants::DB_ACCOUNTANCY, Constants::DB_ACCOUNTANCY,
                         settings()->databaseConnector(),
                         Utils::Database::CreateDatabase);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ACCOUNTANCY).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
    }

    // TODO: start a global transaction for all this code, remove the transactions inside the sub-members, close the transaction
    if (!checkDatabaseScheme()) {
        if(checkIfIsFirstVersion()){
            qDebug() << __FILE__ << QString::number(__LINE__) << "ISFIRSTVERSION";
            if (fieldNamesSql(AccountDB::Constants::Table_MedicalProcedure).size()< AccountDB::Constants::MP_MaxParam) {
                if (!alterTableForNewField(AccountDB::Constants::Table_MedicalProcedure, AccountDB::Constants::MP_OTHERS,FieldIsBlob, QString("NULL"))
                       && !alterTableForNewField(AccountDB::Constants::Table_MedicalProcedure, AccountDB::Constants::MP_COUNTRY,FieldIsBlob, QString("NULL"))) {
                    LOG_ERROR("Unable to add new field in table MP");
                    return false;
                } else {
                    foreach(QString field,fieldNamesSql(AccountDB::Constants::Table_MedicalProcedure)) {
                        qWarning() << __FILE__ << QString::number(__LINE__) << " field =" << field ;
                    }
                    LOG("New Version = "+checkAndReplaceVersionNumber());
                    return true;
                }
            }

        } else {
            LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_ACCOUNTANCY));
            return false;
        }
    }  //checkDatabaseScheme

    if (checkIfVersionBeforeThirdVersion() && driver() == Utils::Database::MySQL) {
        if (alterFieldPatientNameIntToVarchar()) {
            LOG("Field PATIENT_NAME has been changed to varchar");
        } else {
            LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_ACCOUNTANCY));
            return false;
        }
    }
    // End of todo commit transaction

    if (versionHasChanged()) {
        LOG("Version has changed , new version = " + checkAndReplaceVersionNumber());
    }

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    d->m_initialized = true;
    return true;
}

bool AccountBase::isInitialized() const
{
    return d->m_initialized;
}
void AccountBase::logChronos(bool log)
{
    d->m_LogChrono = log;
}

bool AccountBase::createDatabase(const QString &connectionName ,
                                 const QString &dbName,
                                 const QString &pathOrHostName,
                                 TypeOfAccess ,
                                 AvailableDrivers driver,
                                 const QString & login,
                                 const QString & pass,
                                 const int port,
                                 CreationOption
                                 )
{
    // TODO: manage createOption
    if (connectionName != Constants::DB_ACCOUNTANCY)
        return false;

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
        .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                LOG(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        DB.open();
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "__ACCOUNTANCY_CREATOR");
            d.setHostName(pathOrHostName);
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(DB.connectionName()).arg(DB.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("__ACCOUNTANCY_CREATOR"))
            QSqlDatabase::removeDatabase("__ACCOUNTANCY_CREATOR");
        DB.open();
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                         .arg(dbName, DB.lastError().text()));
        return false;
    }

    // Add version number
    DB.transaction();
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_VERSION));
    query.bindValue(Constants::VERSION_ACTUAL, Constants::DB_VERSION_NUMBER);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.commit();
        return false;
    }
    query.finish();
    DB.rollback();
    return true;
}

/*!
 * \brief Queries the database to find an account with the given \uid.
 * \return A pointer to a newly created \sa AccountData object, filled with
 * all fields of this database record.
 */
AccountData *AccountBase::getAccountByUid(const QString &uid)
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if (!connectDatabase(DB, __LINE__))
        return 0;
    if (uid.isEmpty())
        return 0;

    // construct the where clause
    QHash<int, QString> where;
    where.insert(Constants::ACCOUNT_UID, QString("=%1").arg(uid));
    QString req = select(Constants::Table_Account, where);
    DB.transaction();
    QSqlQuery query(req,DB);
    if (query.isActive()) {
        if (query.next()) {
            AccountData *data = new AccountData();
            // ref is a data representation of enum AccountDB::Constants::TableAcc_Fields
            for(int ref = 0 ; ref < Constants::ACCOUNT_MaxParam; ++ref) {
                data->setDataFromDb(ref, query.value(ref));
            }
            query.finish();
            DB.commit();
            return data;
        } else {
            LOG_ERROR("No account with an UID like " + uid);
            query.finish();
            DB.rollback();
            return 0;
        }
    } else {
        LOG_ERROR("No account with an UID like " + uid);
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return 0;
    }
    return 0;
}

void AccountBase::onCoreDatabaseServerChanged()
{
    d->m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_ACCOUNTANCY)) {
        QSqlDatabase::removeDatabase(Constants::DB_ACCOUNTANCY);
    }
    disconnect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    initialize();
}

void AccountBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

bool AccountBase::checkIfIsFirstVersion()
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if (!connectDatabase(DB, __LINE__))
        return false;
    QVariant version;
    DB.transaction();
    QSqlQuery qy(DB);
    QString req = select(Constants::Table_VERSION, Constants::VERSION_ACTUAL);//QString("SELECT %1 FROM %2").arg("ACTUAL","VERSION");
    if (!qy.exec(req)) {
        LOG_QUERY_ERROR(qy);
        qy.finish();
        DB.rollback();
        return false;
    }
    while (qy.next()) {
        version = qy.value(0);
    }
    qy.finish();
    DB.commit();
    if (version == QVariant("0.1")) {
        LOG(QString("VERSION == 0.1"));
        return true;
    }
    return false;
}

QString AccountBase::checkAndReplaceVersionNumber()
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if (!connectDatabase(DB, __LINE__))
        return 0;
    // if (versionHasChanged())
    // {
    DB.transaction();
    QSqlQuery qy(DB);
    /*QString req = QString("INSERT INTO %1 (%2) VALUES ('%3')")
                       .arg("VERSION","ACTUAL",QString(Constants::VERSION_ACTUAL));*/
    qy.prepare(prepareInsertQuery(Constants::Table_VERSION));
    qy.bindValue(Constants::VERSION_ACTUAL, Constants::DB_VERSION_NUMBER);

    if (!qy.exec()) {
        LOG_QUERY_ERROR(qy);
        qy.finish();
        DB.rollback();
        return QString(qy.lastError().text());
    }
    qy.finish();
    DB.commit();
    return QString(Constants::DB_VERSION_NUMBER);
    //   }
}

bool AccountBase::versionHasChanged()
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if (!connectDatabase(DB, __LINE__))
        return 0;
    QString version;
    DB.transaction();
    QSqlQuery qy(DB);
    QString req = select(Constants::Table_VERSION, Constants::VERSION_ACTUAL);//QString("SELECT %1 FROM %2").arg("ACTUAL","VERSION");
    if (!qy.exec(req)) {
        LOG_QUERY_ERROR(qy);
        return false;
    }
    while (qy.next()) {
        version = qy.value(0).toString();
    }
    qy.finish();
    DB.commit();
    if (!version.contains(QString(Constants::DB_VERSION_NUMBER))) {
        return true;
    }
    return false;
}

bool AccountBase::checkIfVersionBeforeThirdVersion()
{
    QString version;
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if (!connectDatabase(DB, __LINE__))
        return false;
    DB.transaction();
    QSqlQuery qy(DB);
    QString req = select(Constants::Table_VERSION, Constants::VERSION_ACTUAL);//QString("SELECT %1 FROM %2").arg("ACTUAL","VERSION");
    if (!qy.exec(req))  {
        LOG_QUERY_ERROR(qy);
        DB.rollback();
        return false;
    }
    while (qy.next())  {
        version = qy.value(0).toString();
    }
    qy.finish();
    DB.commit();
    double versionDouble = version.toDouble();
    if (versionDouble == 0.0) {
        LOG("Alter field PATIENT_NAME: Error conversion of double");
    }
    if (versionDouble < 0.3) {
        return true;
    }
    return false;
}

bool AccountBase::alterFieldPatientNameIntToVarchar()
{
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if (!connectDatabase(DB, __LINE__))
        return false;
    QString tableName = table(AccountDB::Constants::Table_Account);
    QString fieldPatientName    = fieldName(AccountDB::Constants::Table_Account,AccountDB::Constants::ACCOUNT_PATIENT_NAME);
    QString fieldPatientUuid = fieldName(AccountDB::Constants::Table_Account,AccountDB::Constants::ACCOUNT_PATIENT_UID);
    if (WarnDebugMessage)
    qDebug() << __FILE__ << QString::number(__LINE__) << " table + fielName ="
    << tableName+" + "+fieldPatientName  ;
    DB.transaction();
    QSqlQuery qy(DB);
    QString req = QString("ALTER TABLE %1 MODIFY %2 varchar(2000) NULL , MODIFY %3 varchar(200) NULL ;")
                         .arg(tableName,fieldPatientName,fieldPatientUuid);
    if (!qy.exec(req)) {
        LOG_QUERY_ERROR(qy);
        qy.finish();
        DB.rollback();
        return false;
    }
    qy.finish();
    DB.commit();
    return true;
}
