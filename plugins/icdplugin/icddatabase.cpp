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
 ***************************************************************************/
#include "icddatabase.h"
#include "constants.h"

#include <utils/global.h>
#include <utils/log.h>
#include <utils/httpdownloader.h>

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/constants.h>

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

using namespace ICD;
using namespace ICD::Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


namespace ICD {
namespace Internal {


struct Daget {
    int dependOnSid;
    QString dag;
};

class IcdDatabasePrivate
{
public:
    IcdDatabasePrivate(IcdDatabase *base) : q(base)
    {
        m_CachedCodes.setMaxCost(1000);
        m_CachedDaget.setMaxCost(1000);
    }

    ~IcdDatabasePrivate()
    {
        qDeleteAll(m_CachedDependentDaget);
        m_CachedDependentDaget.clear();
    }

public:
    IcdDatabase *q;
    bool m_LogChrono;
    QCache<int, QVariant> m_CachedCodes;
    QCache<int, QString> m_CachedDaget;
    QMultiHash<int, Daget *> m_CachedDependentDaget;
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
/** \brief Returns the unique instance of DrugsBase. If it does not exists, it is created */
IcdDatabase *IcdDatabase::instance()
{
    if (!m_Instance) {
        m_Instance = new IcdDatabase(qApp);
        m_Instance->init();
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

    init();
}

/** \brief Destructor. */
IcdDatabase::~IcdDatabase()
{
    if (d) delete d;
    d=0;
}

bool IcdDatabase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // test driver (ICD is provided with the application --> SQLite)
     if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
         Utils::Log::addError(this, tkTr(Trans::Constants::SQLITE_DRIVER_NOT_AVAILABLE));
         Utils::warningMessageBox(tkTr(Trans::Constants::APPLICATION_FAILURE),
                                  tkTr(Trans::Constants::SQLITE_DRIVER_NOT_AVAILABLE_DETAIL),
                                  "", qApp->applicationName());
          return false;
      }

     // log the path of the database
     QString pathToDb = settings()->path(Core::ISettings::ReadOnlyDatabasesPath) + QDir::separator() + QString(ICD::Constants::DB_ICD10);
     Utils::Log::addMessage(this, tr("Searching databases into dir %1").arg(pathToDb));

     // Connect normal Account Database
     createConnection(ICD::Constants::DB_ICD10, QString(Constants::DB_ICD10) + ".db", pathToDb,
                      Utils::Database::ReadWrite, Utils::Database::SQLite, "", "", CreateDatabase);

     if (!checkDatabaseScheme()) {
         Utils::Log::addError(this, tr("ICD10 database corrupted, please contact your administrator."));
     }

     if (!database().isOpen()) {
         if (!database().open()) {
             Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
         }
     }

     m_initialized = true;
     return true;
}

void IcdDatabase::logChronos(bool state)
{
    Q_UNUSED(state);
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
    } else if (s=="S") {
          return "(†)";
    } else if (s=="T" || s=="U") {
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

QList<int> IcdDatabase::getHeadersSID(const QVariant &SID)
{
    QList<int> toReturn;
    if (!database().isOpen()) {
        if (!database().open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
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
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    return toReturn;
}

QVariant IcdDatabase::getIcdCode(const QVariant &SID)
{
    if (d->m_CachedCodes.keys().contains(SID.toInt())) {
        return *d->m_CachedCodes[SID.toInt()];
    }
    if (!database().isOpen()) {
        if (!database().open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
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
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
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
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
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
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    return QChar();
}

QString IcdDatabase::getHumanReadableIcdDaget(const QVariant &SID)
{
    return humanReadableDagStar(getDagStarCode(SID));
}

QVariant IcdDatabase::getIcdCodeWithDagStar(const QVariant &SID)
{
    return getIcdCode(SID).toString() + getHumanReadableIcdDaget(SID);
}

QVector<int> IcdDatabase::getDagStarDependencies(const QVariant &SID)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
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
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    return sids;
}

QString IcdDatabase::getDagStarCodeWithDependency(const QVariant &SID, const QVariant &dependOnSID)
{
    if (d->m_CachedDependentDaget.keys().contains(SID.toInt())) {
        foreach(Daget *dag, d->m_CachedDependentDaget.values(SID.toInt())) {
            if (dag->dependOnSid == dependOnSID)
                return reversedDagStar(dag->dag);
        }
    }
    if (!database().isOpen()) {
        if (!database().open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
            return QString();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::DAG_SID, QString("=%1").arg(dependOnSID.toString()));
    where.insert(Constants::DAG_ASSOC, QString("=%1").arg(SID.toString()));
    QString req = select(Constants::Table_Dagstar, Constants::DAG_DAGET, where);
    if (query.exec(req)) {
        Daget *dag = new Daget;
        dag->dependOnSid = dependOnSID.toInt();
        if (query.next()) {
            dag->dag = query.value(0).toString();
        }
        d->m_CachedDependentDaget.insert(SID.toInt(), dag);
        return dag->dag;

    } else {
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    return QChar();
}

QString IcdDatabase::getHumanReadableIcdDagetWithDependency(const QVariant &SID, const QVariant &dependOnSID)
{
    return reversedDagStar(getDagStarCodeWithDependency(SID, dependOnSID));
}

QString IcdDatabase::getLabelFromLid(const QVariant &LID)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
            return QString();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    where.insert(Constants::LIBELLE_LID, QString("=%1").arg(LID.toString()));
    QString req = select(Constants::Table_Libelle, getLibelleLanguageField(), where);
    if (query.exec(req)) {
        if (query.next()) {
            return query.value(0).toString();
        }
    } else {
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    return QString();
}

QString IcdDatabase::getSystemLabel(const QVariant &SID)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
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
            return query.value(0).toString();
        }
    } else {
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    return QString();
}

QStringList IcdDatabase::getAllLabels(const QVariant &SID, const int libelleFieldLang)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
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
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    return toReturn;
}

QStringList IcdDatabase::getIncludedLabels(const QVariant &SID)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
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
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
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
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
            return QVector<int>();
        }
    }
    QSqlQuery query(database());
    QHash<int, QString> where;
    QString req;
    QVector<int> toReturn;
    QList<int> fullTreeCode = getHeadersSID(SID);
    foreach(int sid, fullTreeCode) {
        where.clear();
        where.insert(Constants::EXCLUDE_SID, QString("=%1").arg(sid));
        req = select(Constants::Table_Exclude, Constants::EXCLUDE_EXCL, where);
        if (query.exec(req)) {
            while (query.next()) {
                toReturn << query.value(0).toInt();
            }
        } else {
            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
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
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(Constants::DB_ICD10).arg(database().lastError().text()), __FILE__, __LINE__);
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
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    return QString();
}
