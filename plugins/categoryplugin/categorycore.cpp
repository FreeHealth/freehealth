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
 ***************************************************************************/
#include "categorycore.h"
#include "categoryitem.h"
#include "constants.h"
#include "categorybase.h"

#include <QVector>

using namespace Category;
using namespace Internal;

static inline CategoryBase *base() {return CategoryBase::instance();}


CategoryCore *CategoryCore::m_Instance = 0;

CategoryCore *CategoryCore::instance(QObject *parent)
{
    if (!m_Instance) {
        m_Instance = new CategoryCore(parent);
    }
    return m_Instance;
}

CategoryCore::CategoryCore(QObject *parent) :
        QObject(parent)
{
    setObjectName("CategoryCore");
    base();
}

CategoryCore::~CategoryCore()
{
}

/** Public core wrapper to the Category::CategoryBase::getCategories() */
QVector<CategoryItem *> CategoryCore::getCategories(const QString &mime) const
{
    return base()->getCategories(mime);
}

/** Public core wrapper to the Category::CategoryBase::createCategoryTree() */
QList<CategoryItem *> CategoryCore::createCategoryTree(const QVector<CategoryItem *> &cats) const
{
    return base()->createCategoryTree(cats);
}

/** Link Category::ICategoryContentItem to its Category::CategoryItem using the \e uuid of the category. */
bool CategoryCore::linkContentItemWithCategoryItem(const QVector<Category::CategoryItem *> &cats, const QVector<Category::ICategoryContentItem *> &contents) const
{
    for(int i = 0; i < contents.count(); ++i) {
        const int id = contents.at(i)->categoryId();
        if (id < 0)
            continue;

        // Add content to the category
        for(int j = 0; j < cats.count(); ++j) {
            if (cats.at(j)->id() == id) {
                cats.at(j)->addContentItem(contents.at(i));
                contents.at(i)->setCategory(cats.at(j));
                break;
            }
        }
    }
    return true;
}

/** Public core wrapper to the Category::CategoryBase::saveCategory() */
bool CategoryCore::saveCategory(CategoryItem *category)
{
    return base()->saveCategory(category);
}

/** Public core wrapper to the Category::CategoryBase::saveCategories() */
bool CategoryCore::saveCategories(const QVector<CategoryItem *> &categories)
{
    return base()->saveCategories(categories);
}

/** Public core wrapper to the Category::CategoryBase::removeAllExistingCategories() */
bool CategoryCore::removeAllExistingCategories(const QString &mime)
{
    return base()->removeAllExistingCategories(mime);
}
