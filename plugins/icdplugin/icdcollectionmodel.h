/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef ICDCOLLECTIONMODEL_H
#define ICDCOLLECTIONMODEL_H

#include <QAbstractTableModel>


namespace ICD {
namespace Internal {
class IcdCollectionModelPrivate;
class IcdAssociation;
}  // End namespace Internal

class IcdCollectionModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit IcdCollectionModel(QObject *parent = 0);

    // Checking
    bool canAddThisCode(const QVariant &SID) const;
    bool canAddThisAssociation(const Internal::IcdAssociation &pair) const;
    bool addCode(const QVariant &SID);
    bool addAssociation(const Internal::IcdAssociation &pair) const;

    // Model
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    // XML import/export
    QString toXml() const;
    bool fromXml(const QString &xml);

private:
    Internal::IcdCollectionModelPrivate *d;
};

}  // End namespace ICD


#endif // ICDCOLLECTIONMODEL_H
