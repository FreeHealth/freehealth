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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "pmhcategorymodel.h"
#include "pmhcategoryonlyproxymodel.h"
#include "pmhbase.h"
#include "pmhdata.h"
#include "constants.h"
#include "pmhcore.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QColor>

#include <QDebug>

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;


static inline PmhCore *pmhCore() {return PmhCore::instance();}
static inline Internal::PmhBase *base() {return Internal::PmhBase::instance();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline QString currentUserUuid() {return Core::ICore::instance()->user()->value(Core::IUser::Uuid).toString();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}


namespace PMH {
namespace Internal {

class TreeItem
{
public:
    enum DataRepresentation {
        Label = 0
    };

    TreeItem(TreeItem *parent = 0) :
            m_Parent(parent),
            m_IsModified(false),
            m_Cat(0),
            m_Pmh(0)
    {
        if (m_Parent)
            m_Parent->addChildren(this);
    }

    ~TreeItem();

    // Genealogy management
    TreeItem *child(int number) { return m_Children.value(number); }
    int childCount() const { return m_Children.count(); }
    int columnCount() const { return 1; }
    TreeItem *parent() { return m_Parent; }
    void setParent(TreeItem *parent) { m_Parent = parent; }
    bool addChildren(TreeItem *child)
    {
        if (!m_Children.contains(child))
            m_Children.append(child);
        return true;
    }
    bool insertChild(const int row, TreeItem *child)
    {
        if (row > m_Children.count())
            return false;
        m_Children.insert(row, child);
        return true;
    }
    int childNumber() const
    {
        if (m_Parent)
            return m_Parent->m_Children.indexOf(const_cast<TreeItem*>(this));
        return 0;
    }
    //    void sortChildren()
    //    {
    //        qSort(m_Children.begin(), m_Children.end(), TreeItem::lessThan);
    //    }
    bool removeChild(TreeItem *child)
    {
        if (m_Children.contains(child)) {
            m_Children.removeAll(child);
            return true;
        }
        return false;
    }

    // For data management
    QString label() const {return m_Label;}
    void setLabel(QString label) {m_Label = label;}

    QString icon() const {return m_Icon;}
    void setIcon(QString icon) {m_Icon = icon;}

    // Category / PMH
    void setPmhCategory(PmhCategory *cat)
    {
        m_Cat = cat;
        setLabel(cat->label());
        setIcon(cat->data(PmhCategory::ThemedIcon).toString());
    }

    void setPmhData(PmhData *pmh)
    {
        m_Pmh = pmh;
    }
    PmhData *pmhData() const {return m_Pmh;}

    bool isCategory() const {return (m_Cat);}

    // For sort functions
    //    static bool lessThan(TreeItem *item1, TreeItem *item2)
    //    {
    //        // category goes first
    //        // then sort by name
    //        bool sameType = (((item1->isTemplate()) && (item2->isTemplate())) || ((!item1->isTemplate()) && (!item2->isTemplate())));
    //        if (sameType)
    //            return item1->data(Constants::Data_Label).toString() < item2->data(Constants::Data_Label).toString();
    //        return item2->isTemplate();
    //    }

private:
    TreeItem *m_Parent;
    QList<TreeItem*> m_Children;
    QString m_Label, m_Icon;
    QVector<int> m_DirtyRows;
    bool m_IsTemplate, m_IsModified;
    PmhCategory *m_Cat;
    PmhData *m_Pmh;
};

/** \todo Bug: ~TreeItem() Can not be inlined ???? */
TreeItem::~TreeItem()
{
    m_Label.clear();
    m_Icon.clear();
    qDeleteAll(m_Children);
    m_Children.clear();
    m_Pmh=0;m_Cat=0;m_Parent=0;
}

class PmhCategoryModelPrivate
{
public:
    PmhCategoryModelPrivate(PmhCategoryModel *parent) :
            m_Root(0),
            m_CategoryOnlyModel(0),
            m_ShowCategoriesOnly(false),
            q(parent)
    {
        m_Root = new TreeItem;
        m_Root->setLabel("ROOT CATEGORY");
    }

    ~PmhCategoryModelPrivate()
    {
        if (m_Root) {
            delete m_Root; m_Root=0;
        }
    }

    TreeItem *getItem(const QModelIndex &index) const
    {
        if (index.isValid()) {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            if (item) return item;
        }
        return m_Root;
    }

    void categoryToItem(PmhCategory *cat, TreeItem *item)
    {
        // Insert category to item
        item->setPmhCategory(cat);
        m_CategoryToItem.insert(cat, item);

        // Create all children categories
        foreach(PmhCategory *c, cat->children()) {
            categoryToItem(c, new TreeItem(item));
        }
    }

    void pmhToItem(PmhData *pmh, TreeItem *item)
    {
        // Master Item (shows user label)
        item->setPmhData(pmh);
        item->setLabel(pmh->data(PmhData::Label).toString());

        // Add type and status as children of the Master Item
        TreeItem *child = new TreeItem(item);
        child->setLabel(Constants::typeToString(pmh->data(PmhData::Type).toInt()));
        child->setPmhData(pmh);
        child = new TreeItem(item);
        child->setLabel(Constants::statusToString(pmh->data(PmhData::State).toInt()));
        child->setPmhData(pmh);

        // Add episodes
        foreach(PmhEpisodeData *episode, pmh->episodes()) {
            child = new TreeItem(item);
            child->setLabel(episode->data(PmhEpisodeData::Label).toString());
            child->setPmhData(pmh);
            QString label;
            QString dateEnd = tkTr(Trans::Constants::NOW);
            if (!episode->data(PmhEpisodeData::DateEnd).isNull())
                dateEnd = episode->data(PmhEpisodeData::DateEnd).toDate().toString(QLocale().dateFormat());
            label = QString("%1 to %2")
                    .arg(episode->data(PmhEpisodeData::DateStart).toDate().toString(QLocale().dateFormat()))
                    .arg(dateEnd);
            child = new TreeItem(child);
            child->setLabel(label);
            child->setPmhData(pmh);
            // Add ICD codings
            foreach(const QString &icd, episode->data(PmhEpisodeData::IcdLabelStringList).toStringList()) {
                child = new TreeItem(child);
                child->setLabel(icd);
                child->setPmhData(pmh);
            }
        }

        // Find related PmhCategory in TreeItems
        int id = pmh->categoryId();
        for(int i = 0; i < m_Cats.count(); ++i) {
            if (m_Cats.at(i)->id()==id) {
                // Reparent TreeItem
                item->setParent(m_CategoryToItem.value(m_Cats.at(i)));
                m_CategoryToItem.value(m_Cats.at(i))->addChildren(item);
                return;
            }
        }
        // No category found --> reparent to Root TreeItem
        item->setParent(m_Root);
        m_Root->addChildren(item);
    }

    void getCategories()
    {
        // Get all categories from database
        m_Cats.clear();
        m_CategoryToItem.clear();
        m_Cats = base()->getPmhCategory(patient()->data(Core::IPatient::Uid).toString());
        qWarning() << "PMHxxxxxxxxx getCategory" << m_Cats.count();
        // Recreate the category tree
        foreach(PmhCategory *cat, base()->createCategoryTree(m_Cats)) {
            categoryToItem(cat, new TreeItem(m_Root));
        }
    }

    void getPmh()
    {
        m_Pmhs = base()->getPmh(patient()->data(Core::IPatient::Uid).toString());
        qWarning() << "PMHxxxxxxxxx getPMH" << m_Pmhs.count();
//        base()->linkPmhWithCategory(m_Cats, m_Pmhs);
        for(int i = 0; i < m_Pmhs.count(); ++i) {
            pmhToItem(m_Pmhs.at(i), new TreeItem);
//            PmhData *pmh = m_Pmhs.at(i);
//            if (pmh->categoryId()==-1) {
//                m_Root->addPhmData(pmh);
//            }
        }
    }

    void getDataFromDatabase()
    {
        getCategories();
        getPmh();
    }

public:
    TreeItem *m_Root;
    QVector<PmhData *> m_Pmhs;
    QVector<PmhCategory *> m_Cats;
    QHash<PmhCategory *, TreeItem *> m_CategoryToItem;
    QMultiHash<PmhCategory *, PmhData *> m_Cat_Pmhs;
    PmhCategoryOnlyModel *m_CategoryOnlyModel;
    bool m_ShowCategoriesOnly;

private:
    PmhCategoryModel *q;

};


}  // End namespace Internal
}  // End namespace PMH




PmhCategoryModel::PmhCategoryModel(QObject *parent) :
        QAbstractItemModel(parent), d(new Internal::PmhCategoryModelPrivate(this))
{
    d->getDataFromDatabase();
}

PmhCategoryModel::~PmhCategoryModel()
{
    if (d)
        delete d;
    d = 0;
}

void PmhCategoryModel::setShowOnlyCategories(bool state)
{
    d->m_ShowCategoriesOnly = state;
}

QAbstractProxyModel *PmhCategoryModel::categoryOnlyModel()
{
    if (!d->m_CategoryOnlyModel)
        d->m_CategoryOnlyModel = new PmhCategoryOnlyModel(this);
    return d->m_CategoryOnlyModel;
}

/** \brief Return true is the \e index is category, false if \e index is a PMHx */
bool PmhCategoryModel::isCategory(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;
    TreeItem *it = d->getItem(index);
    return it->isCategory();
}

QModelIndex PmhCategoryModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

//     if (!parent.isValid())
//         return QModelIndex();

     TreeItem *parentItem = d->getItem(parent);

     if (d->m_ShowCategoriesOnly) {
         if (!parentItem->isCategory()) {
             return QModelIndex();
         }
     }

     TreeItem *childItem = parentItem->child(row);

     if (childItem) {
         return createIndex(row, column, childItem);
     }
     return QModelIndex();
 }

QModelIndex PmhCategoryModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = d->getItem(index);
    TreeItem *parentItem = childItem->parent();

    if (parentItem == d->m_Root)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int PmhCategoryModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *item = d->getItem(parent);
    if (item) {
        return item->childCount();
    }
    return 0;
}

int PmhCategoryModel::columnCount(const QModelIndex &parent) const
{
    return ColumnCount;
}

void PmhCategoryModel::fetchMore(const QModelIndex &parent)
{
}

bool PmhCategoryModel::canFetchMore(const QModelIndex &parent) const
{
    return false;
}

QVariant PmhCategoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.column() == EmptyColumn)
        return QVariant();

    const TreeItem *it = d->getItem(index);

    switch (role)
    {
    case Qt::EditRole :
    case Qt::DisplayRole :
        {
            if (index.column()==Label) {
                return it->label();
            }
            return QVariant();
        }
    case Qt::ToolTipRole :
        {
            if (!it->isCategory()) {
                PmhData *pmh = it->pmhData();
                if (!pmh)
                    return QVariant();
                QString ret = QString("<b>%1</b><br />"
                                      "%2: %3<br />"
                                      "%4: %5")
                        .arg(pmh->data(PmhData::Label).toString())
                        .arg(tkTr(Trans::Constants::TYPE))
                        .arg(Constants::typeToString(pmh->data(PmhData::Type).toInt()))
                        .arg(tkTr(Trans::Constants::STATUS))
                        .arg(Constants::statusToString(pmh->data(PmhData::State).toInt()))
                        ;
                return ret;
            }
            return QVariant();
        }
    case Qt::ForegroundRole :
        {
            if (index.column()!=Label)
                return QVariant();
            if (it->isCategory()) {
                return QColor(settings()->value(Constants::S_FOREGROUND_CATEGORIES, "darkblue").toString());
            } else {
                return QColor(settings()->value(Constants::S_FOREGROUND_TEMPLATES, "#000").toString());
            }
            return QVariant();
        }
    case Qt::BackgroundRole :
        {
            if (index.column()!=Label)
                return QVariant();
            QColor c;
            if (it->isCategory()) {
                c = QColor(settings()->value(Constants::S_BACKGROUND_CATEGORIES, "white").toString());
            } else {
                c = QColor(settings()->value(Constants::S_BACKGROUND_TEMPLATES, "white").toString());
            }
//            if (Utils::isDebugCompilation()) {
//                if (it->isNewlyCreated()) {
//                    c = QColor(Qt::blue);
//                } else
//                    if (it->isModified()) {
//                    c = QColor(Qt::red);
//                }
//            }
            if (c.name()=="#ffffff")
                return QVariant();
            c.setAlpha(125);
            return c;
        }
    }

    return QVariant();
}

bool PmhCategoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    if (!index.isValid())
//        return QVariant();

//    if (index.column() == EmptyColumn)
//        return QVariant();

//    const TreeItem *it = d->getItem(index);

//    if (role==Qt::EditRole || role == Qt::DisplayRole) {
//        switch (index.column()) {

//        }
//    }

    return true;
}

Qt::ItemFlags PmhCategoryModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/**
  \brief Add or modify a PmhData. If the PmhData pointer does not already exists in the model, the data is created, otherwise it is updated.
  \todo improve this using dataChanged(index, index) ??
*/
bool PmhCategoryModel::addPmhData(PmhData *pmh)
{
    if (d->m_Pmhs.contains(pmh)) {
        // update the model
        for(int i = 0; i < d->m_Root->childCount(); ++i) {
            TreeItem *item = d->m_Root->child(i);
            if (item->pmhData() == pmh) {
                // Remove the row
                d->m_Root->removeChild(item);
                delete item;
                item = 0;
                // Insert the row
                item = new TreeItem(d->m_Root);
                d->pmhToItem(pmh, item);
                // Send to database
                base()->savePmhData(pmh);
                // Reset the model
                /** \todo improve this */
                reset();
                return true;
            }
        }
    } else {
        // save pmh to database
        base()->savePmhData(pmh);
        // insert the pmh to the model
        d->pmhToItem(pmh, new TreeItem);
        reset();
    }
    return true;
}

/**
  \brief Add or modify a PmhCategory. If the PmhCategory pointer does not already exists in the model, the data is created, otherwise it is updated.
  \todo improve this using dataChanged(index, index) ??
*/
bool PmhCategoryModel::addPmhCategoryData(PmhCategory *cat)
{
    if (d->m_Cats.contains(cat)) {
        // update the model
    } else {
        // save the category to database
        base()->savePmhCategory(cat);
        // insert the pmh to the model
        d->categoryToItem(cat, new TreeItem);
        reset();
    }
    return true;
}

Internal::PmhData *PmhCategoryModel::pmhDataforIndex(const QModelIndex &item) const
{
    if (!item.isValid())
        return 0;

    TreeItem *it = d->getItem(item);
    if (it)
        return it->pmhData();
    return 0;
}
