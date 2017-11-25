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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "xmliobase.h"
#include "constants.h"
#include "xmlformcontentreader.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/icommandline.h>

#include <formmanagerplugin/iformio.h>

#include <categoryplugin/categoryitem.h>
#include <categoryplugin/categorycore.h>

#include <printerplugin/textdocumentextra.h>

#include <pmhplugin/constants.h>

#ifdef WITH_ALERTS
#   include <alertplugin/alertcore.h>
#endif

#include <utils/log.h>
#include <utils/global.h>
#include <utils/databaseconnector.h>
#include <datapackutils/constants.h>

#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QDir>
#include <QLocale>
#include <QDateTime>

using namespace XmlForms;
using namespace Internal;
using namespace XmlForms::Constants;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline XmlFormContentReader *reader() {return XmlFormContentReader::instance();}
static inline Core::ICommandLine *commandLine()  { return Core::ICore::instance()->commandLine(); }
static inline Category::CategoryCore *categoryCore() {return  Category::CategoryCore::instance();}

namespace {
static inline bool connectedDatabase(QSqlDatabase &db, int line)
{
    if (!db.isOpen()) {
        if (!db.open()) {
            Utils::Log::addError("XmlIOBase", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(db.lastError().text()), __FILE__, line);
            return false;
        }
    }
    return true;
}

static inline QString normalizedFormUid(const QString &formUid)
{
    QString newUid = formUid;
    newUid.replace("\\", "/");
    if (newUid.endsWith(".xml")) {
        newUid = newUid.left(newUid.lastIndexOf("/"));
    }
    return newUid;
}

// This struct is used in saveForm()
struct FormFile {
    FormFile(const QString &fileExtension, const QString &subDir, const XmlForms::Internal::XmlIOBase::TypeOfContent content) :
        _fileExt(fileExtension), _subDir(subDir), _content(content)
    {}

    QString _fileExt, _subDir;
    XmlForms::Internal::XmlIOBase::TypeOfContent _content;
};
} // anonymous namespace

// Initializing static data
XmlIOBase *XmlIOBase::m_Instance = 0;

/**
  \brief Returns the unique instance of XmlIOBase. If the instance does not exist it is created.
  You should never construct a instance of this object using the constructor.
*/
XmlIOBase *XmlIOBase::instance()
{
    Q_ASSERT(m_Instance);
    return m_Instance;
}

XmlIOBase::XmlIOBase(QObject *parent) :
    QObject(parent), Utils::Database(),
    m_initialized(false),
    _transaction(false)
{
    m_Instance = this;
    setObjectName("XmlIOBase");

    // populate tables and fields of database
    addTable(Table_FORMS,        "FORMS");
    addTable(Table_FORM_CONTENT, "CONTENT");
    addTable(Table_SCHEMA,       "SCHEMA_CHANGES");

    addField(Table_FORMS, FORM_ID,           "FORM_ID",        FieldIsUniquePrimaryKey);
    addField(Table_FORMS, FORM_UUID,         "FORM_UUID",      FieldIsShortText);
    addField(Table_FORMS, FORM_ORIGINALUID,  "FORM_ORG_UUID",  FieldIsShortText);
    addField(Table_FORMS, FORM_ORIGINALDATE, "FORM_ORG_DATE",  FieldIsDate);
    addIndex(Table_FORMS, FORM_UUID);
    addIndex(Table_FORMS, FORM_ORIGINALUID);

    addField(Table_FORM_CONTENT, FORMCONTENT_ID,        "CONTENT_ID",     FieldIsUniquePrimaryKey);
    addField(Table_FORM_CONTENT, FORMCONTENT_FORM_ID,   "FORM_ID",        FieldIsInteger);
    addField(Table_FORM_CONTENT, FORMCONTENT_TYPE,      "TYPE",           FieldIsShortText);
    addField(Table_FORM_CONTENT, FORMCONTENT_MODENAME,  "MODE",           FieldIsShortText);
    addField(Table_FORM_CONTENT, FORMCONTENT_ISVALID,   "ISVALID",        FieldIsBoolean);
    addField(Table_FORM_CONTENT, FORMCONTENT_ISORIGINAL,"ISORIGINAL",     FieldIsBoolean);
    addField(Table_FORM_CONTENT, FORMCONTENT_CONTENT,   "CONTENT",        FieldIsBlob);
    addIndex(Table_FORM_CONTENT, FORMCONTENT_ID);
    addIndex(Table_FORM_CONTENT, FORMCONTENT_FORM_ID);

    // Table SCHEMA_CHANGES
    addTable(Table_SCHEMA, "SCHEMA_CHANGES");
    addField(Table_SCHEMA, SCHEMA_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_SCHEMA, SCHEMA_VERSION, "VERSION_NUMBER", FieldIsInteger);
    addField(Table_SCHEMA, SCHEMA_SCRIPT, "SCRIPT_NAME", FieldIs255Chars);
    addField(Table_SCHEMA, SCHEMA_TIMESTAMP, "TIMESTAMP_UTC_APPLIED", FieldIs19Chars);
    addIndex(Table_SCHEMA, SCHEMA_ID);

    // Connect first run database creation requested
    connect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
}

/** \brief Initialize the database. */
bool XmlIOBase::initialize()
{
    if (m_initialized)
        return true;

    // check database dependency
    categoryCore();

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
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().databaseName()).arg(database().driverName()));
    }

    int currentDatabaseVersion = getSchemaVersionNumber(Constants::DB_NAME);
    if (currentDatabaseVersion != Constants::DB_CURRENT_CODE_VERSION) {
        if(!updateDatabase()) {
            LOG_ERROR(QString("Couldn't upgrade database %1").arg(Constants::DB_NAME));
            return false;
        }
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DB_NAME));
        return false;
    }

    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    m_initialized = true;
    return true;
}

/** Return true if the XmlIOBase is initialized and usable */
bool XmlIOBase::isInitialized() const
{
    return m_initialized;
}

/**
 * Update xmlforms database
 * Old versioning:
 *   fmf/fhio version < 0.11.0): version string = "0.1"
 *   fhio version <= 0.11.0): VERSION_NUMBER <= 1
 * New versioning (fhio version >= 0.11): The version number is an integer,
 * starting from 1 for fhio version 0.11
 * The field VERSION_NUMBER of the last row of the table SCHEMA_CHANGES.
 * By definition, this number must be a positive, non null integer.
 * This function will run all sql update scripts to update the database to
 * the current code version
 */
bool XmlIOBase::updateDatabase() const
{
    WARN_FUNC;
    int currentDatabaseVersion = getSchemaVersionNumber(Constants::DB_NAME);
    QSqlDatabase DB = QSqlDatabase::database(Constants::DB_NAME);
    QString updateScriptFileName;
    currentDatabaseVersion++;
    for (int i = currentDatabaseVersion; i <= Constants::DB_CURRENT_CODE_VERSION; i++) {
        if (driver()==MySQL) {
            updateScriptFileName= QString(":/sql/update/update%1%2.sql")
                    .arg(Constants::DB_NAME)
                    .arg(QString::number(i));
        } else if (driver()==SQLite) {
            updateScriptFileName= QString(":/sql/update/updatesqlite%1%2.sql")
                    .arg(Constants::DB_NAME)
                    .arg(QString::number(i));
        }
        QFile updateScriptFile(updateScriptFileName);
        if(!executeQueryFile(updateScriptFile, DB)) {
            LOG_ERROR(QString("Error during update with updatescript%1").arg(i));
            return false;
        }
    }
return true;
}

/** Create the default users database if it does not exist. */
bool XmlIOBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString &login, const QString &pass,
                    const int port,
                    CreationOption createOption
                   )
{
    Q_UNUSED(access);
    Q_UNUSED(createOption);

    if (connectionName != Constants::DB_NAME)
        return false;

    LOG(QCoreApplication::translate("XmlIOBase",
                                    "Trying to create empty database.\n"
                                    "       Location: %1\n"
                                    "       FileName: %2\n"
                                    "       Driver: %3")
        .arg(pathOrHostName, dbName).arg(driver));

    setConnectionName(connectionName);
    setDriver(driver);

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                LOG(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        if (!DB.open())
            LOG(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2).arg(dbName).arg(DB.lastError().text()));
        setDriver(Utils::Database::SQLite);
    } else if (driver == MySQL) {
        // TODO: test grants here or before ?
        if (QSqlDatabase::connectionNames().contains(connectionName)) {
            DB = QSqlDatabase::database(connectionName);
        } else {
            DB = QSqlDatabase::addDatabase("QMYSQL" , connectionName);
            DB.setHostName(pathOrHostName);
            DB.setUserName(login);
            DB.setPassword(pass);
            DB.setPort(port);
        }
        DB.setDatabaseName("mysql");
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
        //        qWarning() << "createMySQLDatabase(dbName);";
        createMySQLDatabase(dbName);
        // change database connection
        DB.setDatabaseName(dbName);
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
    }

    // create db structure
    if (createTables()) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                  .arg(dbName, DB.lastError().text()));
        return false;
    }

    // Add version number
    if (!setSchemaVersion(Constants::DB_CURRENT_CODE_VERSION, Constants::DB_NAME)) {
        LOG_ERROR(QString("Couldn't set schema version for database %1").arg(Constants::DB_NAME));
        return false;
    }

    // database is readable/writable
    LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(pathOrHostName + QDir::separator() + dbName));

    return true;
}

void XmlIOBase::onCoreDatabaseServerChanged()
{
    m_initialized = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME)) {
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    }
    disconnect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

void XmlIOBase::onCoreFirstRunCreationRequested()
{
    disconnect(Core::ICore::instance(), SIGNAL(firstRunDatabaseCreation()), this, SLOT(onCoreFirstRunCreationRequested()));
    initialize();
}

/**
 * Return true if the form \e form, XmlIOBase::TypeOfForm \e type
 * and \e modeName exists in the database.\n
 * Empty \e modeName are interpreted as the central form.\n
 * The \e form is modified and populate with the database content.
*/
bool XmlIOBase::isFormExists(XmlFormName &form, const int type, QString modeName)
{
    // Form info already available
    bool exists = false;
    if (modeName.isEmpty())
        modeName = "central";
    if (form.isAvailableFromDatabase && form.databaseAvailableContents.contains(type)) {
        if (form.databaseAvailableContents.values(type).contains(modeName))
            exists = true;
    }
    if (exists)
        return exists;

    // Get form info from database
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;
    DB.transaction();
    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORMS, Constants::FORM_ID);
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE);
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID, QString("='%1'").arg(normalizedFormUid(form.uid)));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));
    QString req = select(gets, joins, conds);
    QSqlQuery query(DB);
    if (query.exec(req)) {
        while (query.next()) {
            form.databaseAvailableContents.insertMulti(query.value(1).toInt(), query.value(2).toString());
            form.isAvailableFromDatabase = true;
            form.dbId = query.value(0).toInt();
        }
    } else {
        LOG_QUERY_ERROR(query);
        DB.rollback();
        return false;
    }
    DB.commit();

    if (form.isAvailableFromDatabase && form.databaseAvailableContents.contains(type)) {
        if (form.databaseAvailableContents.values(type).contains(modeName)) {
            return true;
        }
    }
    return false;
}

/** Get all forms descriptions from the database according to the \e query */
QList<Form::FormIODescription *> XmlIOBase::getFormDescription(const Form::FormIOQuery &formQuery)
{
    QList<Form::FormIODescription *> toReturn;
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return toReturn;

    // Just manage type of forms
    DB.transaction();
    _transaction =true;
    QSqlQuery query(DB);
    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID);
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_CONTENT);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);

    Utils::FieldList conds, uid;
    // Get FormUid conditions
    if (formQuery.getAllAvailableFormDescriptions()) {
        uid << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID,
                            QString("like '%'"));
    } else {
        if (formQuery.typeOfForms() & Form::FormIOQuery::CompleteForms) {
            uid << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID,
                                QString("like '%1%'").arg(Core::Constants::TAG_APPLICATION_COMPLETEFORMS_PATH));
            uid << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID,
                                QString("like '%1%'").arg(Core::Constants::TAG_APPLICATION_USER_COMPLETEFORMS_PATH));
            uid << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID,
                                QString("like '%1%'").arg(Core::Constants::TAG_DATAPACK_COMPLETEFORMS_PATH));
        }
        if (formQuery.typeOfForms() & Form::FormIOQuery::SubForms) {
            uid << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID,
                                QString("like '%1%'").arg(Core::Constants::TAG_APPLICATION_SUBFORMS_PATH));
            uid << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID,
                                QString("like '%1%'").arg(Core::Constants::TAG_APPLICATION_USER_SUBFORMS_PATH));
            uid << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID,
                                QString("like '%1%'").arg(Core::Constants::TAG_DATAPACK_SUBFORMS_PATH));
        }
    }
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE, QString("='%1'").arg(Description));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));
    QString where = QString(" WHERE %1 AND %2")
            .arg(getWhereClause(conds))
            .arg(getWhereClause(uid, Utils::Database::OR));
    QString req = select(gets, joins) + where;
    if (query.exec(req)) {
        while (query.next()) {
            QDomDocument doc;
            doc.setContent(query.value(1).toString());
            Form::FormIODescription *descr = reader()->readXmlDescription(doc.firstChildElement(Constants::TAG_FORM_DESCRIPTION), query.value(0).toString());
            if (descr) {
                descr->setData(Form::FormIODescription::Category, descr->data(Form::FormIODescription::Category).toString(), QLocale().name().left(2));
                descr->setData(Form::FormIODescription::FromDatabase, true);
                descr->setData(Form::FormIODescription::HasScreenShot, hasScreenShots(descr->data(Form::FormIODescription::UuidOrAbsPath).toString()));
                toReturn << descr;
            }
        }
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        DB.rollback();
        return toReturn;
    }
    query.finish();
    DB.commit();
    _transaction = false;
    return toReturn;
}

/** Return a QHash with as key the XmlIOBase::TypeOfForm and as value the XML content of the form */
QHash<QString, QString> XmlIOBase::getAllFormFullContent(const QString &formUid)
{
    QHash<QString, QString> toReturn;
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return toReturn;
    // Get all formIds from Forms table
    QVector<int> ids;
    DB.transaction();
    _transaction = true;
    QSqlQuery query(DB);
    QHash<int, QString> where;
    where.insert(Constants::FORM_UUID, QString("='%1'").arg(normalizedFormUid(formUid)));
    QString req = select(Table_FORMS, FORM_ID, where);
    if (query.exec(req)) {
        while (query.next()) {
            ids.append(query.value(0).toInt());
        }
    } else {
        LOG_QUERY_ERROR(query);
        DB.rollback();
        _transaction = false;
        return toReturn;
    }
    query.finish();

    // Retreive all contents
    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME);
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_CONTENT);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);

    for(int i = 0; i < ids.count(); ++i) {
        int id = ids.at(i);
        Utils::FieldList conds;
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE, QString("='%1'").arg(FullContent));
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID, QString("=%1").arg(id));
        req = select(gets, joins, conds);

        if (query.exec(req)) {
            while (query.next()) {
                toReturn.insert(query.value(0).toString(), query.value(1).toString());
            }
        } else {
            LOG_QUERY_ERROR(query);
            DB.rollback();
        }
        query.finish();
    }
    _transaction = false;
    DB.commit();
    return toReturn;
}

/**
 * Return the XML content of the form according to the XmlIOBase::TypeOfForm \e type
 * and the \e formUid. \n
 * This member can be called inside or outside a database transaction. If it is called
 * inside a transaction it does not call neither QSqlDatabase::commit(), nor
 * QSqlDatabase::rollback().
*/
QString XmlIOBase::getFormContent(const QString &formUid, const int type, const QString &modeName)
{
//    qWarning() << Q_FUNC_INFO << formUid << type << modeName;
    // TODO: manage modes
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return QString::null;
    bool insideTransaction = true;
    if (!_transaction) {
        DB.transaction();
        _transaction = true;
        insideTransaction = false;
    }
    QSqlQuery query(DB);
    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_CONTENT);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID, QString("='%1'").arg(normalizedFormUid(formUid)));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE, QString("='%1'").arg(type));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));
    if (modeName.isEmpty()) {
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("='central'"));
    } else {
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("='%1'").arg(modeName));
    }
    QString req = select(gets, joins, conds);

    if (query.exec(req)) {
        if (query.next()) {
            QString var = query.value(0).toString();
            if (!insideTransaction) {
                DB.commit();
                _transaction = false;
            }
            return var;
        }
    } else {
        LOG_QUERY_ERROR(query);
        if (!insideTransaction) {
            DB.rollback();
            _transaction = false;
        }
    }
    if (!insideTransaction)
        _transaction = false;
    return QString();
}

/**
 * Returns the shot \e shotName associated to the form \e formUid
 * This member can be called inside or outside a database transaction. If it is called
 * inside a transaction it does not call neither QSqlDatabase::commit(), nor
 * QSqlDatabase::rollback().
*/
QPixmap XmlIOBase::getScreenShot(const QString &formUid, const QString &shotName)
{
    // Get shot content
    QString content = getFormContent(formUid, ScreenShot, shotName);
    if (content.isEmpty())
        return QPixmap();
    // Create pixmap
    QPixmap pix;
    if (!content.isEmpty())
        pix.loadFromData(QByteArray::fromBase64(content.toUtf8()));
    return pix;
}

/**
 * Returns \e true if the database contains a screenshot for the for \e formUid and
 * the language \e lang, otherwise return \e false.\n
 * This member can be called inside or outside a database transaction. If it is called
 * inside a transaction it does not call neither QSqlDatabase::commit(), nor
 * QSqlDatabase::rollback().
*/
bool XmlIOBase::hasScreenShots(const QString &formUid, const QString &lang)
{
    // can be accessed within ou without a transaction
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;
    bool insideTransaction = true;
    if (!_transaction) {
        DB.transaction();
        _transaction = true;
        insideTransaction = false;
    }
    QSqlQuery query(DB);
    QString req;
    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORMS, Constants::FORM_ID);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID, QString("='%1'").arg(normalizedFormUid(formUid)));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE, QString("='%1'").arg(ScreenShot));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));
    if (!lang.isEmpty()) {
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("LIKE '%1/%'").arg(lang));
        req = select(gets, joins, conds);
    } else {
        req = select(gets, joins, conds);
        conds.clear();
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("LIKE '%1/%'").arg(QLocale().name().left(2)));
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("LIKE 'xx/%'"));
        req += " AND " + getWhereClause(conds, OR);
    }
    bool hasShot = false;
    if (query.exec(req)) {
        if (query.next())
            hasShot = true;
    } else {
        LOG_QUERY_ERROR(query);
        query.finish();
        if (!insideTransaction) {
            DB.rollback();
            _transaction = false;
        }
    }
    if (!insideTransaction) {
        query.finish();
        DB.commit();
        _transaction = false;
    }
    return hasShot;
}

/**
 * Returns the shots for the language \e lang associated to the form \e formUid.\n
 * The returned QHash contains the shot uuid as key and the shot pixmap as value.\n
 * This member can be called inside or outside a database transaction. If it is called
 * inside a transaction it does not call neither QSqlDatabase::commit(), nor
 * QSqlDatabase::rollback().
*/
QHash<QString, QPixmap> XmlIOBase::getScreenShots(const QString &formUid, const QString &lang)
{
    // can be accessed within ou without a transaction
    QHash<QString, QPixmap> pixmaps;
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return pixmaps;
    bool insideTransaction = true;
    if (!_transaction) {
        DB.transaction();
        _transaction = true;
        insideTransaction = false;
    }
    QSqlQuery query(DB);
    QString req;
    Utils::FieldList gets;
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME);
    gets << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_CONTENT);
    Utils::JoinList joins;
    joins << Utils::Join(Constants::Table_FORMS, Constants::FORM_ID, Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_FORM_ID);
    Utils::FieldList conds;
    conds << Utils::Field(Constants::Table_FORMS, Constants::FORM_UUID, QString("='%1'").arg(normalizedFormUid(formUid)));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_TYPE, QString("='%1'").arg(ScreenShot));
    conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_ISVALID, QString("=1"));
    if (!lang.isEmpty()) {
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("LIKE '%1/%'").arg(lang));
        req = select(gets, joins, conds);
    } else {
        req = select(gets, joins, conds);
        conds.clear();
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("LIKE '%1/%'").arg(QLocale().name().left(2)));
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("LIKE 'xx/%'"));
        req += " AND " + getWhereClause(conds, OR);
    }

    int nbShotRead = 0;
    if (query.exec(req)) {
        while (query.next()) {
            QPixmap pix;
            if (!query.value(1).toByteArray().isEmpty())
                pix.loadFromData(QByteArray::fromBase64(query.value(1).toByteArray()));
            pixmaps.insert(query.value(0).toString(), pix);
            ++nbShotRead;
        }
    } else {
        LOG_QUERY_ERROR(query);
        if (!insideTransaction) {
            DB.rollback();
            _transaction = false;
        }
        return pixmaps;
    }
    query.finish();

    if (nbShotRead==0) {
        // Try with xx language
        conds.takeLast();
        conds << Utils::Field(Constants::Table_FORM_CONTENT, Constants::FORMCONTENT_MODENAME, QString("LIKE '%1/%'").arg(Trans::Constants::ALL_LANGUAGE));
        req = select(gets, joins, conds);
        if (query.exec(req)) {
            while (query.next()) {
                QPixmap pix;
                if (!query.value(1).toByteArray().isEmpty())
                    pix.loadFromData(QByteArray::fromBase64(query.value(1).toByteArray()));
                pixmaps.insert(query.value(0).toString(), pix);
                ++nbShotRead;
            }
        } else {
            LOG_QUERY_ERROR(query);
            if (!insideTransaction) {
                DB.rollback();
                _transaction = false;
            }
            return pixmaps;
        }
    }
    if (!insideTransaction) {
        DB.commit();
        _transaction = false;
    }
    return pixmaps;
}

/**
 * Save the \e content of the form \e form to the database and return the used formUid.\n
 * If the \e content is empty the form file is accessed.\n
 * The \e form is modified and populated with the database content. \n
 * This member creates its own transaction.
*/
bool XmlIOBase::saveForm(XmlFormName &form)
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;

//    if (isFormExists(form))
//        return true;

    LOG("Saving forms to database: " + form.uid);

    QDir dir(form.absPath);

    database().transaction();
    _transaction = true;

    // save all XML files && files included in the form using the <file> XML tag
    foreach(const QFileInfo &f, dir.entryInfoList(QStringList() << "*.xml", QDir::Files | QDir::Readable)) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        // save the content of the form file
        QString modeName = f.baseName();
        QString content = Utils::readTextFile(f.absoluteFilePath(), Utils::DontWarnUser);
        if (f.fileName().compare(Constants::PMHXCATEGORIES_FILENAME, Qt::CaseInsensitive)==0) {
            if (!saveContent(form.uid, content, XmlIOBase::PmhCategories, modeName)) {
                LOG_ERROR("Can not save pmhx categories associated with the form to database");
                database().rollback();
                _transaction = false;
                return false;
            } else {
                LOG("Saving pmhx categories file to database " + f.absoluteFilePath());
            }
            if (!savePmhxCategories(form, content)) {
                LOG_ERROR("Can not save pmhx categories associated with the form to database");
                database().rollback();
                _transaction = false;
                return false;
            }
        } else {
            if (!saveContent(form.uid, content, XmlIOBase::FullContent, modeName)) {
                LOG_ERROR("Can not save form to database");
                database().rollback();
                _transaction = false;
                return false;
            } else {
                LOG("Saving attached *xml files to database " + f.absoluteFilePath());
            }
        }

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        // Try to catch file addition
        // File addition is done using the tag ‘file’ or an attrib of the same name
        QDomDocument doc;
        QString error;
        int line, col;
        if (!doc.setContent(content, &error, &line, &col)) {
            LOG_ERROR_FOR("XmlIOBase", tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(line).arg(col).arg(error));
            database().rollback();
            _transaction = false;
            return false;
        }
        QDomNodeList list = doc.elementsByTagName("file");

        for(int i=0; i < list.count(); ++i) {
            const QDomNode &node = list.at(i);
            const QString &include = node.toElement().text();
            if (include.endsWith(".xml", Qt::CaseInsensitive)) {
                XmlFormName includeForm(include);
                if (!saveForm(includeForm)) {
                    LOG_ERROR("unable to save included form: " + includeForm.uid);
                    database().rollback();
                    _transaction = false;
                    return false;
                }
            }
        }
    }

    // Save files
    QList<FormFile> files;
    files << FormFile("js", "scripts", XmlIOBase::ScriptFile);
    files << FormFile("ui", "ui", XmlIOBase::UiFile);
    files << FormFile("qml", "qml", XmlIOBase::QmlFile);
    files << FormFile("html", "html", XmlIOBase::HtmlFile);
    files << FormFile("htm", "html", XmlIOBase::HtmlFile);
    files << FormFile("css", "html", XmlIOBase::CssFile);
    files << FormFile("pdf", "pdf", XmlIOBase::PdfFile);
//    files << FormFile("txt", "scripts", XmlIOBase::ScriptFile);
//    files << FormFile("rtf", "scripts", XmlIOBase::ScriptFile);
    foreach(const FormFile &file, files) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if (!saveFiles(form, file._subDir, file._fileExt, file._content)) {
            LOG_ERROR(QString("Unable to save %1(*.%2) files")
                      .arg(file._subDir).arg(file._fileExt));
            database().rollback();
            _transaction = false;
            return false;
        }
    }

    // Save screenshots
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (!saveScreenShots(form)) {
        LOG_ERROR("Unable to save screenshot files");
        database().rollback();
        _transaction = false;
        return false;
    }

    // Register AlertPacks -- manages save/update of alertpacks
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (!registerAlertPacks(form)) {
        LOG_ERROR("Unable to save screenshot files");
        database().rollback();
        _transaction = false;
        return false;
    }

    // Save PDF
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (!saveFiles(form, "pdf", "pdf", XmlIOBase::PdfFile)) {
        LOG_ERROR("Unable to save PDF files");
        database().rollback();
        _transaction = false;
        return false;
    }

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    database().commit();
    _transaction = false;

    if (isFormExists(form))
        return true;

    return true;
}

/**
 * Creates the PMHx categories related to the \e form
 * reading the QDomDocument from the \e element
 * with the parent category \e parent.
 */
Category::CategoryItem * XmlIOBase::createCategory(const XmlFormName &form, const QDomElement &element, Category::CategoryItem *parent) const
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    // create the category
    Category::CategoryItem *item = new Category::CategoryItem;
    item->setData(Category::CategoryItem::DbOnly_Mime, QString("%1@%2").arg(PMH::Constants::CATEGORY_MIME).arg(form.uid));
    item->setData(Category::CategoryItem::DbOnly_IsValid, 1);
    item->setData(Category::CategoryItem::ThemedIcon, element.attribute(::Constants::ATTRIB_ICON));
    item->setData(Category::CategoryItem::Uuid, element.attribute(::Constants::ATTRIB_UUID));
    item->setData(Category::CategoryItem::SortId, element.attribute(::Constants::ATTRIB_SORT_ID));

    // read the labels
    QDomElement label = element.firstChildElement(::Constants::TAG_SPEC_LABEL);
    while (!label.isNull()) {
        item->setLabel(label.text(), label.attribute(::Constants::ATTRIB_LANGUAGE, Trans::Constants::ALL_LANGUAGE));
        label = label.nextSiblingElement(::Constants::TAG_SPEC_LABEL);
    }

    // get ExtraTag content -> CategoryItem::ExtraXml
    QDomElement extra = element.firstChildElement(::Constants::TAG_SPEC_EXTRA);
    if (!extra.isNull()) {
        QString content;
        QTextStream s(&content);
        extra.save(s, 2);
        item->setData(Category::CategoryItem::ExtraXml, content);
    }

    // reparent item
    if (parent) {
        parent->addChild(item);
        item->setParent(parent);
    }
    // has children ?
    QDomElement child = element.firstChildElement(::Constants::TAG_CATEGORY);
    while (!child.isNull()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        createCategory(form, child, item);
        child = child.nextSiblingElement(::Constants::TAG_CATEGORY);
    }
    return item;
}

/**
 * Save PMHx categories to the categories database.
 * The PMHx category file must be already in the database.
*/
bool XmlIOBase::savePmhxCategories(const XmlFormName &form, const QString &content)
{
    Q_UNUSED(form);
//    QTime chr;
//    chr.start();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (content.isEmpty()) {
        LOG_ERROR("Empty content.");
        return false;
    }
    // analyze XML content
    QDomDocument doc;
    int line = -1;
    int col = -1;
    QString error;
    if (!doc.setContent(content, &error, &line, &col)) {
        LOG_ERROR(QString("Error while loading PMHxCategories XML files."));
        LOG_ERROR(tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(line).arg(col).arg(error));
        return false;
    }
    QDomElement root = doc.firstChildElement(Constants::TAG_MAINXMLTAG);
    QDomElement element = root.firstChildElement(Constants::TAG_PMHX_CATEGORIES);
    element = element.firstChildElement(::Constants::TAG_CATEGORY);
    QVector<Category::CategoryItem *> rootCategories;
    while (!element.isNull()) {
        rootCategories << createCategory(form, element, 0);
        element = element.nextSiblingElement(::Constants::TAG_CATEGORY);
    }
    // Utils::Log::logTimeElapsed(chr, "---", "create categories");
    // save categories in the categories plugin
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (!categoryCore()->saveCategories(rootCategories)) {
        LOG_ERROR(tr("Error while saving PMHxCateogries (%1)").arg(form.uid));
        return false;
    }
    // Utils::Log::logTimeElapsed(chr, "---", "save categories");
    return true;
}

/**
 * Save screenshots files associated with the \e form.
*/
bool XmlIOBase::saveScreenShots(const XmlFormName &form)
{
    QDir shotPath(form.absPath + QDir::separator() + "shots");
    if (shotPath.exists()) {
        LOG_FOR("XmlFormIO","Saving attached screenshots to database " + form.uid);
        QFileInfoList files = Utils::getFiles(shotPath, "*.png", Utils::Recursively);
        if (files.isEmpty())
            return true;

        // Start a transaction
        QSqlDatabase DB = database();
        if (!connectedDatabase(DB, __LINE__))
            return false;
        DB.transaction();
        _transaction = true;

        foreach(const QFileInfo &f, files) {
            QString fp = f.absoluteFilePath();
            QFile file(fp);
            // mode = last dir (lang) + fileName.extension
            int end = fp.lastIndexOf("/");
            int begin = fp.lastIndexOf("/", end - 1) + 1;
            QString mode = fp.mid(begin, end-begin) + "/" + f.fileName();
            if (file.open(QFile::ReadOnly)) {
                QByteArray ba = file.readAll();
                if (!saveContent(form.uid, ba.toBase64(), XmlIOBase::ScreenShot, mode)) {
                    DB.rollback();
                    _transaction = false;
                    return false;
                }
            }
        }

        DB.commit();
        _transaction = false;
    }
    return true;
}

/** Register associated alertpacks for the \e form. */
bool XmlIOBase::registerAlertPacks(const XmlFormName &form)
{
    Q_UNUSED(form)
#ifndef WITH_ALERTS
    // No alerts -> return true
    return true;
#else
    QDir alertPacksPath(form.absPath + QDir::separator() + "alertpacks");
    bool ok = true;
    if (alertPacksPath.exists()) {
        LOG_FOR("XmlFormIO","Saving attached alertpacks to database " + form.uid);
        QFileInfoList files = Utils::getFiles(alertPacksPath, DataPack::Constants::PACKDESCRIPTION_FILENAME, Utils::Recursively);
        foreach(const QFileInfo &f, files) {
            if (!Alert::AlertCore::instance().registerAlertPack(f.absolutePath())) {
                LOG_ERROR("Unable to read alertpack: "+ f.absolutePath());
                ok = false;
            }
        }
    }
    return ok;
#endif
}

/** Save files associated with the forms. */
bool XmlIOBase::saveFiles(const XmlFormName &form, const QString &subDir, const QString &fileExtension, XmlIOBase::TypeOfContent type)
{
//    WARN_FUNC << absPathDir << form.uid;
    QDir path(form.absPath + QDir::separator() + subDir);
    if (!path.exists())
        path.cdUp();
    if (path.exists()) {
        LOG_FOR("XmlFormIO","Saving attached *." + fileExtension + " files to database " + form.uid);
        QFileInfoList files = Utils::getFiles(path, "*." + fileExtension, Utils::Recursively);
        if (files.isEmpty())
            return true;

        // Start a transaction
        QSqlDatabase DB = database();
        if (!connectedDatabase(DB, __LINE__))
            return false;
        DB.transaction();
        _transaction = true;

        foreach(const QFileInfo &f, files) {
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            QString fp = f.absoluteFilePath();
            QString mode = fp;
            mode = "." + mode.remove(form.absPath);
            if (type == PdfFile) {
                // Save base64
                QFile file(fp);
                if (!file.open(QFile::ReadOnly)) {
                    LOG_ERROR("Unable to open file: " + fp);
                    DB.rollback();
                    _transaction = false;
                    return false;
                }
                QString content = file.readAll().toBase64();
                if (!saveContent(form.uid, content, type, mode)) {
                    DB.rollback();
                    _transaction = false;
                    return false;
                }
            } else {
                // Save qstring content
                if (!saveContent(form.uid, Utils::readTextFile(fp, Utils::DontWarnUser), type, mode)) {
                    DB.rollback();
                    _transaction = false;
                    return false;
                }
            }
        }

        DB.commit();
        _transaction = false;
    }
    return true;
}

/**
 * Save the content of an XML form content \e xmlContent into the database
 * using the Form Uuid \e formUid, XmlIOBase::TypeOfForm \e type,
 * Mode name \e modeName and \e date
 * This member can be called inside or outside a database transaction. If it is called
 * inside a transaction it does not call neither QSqlDatabase::commit(), nor
 * QSqlDatabase::rollback().
*/
bool XmlIOBase::saveContent(const QString &formUid, const QString &xmlContent, const int type, const QString &modeName, const QDateTime &date)
{
    QSqlDatabase DB = database();
    if (!connectedDatabase(DB, __LINE__))
        return false;

    bool insideTransaction = true;
    if (!_transaction) {
        insideTransaction = false;
        DB.transaction();
        _transaction = true;
    }
    QSqlQuery query(DB);
    int formId = -1;
    QString req;
    QString mode = modeName;
    if (mode.isEmpty()) {
        mode = "central";
    }
    mode.remove(".xml");

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    QHash<int, QString> where;
    // create in form table ?
    where.insert(FORM_UUID, QString("='%1'").arg(normalizedFormUid(formUid)));
    int nb = count(Table_FORMS, FORM_ID, getWhereClause(Table_FORMS, where));
    if (!nb) {
        // Insert and retrieve FormId
        req = prepareInsertQuery(Table_FORMS);
        query.prepare(req);
        query.bindValue(FORM_ID, QVariant());
        query.bindValue(FORM_UUID, normalizedFormUid(formUid));
        query.bindValue(FORM_ORIGINALUID, formUid);
        query.bindValue(FORM_ORIGINALDATE, date);
        if (query.exec()) {
            formId = query.lastInsertId().toInt();
        } else {
            LOG_QUERY_ERROR(query);
            if (!insideTransaction) {
                DB.rollback();
                _transaction = false;
            }
            return false;
        }
        query.finish();
    } else {
        // Get FormId
        req = select(Table_FORMS, FORM_ID, where);
        if (query.exec(req)) {
            if (query.next()) {
                formId = query.value(0).toInt();
            }
        } else {
            LOG_QUERY_ERROR(query);
            if (!insideTransaction) {
                DB.rollback();
                _transaction = false;
            }
            return false;
        }
        query.finish();
    }

    // update or insert content table ?
    where.clear();
    where.insert(FORMCONTENT_FORM_ID, QString("=%1").arg(formId));
    where.insert(FORMCONTENT_TYPE, QString("='%1'").arg(type));
    where.insert(FORMCONTENT_MODENAME, QString("='%1'").arg(mode));
    where.insert(FORMCONTENT_CONTENT, "IS NOT NULL");
    nb = count(Table_FORM_CONTENT, FORMCONTENT_FORM_ID, getWhereClause(Table_FORM_CONTENT, where));
    if (!nb) {
        // Insert
        req = prepareInsertQuery(Table_FORM_CONTENT);
        query.prepare(req);
        query.bindValue(FORMCONTENT_ID, QVariant());
        query.bindValue(FORMCONTENT_FORM_ID, formId);
        query.bindValue(FORMCONTENT_TYPE, type);
        query.bindValue(FORMCONTENT_MODENAME, mode);
        query.bindValue(FORMCONTENT_ISVALID, 1);
        query.bindValue(FORMCONTENT_ISORIGINAL, 1);
        query.bindValue(FORMCONTENT_CONTENT, xmlContent);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            if (!insideTransaction) {
                DB.rollback();
                _transaction = false;
            }
            return false;
        }
        query.finish();

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        // add the description
        if (mode.compare("central",Qt::CaseInsensitive)==0) {
            int beg = xmlContent.indexOf(QString("<%1>").arg(Constants::TAG_FORM_DESCRIPTION));
            int end = xmlContent.indexOf(QString("</%1>").arg(Constants::TAG_FORM_DESCRIPTION), beg);
            if (beg==-1 || end==-1) {
                LOG_ERROR("no description found");
                if (!insideTransaction) {
                    DB.rollback();
                    _transaction = false;
                }
                return false;
            }
            end += QString("</%1>").arg(Constants::TAG_FORM_DESCRIPTION).length();
            QString descr = xmlContent.mid(beg, end-beg),
            req = prepareInsertQuery(Table_FORM_CONTENT);
            query.prepare(req);
            query.bindValue(FORMCONTENT_ID, QVariant());
            query.bindValue(FORMCONTENT_FORM_ID, formId);
            query.bindValue(FORMCONTENT_TYPE, Description);
            query.bindValue(FORMCONTENT_MODENAME, mode);
            query.bindValue(FORMCONTENT_ISVALID, 1);
            query.bindValue(FORMCONTENT_ISORIGINAL, 1);
            query.bindValue(FORMCONTENT_CONTENT, descr);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                if (!insideTransaction) {
                    DB.rollback();
                    _transaction = false;
                }
                return false;
            }
            query.finish();
        }

    } else {
        // Update
        req = prepareUpdateQuery(Table_FORM_CONTENT, FORMCONTENT_CONTENT, where);
        query.prepare(req);
        query.bindValue(0, xmlContent);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            if (!insideTransaction) {
                DB.rollback();
                _transaction = false;
            }
            return false;
        }
        query.finish();

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        // update the description
        if (mode.compare("central",Qt::CaseInsensitive)==0) {
            int beg = xmlContent.indexOf(QString("<%1>").arg(Constants::TAG_FORM_DESCRIPTION));
            int end = xmlContent.indexOf(QString("</%1>").arg(Constants::TAG_FORM_DESCRIPTION), beg);
            if (beg==-1 || end==-1) {
                LOG_ERROR("no description found");
                if (!insideTransaction) {
                    DB.rollback();
                    _transaction = false;
                }
                return false;
            }
            end += QString("</%1>").arg(Constants::TAG_FORM_DESCRIPTION).length();
            QString descr = xmlContent.mid(beg, end-beg);

            where.insert(FORMCONTENT_TYPE, QString("='%1'").arg(Description));
            req = prepareUpdateQuery(Table_FORM_CONTENT, FORMCONTENT_CONTENT, where);
            query.prepare(req);
            query.bindValue(0, descr);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                if (!insideTransaction) {
                    DB.rollback();
                    _transaction = false;
                }
                return false;
            }
            query.finish();
        }
    }
    if (!insideTransaction) {
        DB.commit();
        _transaction = false;
    }
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    return true;
}

