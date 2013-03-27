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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef FREETOOLBOX_ATCMODEL_H
#define FREETOOLBOX_ATCMODEL_H

#include <QStandardItemModel>

/**
 * \file atcmodel.h
 * \author Eric MAEKER
 * \version 0.8.0
 * \date 02 Dec 2012
*/

namespace DrugsDB {
class DrugsDBCore;
namespace Internal {
class AtcModelPrivate;
}

class AtcModel : public QStandardItemModel
{
    Q_OBJECT
    friend class DrugsDB::Internal::AtcModelPrivate;
    friend class DrugsDB::DrugsDBCore;

protected:
    AtcModel(QObject * parent = 0);
    bool initialize();

public:
    enum DataRepresentation {
        ATC_Code = 0,
        ATC_EnglishLabel,
        ATC_FrenchLabel,
        ATC_DeutschLabel,
        ATC_CodeAndLabel,
        NumberOfColumn
    };
    ~AtcModel();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QStringList getAtcLabel(const QStringList &codes);

    static bool insertAtcCodeToDatabase(const QString &connectionName);

private:
    static AtcModel *_instance;
    Internal::AtcModelPrivate *d;
};

}  //  End namespace DrugsDB

#endif // FREETOOLBOX_ATCMODEL_H
