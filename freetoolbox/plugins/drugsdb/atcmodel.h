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
#ifndef ATCMODEL_H
#define ATCMODEL_H

#include <QAbstractItemModel>
#include <QObject>

/**
 * \file atcmodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.2
 * \date 29 June 2010
*/

namespace DrugsDB {
namespace Internal {
class AtcModelPrivate;
}

class AtcModel : public QAbstractItemModel
{
    Q_OBJECT
    friend class DrugsDB::Internal::AtcModelPrivate;

    AtcModel(QObject * parent = 0);
public:
    enum DataRepresentation {
        ATC_Code = 0,
        ATC_EnglishLabel,
        ATC_FrenchLabel,
        ATC_DeutschLabel,
        ATC_CodeAndLabel,
        NumberOfColumn
    };
    static AtcModel *instance(QObject *parent = 0);
    ~AtcModel();
    void init();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool setData(const QModelIndex &, const QVariant &, int = Qt::EditRole) {return false;}
    QVariant data(const QModelIndex & item, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QStringList getAtcLabel(const QStringList &codes);

    static bool insertAtcCodeToDatabase(const QString &connectionName);

private:
    static AtcModel *m_Instance;
    Internal::AtcModelPrivate *d;
};

}  //  End namespace DrugsDbCreator


#endif // ATCMODEL_H
