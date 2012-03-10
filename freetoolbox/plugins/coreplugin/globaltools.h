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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef GLOBALTOOLS_H
#define GLOBALTOOLS_H

#include <coreplugin/core_exporter.h>

#include <QString>
#include <QHash>
#include <QMultiHash>
#include <QVector>
class QProgressDialog;
class QSqlDatabase;

class Drug;

/**
 * \file globaltools.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.5
 * \date 11 Jan 2011
*/

namespace DrugsDB {
namespace Internal {
class DrugBaseEssentials;
}
}

namespace Core {
class DrugDatabaseDescription;

namespace Tools
{

CORE_EXPORT QString noAccent(const QString & s);

CORE_EXPORT QString getBlock(const QString &content, const int posStart, int &posEnd, const QRegExp &delimiter);
CORE_EXPORT QString getBlock(const QString &content, const int posStart, int &posEnd, const QString &delimiter);

CORE_EXPORT bool executeProcess(const QString &proc);
CORE_EXPORT bool executeSqlFile(const QString &connectionName, const QString &fileName, QProgressDialog *dlg = 0);
CORE_EXPORT bool executeSqlQuery(const QString &sql, const QString &dbName, const QString &file = QString::null, int line = -1);

CORE_EXPORT DrugsDB::Internal::DrugBaseEssentials *drugBase();
CORE_EXPORT QString drugsDatabaseAbsFileName(const QString &country = QString::null);
CORE_EXPORT QString databaseOutputPath();

CORE_EXPORT bool connectDatabase(const QString &connection, const QString &fileName);
CORE_EXPORT bool createMasterDrugInteractionDatabase();
CORE_EXPORT bool createDrugDatabase(const QString &absPath);
CORE_EXPORT bool signDatabase(const QString &connectionName);

CORE_EXPORT int getSourceId(const QString &connection, const QString &dbUid);
CORE_EXPORT int createNewDrugsSource(const QString &connection, const QString &uid, QMultiHash<QString, QVariant> trLabels);
CORE_EXPORT int addLabels(const QString &connection, const int masterLid, QMultiHash<QString, QVariant> trLabels);
CORE_EXPORT bool recreateRoutes();
CORE_EXPORT bool addRoutesToDatabase(const QString &connection, const QString &absFileName);
CORE_EXPORT QHash<int, QString> generateMids(const QStringList &molnames, const int sid, const QString &connection);

CORE_EXPORT bool createAtc(const QString &connection, const QString &code, const QMultiHash<QString, QVariant> &trLabels, const int forceAtcId = -1, const bool warnDuplicates = true);
CORE_EXPORT bool addInteraction(const QString &connection, const QStringList &atc1, const QStringList &atc2, const QString &type, const QMultiHash<QString, QVariant> &risk, const QMultiHash<QString, QVariant> &management);
CORE_EXPORT int addBibliography(const QString &connection, const QString &type, const QString &link, const QString &reference, const QString &abstract, const QString &explain = QString::null, const QString &xml = QString::null);
CORE_EXPORT bool addComponentAtcLinks(const QString &connection, const QMultiHash<int, int> &mol_atc, const int sid);

CORE_EXPORT QVector<int> getAtcIdsFromLabel(const QString &connection, const QString &label);
CORE_EXPORT QVector<int> getAtcIdsFromCode(const QString &connection, const QString &code);

CORE_EXPORT bool saveDrugDatabaseDescription(const QString &fileName, const int completion);

}  // End namespace Tools

}  // End namespace DrugsDbCreator

#endif // GLOBALTOOLS_H
