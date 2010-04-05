#include "formglobalmodel.h"

#include <extensionsystem/pluginmanager.h>

#include <coreplugin/icore.h>

#include "formmanager.h"
#include "iformwidgetfactory.h"
#include "iformitem.h"
#include "iformitemdatafactory.h"

namespace {
static QHash<QString, Core::IFormItemDataFactory *> m_PlugsFactories;
}

using namespace Core;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  Inline static functions  //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline static Core::FormManager *fm() {return Core::ICore::instance()->formManager();}
inline static ExtensionSystem::PluginManager *pm() {return ExtensionSystem::PluginManager::instance();}
inline static void refreshPlugsFactories()
{
    ::m_PlugsFactories.clear();
    foreach(Core::IFormItemDataFactory *fact, pm()->getObjects<Core::IFormItemDataFactory>()) {
        foreach(const QString &name, fact->providedItemDatas()) {
            ::m_PlugsFactories.insert(name,fact);
        }
    }
}

inline static Core::IFormItemData *getItemData(const QModelIndex &idx)
{
    if (!idx.isValid())
        return 0;
    Q_ASSERT(static_cast<Core::IFormItemData*>(idx.internalPointer()));
    return static_cast<Core::IFormItemData*>(idx.internalPointer());
}

inline static Core::FormItem *getItem(const QModelIndex &idx)
{
    Core::IFormItemData *dt = getItemData(idx);
//    qWarning() << "getItem" << idx.data() << dt->parentItem()->uuid();
    if (dt)
        return dt->parentItem();
    return 0;
}

inline static QList<Core::IFormItemData *> itemDatas(Core::FormItem *item)
{
    QList<Core::IFormItemData *> list;
    if (!item)
        return list;
    Core::IFormItemData *cast = 0;
    foreach(QObject *o, item->children()) {
        cast = qobject_cast<Core::IFormItemData *>(o);
        if (cast)
            list << cast;
    }
    return list;
}

inline static int itemDatasCount(Core::FormItem *item)
{
    return itemDatas(item).count();
}
inline static int itemDatasCount(const QModelIndex &idx)
{
    return itemDatas(getItem(idx)).count();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  FormGlobalModel  //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
FormGlobalModel::FormGlobalModel(QObject *parent) :
        QAbstractItemModel(parent)
{
    refreshPlugsFactories();

    // creates an artificial model
    root = new Core::FormItem(this);
    root->setUuid("0");
    root->setObjectName("RootItem");
//    root->createChildItem("1");
//    root->createChildItem("2");
    Core::FormItem *sec = root->createChildItem("3");
    sec->setObjectName("SecItem");
//    sec->createChildItem("3.1");
//    sec->createChildItem("3.2");
//    sec->createChildItem("3.3");
    Core::FormItem *third = root->createChildItem("4");
    third->setObjectName("ThirdItem");
    plugs = new FakeDataFactory(this);
    plugs->createItemData(root)->setData("root1");
//    plugs->createItemData(root)->setData("root2");
//    plugs->createItemData(root)->setData("root3");

    plugs->createItemData(sec)->setData("sec1");
//    plugs->createItemData(sec)->setData("sec2");

    plugs->createItemData(third)->setData("third1");
//    plugs->createItemData(third)->setData("third2");
//    plugs->createItemData(third)->setData("third3");

}

FormGlobalModel::~FormGlobalModel()
{}

QModelIndex FormGlobalModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    Core::FormItem *parentItem = 0;
    if (!parent.isValid()) {
        parentItem = root;
    }
    else {
        parentItem = getItem(parent);
    }

    Core::IFormItemData *dt = itemDatas(parentItem).at(row);
    QModelIndex i;
    if (dt)
        i = createIndex(row, column, dt);
    else
        i = QModelIndex();

    qWarning() << "index" << row << column << parentItem->objectName() << dt->data();
    return i;
}

int FormGlobalModel::columnCount(const QModelIndex &parent) const
{
    return Core::IFormItemData::columnCount();
}

int FormGlobalModel::rowCount(const QModelIndex &parent) const
{
    Core::FormItem *parentItem = 0;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = getItem(parent);

    return itemDatas(parentItem).count();
}

QVariant FormGlobalModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.column()>1)
        return QVariant();

    return getItemData(index)->data();
}

Qt::ItemFlags FormGlobalModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QModelIndex FormGlobalModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    Core::FormItem *item = getItem(index);
    qWarning() << "Item = " << item->objectName() << getItemData(index)->data();
    if (item == root)
        return QModelIndex();

    Core::FormItem *parentItem = static_cast<Core::FormItem*>(item->parent());
    if (parentItem) qWarning() << "Parent = " << parentItem->objectName();
    if (parentItem == root)
        return QModelIndex();

    qWarning() << item->objectName() << parentItem->objectName();
    qWarning() << itemDatas(item) << getItemData(index);

    int row = itemDatas(parentItem).indexOf(getItemData(index));
//    qWarning() << index << childItem << parentItem << row;

    return createIndex(row, 0, parentItem);
}

