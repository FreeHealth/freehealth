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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Alert::Internal::AlertBase
  Alert database manager. For internal use.
  \sa Alert::AlertCore
*/

#include "alertbase.h"
#include "constants.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/icommandline.h>

#include <QDir>
#include <QTreeWidget>

using namespace Alert;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }

AlertBase::AlertBase(QObject *parent) :
    QObject(parent),
    Utils::Database()
{
    setObjectName("AlertBase");

    using namespace Alert::Constants;
    addTable(Table_ALERT, "ALR");
    addTable(Table_ALERT_LABELS, "LBL");
    addTable(Table_ALERT_SCRIPTS, "SCR");
    addTable(Table_ALERT_TIMING, "TIM");
    addTable(Table_ALERT_VERSION, "VER");

    addField(Table_ALERT, ALERT_ID, "A_ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT, ALERT_UID, "A_UID", FieldIsUUID);
    addField(Table_ALERT, ALERT_USER_UID, "U_UID", FieldIsUUID);
    addField(Table_ALERT, ALERT_GROUP_UID, "G_UID", FieldIsUUID);
    addField(Table_ALERT, ALERT_PATIENT_UID, "P_UID", FieldIsUUID);
    addField(Table_ALERT, ALERT_FAMILY_UID, "F_UID", FieldIsUUID);
    addField(Table_ALERT, ALERT_APP_NAME, "APP", FieldIsShortText);
    addField(Table_ALERT, ALERT_CATEGORY_UID, "C_UID", FieldIsUUID);
    addField(Table_ALERT, ALERT_SCRIPTS_ID, "SCR_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_ISVALID, "ISV", FieldIsInteger);

    addField(Table_ALERT, ALERT_VIEW_TYPE, "VIEW_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_CONTENT_TYPE, "CONTENT_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_TIMING_ID, "TIM_ID", FieldIsInteger);
    addField(Table_ALERT, ALERT_CONDITION_TYPE, "COND_ID", FieldIsInteger);

    addField(Table_ALERT, ALERT_LABEL_LABELID, "LBL_LID", FieldIsInteger);
    addField(Table_ALERT, ALERT_DESCRIPTION_LABELID, "DES_LID", FieldIsInteger);
    addField(Table_ALERT, ALERT_COMMENT_LABELID, "COM_LID", FieldIsInteger);
    addField(Table_ALERT, ALERT_CREATION_DATE, "C_DATE", FieldIsDate);
    addField(Table_ALERT, ALERT_LAST_UPDATE_DATE, "U_DATE", FieldIsDate);
    addField(Table_ALERT, ALERT_THEMED_ICON, "THM_ICON", FieldIsDate);
    addField(Table_ALERT, ALERT_THEME_CSS, "CSS", FieldIsLongText);
    addField(Table_ALERT, ALERT_CRYPTED_PASSWORD, "PASS", FieldIsShortText);
    addField(Table_ALERT, ALERT_EXTRA_XML, "EXTRA", FieldIsLongText);

    addIndex(Table_ALERT, ALERT_ID);
    addIndex(Table_ALERT, ALERT_UID);
    addIndex(Table_ALERT, ALERT_USER_UID);
    addIndex(Table_ALERT, ALERT_GROUP_UID);
    addIndex(Table_ALERT, ALERT_PATIENT_UID);
    addIndex(Table_ALERT, ALERT_FAMILY_UID);
    addIndex(Table_ALERT, ALERT_APP_NAME);
    addIndex(Table_ALERT, ALERT_CATEGORY_UID);

    // Translatable description items
    addField(Table_ALERT_LABELS, ALERT_LABELS_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_LABELS, ALERT_LABELS_LABELID, "LID", FieldIsInteger);
    addField(Table_ALERT_LABELS, ALERT_LABELS_LANG, "LNG", FieldIsLanguageText);
    addField(Table_ALERT_LABELS, ALERT_LABELS_VALUE, "VAL", FieldIsShortText);
    addField(Table_ALERT_LABELS, ALERT_LABELS_ISVALID, "ISV", FieldIsBoolean);
    addIndex(Table_ALERT_LABELS, ALERT_LABELS_ID);
    addIndex(Table_ALERT_LABELS, ALERT_LABELS_LABELID);
    addIndex(Table_ALERT_LABELS, ALERT_LABELS_LANG);

    // Timing
    addField(Table_ALERT_TIMING, ALERT_TIMING_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_TIMING, ALERT_TIMING_ISVALID, "ISV", FieldIsBoolean, "1");
    addField(Table_ALERT_TIMING, ALERT_TIMING_STARTDATETIME, "STR", FieldIsDateTime);
    addField(Table_ALERT_TIMING, ALERT_TIMING_ENDDATETIME, "END", FieldIsDateTime);
    addField(Table_ALERT_TIMING, ALERT_TIMING_CYCLES, "CYC", FieldIsInteger);
    addField(Table_ALERT_TIMING, ALERT_TIMING_CYCLINGDELAY, "CDY", FieldIsInteger);
    addField(Table_ALERT_TIMING, ALERT_TIMING_NEXTCYCLE, "NCY", FieldIsDateTime);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_ID);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_STARTDATETIME);
    addIndex(Table_ALERT_TIMING, ALERT_TIMING_ENDDATETIME);

    // Scripting
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPTS_SID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_UID, "S_UID", FieldIsUUID);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_ISVALID, "ISV", FieldIsBoolean, "1");
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_TYPE, "S_TP", FieldIsShortText);
    addField(Table_ALERT_SCRIPTS, ALERT_SCRIPT_CONTENT, "S_CT", FieldIsLongText);

    addField(Table_ALERT_VERSION, VERSION_TEXT, "TXT", FieldIsShortText);
}

AlertBase::~AlertBase()
{
}

/** Connect to the alert database. Return the connection state. */
bool AlertBase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // connect
    if (commandLine()->value(Core::ICommandLine::ClearUserDatabases).toBool()) {
        createConnection(Constants::DB_NAME, Constants::DB_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::DeleteAndRecreateDatabase);
    } else {
        createConnection(Constants::DB_NAME, Constants::DB_NAME,
                         settings()->databaseConnector(),
                         Utils::Database::CreateDatabase);
    }

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

//    d->checkDatabaseVersion();

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

    m_initialized = true;
    return true;
}

/** Private part of the Patients::PatientBase that creates the database. \sa Utils::Database. */
bool AlertBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption /*createOption*/
                   )
{
    Q_UNUSED(access);
    if (connectionName != Constants::DB_NAME)
        return false;

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2).arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase dbcreate = QSqlDatabase::addDatabase("QMYSQL", "__ALERTS_CREATOR");
            dbcreate.setHostName(pathOrHostName);
            dbcreate.setUserName(login);
            dbcreate.setPassword(pass);
            dbcreate.setPort(port);
            if (!dbcreate.open()) {
                Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                         .arg(dbcreate.connectionName()).arg(dbcreate.lastError().text()),
                                         tkTr(Trans::Constants::CONTACT_DEV_TEAM));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), dbcreate);
            if (!q.isActive()) {
                LOG_QUERY_ERROR(q);
                Utils::warningMessageBox(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                                         .arg(dbcreate.connectionName()).arg(dbcreate.lastError().text()),
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
        if (QSqlDatabase::connectionNames().contains("__ALERTS_CREATOR"))
            QSqlDatabase::removeDatabase("__ALERTS_CREATOR");
        if (!DB.open()) {
            Utils::warningMessageBox(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                     .arg(DB.connectionName()).arg(DB.lastError().text()),
                                     tkTr(Trans::Constants::CONTACT_DEV_TEAM));
            return false;
        }
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

    // inform the version
    QSqlQuery query(database());
    query.prepare(prepareInsertQuery(Constants::Table_ALERT_VERSION));
    query.bindValue(Constants::VERSION_TEXT, Constants::DB_ACTUALVERSION);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        return false;
    }

    return true;
}

/** Reconnect the database when the database server changes. \sa Core::ICore::databaseServerChanged(), Core::ISettings::databaseConnector() */
void AlertBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    init();
}

/** For debugging purpose */
void AlertBase::toTreeWidget(QTreeWidget *tree)
{
    Database::toTreeWidget(tree);
    tree->expandAll();
}


