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
#ifndef FULLICDCODEMODEL_H
#define FULLICDCODEMODEL_H

#include <icdplugin/icd_exporter.h>
#include <QAbstractTableModel>

QT_BEGIN_NAMESPACE
class QStringListModel;
QT_END_NAMESPACE

namespace ICD {
class SimpleIcdModel;
namespace Internal {
class FullIcdCodeModelPrivate;
}

class ICD_EXPORT FullIcdCodeModel :public QAbstractTableModel
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
        Memo,
        ColumnCount
    };


    enum DataRepresentation_ForValidation {
        CanBeUsedAlone = 1000,
        MustBeAssociated,
        SelectionIsValid
    };

    explicit FullIcdCodeModel(QObject *parent = 0);
    ~FullIcdCodeModel();

    void setCode(const int SID);
    QVariant getCodeSid() const;

    bool codeCanBeUsedAlone() const;
    bool codeMustBeAssociated() const;
    bool isSelectionValid() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QStringListModel *labelsModel();
    QAbstractItemModel *codeTreeModel();
    QStringListModel *includedLabelsModel();
    QAbstractItemModel *excludedModel();
    SimpleIcdModel *dagStarModel();

//    QVariant headerData(int section, Qt::Orientation orientation,
//                                int role = Qt::DisplayRole) const;


private Q_SLOTS:
    void updateTranslations();

private:
    Internal::FullIcdCodeModelPrivate *d;

};

}  // End namespace ICD


#endif // FULLICDCODEMODEL_H
