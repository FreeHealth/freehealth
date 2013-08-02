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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "categorycore.h"
#include "categoryitem.h"
#include "constants.h"
#include "categorybase.h"

#include <QVector>

using namespace Category;
using namespace Internal;

namespace Category {
namespace Internal {
class CategoryCorePrivate
{
public:
    CategoryCorePrivate() : _categoryBase(0) {}

public:
    Internal::CategoryBase *_categoryBase;
};
}  // Internal
}  // Category

CategoryCore *CategoryCore::m_Instance = 0;

CategoryCore *CategoryCore::instance(QObject *parent)
{
    if (!m_Instance) {
        m_Instance = new CategoryCore(parent);
    }
    return m_Instance;
}

CategoryCore::CategoryCore(QObject *parent) :
    QObject(parent), d(new Internal::CategoryCorePrivate)
{
    setObjectName("CategoryCore");

    // Create the base
    d->_categoryBase = new Internal::CategoryBase(this);
}

CategoryCore::~CategoryCore()
{
}

bool CategoryCore::initialize()
{
    return d->_categoryBase->initialize();
}

/** Public core wrapper to the Category::CategoryBase::getCategories() */
QVector<CategoryItem *> CategoryCore::getCategories(const QString &mime, const QStringList &uuids) const
{
    return d->_categoryBase->getCategories(mime, uuids);
}

/** Public core wrapper to the Category::CategoryBase::createCategoryTree() */
QList<CategoryItem *> CategoryCore::createCategoryTree(const QVector<CategoryItem *> &cats) const
{
    return d->_categoryBase->createCategoryTree(cats);
}

static QVector<Category::CategoryItem *> flattenCategories(Category::CategoryItem *item)
{
    QVector<Category::CategoryItem *> cats;
    cats << item->children().toVector();
    for(int i=0; i < item->childCount(); ++i) {
        cats << flattenCategories(item->child(i));
    }
    return cats;
}

/** Transform a category tree \e categories into a flat QVector. Root items of the \e categories vector will be included. */
QVector<Category::CategoryItem *> CategoryCore::flattenCategoryTree(const QVector<Category::CategoryItem *> &categories)
{
    QVector<Category::CategoryItem *> cats;
    for(int i=0; i < categories.count(); ++i) {
        CategoryItem *item = categories.at(i);
        cats << item << flattenCategories(item);
    }
    return cats;
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
    return d->_categoryBase->saveCategory(category);
}

/** Public core wrapper to the Category::CategoryBase::saveCategories() */
bool CategoryCore::saveCategories(const QVector<CategoryItem *> &categories)
{
    return d->_categoryBase->saveCategories(categories);
}

/** Public core wrapper to the Category::CategoryBase::removeAllExistingCategories() */
bool CategoryCore::removeAllExistingCategories(const QString &mime)
{
    return d->_categoryBase->removeAllExistingCategories(mime);
}

/**
  Search in a category tree after a  Category::CategoryItem pointer whose value for the reference
  \e usingReference is equal to \e searchValue.
  The search includes all the children of the \e category. \n
  For the reference please refer to the Category::CategoryItem::DataRepresentation enumerator. \n
  Return 0 if there are no matching category found.
*/
Category::CategoryItem *CategoryCore::findCategory(const int usingReference, const QVariant &searchValue, Category::CategoryItem *category)
{
    if (!category)
        return 0;
    if (category->data(usingReference) == searchValue)
        return category;
    for(int i = 0; i < category->childCount(); ++i) {
        Category::CategoryItem *child = category->child(i);
        child = findCategory(usingReference, searchValue, child);
        if (child)
            return child;
    }
    return 0;
}
