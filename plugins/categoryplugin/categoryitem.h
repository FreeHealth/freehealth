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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CATEGORYPLUGIN_CATEGORYITEM_H
#define CATEGORYPLUGIN_CATEGORYITEM_H

#include <categoryplugin/category_exporter.h>

#include <QVariant>

namespace Category {
class CategoryItem;

class CATEGORY_EXPORT ICategoryContentItem
{
public:
    ICategoryContentItem() {}
    virtual ~ICategoryContentItem() {}

    virtual int categoryId() const = 0;
    /** \todo code the category uuid used by the linkCategoryContentToCategory() */
//    virtual QString categoryUid() const = 0;
    virtual void setCategory(Category::CategoryItem *cat) = 0;
    virtual Category::CategoryItem *category() const = 0;

};


namespace Internal {
class CategoryItemPrivate;
}  // End namespace Internal

class CATEGORY_EXPORT CategoryItem
{
public:
    enum DataRepresentation {
        DbOnly_Id = 0,
        DbOnly_LabelId,
        DbOnly_ParentId,
        DbOnly_IsValid,
        DbOnly_Mime,
        Uuid,
        ThemedIcon,
        SortId,
        Password,
        ExtraXml
    };

    CategoryItem();
    ~CategoryItem();

    void setParent(CategoryItem *parent);
    CategoryItem *parent() const;
    void addChild(CategoryItem *child);
    void addChildren(const QVector<CategoryItem *> &child);
    void insertChild(CategoryItem *child, int row);
    void updateChildrenSortId();
    void clearChildren();

    CategoryItem *child(int number) const;
    QList<CategoryItem *> children() const;
    int childCount() const;
    int childNumber() const;

    bool setData(const int ref, const QVariant &value);
    QVariant data(const int ref) const;

    bool isDirty() const;
    void setDirty(bool state);

    bool setLabel(const QString &label, const QString &lang = QString::null);
    QString label(const QString &lang = QString::null) const;
    QStringList allLanguagesForLabel() const;
    void clearLabels();
    void removeLabel(const QString &lang = QString::null);

    bool sortChildren();
    static bool lessThan(const CategoryItem *c1, const CategoryItem *c2);

    void addContentItem(ICategoryContentItem *data);
    QList<ICategoryContentItem *> contentItems() const;
    void clearContentItems();

    // code beautifying
    int id() const {return data(DbOnly_Id).toInt();}
    int parentId() const {return data(DbOnly_ParentId).toInt();}
    int sortId() const  {return data(SortId).toInt();}
    QString uuid() const {return data(Uuid).toString();}
    QString cryptedPassword() const {return data(Password).toString();}
    QString mime() const {return data(DbOnly_Mime).toString();}
    QString iconName() const {return data(ThemedIcon).toString();}

    void warn() const;

private:
    Internal::CategoryItemPrivate *d;
};

} // End namespace Category

#endif // CATEGORYPLUGIN_CATEGORYITEM_H
