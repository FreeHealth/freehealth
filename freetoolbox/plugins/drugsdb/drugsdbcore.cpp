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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DrugsDB::DrugsDBCore
 * \brief Central core singleton object for the DrugsDB plugin.
 * \n FreeToolBox specific class.
 */

#include "drugsdbcore.h"
#include "drugdatabasedescription.h"
#include "tools.h"
#include "routesmodel.h"
#include "countries/moleculelinkermodel.h"
#include "ddi/drugdruginteractioncore.h"
#include "ddi/drugdruginteractioncore.h"
#include "atc/atcmodel.h"

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
        _atcModel(0),
        _ddiCore(0),
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
    AtcModel *_atcModel;
    DrugsDB::DrugDrugInteractionCore *_ddiCore;

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
    setObjectName("DrugsDBCore");
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
    d->_ddiCore = new DrugsDB::DrugDrugInteractionCore(this);
    d->_ddiCore->initialize();
    d->_atcModel = new AtcModel(this);
    d->_atcModel->initialize();
    return true;
}

/** Return the singleton of the DrugsDB::RoutesModel pointer */
RoutesModel *DrugsDBCore::routesModel() const
{
    return d->_routesModel;
}

/** Return the molecule to ATC model */
MoleculeLinkerModel *DrugsDBCore::moleculeLinkerModel() const
{
    return d->_moleculeLinkerModel;
}

/** Return the unique ATC Model pointer. This pointer must not be deleted. */
AtcModel *DrugsDBCore::atcModel() const
{
    return d->_atcModel;
}

/** Create the drug database using the absolute path \e absPath, and the connectionName \e connection */
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
    LOG("Drug database created");
    return base;
}

/** Return the singleton of the drug drug interaction core */
DrugsDB::DrugDrugInteractionCore *DrugsDBCore::ddiCore() const
{
    return d->_ddiCore;
}
