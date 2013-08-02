/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSDB_INTERNAL_MOLECULELINKDATA_H
#define DRUGSDB_INTERNAL_MOLECULELINKDATA_H

#include <translationutils/constants.h>

#include <QString>
#include <QHash>
#include <QMultiHash>

namespace DrugsDB {
namespace Internal {
class DrugBaseEssentials;

struct MoleculeLinkData
{
    MoleculeLinkData() :
        database(0),
        sourceId(-1),
        lang(Trans::Constants::ALL_LANGUAGE),
        completionPercentage(0)
    {}

    MoleculeLinkData(DrugBaseEssentials *_database, int _sourceId, const QString &_drugDbUid, const QString &_lang) :
        database(_database),
        sourceId(_sourceId),
        drugDbUid(_drugDbUid),
        lang(_lang),
        completionPercentage(0)
    {}

    bool isValid() const
    {
        return (database
                && sourceId != -1
                && !drugDbUid.isEmpty()
                && !lang.isEmpty()
                );
    }

    // In data
    DrugBaseEssentials *database;
    int sourceId;
    QString drugDbUid, lang;
    QHash<QString, QString> correctedByName;
    QMultiHash<QString, QString> correctedByAtcCode;

    // Out data
    QStringList unfoundMoleculeAssociations;
    double completionPercentage;
    QMultiHash<int, int> moleculeIdToAtcId; // Key: moleculeId, Values: AtcIds
};

} // namespace Internal
} // namespace DrugsDB

#endif // DRUGSDB_INTERNAL_MOLECULELINKDATA_H
