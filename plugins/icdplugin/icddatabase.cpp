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
#include <QMultiHash>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QSet>

using namespace ICD;
using namespace ICD::Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


namespace ICD {
namespace Internal {
/**
  \brief Private part of DrugsBase
  \internal
*/
class IcdDatabasePrivate
{
public:
    IcdDatabasePrivate(IcdDatabase *base) : q(base) {}
    ~IcdDatabasePrivate()
    {
    }

public:
    IcdDatabase *q;
    bool m_LogChrono;
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

    addField(Table_Html,  HTML_SID, "SID");
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
     QString pathToDb = settings()->path(Core::ISettings::ReadOnlyDatabasesPath);
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

