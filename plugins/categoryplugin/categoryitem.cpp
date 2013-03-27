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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Category::ICategoryContentItem
*/


/**
  \class Category::CategoryItem
  \brief Represent a category with all its data.
*/


#include "categoryitem.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <translationutils/constants.h>
#include <translationutils/trans_database.h>

#include <QHash>

#include <QDebug>

using namespace Category;
using namespace Internal;

static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline QString currentUserUuid() {return Core::ICore::instance()->user()->uuid();}

namespace Category {
namespace Internal {
class CategoryItemPrivate
{
public:
    CategoryItemPrivate(CategoryItem *parent) : m_Parent(0), m_IsDirty(false), q(parent) {}

    ~CategoryItemPrivate()
    {
    }

public:
    CategoryItem *m_Parent;
    QHash<int, QVariant> m_Data;
    QHash<QString, QString> m_Labels; //K=lang, V=label
    QList<CategoryItem *> m_Children;
    QList<ICategoryContentItem *> m_ContentChildren;
    bool m_IsDirty;

private:
    CategoryItem *q;
};
}
}

/** \brief Creates a valid empty CategoryItem (id==-1, parentId==-1, labelId==-1). */
CategoryItem::CategoryItem() :
        d(new CategoryItemPrivate(this))
{
    d->m_Data.insert(DbOnly_IsValid, true);
    d->m_Data.insert(DbOnly_Id, -1);
    d->m_Data.insert(DbOnly_ParentId, -1);
    d->m_Data.insert(DbOnly_LabelId, -1);
}

/** \brief Destructor of CategoryItem. Children categories are not deleted. */
CategoryItem::~CategoryItem()
{
    if (d) {
        delete d;
    }
    d = 0;
}

/**
  \brief When building a CategoryItem tree, define the \e parent of the item.
  Warning, the item is not automatically added to the children of the \e parent. \sa addChild().
  Warning, the ParentId is automatically defined.
*/
void CategoryItem::setParent(CategoryItem *parent)
{
    d->m_Parent = parent;
    if (parent)
        setData(DbOnly_ParentId, parent->id());
    d->m_IsDirty = true;
}

/** \brief Return the CategoryItem parent. */
CategoryItem *CategoryItem::parent() const
{
    return d->m_Parent;
}

/**
  Add the item \e child to the list of children.
  Adding an item to the children list will redefine the parent of the \e child.
  \sa setParent()
*/
void CategoryItem::addChild(CategoryItem *child)
{
    d->m_Children.append(child);
    child->setParent(this);
}

/**
  Adds items as \e children.
  Adding an item to the children list will redefine the parent of the \e child.
  \sa setParent()
*/
void CategoryItem::addChildren(const QVector<CategoryItem *> &children)
{
    d->m_Children.append(children.toList());
    for(int i=0; i<children.count(); ++i)
        children.at(i)->setParent(this);
}

/** \brief Insert a CategoryItem tree \e child to the list of children at index \e row. Warning, adding an item to the children list will not redefine the parent of the \e child. \sa setParent()*/
void CategoryItem::insertChild(CategoryItem *child, int row)
{
    d->m_Children.insert(row, child);
}

/** \brief Recalculate the SortId of each item according to the actual children list. */
void CategoryItem::updateChildrenSortId()
{
    for(int i=0; i < d->m_Children.count(); ++i) {
        d->m_Children[i]->setData(CategoryItem::SortId, i+1);
    }
}

/** Clear the children of the item. All pointers are deleted. */
void CategoryItem::clearChildren()
{
    qDeleteAll(d->m_Children);
    d->m_Children.clear();
}

/** \brief Returns the child number \e number. */
CategoryItem *CategoryItem::child(int number) const
{
    if (number < d->m_Children.count()) {
        return d->m_Children.at(number);
    }
    return 0;
}

/** \brief Returns the children categories list of the CategoryItem. */
QList<CategoryItem *> CategoryItem::children() const
{
    return d->m_Children;
}

/** \brief Returns the children categories list count. */
int CategoryItem::childCount() const
{
    return d->m_Children.count();
}

/** \brief Returns the CategoryItem index of this CategoryItem in the parent list of children. */
int CategoryItem::childNumber() const
{
    if (d->m_Parent)
        return d->m_Parent->children().indexOf(const_cast<CategoryItem*>(this));
    return 0;
}

/** \brief Set data \e value for the CategoryItem for \e ref. */
bool CategoryItem::setData(const int ref, const QVariant &value)
{
    if (d->m_Data.value(ref)==value)
        return true;
    if (ref==ThemedIcon) {
        QString name = value.toString();
        if (name.startsWith(Core::Constants::TAG_APPLICATION_THEME_PATH)) {
            name = name.remove(Core::Constants::TAG_APPLICATION_THEME_PATH);
            if (name.startsWith("/"))
                name = name.mid(1);
        }
        d->m_IsDirty = true;
        d->m_Data.insert(ref, name);
        return true;
    }
    d->m_IsDirty = true;
    d->m_Data.insert(ref, value);
    return true;
}

/** \brief Return the data for the CategoryItem for \e ref. */
QVariant CategoryItem::data(const int ref) const
{
    return d->m_Data.value(ref);
}

/** \brief Return the dirty state. */
bool CategoryItem::isDirty() const
{
    return d->m_IsDirty;
}

/** \brief Set the dirty state. */
void CategoryItem::setDirty(bool state)
{
    d->m_IsDirty = state;
}

/** \brief Set the label of this CategoryItem in the language \e lang. */
bool CategoryItem::setLabel(const QString &label, const QString &lang)
{
    if (d->m_Labels.value(lang) == label)
        return true;
    if (lang.isEmpty()) {
        d->m_Labels.insert(QLocale().name().left(2), label);
    } else {
        d->m_Labels.insert(lang, label);
    }
    d->m_IsDirty = true;
    return true;
}

/** \brief Return the label of this CategoryItem in the language \e lang (or the ALL_LANGUAGE if \e lang is not defined). */
QString CategoryItem::label(const QString &lang) const
{
    if (lang.isEmpty() || (!d->m_Labels.keys().contains(lang))) {
        const QString &lang = QLocale().name().left(2);
        QString t = d->m_Labels.value(lang, QString::null);
        if (t.isEmpty()) {
            t = d->m_Labels.value(Trans::Constants::ALL_LANGUAGE, QString::null);
        }
        return t;
    }
    return d->m_Labels.value(lang, QString::null);
}

/** \brief Return all available languages for the label of this CategoryItem. */
QStringList CategoryItem::allLanguagesForLabel() const
{
    return d->m_Labels.keys();
}

/** \brief Clear all labels. */
void CategoryItem::clearLabels()
{
    d->m_Labels.clear();
    d->m_IsDirty = true;
}

/** \brief Remove the label for the language \e lang (\e lang must be formatted in 2char ISO format). */
void CategoryItem::removeLabel(const QString &lang)
{
    if (lang.isEmpty()) {
        d->m_Labels.remove(QLocale().name().left(2));
        d->m_Labels.remove(Trans::Constants::ALL_LANGUAGE);
    } else {
        d->m_Labels.remove(lang);
    }
    d->m_IsDirty = true;
}

/** \brief Sort CategoryItem children according to the lessThan() member. */
bool CategoryItem::sortChildren()
{
    qSort(d->m_Children.begin(), d->m_Children.end(), CategoryItem::lessThan);
    return true;
}

/** \brief Sort CategoryItem children according to the SortId value. */
bool CategoryItem::lessThan(const CategoryItem *c1, const CategoryItem *c2)
{
    return c1->sortId() < c2->sortId();
}

/** \brief Add an ICategoryContentItem to this CategoryItem. */
void CategoryItem::addContentItem(ICategoryContentItem *data)
{
    d->m_ContentChildren.append(data);
}

/** \brief Return all the ICategoryContentItem of this CategoryItem. */
QList<ICategoryContentItem *> CategoryItem::contentItems() const
{
    return d->m_ContentChildren;
}

/** \brief Remove all ICategoryContentItem without deleting pointers. */
void CategoryItem::clearContentItems()
{
    d->m_ContentChildren.clear();
}

void CategoryItem::warn() const
{
    QString tmp = "Category(";
    tmp += QString("Id:%1; ").arg(data(DbOnly_Id).toString());
    tmp += QString("LId:%1; ").arg(data(DbOnly_LabelId).toString());
    tmp += QString("ParentId:%1; ").arg(data(DbOnly_ParentId).toString());
    tmp += QString("IsValid:%1; ").arg(data(DbOnly_IsValid).toString());
    tmp += QString("IsDirty:%1; ").arg(isDirty());
    tmp += QString("SortId:%1; ").arg(data(SortId).toString());
    tmp += "\n          ";
    tmp += QString("Label:%1; ").arg(label());
    tmp += QString("Mime:%1; ").arg(data(DbOnly_Mime).toString());
    tmp += "\n          ";
    tmp += QString("Icon:%1; ").arg(data(ThemedIcon).toString());
    tmp += QString("Password:%1; ").arg(data(Password).toString());
    tmp += "\n          ";
    tmp += QString("Extra:%1; ").arg(data(ExtraXml).toString());
    tmp.chop(2);
    qWarning() << tmp+")";
}
