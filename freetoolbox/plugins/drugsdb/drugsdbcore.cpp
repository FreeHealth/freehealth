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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DrugsDB::DrugsDBCore
 * \brief short description of class
 *
 * Long description of class
 * \sa DrugsDB::
 */

#include "drugsdbcore.h"
#include "drugdatabasedescription.h"
#include "tools.h"
#include "routesmodel.h"
#include "moleculelinkermodel.h"

#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <coreplugin/ftb_constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QDomDocument>

#include <QDebug>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

DrugsDB::DrugsDBCore *DrugsDB::DrugsDBCore::_instance = 0;

namespace DrugsDB {
namespace Internal {
class DrugsDBCorePrivate
{
public:
    DrugsDBCorePrivate(DrugsDBCore *parent) :
        _routesModel(0),
        _moleculeLinkerModel(0),
        q(parent)

    {
    }
    
    ~DrugsDBCorePrivate()
    {
    }

    DrugsDB::Internal::DrugBaseEssentials *drugsBaseFromCache(const QString &connection)
    {
        for(int i=0; i < _drugsDatabases.count(); ++i) {
            if (_drugsDatabases.at(i)->connectionName() == connection)
                return _drugsDatabases.at(i);
        }
        return 0;
    }
    
public:
    QVector<DrugsDB::Internal::DrugBaseEssentials*> _drugsDatabases;
    RoutesModel *_routesModel;
    MoleculeLinkerModel *_moleculeLinkerModel;

private:
    DrugsDBCore *q;
};
}  // namespace Internal
} // end namespace DrugsDB

/*! Constructor of the DrugsDB::DrugsDBCore class */
DrugsDBCore::DrugsDBCore(QObject *parent) :
    QObject(parent),
    d(new DrugsDBCorePrivate(this))
{
    _instance = this;
}

/** Return the single instance of the object */
DrugsDBCore *DrugsDBCore::instance()
{
    return _instance;
}

/*! Destructor of the DrugsDB::DrugsDBCore class */
DrugsDBCore::~DrugsDBCore()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DrugsDBCore::initialize()
{
    d->_routesModel = new RoutesModel(this);
    d->_routesModel->initialize();
    d->_moleculeLinkerModel = new MoleculeLinkerModel(this);
    d->_moleculeLinkerModel->initialize();
    return true;
}

/** Return the singleton of the DrugsDB::RoutesModel pointer */
RoutesModel *DrugsDBCore::routesModel() const
{
    return d->_routesModel;
}

MoleculeLinkerModel *DrugsDBCore::moleculeLinkerModel() const
{
    return d->_moleculeLinkerModel;
}

DrugsDB::Internal::DrugBaseEssentials *DrugsDBCore::drugBase()
{
    static DrugsDB::Internal::DrugBaseEssentials *core = 0;
    if (!core)
        core = new DrugsDB::Internal::DrugBaseEssentials;
    return core;
}

///** Return the drugs database full path, for a specific country or for all countries if no \e country is specified. */
//QString DrugsDBCore::drugsDatabaseAbsFileName(const QString &country)
//{
//    Q_UNUSED(country);
//    //    QString path = QString("%1/%2/%3")
//    //            .arg(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString())
//    //            .arg(Core::Constants::MASTER_DATABASE_NAME)
//    //            .arg(country)
//    //            .arg(Core::Constants::MASTER_DATABASE_FILENAME);
//    //    return QDir::cleanPath(path);
//    //    }
//    return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::MASTER_DATABASE_FILENAME);
//}

bool DrugsDBCore::createMasterDrugInteractionDatabase()
{
//    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
//    if (!db.isOpen() && db.isValid()) {
//        if (!connectDatabase(Core::Constants::MASTER_DATABASE_NAME, drugsDatabaseAbsFileName())) {
//            LOG_ERROR_FOR("Tools", "Unable to create master database");
//            return false;
//        }
//    }

//    if (!db.isValid() || db.tables().count() < DrugsDB::Constants::Table_MaxParam) {
////        executeSqlFile(Core::Constants::MASTER_DATABASE_NAME, masterDatabaseSqlSchema());
//        drugBase()->initialize(databaseOutputPath(), true);
//        addRoutesToDatabase(Core::Constants::MASTER_DATABASE_NAME, routesCsvAbsFile());
//    }
    return true;
}

DrugsDB::Internal::DrugBaseEssentials *DrugsDBCore::createDrugDatabase(const QString &absPath, const QString &connection)
{
    DrugsDB::Internal::DrugBaseEssentials *base = d->drugsBaseFromCache(connection);
    // Already in cache
    if (base)
        return 0;

    // Create a drugessentialbase
    base = new DrugsDB::Internal::DrugBaseEssentials;
    if (!connection.isEmpty())
        base->setConnectionName(connection);
    base->initialize(absPath, true);
    QSqlDatabase db = base->database();
    if (!db.isOpen() && db.isValid()) {
        LOG_ERROR("Unable to connect to drugs database: " + connection);
        return 0;
    }
    d->_drugsDatabases << base;
    return base;
}

bool DrugsDBCore::saveDrugDatabaseDescription(const QString &fileName, const int completion)
{
    QDomDocument doc;
    int line = 0;
    int col = 0;
    QString error;
    if (!doc.setContent(Utils::readTextFile(fileName, Utils::DontWarnUser), &error, &line, &col)) {
        LOG_ERROR_FOR("Tools", QString("Wrong XML: (%1;%2): %3 in file %4").arg(line).arg(col).arg(error).arg(fileName));
        return false;
    }
    DrugDatabaseDescription descr;
    QDomElement root = doc.firstChildElement("DrugDbDescriptor");
    root = root.firstChildElement("description");
    descr.fromDomElement(root);
//    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
//    if (!db.isOpen()) {
//        if (!db.open()) {
//            return false;
//        }
//    }
//    QSqlQuery query(db);
//    QHash<int, QString> where;
//    where.insert(DrugsDB::Constants::SOURCES_DBUID, QString("='%1'").arg(descr.data(Core::DrugDatabaseDescription::Uuid).toString()));
//    query.prepare(DrugsDB::Tools::drugBase()->prepareUpdateQuery(DrugsDB::Constants::Table_SOURCES,
//                                                              QList<int>()
//                                                              << DrugsDB::Constants::SOURCES_LANG
//                                                              << DrugsDB::Constants::SOURCES_WEB
//                                                              << DrugsDB::Constants::SOURCES_COPYRIGHT
//                                                              << DrugsDB::Constants::SOURCES_LICENSE
//                                                              << DrugsDB::Constants::SOURCES_DATE
//                                                              << DrugsDB::Constants::SOURCES_DRUGS_VERSION
//                                                              << DrugsDB::Constants::SOURCES_AUTHORS
//                                                              << DrugsDB::Constants::SOURCES_VERSION
//                                                              << DrugsDB::Constants::SOURCES_PROVIDER
//                                                              << DrugsDB::Constants::SOURCES_WEBLINK
//                                                              << DrugsDB::Constants::SOURCES_DRUGUID_NAME
//                                                              << DrugsDB::Constants::SOURCES_ATC // 10
//                                                              << DrugsDB::Constants::SOURCES_INTERACTIONS
//                                                              << DrugsDB::Constants::SOURCES_COMPL_WEBSITE
//                                                              << DrugsDB::Constants::SOURCES_PACKUID_NAME
//                                                              << DrugsDB::Constants::SOURCES_COMPLETION
//                                                              << DrugsDB::Constants::SOURCES_AUTHOR_COMMENTS  // 15
//                                                              << DrugsDB::Constants::SOURCES_DRUGNAMECONSTRUCTOR
//                                                              << DrugsDB::Constants::SOURCES_FMFCOMPAT
//                                                              << DrugsDB::Constants::SOURCES_OPENREACT_COMPAT // 18
//                                                              , where));
//    int i=0;
//    query.bindValue(i, descr.data(Core::DrugDatabaseDescription::Language));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::WebLink));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::Copyright));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::LicenseName));
//    query.bindValue(++i, QDateTime::currentDateTime().toString(Qt::ISODate));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::Version));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::Author));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::FreeMedFormsCompatVersion));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::Vendor));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::WebLink));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::DrugUidName));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::IsAtcValid));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::IsDDIValid));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::ComplementaryWebLink));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::PackUidName));
//    query.bindValue(++i, completion);
//    query.bindValue(++i, QVariant());
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::DrugNameConstructor));
//    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::FreeMedFormsCompatVersion));
//    query.bindValue(++i, QVariant());
//    if (!query.exec()) {
//        LOG_QUERY_ERROR_FOR("Tools", query);
//        return false;
//    }

    return true;
}

int DrugsDBCore::getSourceId(const QString &connection, const QString &dbUid)
{
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("Tools","Unable to connection database");
            return -1;
        }
    }

    QString req;
    QSqlQuery query(db);

    // Source exists ?
//    req = QString("SELECT SID FROM SOURCES WHERE (DATABASE_UID='%1')").arg(dbUid);
    QHash<int, QString> w;
    w.insert(DrugsDB::Constants::SOURCES_DBUID, QString("='%1'").arg(dbUid));
    req = drugBase()->select(DrugsDB::Constants::Table_SOURCES, DrugsDB::Constants::SOURCES_SID, w);
    if (query.exec(req)) {
        if (query.next()) {
            return query.value(0).toInt();;
        }
    } else {
        LOG_QUERY_ERROR_FOR("Tools", query);
        return -1;
    }
    query.finish();
    return -1;
}

QHash<int, QString> DrugsDBCore::generateMids(const QStringList &molnames, const int sid, const QString &connection)
{
    QHash<int, QString> mids;
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen())
        return mids;

    QString req;
    db.transaction();
    QSqlQuery query(db);

    foreach(const QString &name, molnames) {

        // Ask for an existing MID
        //        req = QString("SELECT MID FROM MOLS WHERE NAME=\"%1\" AND SID=\"%2\";").arg(name).arg(sid);
        QHash<int, QString> w;
        w.insert(DrugsDB::Constants::MOLS_NAME, QString("=\"%1\"").arg(name));
        w.insert(DrugsDB::Constants::MOLS_SID, QString("='%1'").arg(sid));
//        req = DrugsDB::Tools::drugBase()->select(DrugsDB::Constants::Table_MOLS, DrugsDB::Constants::MOLS_MID, w);
//        if (query.exec(req)) {
//            if (query.next()) {
//                // is already in the table MOLS
//                mids.insert(query.value(0).toInt(), name);
//                continue;
//            }
//        } else {
//            LOG_QUERY_ERROR_FOR("Tools", query);
//            continue;
//        }
        query.finish();

        // create a new MID
        //        req = QString("INSERT INTO MOLS (MID, SID, NAME) VALUES (NULL,%1,\"%2\");")
        //              .arg(sid)
        //              .arg(name);
//        query.prepare(DrugsDB::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_MOLS));
//        query.bindValue(DrugsDB::Constants::MOLS_MID, QVariant());
//        query.bindValue(DrugsDB::Constants::MOLS_NAME, name);
//        query.bindValue(DrugsDB::Constants::MOLS_SID, sid);
//        query.bindValue(DrugsDB::Constants::MOLS_WWW, QVariant());
//        if (query.exec()) {
//            mids.insert(query.lastInsertId().toInt(), name);
//            continue;
//        } else {
//            LOG_QUERY_ERROR_FOR("Tools", query);
//            continue;
//        }
//        query.finish();
    }
    db.commit();
    return mids;
}

bool DrugsDBCore::addInteractionData(const QString &connection)
{
    return true;
}
