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
/**
  \class ICD::IcdDatabase
  \brief Provides all read access to the ICD10 database.
*/

#include "icddatabase.h"
#include "icdassociation.h"
#include "constants.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/httpdownloader.h>
#include <utils/databaseconnector.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>
#include <coreplugin/translators.h>

#ifdef FREETOOLBOX
#    include <coreplugin/ftb_constants.h>
#endif

#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>
#include <datapackutils/datapackcore.h>
#include <datapackutils/ipackmanager.h>
#include <datapackutils/pack.h>

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QFile>
#include <QDir>
#include <QCache>
#include <QVariant>
#include <QFileInfo>
#include <QMultiHash>

using namespace ICD;
using namespace ICD::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DataPack::DataPackCore &dataPackCore() { return DataPack::DataPackCore::instance(); }
static inline DataPack::IPackManager *packManager() { return dataPackCore().packManager(); }

// Database fullPath
static inline QString fullDatabasePath() {
#ifdef FREETOOLBOX
    return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/icd10/");
#else
//    return settings()->path(Core::ISettings::ReadOnlyDatabasesPath) + QDir::separator() + QString(ICD::Constants::DB_NAME);
    QString dbRelPath = QString("/%1").arg(Constants::DB_NAME);
    QString tmp;
    tmp = settings()->dataPackInstallPath() + dbRelPath;
    if (QFileInfo(tmp).exists())
        return settings()->dataPackInstallPath();
    tmp = settings()->dataPackApplicationInstalledPath() + dbRelPath;
    return settings()->dataPackApplicationInstalledPath();
#endif
}

namespace ICD {
namespace Internal {

struct Daget {
    int associatedSid;
    QString dag;
};

class IcdDatabasePrivate
{
public:
    IcdDatabasePrivate(IcdDatabase *base) :
            m_LogChrono(false), m_DownloadAndPopulate(false), q(base)
    {
        m_CachedCodes.setMaxCost(1000);
        m_CachedDaget.setMaxCost(1000);
        m_CachedSystemLabelsFromSid.setMaxCost(2000);
        m_CachedSystemLabelsFromLid.setMaxCost(2000);
    }

    ~IcdDatabasePrivate()
    {
        qDeleteAll(m_CachedDependentDaget);
        m_CachedDependentDaget.clear();
    }

public:
    bool m_LogChrono, m_DownloadAndPopulate;
    QCache<int, QVariant> m_CachedCodes;
    QCache<int, QString> m_CachedDaget;
    QCache<int, QString> m_CachedSystemLabelsFromSid;
    QCache<int, QString> m_CachedSystemLabelsFromLid;
    QMultiHash<int, Daget *> m_CachedDependentDaget;

private:
    IcdDatabase *q;
};
}  // End Internal
}  // End ICD

//--------------------------------------------------------------------------------------------------------
//--------------------------------- Initialization of static members -------------------------------------
//--------------------------------------------------------------------------------------------------------
IcdDatabase *IcdDatabase::m_Instance = 0;
bool IcdDatabase::m_initialized = false;

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Initializing Database -------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Returns the unique instance of DrugsBase. If it does not exist, it is created */
IcdDatabase *IcdDatabase::instance()
{
    if (!m_Instance) {
        m_Instance = new IcdDatabase(qApp);
    }
    return m_Instance;
}

/**
   \brief Constructor.
   \private
*/
IcdDatabase::IcdDatabase(QObject *parent) :
        QObject(parent), Utils::Database(), d(0)
{
    d = new IcdDatabasePrivate(this);
    setObjectName("IcdDatabase");

    using namespace ICD::Constants;
    // No need to add indexes while db creation uses icd10.sql file
    addTable(Table_Chapter,    "chapter");
    addTable(Table_Common,     "common");
    addTable(Table_Dagstar,    "dagstar");
    addTable(Table_Descr,      "descr");
    addTable(Table_Exclude,    "exclude");
    addTable(Table_Glossaire,  "glossaire");
    addTable(Table_Html,       "html");
    addTable(Table_Include,    "include");
    addTable(Table_Indir,      "indir");
    addTable(Table_Libelle,    "libelle");
    addTable(Table_Master,     "master");
    addTable(Table_Memo,       "memo");
    addTable(Table_Note,       "note");
    addTable(Table_Ref,        "refer");
    addTable(Table_System,     "system");
    addTable(Table_Version,    "version");

    addField(Table_Chapter,  CHAPTER_CHAPTER, "chap");
    addField(Table_Chapter,  CHAPTER_SID,     "SID");
    addField(Table_Chapter,  CHAPTER_ROM,     "rom");

    addField(Table_Common,  COMMON_SID, "SID");
    addField(Table_Common,  COMMON_MALE, "male");
    addField(Table_Common,  COMMON_FEMALE, "female");
    addField(Table_Common,  COMMON_SEQUELLA, "sequella");
    addField(Table_Common,  COMMON_POST, "post");
    addField(Table_Common,  COMMON_SECOND, "second");
    addField(Table_Common,  COMMON_NOCODE, "nocode");

    addField(Table_Dagstar,  DAG_SID, "SID");
    addField(Table_Dagstar,  DAG_LID, "LID");
    addField(Table_Dagstar,  DAG_ASSOC, "assoc");
    addField(Table_Dagstar,  DAG_DAGET, "daget");
    addField(Table_Dagstar,  DAG_PLUS, "plus");

    addField(Table_Descr,  DESCR_SID, "SID");
    addField(Table_Descr,  DESCR_LID, "LID");

    addField(Table_Exclude,  EXCLUDE_SID, "SID");
    addField(Table_Exclude,  EXCLUDE_EXCL, "excl");
    addField(Table_Exclude,  EXCLUDE_PLUS, "plus");
    addField(Table_Exclude,  EXCLUDE_LID, "LID");
    addField(Table_Exclude,  EXCLUDE_DAGET, "daget");

    addField(Table_Glossaire,  GLOSSAIRE_SID, "SID");
    addField(Table_Glossaire,  GLOSSAIRE_MID, "MID");

    addField(Table_Html,  HTML_REF, "ref");
    addField(Table_Html,  HTML_FR, "FR");
    addField(Table_Html,  HTML_EN, "EN");
    addField(Table_Html,  HTML_DE, "GE");

    addField(Table_Include,  INCLUDE_SID, "SID");
    addField(Table_Include,  INCLUDE_LID, "LID");

    addField(Table_Indir,  INDIR_SID, "SID");
    addField(Table_Indir,  INDIR_LID, "LID");

    addField(Table_Libelle,  LIBELLE_LID, "LID");
    addField(Table_Libelle,  LIBELLE_SID, "SID");
    addField(Table_Libelle,  LIBELLE_SOURCE, "source");
    addField(Table_Libelle,  LIBELLE_VALID, "valid");
    addField(Table_Libelle,  LIBELLE_LIBELLE, "libelle");
    addField(Table_Libelle,  LIBELLE_FR, "FR_OMS");
    addField(Table_Libelle,  LIBELLE_EN, "EN_OMS");
    addField(Table_Libelle,  LIBELLE_DE_DIMDI, "GE_DIMDI");
    addField(Table_Libelle,  LIBELLE_DE_AUTO, "GE_AUTO");
    addField(Table_Libelle,  LIBELLE_FRCHRONOS, "FR_CHRONOS");
    addField(Table_Libelle,  LIBELLE_DATE, "date");
    addField(Table_Libelle,  LIBELLE_AUTHOR, "author");
    addField(Table_Libelle,  LIBELLE_COMMENT, "comment");

    addField(Table_Master,  MASTER_SID, "SID");
    addField(Table_Master,  MASTER_CODE, "code");
    addField(Table_Master,  MASTER_SORT, "sort");
    addField(Table_Master,  MASTER_ABBREV, "abbrev");
    addField(Table_Master,  MASTER_LEVEL, "level");
    addField(Table_Master,  MASTER_TYPE, "type");
    addField(Table_Master,  MASTER_ID1, "id1");
    addField(Table_Master,  MASTER_ID2, "id2");
    addField(Table_Master,  MASTER_ID3, "id3");
    addField(Table_Master,  MASTER_ID4, "id4");
    addField(Table_Master,  MASTER_ID5, "id5");
    addField(Table_Master,  MASTER_ID6, "id6");
    addField(Table_Master,  MASTER_ID7, "id7");
    addField(Table_Master,  MASTER_VALID, "valid");
    addField(Table_Master,  MASTER_DATE, "date");
    addField(Table_Master,  MASTER_AUTHOR, "author");
    addField(Table_Master,  MASTER_COMMENT, "comment");

    addField(Table_Memo,  MEMO_MID, "MID");
    addField(Table_Memo,  MEMO_SID, "SID");
    addField(Table_Memo,  MEMO_SOURCE, "source");
    addField(Table_Memo,  MEMO_VALID, "valid");
    addField(Table_Memo,  MEMO_MEMO, "memo");
    addField(Table_Memo,  MEMO_FR, "FR_OMS");
    addField(Table_Memo,  MEMO_EN, "EN_OMS");
    addField(Table_Memo,  MEMO_DE, "GE_DIMDI");
    addField(Table_Memo,  MEMO_DATE, "date");
    addField(Table_Memo,  MEMO_AUTHOR, "author");
    addField(Table_Memo,  MEMO_COMMENT, "comment");

    addField(Table_Note,  NOTE_SID, "SID");
    addField(Table_Note,  NOTE_MID, "MID");

    addField(Table_Ref,  REF_SID, "SID");
    addField(Table_Ref,  REF_LID, "LID");
    addField(Table_Ref,  REF_REF, "ref");

    addField(Table_System,  SYSTEM_SID, "SID");
    addField(Table_System,  SYSTEM_LID, "LID");

    addField(Table_Version,  VERSION_NAME, "name");
    addField(Table_Version,  VERSION_VERSION, "version");
    addField(Table_Version,  VERSION_BUILD, "build");
    addField(Table_Version,  VERSION_VALID, "valid");
    addField(Table_Version,  VERSION_DATE, "date");
    addField(Table_Version,  VERSION_COMMENT, "expl");

    connect(Core::ICore::instance()->translators(), SIGNAL(languageChanged()), this, SLOT(refreshLanguageDependCache()));

    connect(packManager(), SIGNAL(packInstalled(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
    connect(packManager(), SIGNAL(packRemoved(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
//    connect(packManager(), SIGNAL(packUpdated(DataPack::Pack)), this, SLOT(packChanged(DataPack::Pack)));
}

/** \brief Destructor. */
IcdDatabase::~IcdDatabase()
{
    if (d)
        delete d;
    d=0;
}

bool IcdDatabase::initialize()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // test driver (ICD is provided with the application --> SQLite)
     if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
         LOG_ERROR(tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE).arg("SQLite"));
         Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
                                  tkTr(Trans::Constants::DATABASE_DRIVER_1_NOT_AVAILABLE_DETAIL).arg("SQLite"),
                                  "", qApp->applicationName());
         return false;
      }

     // log the path of the database
     QString pathToDb = fullDatabasePath();
     LOG(tkTr(Trans::Constants::SEARCHING_DATABASE_1_IN_PATH_2).arg(Constants::DB_NAME).arg(pathToDb));

     // Removing existing old connections
     if (QSqlDatabase::contains(Constants::DB_NAME)) {
         QSqlDatabase::removeDatabase(Constants::DB_NAME);
     }

     // Connect normal Account Database
     // Connect Drugs Database
     Utils::DatabaseConnector connector;
//     connector.setAbsPathToReadOnlySqliteDatabase(settings()->path(Core::ISettings::ReadOnlyDatabasesPath));
     connector.setAbsPathToReadOnlySqliteDatabase(fullDatabasePath());
     connector.setHost(QString(Constants::DB_NAME) + ".db");
     connector.setAccessMode(Utils::DatabaseConnector::ReadOnly);
     connector.setDriver(Utils::Database::SQLite);

     if (!createConnection(Constants::DB_NAME, QString(Constants::DB_NAME) + ".db", connector)) {
         d->m_DownloadAndPopulate = true;
     }

     if (!database().isOpen()) {
         if (!database().open()) {
             LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
         } else { // db successfully opened
             LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));

             // check database scheme
             if (!checkDatabaseScheme()) {
                 LOG_ERROR(tr("ICD10 database corrupted, please contact your administrator."));
             }
         }
     } else {
         LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
     }

     if (!d->m_DownloadAndPopulate)
         m_initialized = true;

     if (m_initialized) {
         Q_EMIT databaseInitialized();
     }

     return true;
}

bool IcdDatabase::refreshDatabase()
{
    WARN_FUNC;
    m_initialized = false;
    d->m_DownloadAndPopulate = false;
    if (QSqlDatabase::connectionNames().contains(Constants::DB_NAME))
        QSqlDatabase::removeDatabase(Constants::DB_NAME);
    return initialize();
}

void IcdDatabase::packChanged(const DataPack::Pack &pack)
{
    if (pack.dataType() == DataPack::Pack::ICD) {
        refreshDatabase();
        Q_EMIT databaseChanged();
    }
}

void IcdDatabase::refreshLanguageDependCache()
{
    d->m_CachedSystemLabelsFromLid.clear();
    d->m_CachedSystemLabelsFromSid.clear();
}

void IcdDatabase::logChronos(bool state)
{
    Q_UNUSED(state);
}

QString IcdDatabase::getDatabaseVersion()
{
    QString toReturn;
    if (!m_initialized)
        return toReturn;
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return toReturn;
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::VERSION_VALID, QString("=1"));
    QString req = select(Constants::Table_Version, where);
    if (query.exec(req)) {
        if (query.next()) {
            toReturn = query.value(Constants::VERSION_NAME).toString() +
                       query.value(Constants::VERSION_VERSION).toString() +
                       query.value(Constants::VERSION_BUILD).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

static QString reversedDagStar(const QString &s)
{
    if (s=="F" || s=="G" || s=="H") {
        return "†";
    } else if (s=="S" || s=="T" || s=="U") {
        return "*";
    } else {
        return QString();
    }
}

static QString humanReadableDagStar(const QString &s)
{
    if (s=="F") {
        return "(*)";
    } else if (s=="G" || s=="H") {
        return "*";
    } else if (s=="S" || s=="U") {
          return "(†)";
    } else if (s=="T") {
        return "†";
    } else {
        return QString();
    }
}

static int getLibelleLanguageField()
{
    const QString &lang = QLocale().name().left(2);
    int langField = Constants::LIBELLE_EN;
    if (lang=="fr") {
        langField = Constants::LIBELLE_FR;
    } else if (lang=="de") {
        langField = Constants::LIBELLE_DE_DIMDI;
    }
    return langField;
}

bool IcdDatabase::isDownloadAndPopulatingNeeded() const
{
    return (d->m_DownloadAndPopulate);
}

QList<int> IcdDatabase::getHeadersSID(const QVariant &SID)
{
    QList<int> toReturn;
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return toReturn;
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::MASTER_SID, QString("=%1").arg(SID.toString()));
    QString req = select(Constants::Table_Master, QList<int>()
                         << Constants::MASTER_ID1
                         << Constants::MASTER_ID2
                         << Constants::MASTER_ID3
                         << Constants::MASTER_ID4
                         << Constants::MASTER_ID5
                         << Constants::MASTER_ID6
                         << Constants::MASTER_ID7
                         , where);
    if (query.exec(req)) {
        if (query.next()) {
            toReturn
                    << query.value(0).toInt()
                    << query.value(1).toInt()
                    << query.value(2).toInt()
                    << query.value(3).toInt()
                    << query.value(4).toInt()
                    << query.value(5).toInt()
                    << query.value(6).toInt();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

QVariant IcdDatabase::getSid(const QString &code)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QVariant();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::MASTER_CODE, QString("='%1'").arg(code));
    QString req = select(Constants::Table_Master, Constants::MASTER_SID, where);
    if (query.exec(req)) {
        if (query.next()) {
            QVariant *s = new QVariant(code);
            d->m_CachedCodes.insert(query.value(0).toInt(), s);
            return query.value(0);
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return QVariant();
}

QVariant IcdDatabase::getIcdCode(const QVariant &SID)
{
    if (d->m_CachedCodes.keys().contains(SID.toInt())) {
        return *d->m_CachedCodes[SID.toInt()];
    }
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QVariant();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::MASTER_SID, QString("=%1").arg(SID.toString()));
    QString req = select(Constants::Table_Master, Constants::MASTER_CODE, where);
    if (query.exec(req)) {
        if (query.next()) {
            QVariant *qvar = new QVariant(query.value(0));
            d->m_CachedCodes.insert(SID.toInt(), qvar);
            return *qvar;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return QVariant();
}

QString IcdDatabase::getDagStarCode(const QVariant &SID)
{
    if (d->m_CachedDaget.keys().contains(SID.toInt())) {
        QString *s = d->m_CachedDaget[SID.toInt()];
        return *s;
    }
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QString();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::DAG_SID, QString("=%1").arg(SID.toString()));
    QString req = select(Constants::Table_Dagstar, QList<int>() << Constants::DAG_DAGET << Constants::DAG_ASSOC, where);
    if (query.exec(req)) {
        QString *s = new QString();
        while (query.next()) {
            *s = query.value(0).toString();
            if (query.value(1).toInt()==0) {
                break;
            }
        }
        d->m_CachedDaget.insert(SID.toInt(), s);
        return *s;

    } else {
        LOG_QUERY_ERROR(query);
    }
    return QChar();
}

QString IcdDatabase::invertDagCode(const QString &s) const
{
    if (s=="F")
        return "S";
    else if (s=="G")
        return "T";
    else if (s=="H")
        return "U";
    else if (s=="S")
        return "F";
    else if (s=="T")
        return "G";
    else if (s=="U")
        return "H";
    return s;
}

bool IcdDatabase::isDagetADag(const QString &dagCode) const
{
    return (dagCode=="S" || dagCode=="T" || dagCode=="U");
}

QString IcdDatabase::getHumanReadableIcdDaget(const QVariant &SID)
{
    return humanReadableDagStar(getDagStarCode(SID));
}

QVariant IcdDatabase::getIcdCodeWithDagStar(const QVariant &SID)
{
    QString tmp = getIcdCode(SID).toString() + getHumanReadableIcdDaget(SID);
    return QVariant(tmp);
}

QVector<int> IcdDatabase::getDagStarDependencies(const QVariant &SID)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QVector<int>();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::DAG_SID, QString("=%1").arg(SID.toString()));
    QString req = select(Constants::Table_Dagstar, Constants::DAG_ASSOC, where);
    QVector<int> sids;
    if (query.exec(req)) {
        while (query.next()) {
             sids << query.value(0).toInt();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return sids;
}

Internal::IcdAssociation IcdDatabase::getAssociation(const QVariant &mainSID, const QVariant &associatedSID)
{
    if (d->m_CachedDependentDaget.keys().contains(mainSID.toInt())) {
        foreach(Daget *dag, d->m_CachedDependentDaget.values(mainSID.toInt())) {
            if (dag->associatedSid == associatedSID) {
                Internal::IcdAssociation asso(mainSID, associatedSID, dag->dag);
                asso.setMainHumanReadableDaget(humanReadableDagStar(dag->dag));
                asso.setAssociatedHumanReadableDaget(reversedDagStar(dag->dag));
                return asso;
            }
        }
    }
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return Internal::IcdAssociation();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::DAG_SID, QString("=%1").arg(mainSID.toString()));
    where.insert(Constants::DAG_ASSOC, QString("=%1").arg(associatedSID.toString()));
    QString req = select(Constants::Table_Dagstar, Constants::DAG_DAGET, where);
    if (query.exec(req)) {
        Daget *dag = new Daget;
        dag->associatedSid = associatedSID.toInt();
        if (query.next()) {
            dag->dag = query.value(0).toString();
        }
        Internal::IcdAssociation asso(mainSID, associatedSID, dag->dag);
        asso.setMainHumanReadableDaget(humanReadableDagStar(dag->dag));
        asso.setAssociatedHumanReadableDaget(reversedDagStar(dag->dag));
        d->m_CachedDependentDaget.insert(mainSID.toInt(), dag);

//        qWarning() << "Base" << asso.mainCodeWithDagStar() << asso.associatedCodeWithDagStar() << asso.dagCode();

        return asso;
    } else {
        LOG_QUERY_ERROR(query);
    }
    return Internal::IcdAssociation();
}

bool IcdDatabase::codeCanBeUsedAlone(const QVariant &SID)
{
    const QString &s = this->getDagStarCode(SID);
    if (s=="F" || s =="S" || s.isEmpty()) {
        return true;
    }
    return false;
}

//QString IcdDatabase::getDagStarCodeWithDependency(const QVariant &SID, const QVariant &dependOnSID)
//{
//    if (d->m_CachedDependentDaget.keys().contains(SID.toInt())) {
//        foreach(Daget *dag, d->m_CachedDependentDaget.values(SID.toInt())) {
//            if (dag->dependOnSid == dependOnSID)
//                return reversedDagStar(dag->dag);
//        }
//    }
//    if (!database().isOpen()) {
//        if (!database().open()) {
//            LOG_ERROR(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
//            return QString();
//        }
//    }
//    QSqlQuery query(database());
//    QHash<int, QString> where;
//    where.insert(Constants::DAG_SID, QString("=%1").arg(dependOnSID.toString()));
//    where.insert(Constants::DAG_ASSOC, QString("=%1").arg(SID.toString()));
//    QString req = select(Constants::Table_Dagstar, Constants::DAG_DAGET, where);
//    if (query.exec(req)) {
//        Daget *dag = new Daget;
//        dag->dependOnSid = dependOnSID.toInt();
//        if (query.next()) {
//            dag->dag = query.value(0).toString();
//        }
//        d->m_CachedDependentDaget.insert(SID.toInt(), dag);
//        return dag->dag;

//    } else {
//                LOG_QUERY_ERROR(query);
//    }
//    return QChar();
//}

//QString IcdDatabase::getHumanReadableIcdDagetWithDependency(const QVariant &SID, const QVariant &dependOnSID)
//{
//    return reversedDagStar(getDagStarCodeWithDependency(SID, dependOnSID));
//}

QString IcdDatabase::getLabelFromLid(const QVariant &LID)
{
    int lid = LID.toInt();
    if (d->m_CachedSystemLabelsFromLid.keys().contains(lid)) {
        return *d->m_CachedSystemLabelsFromLid[lid];
    }
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QString();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::LIBELLE_LID, QString("=%1").arg(LID.toString()));
    QString req = select(Constants::Table_Libelle, getLibelleLanguageField(), where);
    if (query.exec(req)) {
        if (query.next()) {
            QString *s = new QString(query.value(0).toString());
            d->m_CachedSystemLabelsFromLid.insert(lid, s);
            return *s;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return QString();
}

QString IcdDatabase::getSystemLabel(const QVariant &SID)
{
    int sid = SID.toInt();
    if (d->m_CachedSystemLabelsFromSid.keys().contains(sid)) {
        return *d->m_CachedSystemLabelsFromSid[sid];
    }
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QString();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::SYSTEM_SID, QString("=%1").arg(SID.toString()));
    // "SELECT FR_OMS FROM `libelle`, `system` WHERE  (`system`.`SID`=193) AND `libelle`.`SID`=`system`.`SID`"
    QString req = select(Constants::Table_Libelle, getLibelleLanguageField()) + ", `" +
                  table(Constants::Table_System) +
                  "` WHERE " + getWhereClause(Constants::Table_System, where) +
                  " AND " + fieldEquality(Constants::Table_Libelle, Constants::LIBELLE_SID,
                                Constants::Table_System, Constants::SYSTEM_SID);

    if (query.exec(req)) {
        if (query.next()) {
            QString *s = new QString(query.value(0).toString());
            d->m_CachedSystemLabelsFromSid.insert(sid, s);
            return *s;
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return QString();
}

QStringList IcdDatabase::getAllLabels(const QVariant &SID, const int libelleFieldLang)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QStringList();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    QString req;

    int field = libelleFieldLang;
    if (libelleFieldLang==-1) {
        field = getLibelleLanguageField();
    }

    // get all labels
    where.insert(Constants::LIBELLE_SID, QString("=%1").arg(SID.toInt()));
    req = select(Constants::Table_Libelle, field, where);
    QStringList toReturn;
    if (query.exec(req)) {
        while (query.next()) {
            toReturn << query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return toReturn;
}

QString IcdDatabase::getAssociatedLabel(const QVariant &mainSID, const QVariant &associatedSID)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QString();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::DAG_SID, QString("=%1").arg(mainSID.toString()));
    where.insert(Constants::DAG_ASSOC, QString("=%1").arg(associatedSID.toString()));
    QString req = select(Constants::Table_Libelle, getLibelleLanguageField()) + ", `" +
                  table(Constants::Table_Dagstar) +
                  "` WHERE " + getWhereClause(Constants::Table_Dagstar, where) +
                  " AND " + fieldEquality(Constants::Table_Libelle, Constants::LIBELLE_LID,
                                          Constants::Table_Dagstar, Constants::DAG_LID);

    if (query.exec(req)) {
        if (query.next()) {
            return query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return QString();
}

QStringList IcdDatabase::getIncludedLabels(const QVariant &SID)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QStringList();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    QString req;
    QVector<int> lids;
    QList<int> fullTreeCode = getHeadersSID(SID);
    foreach(int sid, fullTreeCode) {
        where.clear();
        where.insert(Constants::INCLUDE_SID, QString("=%1").arg(sid));
        req = select(Constants::Table_Include, Constants::INCLUDE_LID, where);
        if (query.exec(req)) {
            while (query.next()) {
                lids << query.value(0).toInt();
            }
        } else {
            LOG_QUERY_ERROR(query);
        }
    }
    QStringList toReturn;
    foreach(const int lid, lids) {
        toReturn << this->getLabelFromLid(lid);
    }
    return toReturn;
}

QVector<int> IcdDatabase::getExclusions(const QVariant &SID)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QVector<int>();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    QString req;
    QVector<int> toReturn;
    QList<int> fullTreeCode = getHeadersSID(SID);
    fullTreeCode << SID.toInt();
    foreach(int sid, fullTreeCode) {
        if (sid==0)
            continue;
        where.clear();
        where.insert(Constants::EXCLUDE_SID, QString("=%1").arg(sid));
        req = select(Constants::Table_Exclude, Constants::EXCLUDE_EXCL, where);
        if (query.exec(req)) {
            while (query.next()) {
                toReturn << query.value(0).toInt();
            }
        } else {
            LOG_QUERY_ERROR(query);
        }
    }
    return toReturn;
}

QString IcdDatabase::getMemo(const QVariant &SID)
{
    // get the MID linked to the SID
    // get the Note
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_NAME).arg(database().lastError().text()));
            return QString();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::NOTE_SID, QString("=%1").arg(SID.toString()));
    int labelField;
    const QString &lang = QLocale().name().left(2);
    if (lang=="en") {
        labelField = Constants::MEMO_EN;
    } else if (lang=="fr") {
        labelField = Constants::MEMO_FR;
    } else if (lang=="de") {
        labelField = Constants::MEMO_DE;
    }
    // SELECT `FR_OMS` FROM `memo`, `note` WHERE  (`note`.`SID` =4508) AND `note`.`SID`=`memo`.`SID`
    QString req = select(Constants::Table_Memo, labelField) + ", `" +
                  table(Constants::Table_Note) +
                  "` WHERE " + getWhereClause(Constants::Table_Note, where) +
                  " AND " + fieldEquality(Constants::Table_Note, Constants::NOTE_SID,
                                Constants::Table_Memo, Constants::MEMO_SID);

    if (query.exec(req)) {
        if (query.next()) {
            return query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    return QString();
}
