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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PMHCATEGORYMODEL_H
#define PMHCATEGORYMODEL_H

#include <pmhplugin/pmh_exporter.h>

#include <categoryplugin/icategorymodelhelper.h>

#include <QAbstractItemModel>
#include <QAbstractProxyModel>

/**
 * \file pmhcategorymodel.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.7.2
 * \date 05 May 2012
*/

namespace Form {
class FormMain;
}

namespace PMH {
namespace Internal {
class PmhCategoryModelPrivate;
class PmhData;
class PmhCategory;
}  // End namespace Internal

class PMH_EXPORT PmhCategoryModel : public Category::ICategoryModelHelper
{
    Q_OBJECT
public:
    enum DataRepresentation {
        Label = 0,
        Type,
        Id,
        EmptyColumn,
        ColumnCount
    };

    // Model
    explicit PmhCategoryModel(QObject *parent = 0);
    ~PmhCategoryModel();

    void setRootFormUid(const QString &uid);
    QString rootFormUid() const;
    void refreshFromDatabase();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int pmhCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

//    QVariant headerData(int section, Qt::Orientation orientation, int role) const {return section;}

    // Data
    bool addPmhData(Internal::PmhData *pmh);
    Internal::PmhData *pmhDataforIndex(const QModelIndex &item) const;
    QModelIndex indexForPmhData(const Internal::PmhData *pmh, const QModelIndex &rootStart = QModelIndex()) const;

    // Helpers
    void updateFontAndColors(const QModelIndex &parent = QModelIndex());

    // Category::ICategoryModelHelper and extra
    bool isPmhx(const QModelIndex &item) const;
    bool isCategory(const QModelIndex &item) const;
    bool isForm(const QModelIndex &item) const;
    bool isSynthesis(const QModelIndex &item) const;

    Category::CategoryItem *categoryForIndex(const QModelIndex &item) const;
    QModelIndex indexForCategory(const Category::CategoryItem *category) const;

    Form::FormMain *formForIndex(const QModelIndex &item) const;
    bool activateFormEpisode(const QModelIndex &formIndex) const;

    void addCategory(Category::CategoryItem *category, int row, const QModelIndex &parentCategory);
    void updateCategory(Category::CategoryItem *category);

    virtual QString mime() const;

    // Patient synthesis
    QString indexToHtml(const QModelIndex &index, int indent = 0) const;
    void refreshSynthesis();
    QString synthesis(const QModelIndex &parent = QModelIndex()) const;

public Q_SLOTS:
    // Connections to Core::IPatient
    void onCurrentPatientChanged();

private Q_SLOTS:
    void updateCategoryLabel(const Category::CategoryItem *category);
    void retranslate();

private:
    Internal::PmhCategoryModelPrivate *d;
};


}  // End namespace PMH


#endif // PMHCATEGORYMODEL_H
