#include "pmhcategorymodel.h"
#include "pmhbase.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QSqlQuery>
#include <QSqlDatabase>

#include <QDebug>

using namespace PMH;

static inline Internal::PmhBase *base() {return Internal::PmhBase::instance();}
static inline Core::IPatient *patient()  { return Core::ICore::instance()->patient(); }
static inline QString currentUserUuid() {return Core::ICore::instance()->user()->value(Core::IUser::Uuid).toString();}


namespace PMH {
namespace Internal {

class TreeItem
{
public:
    TreeItem(const QHash<int, QVariant> &datas, TreeItem *parent = 0) :
            m_Parent(parent),
            m_Datas(datas),
            m_IsModified(false)
    {
        if (m_Parent)
            m_Parent->addChildren(this);
    }
    ~TreeItem() { qDeleteAll(m_Children); }

    // Genealogy management
    TreeItem *child(int number) { return m_Children.value(number); }
    int childCount() const { return m_Children.count(); }
    int columnCount() const { return PmhCategoryModel::ColumnCount; }
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
    QVariant data(int column) const
    {
        return m_Datas.value(column, QVariant());
    }

    bool setData(int column, const QVariant &value)
    {
        //        qWarning()<< data(column) << value << (data(column)==value);
        if (data(column)==value)
            return true;
        m_Datas.insert(column, value);
        m_IsModified = true;
        if (!m_DirtyRows.contains(column))
            m_DirtyRows.append(column);
        return true;
    }

    QVector<int> dirtyRows() const
    {
        return m_DirtyRows;
    }

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
    QHash<int, QVariant> m_Datas;
    QVector<int> m_DirtyRows;
    bool m_IsTemplate, m_IsModified;
};


class PmhCategoryModelPrivate
{
public:
    PmhCategoryModelPrivate(PmhCategoryModel *parent) :
            m_Root(0),
            q(parent)
    {}

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


    void getCategories()
    {
        // Get all categories from database
        QSqlQuery query(base()->database());
        QString req = base()->select(Constants::Table_CATEGORIES);
        QHash<int, TreeItem *> items;
        if (query.exec(req)) {
            while (query.next()) {
                QHash<int, QVariant> datas;
                datas.insert(Constants::CATEGORY_ID, query.value(Constants::CATEGORY_ID));
                datas.insert(Constants::CATEGORY_PARENT, query.value(Constants::CATEGORY_PARENT));
                datas.insert(Constants::CATEGORY_LABEL_ID, query.value(Constants::CATEGORY_LABEL_ID));
                datas.insert(Constants::CATEGORY_ISCHONICDISEASE, query.value(Constants::CATEGORY_ISCHONICDISEASE));
                datas.insert(Constants::CATEGORY_ISRISKFACTOR, query.value(Constants::CATEGORY_ISRISKFACTOR));
                items.insert(query.value(Constants::CATEGORY_ID).toInt(), new TreeItem(datas, 0));
            }
        } else {
            Utils::Log::addQueryError(q,query,__FILE__,__LINE__);
        }

        // Reparent items
        foreach(TreeItem *item, items.values()) {
            TreeItem *parent = items.value(item->data(Constants::CATEGORY_PARENT).toInt(), 0);
            if (!parent) {
                parent = m_Root;
            }
            item->setParent(parent);
            parent->addChildren(item);
        }
    }

public:
    TreeItem *m_Root;

private:
    PmhCategoryModel *q;

};

}  // End namespace Internal
}  // End namespace PMH



PmhCategoryModel::PmhCategoryModel(QObject *parent) :
        QAbstractItemModel(parent), d(new Internal::PmhCategoryModelPrivate(this))
{
    d->getCategories();
}

PmhCategoryModel::~PmhCategoryModel()
{
    if (d)
        delete d;
    d = 0;
}

QModelIndex PmhCategoryModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (parent.isValid() && parent.column() != 0)
         return QModelIndex();

//     if (!parent.isValid())
//         return QModelIndex();

     Internal::TreeItem *parentItem = d->getItem(parent);
     Internal::TreeItem *childItem = parentItem->child(row);

     if (childItem) {
         return createIndex(row, column, childItem);
     }
     return QModelIndex();
 }

QModelIndex PmhCategoryModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     Internal::TreeItem *childItem = d->getItem(index);
     Internal::TreeItem *parentItem = childItem->parent();

     if (parentItem == d->m_Root)
         return QModelIndex();

     return createIndex(parentItem->childNumber(), 0, parentItem);
 }

int PmhCategoryModel::rowCount(const QModelIndex &parent) const
{
    Internal::TreeItem *item = d->getItem(parent);
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

    const Internal::TreeItem *it = d->getItem(index);

    if (role==Qt::DisplayRole) {
        if (index.column() == Label)
            return it->data(Constants::CATEGORY_LABEL_ID);
    }

    return QVariant();
}

bool PmhCategoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return true;
}

Qt::ItemFlags PmhCategoryModel::flags(const QModelIndex &index) const
{
}
