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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "bibliobase.h"

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/ftb_constants.h>

#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constanttranslations.h>

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QDir>

using namespace Biblio;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline QString workingPath()        {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString()) + QDir::separator();}

namespace {
    enum Tables {
        Table_BIB = 0
    };
    enum fieldsBIB {
        BIB_ID = 0, BIB_PMID, BIB_XML,
        BIB_MaxParam
    };

    const char* const DB_NAME = "biblio";
}

BiblioBase::BiblioBase(QObject *parent) :
    QObject(parent), Utils::Database(), m_initialized(false)
{
    setObjectName("BiblioBase");
    addTable(Table_BIB, "BIB");

    addField(Table_BIB, BIB_ID, "ID", FieldIsUniquePrimaryKey);
    addField(Table_BIB, BIB_PMID, "PMID");
    addField(Table_BIB, BIB_XML, "XML");
}

bool BiblioBase::init()
{
    // only one base can be initialized
    if (m_initialized)
        return true;

    // connect
    Utils::DatabaseConnector c;
    c.setAccessMode(Utils::DatabaseConnector::ReadWrite);
    c.setDriver(Utils::Database::SQLite);
    c.setAbsPathToReadWriteSqliteDatabase(workingPath());

    createConnection(DB_NAME, DB_NAME, c, Utils::Database::CreateDatabase);

    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2).arg(DB_NAME).arg(database().lastError().text()));
        } else {
            LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
        }
    } else {
        LOG(tkTr(Trans::Constants::CONNECTED_TO_DATABASE_1_DRIVER_2).arg(database().connectionName()).arg(database().driverName()));
    }

    if (!checkDatabaseScheme()) {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_SCHEMA_ERROR).arg(DB_NAME));
        return false;
    }

    m_initialized = true;
    return true;
}

bool BiblioBase::createDatabase(const QString &connectionName , const QString &dbName,
                    const QString &pathOrHostName,
                    TypeOfAccess /*access*/, AvailableDrivers driver,
                    const QString & /*login*/, const QString & /*pass*/,
                    const int /*port*/,
                    CreationOption /*createOption*/
                   )
{
    if (connectionName != DB_NAME)
        return false;
    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2).arg(dbName).arg(pathOrHostName));

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

    return true;
}

bool BiblioBase::hasPmid(const QString &pmid)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                      .arg(database().connectionName()).arg(database().lastError().text()));
            return false;
        }
    }
    QHash<int, QString> where;
    where.insert(BIB_PMID, QString("='%1'").arg(pmid));
    return count(Table_BIB, BIB_ID, getWhereClause(Table_BIB, where));
}

void BiblioBase::save(const QString &pmid, const QString &xml)
{
    if (hasPmid(pmid))
        return;
    QSqlQuery query(database());
    query.prepare(prepareInsertQuery(Table_BIB));
    query.bindValue(BIB_ID, QVariant());
    query.bindValue(BIB_PMID, pmid);
    query.bindValue(BIB_XML, xml);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
    }
}

QString BiblioBase::getXmlForPMID(const QString &pmid)
{
    if (!database().isOpen()) {
        if (!database().open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                      .arg(database().connectionName()).arg(database().lastError().text()));
            return QString();
        }
    }
    QHash<int, QString> where;
    where.insert(BIB_PMID, QString("='%1'").arg(pmid));
    QSqlQuery query(database());
    if (query.exec(select(Table_BIB, BIB_XML, where))) {
        if (query.next())
            return query.value(0).toString();
    } else {
        LOG_QUERY_ERROR(query);
    }
    return QString();
}
