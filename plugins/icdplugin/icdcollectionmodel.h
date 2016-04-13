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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ICDCOLLECTIONMODEL_H
#define ICDCOLLECTIONMODEL_H

#include <icdplugin/icd_exporter.h>
#include <QStandardItemModel>


namespace ICD {
namespace Internal {
class IcdCollectionModelPrivate;
class IcdAssociation;
}  // End namespace Internal

class ICD_EXPORT IcdCollectionModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum DataRepresentation {
        CodeWithDaget = 0,
        Label,
        CodeWithoutDaget,
        HumanReadableDaget,
        DagCode,
        SID,
        ColumnCount
    };

    explicit IcdCollectionModel(QObject *parent = 0);
    ~IcdCollectionModel();

    void setCollectionIsSimpleList(bool state);
    bool isCollectionSimpleList() const;
    bool isEmpty() const {return (rowCount()==0);}

    // Checking
    bool canAddThisCode(const QVariant &SID, bool checkDaget = true) const;
    bool canAddThisAssociation(const Internal::IcdAssociation &pair) const;
    bool addCode(const QVariant &SID);
    bool addCode(const QString &code);
    bool addAssociation(const Internal::IcdAssociation &pair);

    // Direct access to data and model cleaning
    void clearCollection();
    QStringList includedCodesWithDaget() const;
    QStringList includedCodesWithoutDaget() const;
    QString includedLabelsToHtml() const;
    QStringList includedLabels() const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

private Q_SLOTS:
    void languageChanged();

private:
    Internal::IcdCollectionModelPrivate *d;
};

}  // End namespace ICD


#endif // ICDCOLLECTIONMODEL_H
