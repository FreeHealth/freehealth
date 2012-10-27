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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSDB_DRUGSDBCORE_H
#define DRUGSDB_DRUGSDBCORE_H

#include <drugsdb/drugsdb_exporter.h>
#include <QObject>
#include <QMultiHash>

/**
 * \file drugsdbcore.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 21 Oct 2012
*/

namespace DrugsDB {
class RoutesModel;
class MoleculeLinkerModel;

namespace Internal {
class DrugsDbPlugin;
class DrugsDBCorePrivate;
class DrugBaseEssentials;
}  // namespace Internal

class DRUGSDB_EXPORT DrugsDBCore : public QObject
{
    Q_OBJECT
    friend class DrugsDB::Internal::DrugsDbPlugin;

protected:
    explicit DrugsDBCore(QObject *parent = 0);
    bool initialize();

public:
    static DrugsDBCore *instance();
    ~DrugsDBCore();

    RoutesModel *routesModel() const;
    MoleculeLinkerModel *moleculeLinkerModel() const;

    // TODO:: remove me
    DrugsDB::Internal::DrugBaseEssentials *drugBase();
    bool createMasterDrugInteractionDatabase();
    // END
    DrugsDB::Internal::DrugBaseEssentials *createDrugDatabase(const QString &absPath, const QString &connection);

    bool addInteractionData(const QString &connection);

private:
    static DrugsDBCore *_instance;
    Internal::DrugsDBCorePrivate *d;
};

} // namespace DrugsDB

#endif  // DRUGSDB_DRUGSDBCORE_H

