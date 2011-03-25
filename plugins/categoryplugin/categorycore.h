/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#ifndef CATEGORY_CORE_H
#define CATEGORY_CORE_H

#include <categoryplugin/category_exporter.h>
#include <QObject>

namespace Category {
class CategoryItem;
class ICategoryContentItem;

class CATEGORY_EXPORT CategoryCore : public QObject
{
    Q_OBJECT

protected:
    CategoryCore(QObject *parent = 0);

public:
    // Constructor
    static CategoryCore *instance(QObject *parent = 0);
    virtual ~CategoryCore();

    QVector<CategoryItem *> getCategories(const QString &mime) const;
    QList<CategoryItem *> createCategoryTree(const QVector<CategoryItem *> &cats) const;
    bool linkContentItemWithCategoryItem(const QVector<Category::CategoryItem *> &cats, const QVector<Category::ICategoryContentItem *> &contents) const;

    bool saveCategory(CategoryItem *category);

private:
    static CategoryCore *m_Instance;
};

}  // End namespace Category


#endif // CATEGORY_CORE_H
