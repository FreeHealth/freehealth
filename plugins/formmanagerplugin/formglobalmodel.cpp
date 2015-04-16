/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2015 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#include "formglobalmodel.h"

#include <extensionsystem/pluginmanager.h>

#include <coreplugin/icore.h>

#include "formmanager.h"
#include "iformwidgetfactory.h"
#include "iformitem.h"
#include "iformitemdatafactory.h"

namespace {
static QHash<QString, Form::IFormItemDataFactory *> m_PlugsFactories;
}

using namespace Form;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////  Inline static functions  //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline Form::FormManager *formManager() { return Form::FormManager::instance(); }
static inline ExtensionSystem::PluginManager *pm() {return ExtensionSystem::PluginManager::instance();}
static inline void refreshPlugsFactories()
{
    ::m_PlugsFactories.clear();
    foreach(Form::IFormItemDataFactory *fact, pm()->getObjects<Form::IFormItemDataFactory>()) {
        foreach(const QString &name, fact->providedItemDatas()) {
            ::m_PlugsFactories.insert(name,fact);
        }
    }
}

static inline Form::IFormItemData *getItemData(const QModelIndex &idx)
{
    if (!idx.isValid())
        return 0;
    Q_ASSERT(static_cast<Form::IFormItemData*>(idx.internalPointer()));
    return static_cast<Form::IFormItemData*>(idx.internalPointer());
}

static inline Form::FormItem *getItem(const QModelIndex &idx)
{
    Form::IFormItemData *dt = getItemData(idx);
//    qWarning() << "getItem" << idx.data() << dt->parentItem()->uuid();
    if (dt)
        return dt->parentItem();
    return 0;
}

static inline QList<Form::IFormItemData *> itemDatas(Form::FormItem *item)
{
    QList<Form::IFormItemData *> list;
    if (!item)
        return list;
    Form::IFormItemData *cast = 0;
    foreach(QObject *o, item->children()) {
        cast = qobject_cast<Form::IFormItemData *>(o);
        if (cast)
            list << cast;
    }
    return list;
}

static inline int itemDatasCount(Form::FormItem *item)
{
    return itemDatas(item).count();
}
static inline int itemDatasCount(const QModelIndex &idx)
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
    root = new Form::FormItem(this);
    root->setUuid("0");
    root->setObjectName("RootItem");
//    root->createChildItem("1");
//    root->createChildItem("2");
    Form::FormItem *sec = root->createChildItem("3");
    sec->setObjectName("SecItem");
//    sec->createChildItem("3.1");
//    sec->createChildItem("3.2");
//    sec->createChildItem("3.3");
    Form::FormItem *third = root->createChildItem("4");
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
{
}

QModelIndex FormGlobalModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    Form::FormItem *parentItem = 0;
    if (!parent.isValid()) {
        parentItem = root;
    }
    else {
        parentItem = getItem(parent);
    }

    Form::IFormItemData *dt = itemDatas(parentItem).at(row);
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
    return Form::IFormItemData::columnCount();
}

int FormGlobalModel::rowCount(const QModelIndex &parent) const
{
    Form::FormItem *parentItem = 0;
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

    Form::FormItem *item = getItem(index);
    qWarning() << "Item = " << item->objectName() << getItemData(index)->data();
    if (item == root)
        return QModelIndex();

    Form::FormItem *parentItem = static_cast<Form::FormItem*>(item->parent());
    if (parentItem) qWarning() << "Parent = " << parentItem->objectName();
    if (parentItem == root)
        return QModelIndex();

    qWarning() << item->objectName() << parentItem->objectName();
    qWarning() << itemDatas(item) << getItemData(index);

    int row = itemDatas(parentItem).indexOf(getItemData(index));
//    qWarning() << index << childItem << parentItem << row;

    return createIndex(row, 0, parentItem);
}

