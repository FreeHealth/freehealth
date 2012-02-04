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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/

/**
   \class PMH::PmhCategoryModel
   General model for the categorized PMHx.
   Unique instance is available in the PMH::PmhCore.
 */

#include "pmhcategorymodel.h"
#include "pmhbase.h"
#include "pmhdata.h"
#include "constants.h"
#include "pmhcore.h"
#include "pmhepisodemodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>
#include <coreplugin/constants_icons.h>

#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/episodemodel.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QColor>
#include <QFont>
#include <QDomDocument>

#include <QDebug>

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;


static inline PmhCore *pmhCore() {return PmhCore::instance();}
static inline PmhBase *base() {return PmhBase::instance();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline QString currentUserUuid() {return Core::ICore::instance()->user()->uuid();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::Translators *translators() {return Core::ICore::instance()->translators();}
static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }


namespace {

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
                m_Pmh(0),
                m_Form(0),
                m_EpisodeModel(0)
        {
            if (m_Parent)
                m_Parent->addChildren(this);
        }

        ~TreeItem()
        {
            m_Label.clear();
            qDeleteAll(m_Children);
            m_Children.clear();
//            delete m_Form;
//            delete m_EpisodeModel;
            m_Pmh=0;
            m_Cat=0;
            m_Parent=0;
            m_Form=0;
            m_EpisodeModel=0;
        }


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

        QIcon icon() const {return m_Icon;}
        void setIcon(QIcon icon) {m_Icon = icon;}

        // Category / PMH
        void setPmhCategory(Category::CategoryItem *cat)
        {
            m_Cat = cat;
            setLabel(cat->label());
            setIcon(theme()->icon(cat->iconName()));
        }
        Category::CategoryItem *pmhCategory() const {return m_Cat;}

        void setPmhData(PmhData *pmh)
        {
            m_Pmh = pmh;
        }
        PmhData *pmhData() const {return m_Pmh;}

        bool isCategory() const {return (m_Cat);}
        bool isPmh() const {return (m_Pmh);}

        // Forms
        void setForm(Form::FormMain *form, Form::EpisodeModel *model)
        {
            m_Form = form;
            m_EpisodeModel = model;
        }
        Form::FormMain *form() const {return m_Form;}
        Form::EpisodeModel *episodeModel() const {return m_EpisodeModel;}

        bool isForm() const {return ((m_Form) && (m_EpisodeModel));}

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

        void warn(int indent = 0)
        {
            QString sp;
            if (indent)
                sp.fill(' ', indent);
            qWarning() << sp + label();
            for(int i = 0; i < m_Children.count(); ++i) {
                m_Children.at(i)->warn(indent+2);
            }
        }


    private:
        TreeItem *m_Parent;
        QList<TreeItem*> m_Children;
        QString m_Label;
        QIcon m_Icon;
        QVector<int> m_DirtyRows;
        bool m_IsModified;
        Category::CategoryItem *m_Cat;
        PmhData *m_Pmh;
        Form::FormMain *m_Form;
        Form::EpisodeModel *m_EpisodeModel;
    };

}


namespace PMH {
namespace Internal {
class PmhCategoryModelPrivate
{
public:
    PmhCategoryModelPrivate(PmhCategoryModel *parent) :
            m_Root(0),
            q(parent)
    {
        clearTree();
    }

    ~PmhCategoryModelPrivate()
    {
        // Delete all TreeItem
        if (m_Root) {
            delete m_Root; m_Root=0;
        }
        // Delete all PmhData and PmhCategory pointers
        qDeleteAll(m_Pmhs);
        m_Pmhs.clear();
        qDeleteAll(m_Cats);
    }

    void clearTree()
    {
        if (m_Root) {
            delete m_Root;
            m_Root = 0;
        }
        m_Root = new TreeItem;
        m_Root->setLabel("ROOT CATEGORY");
        Category::CategoryItem *cat = new Category::CategoryItem;
        m_Root->setPmhCategory(cat);
    }

    TreeItem *getItem(const QModelIndex &index) const
    {
        if (index.isValid()) {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            if (item) return item;
        }
        return m_Root;
    }

    void episodeModelToTreeItem(Form::FormMain *form, TreeItem *parentItem, Form::EpisodeModel *model, const QModelIndex &index = QModelIndex())
    {
        for(int i = 0; i < model->rowCount(index); ++i) {
            QModelIndex idx = model->index(i, Form::EpisodeModel::Label, index);
            // Do not include the episodes
            if (!model->isForm(idx))
                continue;
            if (model->isLastEpisodeIndex(idx))
                continue;
            TreeItem *newItem = new TreeItem(parentItem);
            newItem->setLabel(idx.data().toString());
            newItem->setForm(model->formForIndex(idx), model);
            // Read all its children
            episodeModelToTreeItem(form, newItem, model, idx);
        }
    }

    void categoryToItem(Category::CategoryItem *cat, TreeItem *item)
    {
        // Insert category to item
        item->setPmhCategory(cat);
        m_CategoryToItem.insert(cat, item);

        // Category has forms ?
        const QString &xml = cat->data(Category::CategoryItem::ExtraXml).toString();
        if (!xml.isEmpty()) {
            /** \todo improve this +++ this should be part of the XmlFormIO plugin */
            // Check the addfile tag
            QDomDocument doc;
            doc.setContent(xml);
            QDomElement addFile = doc.documentElement();
            addFile = addFile.firstChildElement("file");
            if (!addFile.isNull()) {
                // Load the form
                QList<Form::FormMain*> forms = formManager()->loadFormFile(addFile.text());
                if (!forms.isEmpty()) {
                    // Create the EpisodeModel with the form
                    Form::EpisodeModel *model = new Form::EpisodeModel(forms.at(0), q);
                    model->init(false);
                    // Translate all modelindex to TreeItem
                    episodeModelToTreeItem(forms.at(0), item, model);
                }
            }
        }

        // Create all children categories
        foreach(Category::CategoryItem *c, cat->children()) {
            categoryToItem(c, new TreeItem(item));
        }
    }

    void pmhToItem(PmhData *pmh, TreeItem *item, int childNumber = -1)
    {
        // Master Item (shows user label)
        item->setPmhData(pmh);
        item->setLabel(pmh->data(PmhData::Label).toString());
        m_PmhToItems.insert(pmh, item);

        // Add type and status as children of the Master Item
        TreeItem *child = new TreeItem(item);
        child->setLabel(Constants::typeToString(pmh->data(PmhData::Type).toInt()));
        child->setPmhData(pmh);
        child = new TreeItem(item);
        child->setLabel(Constants::statusToString(pmh->data(PmhData::State).toInt()));
        child->setPmhData(pmh);

        // Add episodes
        if (pmh->episodeModel()->rowCount() == 1) {
            // One episodeData PMHx
            PmhEpisodeData *episode = pmh->episodes().at(0);
            QString label = episode->data(PmhEpisodeData::Label).toString();
            child = new TreeItem(item);
            child->setPmhData(pmh);
            if (!episode->data(PmhEpisodeData::DateEnd).isNull()) {
                label += QString(" (%1 %2 %3)")
                        .arg(episode->data(PmhEpisodeData::DateStart).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)))
                        .arg(tkTr(Trans::Constants::TO))
                        .arg(episode->data(PmhEpisodeData::DateEnd).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)));
            } else {
                label += QString(" (%1)")
                        .arg(episode->data(PmhEpisodeData::DateStart).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)));
            }
            child->setLabel(label);

            // Add ICD codings
            foreach(const QString &icd, episode->data(PmhEpisodeData::IcdLabelStringList).toStringList()) {
                child = new TreeItem(child);
                child->setLabel(icd);
                child->setPmhData(pmh);
                child->setIcon(theme()->icon(Core::Constants::ICONFREEICD));
            }

        } else {
            // Multiple episodeData PMHx
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
        }

        // Find related PmhCategory in TreeItems
        int id = pmh->categoryId();
        for(int i = 0; i < m_Cats.count(); ++i) {
            if (m_Cats.at(i)->id()==id) {
                // Reparent TreeItem
                item->setParent(m_CategoryToItem.value(m_Cats.at(i)));
                if (childNumber==-1) {
                    m_CategoryToItem.value(m_Cats.at(i))->addChildren(item);
                } else {
                    m_CategoryToItem.value(m_Cats.at(i))->insertChild(childNumber,item);
                }
                return;
            }
        }
        // No category found --> reparent to Root TreeItem
        item->setParent(m_Root);
        if (childNumber==-1) {
            m_Root->addChildren(item);
        } else {
            m_Root->insertChild(childNumber,item);
        }
    }

    void getCategories(bool getFromDatabase = false)
    {
        if (getFromDatabase) {
            // Get all categories from database
            m_Cats.clear();
            m_CategoryToItem.clear();
            m_Cats = base()->getPmhCategory();
        }
        // Recreate the category tree
        foreach(Category::CategoryItem *cat, base()->createCategoryTree(m_Cats)) {
            m_Root->pmhCategory()->addChild(cat);
            categoryToItem(cat, new TreeItem(m_Root));
        }
    }

    void getPmh()
    {
        m_Pmhs = base()->getPmh();
        base()->linkPmhWithCategory(m_Cats, m_Pmhs);
        for(int i = 0; i < m_Pmhs.count(); ++i) {
            pmhToItem(m_Pmhs.at(i), new TreeItem);
        }
    }

    void getDataFromDatabase()
    {
        getCategories(true);
        getPmh();
    }

public:
    TreeItem *m_Root;
    QVector<PmhData *> m_Pmhs;
    QVector<Category::CategoryItem *> m_Cats;
    QHash<Category::CategoryItem *, TreeItem *> m_CategoryToItem;
    QHash<PmhData *, TreeItem *> m_PmhToItems;
    QMultiHash<Category::CategoryItem *, PmhData *> m_Cat_Pmhs;

private:
    PmhCategoryModel *q;

};

}  // End namespace Internal
}  // End namespace PMH


PmhCategoryModel::PmhCategoryModel(QObject *parent) :
        Category::ICategoryModelHelper(parent), d(new Internal::PmhCategoryModelPrivate(this))
{
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(patientChanged()));
    connect(translators(), SIGNAL(languageChanged()), this, SLOT(retranslate()));
}

PmhCategoryModel::~PmhCategoryModel()
{
    if (d)
        delete d;
    d = 0;
}

/** Clear and refresh the whole model */
void PmhCategoryModel::refreshFromDatabase()
{
    if (!patient())
        return;
    qDeleteAll(d->m_Pmhs);
    d->m_Pmhs.clear();
    d->m_Cat_Pmhs.clear();
    d->m_PmhToItems.clear();
    d->m_CategoryToItem.clear();
    d->m_Cats.clear();
    d->clearTree();
    d->getDataFromDatabase();
    reset();
}

QModelIndex PmhCategoryModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

//     if (!parent.isValid())
//         return QModelIndex();

     TreeItem *parentItem = d->getItem(parent);
     if (!parentItem)
         return QModelIndex();

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
    if (!childItem)
        return QModelIndex();

    TreeItem *parentItem = childItem->parent();

    if ((parentItem == d->m_Root) || (!parentItem))
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

int PmhCategoryModel::columnCount(const QModelIndex &) const
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
                if (it->isCategory())
                    return it->label();// + " " + QString::number(it->pmhCategory()->sortId());
                return it->label();
            } else if (index.column()==Id) {
                if (it->isCategory()) {
                    return it->pmhCategory()->id();
                } else if (it->isForm()) {
                    return it->form()->uuid();
                } else if (it->isPmh()) {
                    return it->pmhData()->data(PmhData::Uid);
                }
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
    case Qt::FontRole :
        {
            if (index.column()!=Label)
                return QVariant();
            QFont font;
            if (it->isCategory()) {
                font.fromString(settings()->value(Constants::S_FONT_CATEGORIES).toString());
            } else {
                font.fromString(settings()->value(Constants::S_FONT_PMH).toString());
                font.setBold(true);
                if (!isCategory(index.parent()))
                    font.setBold(false);
                if (!index.parent().isValid())
                    font.setBold(true);
            }
            return font;
        }
    case Qt::ForegroundRole :
        {
            if (index.column()!=Label)
                return QVariant();
            if (it->isCategory()) {
                return QColor(settings()->value(Constants::S_FOREGROUND_CATEGORIES, "darkblue").toString());
            } else {
                return QColor(settings()->value(Constants::S_FOREGROUND_PMH, "#000").toString());
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
                if (it->pmhCategory()->isDirty())
                    c.setRed(255);
            } else {
                c = QColor(settings()->value(Constants::S_BACKGROUND_PMH, "white").toString());
            }
            if (c.name()=="#ffffff")
                return QVariant();
            c.setAlpha(125);
            return c;
        }
    case Qt::DecorationRole :
        {
            if (it->isCategory()) {
                return theme()->icon(it->pmhCategory()->iconName(), Core::ITheme::SmallIcon);
            }
            if (!it->icon().isNull())
                return it->icon();
        }
    }

    return QVariant();
}

/** setData only works on category labels */
bool PmhCategoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (index.column() != Label)
        return false;

    TreeItem *it = d->getItem(index);

    if (!it->isCategory())
        return false;

    if (role==Qt::EditRole || role == Qt::DisplayRole) {
//        it->pmhCategory()->clearLabels();
        it->pmhCategory()->setLabel(value.toString());
        it->setLabel(value.toString());
        Q_EMIT dataChanged(index, index);
    }

    return true;
}

Qt::ItemFlags PmhCategoryModel::flags(const QModelIndex &index) const
{
    if (isCategory(index)) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/** Remove PMHx or Categories. */
bool PmhCategoryModel::removeRows(int row, int count, const QModelIndex &parent)
{
//    d->m_Root->warn();
    int max = row+count;
    TreeItem *parentItem = 0;
//    qWarning() << "beforeRemoveRows" << row << count << max;

    for(int i = row; i < max; ++i) {
        QModelIndex indexToDelete = index(row, 0, parent);
//        qWarning() << i << indexToDelete;
        if (!indexToDelete.isValid())
            continue;
        TreeItem *item = d->getItem(indexToDelete);
        if (!item)
            continue;

//        qWarning() << "itemIsCat" << item->isPmh() << item->label();

        // Item is a PMH
        if (item->isPmh()) {
            // Get the root index of the PMH
            while (true) {
                if (isCategory(indexToDelete.parent()))
                    break;
                indexToDelete = indexToDelete.parent();
            }
            beginRemoveRows(indexToDelete.parent(), indexToDelete.row(), indexToDelete.row()+1);
            item = d->getItem(indexToDelete);
            if (!item)
                continue;
            PmhData *pmh = item->pmhData();
            if (pmh) {
                if (pmh->data(PmhData::IsValid).toBool()) {
                    pmh->setData(PmhData::IsValid, false);
                    base()->updatePmhData(pmh);
                    if (d->m_Pmhs.contains(pmh))
                        d->m_Pmhs.remove(d->m_Pmhs.indexOf(pmh));
                }
            }
            // remove from treeItems
            parentItem = d->getItem(indexToDelete.parent());
            if (!parentItem)
                continue;
            parentItem->removeChild(item);
            delete item;
            item = 0;
            endRemoveRows();
        } else {
            // Item is a category
            // Remove children
            removeRows(0, rowCount(indexToDelete), indexToDelete);

            beginRemoveRows(indexToDelete.parent(), indexToDelete.row(), indexToDelete.row()+1);
            Category::CategoryItem *cat = item->pmhCategory();
            cat->setData(Category::CategoryItem::DbOnly_IsValid, false);
            d->m_CategoryToItem.remove(cat);
            d->m_Cats.remove(d->m_Cats.indexOf(cat));
            d->m_Cat_Pmhs.remove(cat);
            base()->savePmhCategory(cat);
            // remove from treeItems
            parentItem = d->getItem(indexToDelete.parent());
            if (!parentItem)
                continue;
            parentItem->removeChild(item);
            delete item;
            item = 0;
            endRemoveRows();
        }
    }
//    qWarning() << "afterRemoveRows";
//    d->m_Root->warn();
    return true;
}

/**
  Add or modify a PmhData. If the PmhData pointer does not already exists in the model,
  the data is created, otherwise it is updated.
*/
bool PmhCategoryModel::addPmhData(PmhData *pmh)
{
    if (d->m_Pmhs.contains(pmh)) {
        // Update PMH
        TreeItem *oldItem = d->m_PmhToItems.value(pmh);
        TreeItem *parentOldItem = oldItem->parent(); //parent should be a category
        QModelIndex newParentIndex;
        // Insert the row to the right category
        for(int i=0; i < d->m_Cats.count(); ++i) {
            Category::CategoryItem *cat = d->m_Cats.at(i);
            if (cat->id() == pmh->categoryId()) {
                newParentIndex = indexForCategory(cat);
                pmh->setCategory(cat);
                break;
            }
        }
        if (!newParentIndex.isValid()) {
            LOG_ERROR("Unable to update PmhCategoryModel");
            return false;
        }

        QModelIndex pmhOldIndex = indexForPmhData(pmh);

        qWarning() << oldItem->label() << pmhOldIndex;

        beginInsertRows(newParentIndex, rowCount(newParentIndex), rowCount(newParentIndex));
        TreeItem *item = new TreeItem;
        d->pmhToItem(pmh, item, rowCount(newParentIndex));
        endInsertRows();

        // Remove the row
        beginRemoveRows(pmhOldIndex.parent(), pmhOldIndex.row(), pmhOldIndex.row());
        parentOldItem->removeChild(oldItem);
        delete oldItem;
        oldItem = 0;
        endRemoveRows();

        // Send to database
        base()->savePmhData(pmh);

        return true;
    } else {
        // Add PMH
        QModelIndex newParentIndex;
        for(int i=0; i < d->m_Cats.count(); ++i) {
            Category::CategoryItem *cat = d->m_Cats.at(i);
            if (cat->id() == pmh->categoryId()) {
                newParentIndex = indexForCategory(cat);
                pmh->setCategory(cat);
                break;
            }
        }

        // Save PMH to database
        base()->savePmhData(pmh);
        d->m_Pmhs.append(pmh);

        // insert the pmh to the model
        beginInsertRows(newParentIndex, rowCount(newParentIndex), rowCount(newParentIndex));
        TreeItem *item = new TreeItem;
        d->pmhToItem(pmh, item, rowCount(newParentIndex));
        endInsertRows();
    }
    return true;
}

/** Returns the related PmhData pointer related to the QModelIndex \e item. Warning, the pointer should not be deleted. */
Internal::PmhData *PmhCategoryModel::pmhDataforIndex(const QModelIndex &item) const
{
    if (!item.isValid())
        return 0;

    TreeItem *it = d->getItem(item);
    if (it)
        return it->pmhData();
    return 0;
}

/** Return the QModelIndex corresponding to the \e pmh starting the search from the \e rootStart QModelIndex */
QModelIndex PmhCategoryModel::indexForPmhData(const Internal::PmhData *pmh, const QModelIndex &rootStart) const
{
    // get TreeItem
    TreeItem *item = d->m_PmhToItems.value((Internal::PmhData *)pmh);
    for(int i = 0; i < rowCount(rootStart); ++i) {
        if (d->getItem(index(i,0,rootStart))==item) {
            return index(i,0,rootStart);
        }
        QModelIndex child = indexForPmhData(pmh, index(i,0,rootStart));
        if (child.isValid())
            return child;
    }
    return QModelIndex();
}

/** Call this member when you want views to update their fonts and color fore and background. */
void PmhCategoryModel::updateFontAndColors(const QModelIndex &parent)
{
    Q_EMIT dataChanged(index(parent.row(), 0, parent.parent()), index(parent.row(), columnCount(), parent.parent()));
    for(int i = 0; i < rowCount(parent); ++i) {
        updateFontAndColors(index(i,0,parent));
    }
}

/** Return true if the \e index is a PMHx */
bool PmhCategoryModel::isPmhx(const QModelIndex &index) const
{
    if (!index.isValid())
        return true; // root is a category
    TreeItem *it = d->getItem(index);
    return it->isPmh();
}

/** Return true is the \e index is category, false if \e index is a PMHx */
bool PmhCategoryModel::isCategory(const QModelIndex &index) const
{
    if (!index.isValid())
        return true; // root is a category
    TreeItem *it = d->getItem(index);
    return it->isCategory();
}

/** Return true if the \e index represents a Form::FormMain pointer */
bool PmhCategoryModel::isForm(const QModelIndex &index) const
{
    if (!index.isValid())
        return true; // root is a category
    TreeItem *it = d->getItem(index);
    return it->isForm();
}

/** Returns the related Category::CategoryItem pointer related to the QModelIndex \e item. Warning, the pointer should not be deleted. */
Category::CategoryItem *PmhCategoryModel::categoryForIndex(const QModelIndex &item) const
{
    if (!item.isValid())
        return 0;

    TreeItem *it = d->getItem(item);
    if (it)
        return it->pmhCategory();
    return 0;
}

static QModelIndex categoryIndex(const Category::CategoryItem *category, const QModelIndex &parent, const PmhCategoryModel *model)
{
    // Test parent
    if (model->isCategory(parent)) {
        QModelIndex item = model->index(parent.row(), 0, parent.parent());
        if (model->categoryForIndex(item)==category) {
            return item;
        }
    }
    // Test its children
    for(int i = 0; i < model->rowCount(parent); ++i) {
        QModelIndex item = model->index(i, 0, parent);
        QModelIndex ret = categoryIndex(category, item, model);
        if (ret.isValid())
            return model->index(ret.row(), 0, ret.parent());
    }
    return QModelIndex();
}

/** Return the QModelIndex corresponding to the category with the specified \e id, or return an invalid index. */
QModelIndex PmhCategoryModel::indexForCategory(const Category::CategoryItem *category) const
{
    for(int i = 0; i < rowCount(); ++i) {
        QModelIndex ret = categoryIndex(category, index(i,0), this);
        if (ret.isValid()) {
            return ret;
        }
    }
    return QModelIndex();
}

/** Return the Form::FormMain pointer corresponding to the QModelIndex \e item. The returned pointer must not be deleted. */
Form::FormMain *PmhCategoryModel::formForIndex(const QModelIndex &item) const
{
    if (!item.isValid())
        return 0;

    TreeItem *it = d->getItem(item);
    if (it) {
        if (it->isForm()) {
            return it->form();
        }
    }
    return 0;
}

bool PmhCategoryModel::activateFormEpisode(const QModelIndex &formIndex)
{
    if (!formIndex.isValid())
        return false;

    TreeItem *it = d->getItem(formIndex);
    if (!it)
        return false;
    if (!it->isForm())
        return false;

    // get the Form::FormMain index
    Form::EpisodeModel *model = it->episodeModel();
    if (!model)
        return false;
    QModelIndex idx = model->indexForForm(it->form()->uuid());
    if (!idx.isValid())
        return false;

    if (!model->hasChildren(idx)) {
        // Create the unique episode
        model->insertRow(0, idx);
    }

    model->activateEpisode(model->index(0, 0, idx), model->index(idx.row(), Form::EpisodeModel::FormUuid, idx.parent()).data().toString());

    return true;
}

/**
  Add or modify a Category::CategoryItem in the model and in the database.
  \sa PMH::PmhCore::saveCategory(), PMH::PmhBase::savePmhCategory()
*/
void PmhCategoryModel::addCategory(Category::CategoryItem *cat, int row, const QModelIndex &parentCategory)
{
    if (d->m_Cats.contains(cat)) {
        updateCategory(cat);
    } else {
        TreeItem *parent = d->getItem(parentCategory);
        if (!parent || !parent->isCategory()) {
            parent = d->m_Root;
        }
        if (parent->isCategory()) {
            Category::CategoryItem *parentCat = parent->pmhCategory();
            // get the category row
            int catRow = -1;
            for(int i = 0; i < row; ++i) {
                if (this->isCategory(this->index(i,0,parentCategory)))
                    ++catRow;
            }
            parentCat->insertChild(cat, catRow+1);
            cat->setData(Category::CategoryItem::DbOnly_ParentId, parentCat->id());
            parentCat->updateChildrenSortId();
        }
        TreeItem *item = new TreeItem(parent);
        item->setPmhCategory(cat);
        parent->removeChild(item);
        parent->insertChild(row, item);
        // save the category to database
        base()->savePmhCategory(cat);
        if (parent->isCategory()) {
            // save sortIds
            for(int i=0; i < parent->pmhCategory()->childCount(); ++i) {
                base()->savePmhCategory(parent->pmhCategory()->child(i));
            }
        }
        Q_EMIT layoutChanged();
//        reset();
    }
}

/** Update a Category::CategoryItem in the model and in the database. \sa PMH::PmhCore::saveCategory(), PMH::PmhBase::savePmhCategory()*/
void PmhCategoryModel::updateCategory(Category::CategoryItem *category)
{
    QModelIndex cat = indexForCategory(category);
    TreeItem *item = d->getItem(cat);
    if (!item)
        return;
    item->setLabel(category->label());
    base()->savePmhCategory(category);
    Q_EMIT dataChanged(cat, cat);
}

/** Return the Mime of the model. */
QString PmhCategoryModel::mime() const
{
    return Constants::CATEGORY_MIME;
}

/** Update the model when the current patient changes. */
void PmhCategoryModel::patientChanged()
{
    qDeleteAll(d->m_Pmhs);
    d->m_Pmhs.clear();
    d->m_Cat_Pmhs.clear();
    d->m_PmhToItems.clear();
    d->m_CategoryToItem.clear();
    for(int i=0; i < d->m_Cats.count(); ++i) {
        d->m_Cats.at(i)->clearContentItems();
    }
    d->clearTree();
    // Do not reload categories
    if (d->m_Cats.isEmpty()) {
        d->getDataFromDatabase();
    } else {
        d->getCategories(false);
        d->getPmh();
    }
    reset();
}

void PmhCategoryModel::updateCategoryLabel(const Category::CategoryItem *category)
{
    QModelIndex cat = indexForCategory(category);
    TreeItem *item = d->getItem(cat);
    if (!item)
        return;
    item->setLabel(category->label());
    Q_EMIT dataChanged(cat, cat);
}

/** Slot connected to the QEvent::LanguageChanged */
void PmhCategoryModel::retranslate()
{
    QHashIterator<Category::CategoryItem *, TreeItem *> i(d->m_CategoryToItem);
    while (i.hasNext()) {
        i.next();
        i.value()->setLabel(i.key()->label());
        QModelIndex idx = indexForCategory(i.key());
        Q_EMIT dataChanged(idx,idx);
    }
}
