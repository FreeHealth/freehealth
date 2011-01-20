#include "pmhbase.h"
#include "constants.h"

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>

using namespace PMH::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


PmhBase *PmhBase::m_Instance = 0;
bool PmhBase::m_initialized = false;

PmhBase *PmhBase::instance()
{
    if (!m_Instance) {
        m_Instance = new PmhBase(qApp);
        m_Instance->init();
    }
    return m_Instance;
}

PmhBase::PmhBase(QObject *parent) :
        QObject(parent), Utils::Database()
{
    using namespace PMH::Constants;
    addTable(Table_MASTER,     "MASTER");
    addTable(Table_EPISODE,    "EPISODE");
    addTable(Table_ICD,        "ICD_CODING");
    addTable(Table_CATEGORIES, "PMH_CATEGORIES");
    addTable(Table_CATEGORY_LABEL, "PMH_CAT_LABEL");

//    addTable(Table_VERSION, "VERSION");

    addField(Table_MASTER, MASTER_ID,            "ID",             FieldIsUniquePrimaryKey);
    addField(Table_MASTER, MASTER_PATIENT_UID,   "PATIENT_UUID",   FieldIsUUID);
    addField(Table_MASTER, MASTER_USER_UID,      "USER_UUID",      FieldIsUUID);
    addField(Table_MASTER, MASTER_EPISODE_ID,    "MH_EPISODE_ID",  FieldIsInteger);
    addField(Table_MASTER, MASTER_CONTACTS_ID,   "MH_CONTACTS_ID", FieldIsInteger);
    addField(Table_MASTER, MASTER_LABEL,         "LABEL",          FieldIsShortText);
    addField(Table_MASTER, MASTER_TYPE,          "TYPE_ID",        FieldIsInteger);
    addField(Table_MASTER, MASTER_STATE,         "STATE_ID",       FieldIsInteger);

    addField(Table_EPISODE, EPISODE_ID,            "ID",             FieldIsUniquePrimaryKey);
    addField(Table_EPISODE, EPISODE_MASTER_ID,     "MASTER_ID",      FieldIsInteger);
    addField(Table_EPISODE, EPISODE_DATE_START,    "DATE_START",     FieldIsDate);
    addField(Table_EPISODE, EPISODE_DATE_END,      "DATE_END",       FieldIsDate);
    addField(Table_EPISODE, EPISODE_CONF_INDEX,    "CONF_INDEX",     FieldIsInteger);
    addField(Table_EPISODE, EPISODE_ICD_CODES,     "ICD_ID",         FieldIsInteger);
    addField(Table_EPISODE, EPISODE_LABEL,         "LABEL",         FieldIsInteger);

    addField(Table_ICD, ICD_ID,       "ICD_ID",         FieldIsUniquePrimaryKey);
    addField(Table_ICD, ICD_CONTENT,  "ICD_CONTENT",    FieldIsLongText);



    connect(Core::ICore::instance(), SIGNAL(databaseServerChanged()), this, SLOT(onCoreDatabaseServerChanged()));
}

PmhBase::~PmhBase()
{
}

bool PmhBase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // Check settings --> SQLite or MySQL ?
    if (settings()->value(Core::Constants::S_USE_EXTERNAL_DATABASE, false).toBool()) {
        createConnection(Constants::DB_NAME,
                         Constants::DB_NAME,
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_HOST, QByteArray("localhost").toBase64()).toByteArray())),
                         Utils::Database::ReadWrite,
                         Utils::Database::MySQL,
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_LOG, QByteArray("root").toBase64()).toByteArray())),
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PASS, QByteArray("").toBase64()).toByteArray())),
                         QString(QByteArray::fromBase64(settings()->value(Core::Constants::S_EXTERNAL_DATABASE_PORT, QByteArray("").toBase64()).toByteArray())).toInt(),
                         Utils::Database::CreateDatabase);
    } else {
        createConnection(Constants::DB_NAME,
                         Constants::DB_FILENAME,
                         settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + QString(Constants::DB_NAME),
                         Utils::Database::ReadWrite,
                         Utils::Database::SQLite,
                         "log", "pas", 0,
                         Utils::Database::CreateDatabase);
    }
//    checkDatabaseVersion();

    m_initialized = true;
    return true;
}

bool PmhBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess access, AvailableDrivers driver,
                    const QString & login, const QString & pass,
                    const int port,
                    CreationOption /*createOption*/
                   )
{
    if (connectionName != Constants::DB_NAME)
        return false;

    Utils::Log::addMessage(this, tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
                           .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    if (driver == SQLite) {
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if (!QDir(pathOrHostName).exists())
            if (!QDir().mkpath(pathOrHostName))
                tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
        DB.setDatabaseName(QDir::cleanPath(pathOrHostName + QDir::separator() + dbName));
        DB.open();
        setDriver(Utils::Database::SQLite);
    }
    else if (driver == MySQL) {
        DB = QSqlDatabase::database(connectionName);
        if (!DB.open()) {
            QSqlDatabase d = QSqlDatabase::addDatabase("QMYSQL", "CREATOR");
            d.setHostName(pathOrHostName);
            d.setUserName(login);
            d.setPassword(pass);
            d.setPort(port);
            if (!d.open()) {
                Utils::warningMessageBox(tr("Unable to connect the Templates host."),tr("Please contact dev team."));
                return false;
            }
            QSqlQuery q(QString("CREATE DATABASE `%1`").arg(dbName), d);
            if (!q.isActive()) {
                Utils::Log::addQueryError("Database", q);
                Utils::warningMessageBox(tr("Unable to create the Templates database."),tr("Please contact dev team."));
                return false;
            }
            if (!DB.open()) {
                Utils::warningMessageBox(tr("Unable to connect the Templates database."),tr("Please contact dev team."));
                return false;
            }
            DB.setDatabaseName(dbName);
        }
        if (QSqlDatabase::connectionNames().contains("CREATOR"))
            QSqlDatabase::removeDatabase("CREATOR");
        DB.open();
        setDriver(Utils::Database::MySQL);
    }

    // create db structure
    // before we need to inform Utils::Database of the connectionName to use
    setConnectionName(connectionName);

    if (createTables()) {
        Utils::Log::addMessage(this, tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(dbName));
    } else {
        Utils::Log::addError(this, tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                         .arg(dbName, DB.lastError().text()));
        return false;
    }

    return true;
}

void PmhBase::onCoreDatabaseServerChanged()
{
//    m_initialized = false;
//    if (QSqlDatabase::connectionNames().contains(Templates::Constants::DB_TEMPLATES_NAME)) {
//        QSqlDatabase::removeDatabase(Templates::Constants::DB_TEMPLATES_NAME);
//    }
//    init();
}

