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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DRUGSDB_TOOLS_H
#define DDIMANAGER_DRUGSDB_TOOLS_H

#include <drugsdbplugin/drugsdb_exporter.h>

#include <QString>
#include <QHash>
#include <QMultiHash>
#include <QVector>
#include <QVariant>
QT_BEGIN_NAMESPACE
class QProgressDialog;
class QSqlDatabase;
QT_END_NAMESPACE

/**
 * \file tools.h
 * \author Eric MAEKER
 * \version 0.10.0
 * \date 11 Jan 2014
*/

namespace DrugsDB {
namespace Internal {
class DrugBaseEssentials;
}  // namespace Internal
}  // namespace DrugsDB

namespace DrugsDb {
class DrugDatabaseDescription;
class Drug;
}  // namespace DrugsDb

namespace DrugsDb {
namespace Tools {

DRUGSDB_EXPORT QString getBlock(const QString &content, const int posStart, int &posEnd, const QRegExp &delimiter);
DRUGSDB_EXPORT QString getBlock(const QString &content, const int posStart, int &posEnd, const QString &delimiter);

DRUGSDB_EXPORT bool executeProcess(const QString &proc);

DRUGSDB_EXPORT bool connectDatabase(const QString &connection, const QString &fileName);
DRUGSDB_EXPORT bool signDatabase(const QString &connectionName);

DRUGSDB_EXPORT int addLabels(DrugsDB::Internal::DrugBaseEssentials *database, const int masterLid, const QMultiHash<QString, QVariant> &trLabels);
DRUGSDB_EXPORT bool createAtc(DrugsDB::Internal::DrugBaseEssentials *database, const QString &code, const QMultiHash<QString, QVariant> &trLabels, const int forceAtcId = -1, const bool warnDuplicates = true);
DRUGSDB_EXPORT bool addInteraction(DrugsDB::Internal::DrugBaseEssentials *database, const QStringList &atc1, const QStringList &atc2, const QString &type, const QMultiHash<QString, QVariant> &risk, const QMultiHash<QString, QVariant> &management);
DRUGSDB_EXPORT int addBibliography(DrugsDB::Internal::DrugBaseEssentials *database, const QString &type, const QString &link, const QString &reference, const QString &abstract, const QString &explain = QString::null, const QString &xml = QString::null);
DRUGSDB_EXPORT bool addComponentAtcLinks(DrugsDB::Internal::DrugBaseEssentials *database, const QMultiHash<int, int> &mol_atc, const int sid);

DRUGSDB_EXPORT QVector<int> getAtcIdsFromLabel(DrugsDB::Internal::DrugBaseEssentials *database, const QString &label);
DRUGSDB_EXPORT QVector<int> getAtcIdsFromCode(DrugsDB::Internal::DrugBaseEssentials *database, const QString &code);

}  // End namespace Tools
}  // End namespace DrugsDb

#endif // DDIMANAGER_DRUGSDB_TOOLS_H
