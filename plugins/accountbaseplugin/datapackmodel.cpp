#include "datapackmodel.h"

#include "constants.h"
#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/icommandline.h>
#include <coreplugin/constants.h>
#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>

#include <QSqlTableModel>
#include <QFileInfo>
#include <QDir>


using namespace AccountDB;
using namespace Constants;
using namespace Trans::ConstantTranslations;

enum Warn{WarnDebug = true};

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
// This is just a code easier (for user's reading)
static inline bool connectDatabase(QSqlDatabase &DB, const int line)
{
    Q_UNUSED(line);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR_FOR("MPDatapack", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
    }
    return true;
}

// Find the database to use. In priority order:
// - User datapack
// - Application installed datapack
static QString databasePath()
{
    QString dbRelPath = QString("/%1/%2").arg(Constants::DATAPACK_DB).arg(Constants::DATAPACK_ACCOUNTANCY_FILENAME);
    QString tmp;
    tmp = settings()->dataPackInstallPath() + dbRelPath;
    qDebug() << __FILE__ << QString::number(__LINE__) << " tmp =" << tmp ;
    if (QFileInfo(tmp).exists())
        return settings()->dataPackInstallPath();
    tmp = settings()->dataPackApplicationInstalledPath() + dbRelPath;
    return settings()->dataPackApplicationInstalledPath();
}

static QString databaseFileName()
{
    return databasePath() + QDir::separator() + Constants::DATAPACK_DB;
}

DatapackBase::DatapackBase(QObject *parent): QObject(parent), Utils::Database()
{
    _init = false;
    addTable(Table_MedicalProcedure,  "medical_procedure");    
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
    addField(Table_MedicalProcedure, MP_COUNTRY,        "COUNTRY",        FieldIsBlob);    

    //createConnection(Constants::DATAPACK_ACCOUNTANCY, Constants::DATAPACK_ACCOUNTANCY, connector);TODO

    // COMMENT: Eric
    // Je pense que des indexes seraient les bienvenus
    // Ici comme on se trouve dans datapack -> MP_USER_UID est il vraiment nécessaire et utile
    // on initialise jamais dans le constructeur pour éviter effet de Bord
}

bool DatapackBase::initialize() 
{
    if(WarnDebug)
    qDebug() << __FILE__ << QString::number(__LINE__) << " initialize =" << "INITIALIZE" ;
    if (_init)
        return true;
    setConnectionName(Constants::DATAPACK_DB);
    setDriver(Utils::Database::SQLite);
    if(WarnDebug)
    qDebug() << __FILE__ << QString::number(__LINE__) << " initialize =" << "INITIALIZE again" ;
    // test driver
    // use only SQLite with datapacks
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        LOG_ERROR_FOR("DatapackBase", tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE).arg("SQLite"));
        Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
                                 tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL).arg("SQLite"),
                                 "", qApp->applicationName());
        return false;
    }

    // Connect Database
    Utils::DatabaseConnector connector;
    QString pathToDb = databasePath();
    qDebug() << __FILE__ << QString::number(__LINE__) << "pathToDb  =" <<pathToDb  ;
    if (!QFileInfo(pathToDb).isDir())
        pathToDb = QFileInfo(pathToDb).absolutePath();
    connector.setAbsPathToReadOnlySqliteDatabase(pathToDb);
    connector.setHost(QFileInfo(databaseFileName()).fileName());
    connector.setAccessMode(Utils::DatabaseConnector::ReadOnly);
    connector.setDriver(Utils::Database::SQLite);

    LOG_FOR("DatapackBase", tkTr(Trans::Constants::SEARCHING_DATABASE_1_IN_PATH_2).arg(Constants::DATAPACK_ACCOUNTANCY).arg(pathToDb));

    createConnection(Constants::DATAPACK_DB, Constants::DATAPACK_ACCOUNTANCY_FILENAME,
                     connector,
                     Utils::Database::WarnOnly);

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DATAPACK_ACCOUNTANCY).arg(database().lastError().text()));
        } else {
            qDebug() << __FILE__ << QString::number(__LINE__) << " !database().isOpen() "   ;
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        qDebug() << __FILE__ << QString::number(__LINE__) << " database().isOpen() "   ;
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

    // Code optionnel de vérification de la base
    /*if (!checkDatabaseScheme()) {
        LOG_ERROR_FOR("DatapackBase", tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(Constants::DATAPACK_ACCOUNTANCY));
        setConnectionName(Constants::DATAPACK_DB);
        qWarning() << __FILE__ << QString::number(__LINE__) << "connectionName err = " << connectionName() ;
        return false;
    }*/

    //if (!checkDatabaseVersion()) {
    //    LOG_ERROR_FOR("DatapackBase", QString("Wrong database version. Db: %1; Current: %2").arg(version()).arg(::CURRENTVERSION));
    //    return false;
    //} else {
    //    LOG_FOR("DatapackBase", QString("Using DatapackBase database version " + version()));
    //}

    setConnectionName(Constants::DATAPACK_DB);
    qWarning() << __FILE__ << QString::number(__LINE__) << "connectionName = " << connectionName() ;
    _init = true;
    return true;
}

DatapackBase::~DatapackBase(){}

DatapackBase *DatapackBase::m_Instance = 0;

DatapackBase *DatapackBase::instance()
{
    if (!m_Instance) {
        m_Instance = new DatapackBase(qApp);
    }
    return m_Instance;
}

namespace AccountDB {
namespace Internal {

class DatapackMPModelPrivate
{
public:
    DatapackMPModelPrivate(DatapackMPModel *parent) : m_SqlTable(0), m_IsDirty(false), q(parent)
    {
        // Ensure that the DatapackBase is created and initialized
        AccountDB::DatapackBase *db = AccountDB::DatapackBase::instance();
        db->initialize();
        m_SqlTable = new QSqlTableModel(q, db->database());
        m_SqlTable->setTable(db->table(Constants::Table_DatapackMP));
//        m_SqlTable->setFilter(USER_UID);
    }
    ~DatapackMPModelPrivate () {}

public:
    QSqlTableModel *m_SqlTable;
    bool m_IsDirty;

private:
    DatapackMPModel *q;
};

}  // End namespace Internal
}  // End namespace AccountDB

DatapackMPModel::DatapackMPModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::DatapackMPModelPrivate(this))
{
 
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    d->m_SqlTable->select();
}

DatapackMPModel::~DatapackMPModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int DatapackMPModel::rowCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->rowCount(parent);
}

int DatapackMPModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}

void DatapackMPModel::setUserUuid(const QString &uuid)
{
    QHash<int, QString> where;
    where.insert(Constants::MP_USER_UID, QString("='%1'").arg(uuid));
    d->m_SqlTable->setFilter(DatapackBase::instance()->getWhereClause(Constants::Table_DatapackMP, where));
}

QVariant DatapackMPModel::data(const QModelIndex &index, int role) const
{
    return d->m_SqlTable->data(index, role);
}

bool DatapackMPModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = d->m_SqlTable->setData(index, value, role);
    d->m_IsDirty = d->m_SqlTable->isDirty(index);
    return ret;
}

void DatapackMPModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
}

QString DatapackMPModel::filter(){
    return d->m_SqlTable->filter();
}


QVariant DatapackMPModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

bool DatapackMPModel::insertRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnRowChange);
    bool ret = d->m_SqlTable->insertRows(row, count, parent);
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    return ret;
}

bool DatapackMPModel::removeRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
    return d->m_SqlTable->removeRows(row, count, parent);
}

bool DatapackMPModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        d->m_IsDirty = false;
        return true;
    }
    return false;
}

void DatapackMPModel::revert()
{
    d->m_IsDirty = false;
    d->m_SqlTable->revert();
}

bool DatapackMPModel::isDirty() const
{
    return d->m_IsDirty;
}

QSqlError DatapackMPModel::lastError(){
    return d->m_SqlTable->lastError();
}
