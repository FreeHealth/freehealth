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
 *       Jerome Pinguet <jerome@jerome.cc>                                 *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class PMH::PmhCategoryModel
 *  General model for the categorized PMHx.
 *  Its unique instance is available in the PMH::PmhCore.
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

#include <formmanagerplugin/formcore.h>
#include <formmanagerplugin/iformitem.h>
#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/episodemanager.h>
#include <formmanagerplugin/formtreemodel.h>
#include <formmanagerplugin/episodemodel.h>

#include <categoryplugin/categorycore.h>

#include <utils/log.h>
#include <utils/global.h>
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

static inline PmhBase *base() {return PmhBase::instance();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::Translators *translators() {return Core::ICore::instance()->translators();}
static inline Form::FormManager &formManager() {return Form::FormCore::instance().formManager();}
static inline Form::EpisodeManager &episodeManager() {return Form::FormCore::instance().episodeManager();}

namespace {

    // FIXME: use a QStandardItemModel
    class TreeItem
    {
    public:
        enum DataRepresentation {
            Label = 0
        };

        TreeItem(TreeItem *parent = 0) :
                m_Parent(parent),
                m_Cat(0),
                m_Pmh(0),
                m_Form(0),
                m_FormEpisodeModel(0)
        {
            if (m_Parent)
                m_Parent->addChildren(this);
        }

        ~TreeItem()
        {
            m_Label.clear();
            qDeleteAll(m_Children);
            m_Children.clear();
            m_Pmh=0;
            m_Cat=0;
            m_Parent=0;
            m_Form=0;
            m_FormEpisodeModel=0;
        }


        // Genealogy management
        TreeItem *child(int number) { return m_Children.value(number); }
        int childCount() const { return m_Children.count(); }
        int columnCount() const { return 1; }
        TreeItem *parent() { return m_Parent; }
        void setParent(TreeItem *parent) { m_Parent = parent; }
        bool addChildren(TreeItem *child)
        {
            //qDebug() << Q_FUNC_INFO;
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
            //qDebug() << Q_FUNC_INFO;
            if (m_Parent!=nullptr) {
                //qDebug() << m_Parent;
                //qDebug() << m_Children;
                //qDebug() << const_cast<TreeItem*>(this);
                //qDebug() << m_Parent->m_Children.indexOf(const_cast<TreeItem*>(this));
                return m_Parent->m_Children.indexOf(const_cast<TreeItem*>(this));
            }
            return 0;
        }
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
            m_FormEpisodeModel = model;
        }
        Form::FormMain *form() const {return m_Form;}
        Form::EpisodeModel *formEpisodeModel() const {return m_FormEpisodeModel;}

        bool isForm() const {return ((m_Form) && (m_FormEpisodeModel));}

        void warn(int indent = 0)
        {
            QString sp;
            if (indent)
                sp.fill(' ', indent);
            //qDebug() << sp + label();
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
        Category::CategoryItem *m_Cat;
        PmhData *m_Pmh;
        Form::FormMain *m_Form;
        Form::EpisodeModel *m_FormEpisodeModel;
    };
}

namespace PMH {
namespace Internal {
class PmhCategoryModelPrivate
{
public:
    PmhCategoryModelPrivate(PmhCategoryModel */*parent*/) :
        _rootItem(0),
        _overview(0) // ,q(parent)
    {     
        clearTree();
    }

    ~PmhCategoryModelPrivate()
    {
        // Delete all TreeItem
        if (_rootItem) {
            delete _rootItem; _rootItem=0;
        }
        // Delete all PmhData and PmhCategory pointers
        qDeleteAll(_pmh);
        _pmh.clear();
        qDeleteAll(_categoryTree);
        _categoryTree.clear();
    }

    void clearTree()
    {
        if (_rootItem) {
            delete _rootItem;
            _rootItem = 0;
        }
        _rootItem = new TreeItem;
        _rootItem->setLabel("ROOT CATEGORY");
        Category::CategoryItem *cat = new Category::CategoryItem;
        _rootItem->setPmhCategory(cat);
        _flattenCategoryList.clear();
        _categoryTree.clear();
    }

    TreeItem *getItem(const QModelIndex &index) const
    {
        if (index.isValid()) {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            if (item) return item;
        }
        return _rootItem;
    }

    void formModelToTreeItem(Form::FormMain *form, TreeItem *parentItem, Form::FormTreeModel *formTreeModel, const QModelIndex &index = QModelIndex())
    {
        // Create an item for each form
        for(int i = 0; i < formTreeModel->rowCount(index); ++i) {
            QModelIndex idx = formTreeModel->index(i, Form::FormTreeModel::Label, index);
            TreeItem *newItem = new TreeItem(parentItem);
            newItem->setLabel(formTreeModel->data(idx).toString());
            newItem->setForm(formTreeModel->formForIndex(idx), episodeManager().episodeModel(form));
            // Read all its children
            formModelToTreeItem(form, newItem, formTreeModel, idx);
        }
    }

    void categoryToItem(Category::CategoryItem *cat, TreeItem *item)
    {
        // Insert category to item
        item->setPmhCategory(cat);
        _categoryToItem.insert(cat, item);

        // Category has forms ?
        const QString &xml = cat->data(Category::CategoryItem::ExtraXml).toString();
        if (!xml.isEmpty()) {
            // Check the addfile tag
            QDomDocument doc;
            doc.setContent(xml);
            QDomElement addFile = doc.documentElement();
            addFile = addFile.firstChildElement("file");
            if (!addFile.isNull()) {
                // Get the FormTreeModel with the form
                Form::FormTreeModel *formModel = formManager().formTreeModelForSubForm(addFile.text());
                // Translate all modelindex to TreeItem
                formModelToTreeItem(formModel->formForIndex(formModel->index(0,0)), item, formModel);
            }
        }

        // Create all children categories
        foreach(Category::CategoryItem *c, cat->children()) {
            categoryToItem(c, new TreeItem(item));
        }
    }

    /**
     * \brief Create treeItem items to display pmh data information in the tree
     * view
     * \param pmh
     * \param item
     * \param childNumber
     */
    void pmhToItem(PmhData *pmh, TreeItem *item, int childNumber = -1)
    {
        // Master Item (show user label)
        item->setPmhData(pmh);
        item->setLabel(pmh->data(PmhData::Label).toString());
        _pmhToItems.insert(pmh, item);

        // Add type as child of Master Item
        pmhToItemType(pmh, item);

        // Add status as child of the Master Item
        pmhToItemStatus(pmh, item);

        foreach(PmhEpisodeData *episode, pmh->episodes()) {
            // If at least one date exists, create a new item to show it
            if (!episode->data(PmhEpisodeData::DateStart).isNull()
                    || !episode->data(PmhEpisodeData::DateEnd).isNull()) {
                pmhToItemDate(pmh, item, episode);
            }
            // Add ICD codings
            foreach(const QString &icd, episode->data(PmhEpisodeData::IcdLabelStringList).toStringList()) {
                TreeItem *child = new TreeItem(item);
                child->setLabel(icd);
                child->setPmhData(pmh);
            }
        }

        // Find related PmhCategory in TreeItems
        int id = pmh->categoryId();
        for(int i = 0; i < _flattenCategoryList.count(); ++i) {
            Category::CategoryItem *cat = _flattenCategoryList.at(i);
            if (cat->id()==id) {
                // Reparent TreeItem
                item->setParent(_categoryToItem.value(cat));
                if (childNumber==-1) {
                    _categoryToItem.value(cat)->addChildren(item);
                } else {
                    _categoryToItem.value(cat)->insertChild(childNumber,item);
                }
                return;
            }
        }
        // No category found --> reparent to Root TreeItem
        item->setParent(_rootItem);
        if (childNumber==-1) {
            _rootItem->addChildren(item);
        } else {
            _rootItem->insertChild(childNumber,item);
        }
    }

    /**
     * \brief pmhToItemType: create a child TreeItem for a pmh TreeItem item
     * representing pmhdata "type" (Chronic disease, ...) in treeView
     * \param pmh
     * \param item
     * If pmh type is not defined, do not create any item.
     */
    void pmhToItemType(PmhData *pmh, TreeItem *item)
    {
        if (pmh->data(PmhData::Type).toInt()
                != PMH::Constants::MHType::NoTypeDefined) {
            TreeItem *child = new TreeItem(item);
            child->setLabel(Constants::typeToString(pmh->data(PmhData::Type).toInt()));
            child->setPmhData(pmh);
        }
    }

    /**
     * \brief pmhToItemStatus: create a child TreeItem for a pmh TreeItem item
     * representing pmhdata "status" (Active, In remission...) in treeView
     * \param pmh
     * \param item
     * If pmh status is not defined, do not create any item.
     */
    void pmhToItemStatus(PmhData *pmh, TreeItem *item)
    {
        if (pmh->data(PmhData::State).toInt()
                != PMH::Constants::MHStatus::NoStatusDefined) {
            TreeItem *child = new TreeItem(item);
            child->setLabel(Constants::statusToString(pmh->data(PmhData::State).toInt()));
            child->setPmhData(pmh);
        }
    }

    /**
     * \brief pmhToItemDate add a child item to a PMH item showing start and end
     * dates
     * \param pmh
     * \param item
     * Rightwards (or leftwards for right to left languages) arrow is added.
     */
    void pmhToItemDate(PmhData *pmh, TreeItem *item, PmhEpisodeData *episode)
    {
        QString label;
        TreeItem *child = new TreeItem(item);
        child->setPmhData(pmh);
        label = QString("%1%2%3%4")
                .arg((pmh->episodes().indexOf(episode)==0) ? QString() : (episode->data(PmhEpisodeData::Label).toString() + ":"))
                .arg(episode->data(PmhEpisodeData::DateStart).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)))
                .arg(((QGuiApplication::isLeftToRight()) ? QString("\u279D") : QString("\u2190")))
                .arg(episode->data(PmhEpisodeData::DateEnd).toDate().toString(QLocale().dateFormat(QLocale::ShortFormat)));
        child->setLabel(label);
    }

    Category::CategoryItem *findCategory(const int id)
    {
        for(int i = 0; i < _flattenCategoryList.count(); ++i) {
            Category::CategoryItem *cat = _flattenCategoryList.at(i);
            if (cat->id() == id)
                return cat;
        }
        return 0;
    }

    void getCategories(bool getFromDatabase = false)
    {
        if (getFromDatabase) {
            qDeleteAll(_categoryTree);
            _categoryTree.clear();
            // Get all categories from database
            _categoryToItem.clear();
            if (!_overview) {
                _overview = new Category::CategoryItem;
                _overview->setData(Category::CategoryItem::DbOnly_Id, -2);
                _overview->setData(Category::CategoryItem::DbOnly_LabelId, -1);
                _overview->setData(Category::CategoryItem::DbOnly_ParentId, -3);
                _overview->setData(Category::CategoryItem::DbOnly_Mime, -1);
                _overview->setData(Category::CategoryItem::ThemedIcon, Core::Constants::ICONPATIENTOVERVIEW);
                _overview->setData(Category::CategoryItem::SortId, -1);
                _overview->setLabel(tkTr(Trans::Constants::OVERVIEW), Trans::Constants::ALL_LANGUAGE);
            }
            _categoryTree << _overview;
            _categoryTree << base()->getPmhCategory(_rootUid);
        }
        // Recreate the category tree
        foreach(Category::CategoryItem *cat, _categoryTree) { ///base()->createCategoryTree(_categoryTree)) {
            _rootItem->pmhCategory()->addChild(cat);
            categoryToItem(cat, new TreeItem(_rootItem));
        }
        _flattenCategoryList << _categoryToItem.uniqueKeys().toVector();
    }

    void getPmh()
    {
        _pmh = base()->getPmh();
        base()->linkPmhWithCategory(_flattenCategoryList, _pmh);
        for(int i = 0; i < _pmh.count(); ++i) {
            pmhToItem(_pmh.at(i), new TreeItem);
        }
    }

    void getDataFromDatabase()
    {
        getCategories(true);
        getPmh();
    }

public:
    TreeItem *_rootItem;
    QVector<PmhData *> _pmh;
    QVector<Category::CategoryItem *> _categoryTree, _flattenCategoryList;
    QHash<Category::CategoryItem *, TreeItem *> _categoryToItem;
    QHash<PmhData *, TreeItem *> _pmhToItems;
    QMultiHash<Category::CategoryItem *, PmhData *> _categoryToMultiPmh;
    Category::CategoryItem *_overview;
    QString _htmlSynthesis;
    QString _rootUid;

private:
//    PmhCategoryModel *q;
};

}  // End namespace Internal
}  // End namespace PMH


PmhCategoryModel::PmhCategoryModel(QObject *parent) :
        Category::ICategoryModelHelper(parent), d(new Internal::PmhCategoryModelPrivate(this))
{
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
    connect(translators(), SIGNAL(languageChanged()), this, SLOT(retranslate()));
}

PmhCategoryModel::~PmhCategoryModel()
{
    if (d)
        delete d;
    d = 0;
}

/**
  Define the uid of the root form that contains the categories to use in the model.
  \sa Forms::FormManager::loadPmhCategories(), XmlForms::XmlFormIO::loadPmhCategories()
*/
void PmhCategoryModel::setRootFormUid(const QString &uid)
{
    d->_rootUid = uid;
}

/** Return the uid of the root form that contains the categories to use in the model. */
QString PmhCategoryModel::rootFormUid() const
{
    return d->_rootUid;
}

/** Clear and refresh the whole model */
void PmhCategoryModel::refreshFromDatabase()
{
    if (patient()->uuid().isEmpty())
        return;
    beginResetModel();
    qDeleteAll(d->_pmh);
    d->_pmh.clear();
    d->_categoryToMultiPmh.clear();
    d->_pmhToItems.clear();
    d->_categoryToItem.clear();
    d->_categoryTree.clear();
    d->_flattenCategoryList.clear();
    d->_htmlSynthesis.clear();
    d->clearTree();
    d->getDataFromDatabase();
    endResetModel();
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

    if ((parentItem == d->_rootItem) || (!parentItem))
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

/**
 * \brief PmhCategoryModel::pmhCount
 * \param parent item
 * \return number parent's items children (PMH item or form item)
 */
int PmhCategoryModel::pmhCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;
    TreeItem *item = d->getItem(parent);
    Q_ASSERT(item);
    if (!item)
        return 0;
    int n = 0;
    if (item->isCategory()) {
        for(int i=0; i < item->childCount(); ++i) {
            n += pmhCount(index(i,0,parent));
        }
    } else if (item->isPmh() || item->isForm()) {
        ++n;
    }
    return n;
}

int PmhCategoryModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *item = d->getItem(parent);
    Q_ASSERT(item);
    if (!item)
        return 0;
    if (item->isForm())
        return 0;
    if (item)
        return item->childCount();
    return 0;
}

int PmhCategoryModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void PmhCategoryModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);
}

bool PmhCategoryModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return false;
}

QVariant PmhCategoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.column() == EmptyColumn)
        return QVariant();

    const TreeItem *it = d->getItem(index);
    Q_ASSERT(it);
    if (!it)
        return QVariant();

    switch (role)
    {
    case Qt::EditRole :
    case Qt::DisplayRole :
        {
            if (index.column()==Label) {
                if (it->isCategory()) {
                    if (it->pmhCategory() == d->_overview)
                        return tkTr(Trans::Constants::PATIENT_OVERVIEW);
                    else
                        return it->label();// + " " + QString::number(it->pmhCategory()->sortId());
                }
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
            break;
        }
    case Qt::SizeHintRole:
    {
        if (it->isCategory()) {
            QSize size;
            QFont font = data(index, Qt::FontRole).value<QFont>();
            QFontMetrics fm(font);
            size.setHeight(fm.height() + 5);
            return size;
        }
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
    Q_ASSERT(it);
    if (!it)
        return false;

    if (!it->isCategory())
        return false;

    if (role==Qt::EditRole || role == Qt::DisplayRole) {
        it->pmhCategory()->setLabel(value.toString());
        it->setLabel(value.toString());
        d->_htmlSynthesis.clear();
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
    //qDebug() << Q_FUNC_INFO;
    d->_rootItem->warn();
    int max = row+count;
    TreeItem *parentItem = 0;
    //qDebug() << "beforeRemoveRows" << row << count << max;

    for(int i = row; i < max; ++i) {
        QModelIndex indexToDelete = index(row, 0, parent);
        //qDebug() << i << indexToDelete;
        if (!indexToDelete.isValid())
            continue;

        parentItem = d->getItem(indexToDelete.parent());

        TreeItem *item = d->getItem(indexToDelete);
        if (!item)
            continue;

        //qDebug() << "itemIsCat" << item->isPmh() << item->label();

        // Item is a PMH
        if (item->isPmh()) {
            QModelIndex pmhcategory;
            // Get the root index of the PMH
            while (true) {
                if (isCategory(indexToDelete.parent()))
                    break;
                pmhcategory = indexToDelete.parent();
            }
            beginRemoveRows(pmhcategory, indexToDelete.row(), indexToDelete.row());
            item = d->getItem(indexToDelete);
            //qDebug() << "item: " << item;
            if (!item)
                continue;
            PmhData *pmh = item->pmhData();
            if (pmh) {
                if (pmh->data(PmhData::IsValid).toBool()) {
                    pmh->setData(PmhData::IsValid, false);
                    base()->updatePmhData(pmh);
                    //base()->updatePmhData(pmh);
                    //if (d->_pmh.contains(pmh))
                        //d->_pmh.remove(d->_pmh.indexOf(pmh));
                }
            }

            endRemoveRows();
            // remove from treeItems
            //if (parentItem && item) {
            //    parentItem->removeChild(item);
            //    delete item;
            //    item = 0;
            //}
            refreshFromDatabase();

        } else {
            // Item is a category
            // Remove children
            removeRows(0, rowCount(indexToDelete), indexToDelete);

            beginRemoveRows(indexToDelete.parent(), indexToDelete.row(), indexToDelete.row()+1);
            Category::CategoryItem *cat = item->pmhCategory();
            cat->setData(Category::CategoryItem::DbOnly_IsValid, false);

            // TODO: recode this while d._categoryTree is not linear at all.
            d->_flattenCategoryList.remove(d->_flattenCategoryList.indexOf(cat));
            d->_categoryToItem.remove(cat);
            d->_categoryTree.remove(d->_categoryTree.indexOf(cat));
            d->_categoryToMultiPmh.remove(cat);
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
//    d->_rootItem->warn();
    d->_htmlSynthesis.clear();
    return true;
}

/**
  Add or modify a PmhData. If the PmhData pointer does not already exists in the model,
  the data is created, otherwise it is updated.
*/
bool PmhCategoryModel::addPmhData(PmhData *pmh)
{
    //qDebug()<< Q_FUNC_INFO;
    QStringList pmhdata;
    for (int i = 0; i < 8; i++) {
        pmhdata << pmh->data(i).toString();
    }
    //qDebug() << "pmhdata 1 " << pmh << "values: " << pmhdata;

    if (d->_pmh.contains(pmh)) {
        //qDebug() << "inside addPmhData: Update PMH";
        TreeItem *oldItem = d->_pmhToItems.value(pmh, 0);
        Q_ASSERT(oldItem);
        if (!oldItem)
            return false;
        TreeItem *parentOldItem = oldItem->parent(); //parent should be a category
        QModelIndex newParentIndex;

        // Insert the row to the right category
        Category::CategoryItem *cat = d->findCategory(pmh->categoryId());

        if (!cat) {
            //qDebug() << "NO CATEGORY";
            return false;
        }
        newParentIndex = indexForCategory(cat);
        //pmh->setCategory(cat);

        if (!newParentIndex.isValid()) {
            LOG_ERROR("Unable to update PmhCategoryModel");
            return false;
        }

        QModelIndex pmhOldIndex = indexForPmhData(pmh);

        /*qDebug() << "ADD PMH DATA"
                   << "cat->label()" << cat->label()
                   <<  "cat->id()" << cat->id()
                   << "newParentIndex.data()" << newParentIndex.data()
                   << "pmhOldIndex.data();" << pmhOldIndex.data();

        qDebug() << "beginInsertRows ("
                   << newParentIndex
                   << ", " << rowCount(newParentIndex)
                   << ", " << rowCount(newParentIndex)
                   << ")";
        */
        TreeItem *item = new TreeItem;

        beginInsertRows(newParentIndex, rowCount(newParentIndex), rowCount(newParentIndex));

        endInsertRows();

        d->pmhToItem(pmh, item, rowCount(newParentIndex));

        pmhdata.clear();
        for (int i = 0; i < 8; i++) {
            pmhdata << pmh->data(i).toString();
        }
        //qDebug() << "pmhdata 2 " << pmh << "values: " << pmhdata;

        // Remove the row
        /*qDebug() << "beginRemoveRows ("
                   << pmhOldIndex.parent()
                   << ", " << pmhOldIndex.row()
                   << ", " << pmhOldIndex.row()
                   << ")";
        */
        beginRemoveRows(pmhOldIndex.parent(), pmhOldIndex.row(), pmhOldIndex.row());
        //qDebug() << "parentOldItem: " << parentOldItem;
        parentOldItem->removeChild(oldItem);
        delete oldItem;
        oldItem = 0;
        endRemoveRows();

        pmhdata.clear();
        for (int i=0; i < 8; i++) {
            pmhdata << pmh->data(i).toString();
        }
        //qDebug() << "pmhdata 3 " << pmh << "values: " <<pmhdata;

        d->_htmlSynthesis.clear();
        QAbstractItemModel *m = dynamic_cast<QAbstractItemModel*>(this);
        m->dataChanged(QModelIndex(), QModelIndex());
        // Send to database

        pmhdata.clear();
        for (int i=0; i < 8; i++) {
            pmhdata << pmh->data(i).toString();
        }
        //qDebug() << "pmhdata 4 " << pmh << "values: " <<pmhdata;

        base()->savePmhData(pmh);
        return true;
    } else {
        //qDebug() << "Add PMH";
        QModelIndex newParentIndex;

        // Insert the row to the right category
        Category::CategoryItem *cat = d->findCategory(pmh->categoryId());
        if (!cat) {
            //qDebug() << "NO CATEGORY";
            return false;
        }
        newParentIndex = indexForCategory(cat);
        pmh->setCategory(cat);
        if (!newParentIndex.isValid()) {
            LOG_ERROR("Unable to update PmhCategoryModel");
            return false;
        }

        // Save PMH to database
        base()->savePmhData(pmh);
        d->_pmh.append(pmh);

        // insert the pmh to the model
        beginInsertRows(newParentIndex, rowCount(newParentIndex), rowCount(newParentIndex));
        TreeItem *item = new TreeItem;
        d->pmhToItem(pmh, item, rowCount(newParentIndex));
        endInsertRows();
        d->_htmlSynthesis.clear();
        QAbstractItemModel *m = dynamic_cast<QAbstractItemModel*>(this);
        m->layoutChanged();
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
    //qDebug() << Q_FUNC_INFO;
    // get TreeItem
    TreeItem *item = d->_pmhToItems.value((Internal::PmhData *)pmh, 0);
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

/** Return true if the \e index represents synthesis item. */
bool PmhCategoryModel::isSynthesis(const QModelIndex &item) const
{
    if (!item.isValid())
        return false; // root is a category
    TreeItem *it = d->getItem(item);
    if (it->isCategory())
        return (it->pmhCategory() == d->_overview);
    return false;
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

/**
 * Activate the last episode of the form \e formIndex
 * \sa Form::EpisodeModel::populateFormWithEpisodeContent()
 */
bool PmhCategoryModel::activateFormEpisode(const QModelIndex &formIndex) const
{
    if (!formIndex.isValid())
        return false;

    TreeItem *it = d->getItem(formIndex);
    if (!it)
        return false;
    if (!it->isForm())
        return false;

    // get the Form::FormMain index
    Form::EpisodeModel *model = it->formEpisodeModel();
    if (!model)
        return false;

    if (model->rowCount() == 0) {
        // Create the unique episode
        model->insertRow(0);
    }

    return model->populateFormWithEpisodeContent(model->index(0, 0), true);
}

/**
  Add or modify a Category::CategoryItem in the model and in the database.
  \sa PMH::PmhCore::saveCategory(), PMH::PmhBase::savePmhCategory()
*/
void PmhCategoryModel::addCategory(Category::CategoryItem *cat, int row, const QModelIndex &parentCategory)
{
    Q_ASSERT(!d->_rootUid.isEmpty());
    if (d->_rootUid.isEmpty()) {
        LOG_ERROR("No root uid defined - can not create PMHx category");
        return;
    }

    // ensure the category gets the correct mime type for PMHx
    cat->setData(Category::CategoryItem::DbOnly_Mime, QString("%1@%2").arg(PMH::Constants::CATEGORY_MIME).arg(d->_rootUid));

    // TODO: recode this, not more linear _categoryTree
    if (d->_categoryTree.contains(cat)) {
        updateCategory(cat);
    } else {
        TreeItem *parent = d->getItem(parentCategory);
        if (!parent || !parent->isCategory()) {
            parent = d->_rootItem;
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
    d->_htmlSynthesis.clear();
}

/** Update a Category::CategoryItem in the model and in the database. \sa PMH::PmhCore::saveCategory(), PMH::PmhBase::savePmhCategory()*/
void PmhCategoryModel::updateCategory(Category::CategoryItem *category)
{
    Q_ASSERT(!d->_rootUid.isEmpty());
    if (d->_rootUid.isEmpty()) {
        LOG_ERROR("No root uid defined - can not create PMHx category");
        return;
    }

    // ensure the category gets the correct mime type for PMHx
    category->setData(Category::CategoryItem::DbOnly_Mime, QString("%1@%2").arg(PMH::Constants::CATEGORY_MIME).arg(d->_rootUid));

    QModelIndex cat = indexForCategory(category);
    TreeItem *item = d->getItem(cat);
    if (!item)
        return;
    item->setLabel(category->label());
    base()->savePmhCategory(category);
    d->_htmlSynthesis.clear();
    Q_EMIT dataChanged(cat, cat);
}

/** Return the Mime of the model. */
QString PmhCategoryModel::mime() const
{
    return QString("%1@%2").arg(Constants::CATEGORY_MIME).arg(d->_rootUid);
}

QString PmhCategoryModel::indexToHtml(const QModelIndex &index, int indent) const
{
    QString html;
    if (isSynthesis(index))
        return QString();

    if (isCategory(index)) {
        int _rowCount = rowCount(index);
        int _pmhCount = pmhCount(index);
        if (!_pmhCount)
            return QString();
        html = QString("<p style=\"margin:0px 0px 0px %1px\"><span style=\"font-weight:bold;\">%2 (%3)</span><br />")
                .arg(indent*10)
                .arg(index.data(Qt::DisplayRole).toString()).arg(_pmhCount);
        for(int i=0; i < _rowCount; ++i) {
            html += indexToHtml(this->index(i, 0, index), indent+2);
        }
        html += "</p>";
    } else if (isPmhx(index)) {
        QString id;
        for(int i=0; i < indent; ++i)
            id += "&nbsp;&nbsp;";
        html += QString("•&nbsp;%1<br />").arg(index.data(Qt::ToolTipRole).toString().replace("<br />","; "));
    } else if (isForm(index)) {
        // populate form
        activateFormEpisode(index);
        Form::FormMain *form = formForIndex(index);
        // get print mask
        html = formManager().formPrintHtmlOutput(form);
        QString css = Utils::htmlTakeAllCssContent(html);
        html = Utils::htmlBodyContent(html);
        html.prepend(css);
        // clear form
        if (form)
            form->clear();
    }
    return html;
}

void PmhCategoryModel::refreshSynthesis()
{
    d->_htmlSynthesis.clear();
}

/** Return the synthesis of the whole model of a specific category \e index */
QString PmhCategoryModel::synthesis(const QModelIndex &parent) const
{
    if (parent==QModelIndex() || isSynthesis(parent)) {
        if (d->_htmlSynthesis.isEmpty()) {
            d->_htmlSynthesis ="<html><style>p{margin:0 0 0 0}</style><body>";
            d->_htmlSynthesis += QString("<p align=center style=\"font-weight:bold;font-size:16pt\">%1<hr/></p>").arg(tr("Patient PMHx synthesis"));
            for(int i=0; i < rowCount(parent); ++i) {
                d->_htmlSynthesis += indexToHtml(index(i, 0, parent));
            }
            d->_htmlSynthesis += "</body></html>";
        }
        return d->_htmlSynthesis;
    } else if (isCategory(parent)) {
        QString html ="<html><style>p{margin:0 0 0 0}</style><body>";
        html += QString("<p align=center style=\"font-weight:bold;font-size:16pt\">%1<br />%2<hr/></p>")
                .arg(tr("Patient PMHx synthesis"))
                .arg(data(parent).toString());
        for(int i=0; i < rowCount(parent); ++i) {
            html += indexToHtml(index(i, 0, parent));
        }
        html += "</body></html>";
        return html;
    }
    return QString::null;
}

/** Update the model when the current patient changes. */
void PmhCategoryModel::onCurrentPatientChanged()
{
    beginResetModel();
    qDeleteAll(d->_pmh);
    d->_pmh.clear();
    d->_categoryToMultiPmh.clear();
    d->_pmhToItems.clear();
    d->_categoryToItem.clear();
    d->_htmlSynthesis.clear();

    // TODO: recode this no more linear
    for(int i=0; i < d->_categoryTree.count(); ++i) {
        d->_categoryTree.at(i)->clearContentItems();
    }
    d->clearTree();
    // Do not reload categories
    if (d->_categoryTree.isEmpty()) {
        d->getDataFromDatabase();
    } else {
        d->getCategories(false);
        d->getPmh();
    }
    endResetModel();
}

/** Update the category label (retranslate for eg) */
void PmhCategoryModel::updateCategoryLabel(const Category::CategoryItem *category)
{
    QModelIndex cat = indexForCategory(category);
    TreeItem *item = d->getItem(cat);
    if (!item)
        return;
    item->setLabel(category->label());
    d->_htmlSynthesis.clear();
    Q_EMIT dataChanged(cat, cat);
}

/** Slot connected to the QEvent::LanguageChanged */
void PmhCategoryModel::retranslate()
{
    QHashIterator<Category::CategoryItem *, TreeItem *> i(d->_categoryToItem);
    while (i.hasNext()) {
        i.next();
        i.value()->setLabel(i.key()->label());
        QModelIndex idx = indexForCategory(i.key());
        Q_EMIT dataChanged(idx,idx);
    }
    d->_htmlSynthesis.clear();
}
