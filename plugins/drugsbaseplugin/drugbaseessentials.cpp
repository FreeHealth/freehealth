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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "drugbaseessentials.h"
#include <drugsbaseplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/dialogs/settingsdialog.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>

using namespace DrugsDB;
using namespace DrugsDB::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline QString databaseFileName() {return settings()->databasePath() + QDir::separator() + QString(Constants::DB_DRUGS_NAME) + QDir::separator() + QString(Constants::DB_DRUGS_FILENAME);}

namespace {
#if DRUGS_DATABASE_VERSION == 0x000804
const char * const CURRENTVERSION = "0.8.4";
#elif DRUGS_DATABASE_VERSION == 0x000604
const char * const CURRENTVERSION = "0.6.4";
#else
const char * const CURRENTVERSION = "0.0.0";
#endif

struct ftype {
    ftype(int _f, Utils::Database::TypeOfField _t) : f(_f), t(_t) {}
    int f;
    Utils::Database::TypeOfField t;
};

}

static inline bool connectDatabase(QSqlDatabase &DB, const QString &file, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("DrugBaseEssentials", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 file, line);
            return false;
        }
    }
    return true;
}

DrugBaseEssentials::DrugBaseEssentials():
    Utils::Database(),
    m_dbcore_initialized(false),
    m_isDefaultDb(false)
{
    setConnectionName(Constants::DB_DRUGS_NAME);
    using namespace Constants;
    QMultiHash<int, ftype> types;
    int i = Table_MASTER;
    types.insertMulti(i, ftype(MASTER_DID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(MASTER_UID1, FieldIsUUID));
    types.insertMulti(i, ftype(MASTER_UID2, FieldIsUUID));
    types.insertMulti(i, ftype(MASTER_UID3, FieldIsUUID));
    types.insertMulti(i, ftype(MASTER_OLDUID,FieldIsUUID));
    types.insertMulti(i, ftype(MASTER_SID, FieldIsInteger));
    i = Table_SOURCES;
    types.insertMulti(i, ftype(SOURCES_SID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(SOURCES_DBUID, FieldIsUUID));
    types.insertMulti(i, ftype(SOURCES_MASTERLID, FieldIsInteger));
    types.insertMulti(i, ftype(SOURCES_LANG, FieldIsLanguageText));
    types.insertMulti(i, ftype(SOURCES_WEB, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_COPYRIGHT, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_LICENSE, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_DATE, FieldIsDateTime));
    types.insertMulti(i, ftype(SOURCES_DRUGS_VERSION,FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_AUTHORS, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_VERSION, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_PROVIDER, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_WEBLINK, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_DRUGUID_NAME, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_ATC, FieldIsBoolean));
    types.insertMulti(i, ftype(SOURCES_INTERACTIONS, FieldIsBoolean));
    types.insertMulti(i, ftype(SOURCES_COMPL_WEBSITE,FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_PACKUID_NAME,FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_COMPLETION, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_AUTHOR_COMMENTS, FieldIsLongText));
    types.insertMulti(i, ftype(SOURCES_DRUGNAMECONSTRUCTOR, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_FMFCOMPAT, FieldIsShortText));
    types.insertMulti(i, ftype(SOURCES_OPENREACT_COMPAT, FieldIsShortText));
    i = Table_LABELS;
    types.insertMulti(i, ftype(LABELS_LID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(LABELS_LANG, FieldIsLanguageText));
    types.insertMulti(i, ftype(LABELS_LABEL, FieldIsShortText));
    i = Table_LABELSLINK;
    types.insertMulti(i, ftype(LABELSLINK_MASTERLID, FieldIsInteger));
    types.insertMulti(i, ftype(LABELSLINK_LID, FieldIsInteger));
    i= Table_BIB;
    types.insertMulti(i, ftype(BIB_BIBID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(BIB_TYPE, FieldIsShortText));
    types.insertMulti(i, ftype(BIB_LINK, FieldIsShortText));
    types.insertMulti(i, ftype(BIB_TEXTREF, FieldIsShortText));
    types.insertMulti(i, ftype(BIB_ABSTRACT, FieldIsLongText));
    types.insertMulti(i, ftype(BIB_EXPLAIN, FieldIsLongText));
    types.insertMulti(i, ftype(BIB_XML, FieldIsBlob));
    i = Table_BIB_LINK;
    types.insertMulti(i, ftype(BIB_LINK_MASTERID, FieldIsInteger));
    types.insertMulti(i, ftype(BIB_LINK_BIBID, FieldIsInteger));
    i = Table_DRUGS;
    types.insertMulti(i, ftype(DRUGS_ID , FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(DRUGS_DID ,FieldIsInteger));
    types.insertMulti(i, ftype(DRUGS_SID,  FieldIsInteger));
    types.insertMulti(i, ftype(DRUGS_NAME, FieldIsShortText));
    types.insertMulti(i, ftype(DRUGS_ATC_ID, FieldIsInteger));
    types.insertMulti(i, ftype(DRUGS_STRENGTH,FieldIsShortText));
    types.insertMulti(i, ftype(DRUGS_VALID, FieldIsBoolean));
    types.insertMulti(i, ftype(DRUGS_MARKET, FieldIsBoolean));
    types.insertMulti(i, ftype(DRUGS_AID_MASTER_LID, FieldIsInteger));
    types.insertMulti(i, ftype(DRUGS_LINK_SPC, FieldIsShortText));
    types.insertMulti(i, ftype(DRUGS_EXTRA_XML,FieldIsLongText));
    i = Table_MOLS;
    types.insertMulti(i, ftype(MOLS_MID,FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(MOLS_SID, FieldIsInteger));
    types.insertMulti(i, ftype(MOLS_NAME,FieldIsShortText));
    types.insertMulti(i, ftype(MOLS_WWW, FieldIsShortText));
    i = Table_COMPO;
    types.insertMulti(i, ftype(COMPO_ID,FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(COMPO_DID,FieldIsInteger));
    types.insertMulti(i, ftype(COMPO_MID,FieldIsInteger));
    types.insertMulti(i, ftype(COMPO_STRENGTH, FieldIsShortText));
    types.insertMulti(i, ftype(COMPO_STRENGTH_NID, FieldIsInteger));
    types.insertMulti(i, ftype(COMPO_DOSE_REF, FieldIsShortText));
    types.insertMulti(i, ftype(COMPO_REF_NID, FieldIsInteger));
    types.insertMulti(i, ftype(COMPO_NATURE, FieldIsInteger));
    types.insertMulti(i, ftype(COMPO_LK_NATURE, FieldIsInteger));
    i = Table_UNITS;
    types.insertMulti(i, ftype(UNITS_NID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(UNITS_VALUE, FieldIsShortText));
    i = Table_LK_MOL_ATC;
    types.insertMulti(i, ftype(LK_MID,FieldIsInteger));
    types.insertMulti(i, ftype(LK_ATC_ID, FieldIsInteger));
    types.insertMulti(i, ftype(LK_ATC_SID,FieldIsInteger));
    i = Table_PACKAGING;
    types.insertMulti(i, ftype(PACK_DID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(PACK_SID,  FieldIsInteger));
    types.insertMulti(i, ftype(PACK_PACK_UID,FieldIsUUID));
    types.insertMulti(i, ftype(PACK_LABEL,  FieldIsShortText));
    types.insertMulti(i, ftype(PACK_STATUS, FieldIsOneChar));
    types.insertMulti(i, ftype(PACK_MARKET, FieldIsInteger));
    types.insertMulti(i, ftype(PACK_DATE,   FieldIsDateTime));
    types.insertMulti(i, ftype(PACK_OPTION_CODE, FieldIsShortText));
    i = Table_DRUG_ROUTES;
    types.insertMulti(i, ftype(DRUG_ROUTES_DID, FieldIsInteger));
    types.insertMulti(i, ftype(DRUG_ROUTES_RID, FieldIsInteger));
    i = Table_DRUG_FORMS;
    types.insertMulti(i, ftype(DRUG_FORMS_DID,FieldIsInteger));
    types.insertMulti(i, ftype(DRUG_FORMS_MASTERLID,FieldIsInteger));
    i = Table_ROUTES;
    types.insertMulti(i, ftype(ROUTES_RID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(ROUTES_MASTERLID, FieldIsInteger));
    types.insertMulti(i, ftype(ROUTES_SYSTEMIC, FieldIsShortText));
    i = Table_SEARCHENGINES;
    types.insertMulti(i, ftype(SEARCHENGINE_ID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(SEARCHENGINE_LABEL, FieldIsShortText));
    types.insertMulti(i, ftype(SEARCHENGINE_URL,FieldIsShortText));
    i = Table_VERSION;
    types.insertMulti(i, ftype(VERSION_ID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(VERSION_VERSION, FieldIsShortText));
    types.insertMulti(i, ftype(VERSION_DATE, FieldIsDate));
    types.insertMulti(i, ftype(VERSION_COMMENT, FieldIsLongText));
    i = Table_ATC;
    types.insertMulti(i, ftype(ATC_ID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(ATC_CODE, FieldIsShortText));
    types.insertMulti(i, ftype(ATC_WARNDUPLICATES, FieldIsBoolean));
    i = Table_INTERACTIONS;
    types.insertMulti(i, ftype(INTERACTIONS_IAID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(INTERACTIONS_ATC_ID1, FieldIsInteger));
    types.insertMulti(i, ftype(INTERACTIONS_ATC_ID2, FieldIsInteger));
    i = Table_IAKNOWLEDGE;
    types.insertMulti(i, ftype(IAKNOWLEDGE_IAKID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(IAKNOWLEDGE_TYPE, FieldIsShortText));
    types.insertMulti(i, ftype(IAKNOWLEDGE_RISK_MASTERLID, FieldIsInteger));
    types.insertMulti(i, ftype(IAKNOWLEDGE_MANAGEMENT_MASTERLID, FieldIsInteger));
    types.insertMulti(i, ftype(IAKNOWLEDGE_BIB_MASTERID, FieldIsInteger));
    types.insertMulti(i, ftype(IAKNOWLEDGE_WWW, FieldIsShortText));
    i = Table_IA_IAK;
    types.insertMulti(i, ftype(IA_IAK_IAID, FieldIsInteger));
    types.insertMulti(i, ftype(IA_IAK_IAKID, FieldIsInteger));
    i = Table_ATC_LABELS;
    types.insertMulti(i, ftype(ATC_LABELS_ATCID, FieldIsInteger));
    types.insertMulti(i, ftype(ATC_LABELS_MASTERLID, FieldIsInteger));
    i = Table_IAM_TREE;
    types.insertMulti(i, ftype(IAM_TREE_ID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(IAM_TREE_ID_CLASS, FieldIsInteger));
    types.insertMulti(i, ftype(IAM_TREE_ID_ATC, FieldIsInteger));
    types.insertMulti(i, ftype(IAM_TREE_BIBMASTERID,FieldIsInteger));
    i = Table_PIM_SOURCES;
    types.insertMulti(i, ftype(PIM_SOURCES_SID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(PIM_SOURCES_UID, FieldIsUUID));
    types.insertMulti(i, ftype(PIM_SOURCES_NAME, FieldIsShortText));
    types.insertMulti(i, ftype(PIM_SOURCES_PMID, FieldIsShortText));
    types.insertMulti(i, ftype(PIM_SOURCES_COUNTRY,FieldIsShortText));
    types.insertMulti(i, ftype(PIM_SOURCES_WWW, FieldIsShortText));
    i = Table_PIM_TYPES;
    types.insertMulti(i, ftype(PIM_TYPES_TID,FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(PIM_TYPES_UID,FieldIsUUID));
    types.insertMulti(i, ftype(PIM_TYPES_MASTER_LID, FieldIsInteger));
    i = Table_PIMS;
    types.insertMulti(i, ftype(PIMS_ID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(PIMS_SID, FieldIsInteger));
    types.insertMulti(i, ftype(PIMS_TID, FieldIsInteger));
    types.insertMulti(i, ftype(PIMS_LEVEL, FieldIsInteger));
    types.insertMulti(i, ftype(PIMS_RISK_MASTER_LID, FieldIsInteger));
    i = Table_PIMS_RELATED_ATC;
    types.insertMulti(i, ftype(PIMS_RELATC_RMID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(PIMS_RELATC_PIM_ID, FieldIsInteger));
    types.insertMulti(i, ftype(PIMS_RELATC_ATC_ID, FieldIsInteger));
    types.insertMulti(i, ftype(PIMS_RELATC_MAXDAYDOSE, FieldIsReal));
    types.insertMulti(i, ftype(PIMS_RELATC_MAXDAYDOSEUNIT, FieldIsInteger));
    i = Table_PIMS_RELATED_ICD;
    types.insertMulti(i, ftype(PIMS_RELICD_RMID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(PIMS_RELICD_PIM_ID, FieldIsInteger));
    types.insertMulti(i, ftype(PIMS_RELICD_ICD_SID, FieldIsInteger));
    i = Table_CURRENTVERSION;
    types.insertMulti(i, ftype(CURRENTVERSION_ID, FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(CURRENTVERSION_NUMBER, FieldIsShortText));
#if DRUGS_DATABASE_VERSION >= 0x000804
    i = Table_DRUG_SPC;
    types.insertMulti(i, ftype(DRUG_SPC_DID,FieldIsInteger));
    types.insertMulti(i, ftype(DRUG_SPC_SPCCONTENT_ID,FieldIsInteger));
    i = Table_SPC_CONTENT;
    types.insertMulti(i, ftype(SPCCONTENT_ID,FieldIsUniquePrimaryKey));
    types.insertMulti(i, ftype(SPCCONTENT_LABEL,FieldIsLongText));
    types.insertMulti(i, ftype(SPCCONTENT_URL_SOURCE,FieldIsShortText));
    types.insertMulti(i, ftype(SPCCONTENT_DATEOFDOWNLOAD,FieldIsDateTime));
    types.insertMulti(i, ftype(SPCCONTENT_HTMLCONTENT,FieldIsBlob));
#endif

    for(int i=0; i < Table_MaxParam; ++i) {
        addTable(i, "t" + QString::number(i));
        const QList<ftype> &tp = types.values(i);
        for(int j=0; j < tp.count(); ++j) {
            addField(i, tp.at(j).f, "f"+QString::number(tp.count()-j), tp.at(j).t);
        }
    }
    addIndex(Table_SOURCES, SOURCES_SID);
    addIndex(Table_MASTER, MASTER_DID);
    addIndex(Table_LABELS, LABELS_LID);
    addIndex(Table_LABELSLINK, LABELSLINK_LID);
    addIndex(Table_LABELSLINK, LABELSLINK_MASTERLID);
    addIndex(Table_BIB, BIB_BIBID);
    addIndex(Table_BIB_LINK, BIB_LINK_BIBID);
    addIndex(Table_BIB_LINK, BIB_LINK_MASTERID);
    addIndex(Table_DRUGS, DRUGS_DID);
    addIndex(Table_MOLS, MOLS_MID);
    addIndex(Table_COMPO, COMPO_DID);
    addIndex(Table_COMPO, COMPO_MID);
    addIndex(Table_UNITS, UNITS_NID);
    addIndex(Table_LK_MOL_ATC, LK_MID);
    addIndex(Table_LK_MOL_ATC, LK_ATC_ID);
    addIndex(Table_LK_MOL_ATC, LK_ATC_SID);
    addIndex(Table_DRUG_ROUTES, DRUG_ROUTES_DID);
    addIndex(Table_DRUG_ROUTES, DRUG_ROUTES_RID);
    addIndex(Table_DRUG_FORMS, DRUG_FORMS_DID);
    addIndex(Table_DRUG_FORMS, DRUG_FORMS_MASTERLID);
    addIndex(Table_ROUTES, ROUTES_RID);
    addIndex(Table_ROUTES, ROUTES_MASTERLID);
    addIndex(Table_ATC, ATC_CODE);
    addIndex(Table_ATC, ATC_ID);
    addIndex(Table_INTERACTIONS, INTERACTIONS_IAID);
    addIndex(Table_INTERACTIONS, INTERACTIONS_ATC_ID1);
    addIndex(Table_INTERACTIONS, INTERACTIONS_ATC_ID2);

    addIndex(Table_IAKNOWLEDGE, IAKNOWLEDGE_IAKID);
    addIndex(Table_IAKNOWLEDGE, IAKNOWLEDGE_RISK_MASTERLID);
    addIndex(Table_IAKNOWLEDGE, IAKNOWLEDGE_MANAGEMENT_MASTERLID);
    addIndex(Table_IAKNOWLEDGE, IAKNOWLEDGE_BIB_MASTERID);
    addIndex(Table_IA_IAK, IA_IAK_IAID);
    addIndex(Table_IA_IAK, IA_IAK_IAKID);
    addIndex(Table_ATC_LABELS, ATC_LABELS_ATCID);
    addIndex(Table_ATC_LABELS, ATC_LABELS_MASTERLID);
    addIndex(Table_IAM_TREE, IAM_TREE_ID_CLASS);
    addIndex(Table_IAM_TREE, IAM_TREE_ID_ATC);
    addIndex(Table_IAM_TREE, IAM_TREE_BIBMASTERID);
    addIndex(Table_PIMS_RELATED_ATC, PIMS_RELATC_RMID);
    addIndex(Table_PIMS_RELATED_ATC, PIMS_RELATC_PIM_ID);
    addIndex(Table_PIMS_RELATED_ATC, PIMS_RELATC_ATC_ID);
    addIndex(Table_PIMS_RELATED_ICD, PIMS_RELICD_RMID);
    addIndex(Table_PIMS_RELATED_ICD, PIMS_RELICD_PIM_ID);
    addIndex(Table_PIMS_RELATED_ICD, PIMS_RELICD_ICD_SID);
    addIndex(Table_PIMS, PIMS_ID);
    addIndex(Table_PIM_TYPES, PIM_TYPES_TID);
    addIndex(Table_PIM_SOURCES, PIM_SOURCES_SID);

#if DRUGS_DATABASE_VERSION >= 0x000804
    addIndex(Table_DRUG_SPC, DRUG_SPC_DID);
    addIndex(Table_DRUG_SPC, DRUG_SPC_SPCCONTENT_ID);
    addIndex(Table_SPC_CONTENT, SPCCONTENT_ID);
#endif
}

/** Force the re-initialization of the database. Call initialize() after this. */
void DrugBaseEssentials::forceFullDatabaseRefreshing()
{
    m_dbcore_initialized = false;
}

/**
 * Initialize the database (create it is required). \n
 * By default, the connection is defined to DrugsDB::Constants::DB_DRUGS_NAME.
 * If you want your own connection name, you can set it with the
 * setConnectionName() and you must set it \b before calling initialize().
 */
bool DrugBaseEssentials::initialize(const QString &pathToDb, bool createIfNotExists)
{
    if (m_dbcore_initialized)
        return true;
    setDriver(Utils::Database::SQLite);

    // test driver
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        LOG_ERROR_FOR("DrugBaseEssentials", tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE).arg("SQLite"));
        Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
                                 tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL).arg("SQLite"),
                                 "", qApp->applicationName());
        return false;
    }

    // Connect Drugs Database
    Utils::DatabaseConnector drugConnector;
    QString path = pathToDb;
    if (!QFileInfo(pathToDb).isDir())
        path = QFileInfo(pathToDb).absolutePath();
//    if (!path.endsWith(Constants::DB_DRUGS_NAME)) {
//        path.append(QDir::separator() + QString(Constants::DB_DRUGS_NAME));
//    }
    drugConnector.setAbsPathToReadOnlySqliteDatabase(path);
    drugConnector.setHost(QFileInfo(databaseFileName()).fileName());
    drugConnector.setAccessMode(Utils::DatabaseConnector::ReadOnly);
    drugConnector.setDriver(Utils::Database::SQLite);

    LOG_FOR("DrugBaseEssentials", tkTr(Trans::Constants::SEARCHING_DATABASE_1_IN_PATH_2).arg(connectionName()).arg(pathToDb));

    if (createIfNotExists) {
        createConnection(connectionName(), Constants::DB_DRUGS_FILENAME,
                         drugConnector,
                         Utils::Database::CreateDatabase);
    } else {
        createConnection(connectionName(), Constants::DB_DRUGS_FILENAME,
                         drugConnector,
                         Utils::Database::WarnOnly);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR_FOR("DrugBaseEssentials",tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(connectionName()).arg(database().lastError().text()));
        } else {
            LOG_FOR("DrugBaseEssentials",tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG_FOR("DrugBaseEssentials", tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR_FOR("DrugBaseEssentials",tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(connectionName()));
        return false;
    }

    if (!checkDatabaseVersion()) {
        LOG_ERROR_FOR("DrugBaseEssentials", QString("Wrong database version. Db: %1; Current: %2").arg(version()).arg(::CURRENTVERSION));
        return false;
    } else {
        LOG_FOR("DrugBaseEssentials", QString("Using drug database version " + version()));
    }

    m_dbcore_initialized = true;
    return true;
}

void DrugBaseEssentials::setVersion(const QString &version)
{
    QSqlDatabase DB = QSqlDatabase::database(connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return;
    executeSQL(prepareDeleteQuery(Constants::Table_CURRENTVERSION, QHash<int,QString>()), DB);
    QSqlQuery query(DB);
    query.prepare(prepareInsertQuery(Constants::Table_CURRENTVERSION));
    query.bindValue(Constants::CURRENTVERSION_ID, QVariant());
    query.bindValue(Constants::CURRENTVERSION_NUMBER, version);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("DrugBaseEssentials", query);
    }
}

QString DrugBaseEssentials::version() const
{
    QSqlDatabase DB = QSqlDatabase::database(connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString();
    QSqlQuery query(DB);
    query.prepare(select(Constants::Table_CURRENTVERSION));
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("DrugBaseEssentials", query);
    } else {
        if (query.next()) {
            return query.value(Constants::CURRENTVERSION_NUMBER).toString();
        }
    }
    return QString();
}

bool DrugBaseEssentials::checkDatabaseVersion() const
{
    return (version()==::CURRENTVERSION);
}

int DrugBaseEssentials::getSourceId(const QString &drugsDbUid)
{
    QSqlDatabase DB = QSqlDatabase::database(connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return -1;
    QSqlQuery query(DB);
    QHash<int, QString> where;
    where.insert(Constants::SOURCES_DBUID, QString("='%1'").arg(drugsDbUid));
    query.prepare(select(Constants::Table_SOURCES, Constants::SOURCES_SID, where));
    if (query.exec()) {
        if (query.next()) {
            return query.value(0).toInt();
        }
    } else {
        LOG_QUERY_ERROR_FOR("DrugBaseEssentials", query);
    }
    return -1;
}

bool DrugBaseEssentials::createDatabase(const QString &connection, const QString &prefixedDbName,
                                  const Utils::DatabaseConnector &connector,
                                  CreationOption createOption
                                  )
{
//    WARN_FUNC << connectionName << prefixedDbName;
//    qWarning() << connector;

    if (connection != connectionName())
        return false;
    if (connector.driver() != SQLite) {
        return false;
    }
    if (createOption!=Utils::Database::CreateDatabase)
        return false;
    QString pathOrHostName = connector.absPathToSqliteReadOnlyDatabase() + QDir::separator() + QString(connectionName());
    LOG_FOR("DrugBaseEssentials", tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2).arg(prefixedDbName).arg(pathOrHostName));

    setConnectionName(connectionName());
    setDriver(connector.driver());

    // create an empty database and connect
    if (QSqlDatabase::connectionNames().contains(connectionName())) {
        QSqlDatabase::removeDatabase(connectionName());
    }

    if (!Utils::checkDir(pathOrHostName, true, "DrugBaseEssentials")) {
        LOG_ERROR_FOR("DrugBaseEssentials", tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        return false;
    }

    QSqlDatabase DB;
    DB = QSqlDatabase::addDatabase("QSQLITE", connectionName());
    DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + prefixedDbName));
    if (!DB.open())
        LOG_FOR("DrugBaseEssentials", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(prefixedDbName).arg(DB.lastError().text()));
    setDriver(Utils::Database::SQLite);

    // create db structure
    if (createTables()) {
        LOG_FOR("DrugBaseEssentials", tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(prefixedDbName));
    } else {
        LOG_ERROR_FOR("DrugBaseEssentials", tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                      .arg(prefixedDbName, DB.lastError().text()));
        return false;
    }

    setVersion(::CURRENTVERSION);

    // database is readable/writable
    LOG_FOR("DrugBaseEssentials", tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(pathOrHostName + QDir::separator() + prefixedDbName));
    return true;
}
