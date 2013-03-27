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
 ***************************************************************************/
#ifndef ICDMODELS_H
#define ICDMODELS_H

#include <icdplugin/icd_exporter.h>

#include <QAbstractTableModel>

QT_BEGIN_NAMESPACE
class QStringListModel;
QT_END_NAMESPACE

namespace ICD {
namespace Internal {
class SimpleIcdModelPrivate;
class IcdAssociation;
}

class ICD_EXPORT SimpleIcdModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        SID_Code = 0,
        ICD_Code,
        ICD_CodeWithDagetAndStar,
        Label,
        Daget,
        Type,
        ColumnCount
    };

    explicit SimpleIcdModel(QObject *parent = 0);
    ~SimpleIcdModel();

    void addCodes(const QVector<int> &codes, bool getAllLabels = false);

    void setUseDagDependencyWithSid(const QVariant &SID);
    void setCheckable(bool state);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QStringListModel *labelsModel(const QModelIndex &index);

    QVector<QVariant> getCheckedSids() const;
    QVector<Internal::IcdAssociation> getCheckedAssociations() const;
    int numberOfCheckedItems() const;

//    QVariant headerData(int section, Qt::Orientation orientation,
//                                int role = Qt::DisplayRole) const;

private Q_SLOTS:
    void updateTranslations();

private:
    Internal::SimpleIcdModelPrivate *d;

};



}  //  End namespace ICD

#endif // ICDMODELS_H
