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

/**
 * Add all interaction data to a drug database.
 * This includes:
 * - full ATC
 * - DDI
 * - PIM
 * - all future devs (pregnancy compatibility...)
 */
bool DrugsDBCore::addInteractionData(const QString &connection)
{
    return true;
}
