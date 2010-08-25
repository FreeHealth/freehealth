/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "atctreemodel.h"

#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/constants.h>

#include <utils/log.h>

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QLocale>

#include <QDebug>

using namespace DrugsDB;

static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}

namespace DrugsDB {
namespace Internal {

class AtcItem
{
public:
    AtcItem(AtcItem *parent = 0) :
            m_Parent(parent)
    {
    }
    ~AtcItem() { qDeleteAll(m_Children); }

    // Genealogy management
    AtcItem *child(int number) { return m_Children.value(number); }
    int childCount() const { return m_Children.count(); }
    AtcItem *parent() { return m_Parent; }
    void setParent(AtcItem *parent) { m_Parent = parent; }
    bool addChildren(AtcItem *child)
    {
        if (!m_Children.contains(child))
            m_Children.append(child);
        return true;
    }
    bool insertChild(const int row, AtcItem *child)
    {
        if (row > m_Children.count())
            return false;
        m_Children.insert(row, child);
        return true;
    }
    int childNumber() const
    {
        if (m_Parent)
            return m_Parent->m_Children.indexOf(const_cast<AtcItem *>(this));
        return 0;
    }
    void sortChildren()
    {
        qSort(m_Children.begin(), m_Children.end(), AtcItem::lessThan);
    }

    void setData(const int id, const QVariant &value) {m_Datas.insert(id, value.toString());}

    // Access to datas
    QString data(const int id) const
    {
        if (id == AtcTreeModel::ATC_Label) {
            QString l = QLocale().name().left(2);
            if (l=="fr")
                return m_Datas.value(AtcTreeModel::ATC_FrenchLabel);
            else if (l=="de")
                return m_Datas.value(AtcTreeModel::ATC_DeutschLabel);
            else
                return m_Datas.value(AtcTreeModel::ATC_EnglishLabel);
        }
        return m_Datas.value(id);
    }
    QString code() const {return m_Datas.value(AtcTreeModel::ATC_Code);}
    QString english() const {return m_Datas.value(AtcTreeModel::ATC_EnglishLabel);}
    QString french() const {return m_Datas.value(AtcTreeModel::ATC_FrenchLabel);}

    // For sort functions
    static bool lessThan(AtcItem *item1, AtcItem *item2)
    {
        return item1->code() < item2->code();
    }

private:
    AtcItem *m_Parent;
    QList<AtcItem*> m_Children;
    QHash<int, QString> m_Datas;
};


class AtcTreeModelPrivate
{
public:
    AtcTreeModelPrivate(AtcTreeModel *parent) :
            m_Root(0), q(parent)
    {
        m_Root = new AtcItem;
        m_Root->setData(0, "ATC_ROOT_ITEM");
    }

    ~AtcTreeModelPrivate()
    {}

    void getTree()
    {
        QHash<int, QString> where;
        where.insert(Constants::ATC_ID, " < 100000");
        QString req = drugsBase()->selectInteractionsSql(Constants::Table_ATC,
                                         QList<int>()
                                         << Constants::ATC_CODE
                                         << Constants::ATC_EN
                                         << Constants::ATC_FR,
                                         where);
        QSqlQuery query(req, QSqlDatabase::database(Constants::DB_IAM_NAME));
        QList<AtcItem *> list;
        if (query.isActive()) {
            while (query.next()) {
                AtcItem *item = new AtcItem();
                item->setData(AtcTreeModel::ATC_Code, query.value(0));
                item->setData(AtcTreeModel::ATC_EnglishLabel, query.value(1));
                item->setData(AtcTreeModel::ATC_FrenchLabel, query.value(2));
                list.append(item);
            }
        } else {
            Utils::Log::addError(q, query.lastError().text(),__FILE__, __LINE__);
        }
        query.finish();

        AtcItem *last = 0;
        AtcItem *lastOne = 0;
        AtcItem *lastThree = 0;
        AtcItem *lastFour = 0;
        AtcItem *lastFive = 0;
        AtcItem *parent = 0;
        foreach(AtcItem *item, list) {
            switch (item->code().count())
            {
            case 1: parent = m_Root;     lastOne = item; break;
            case 3: parent = lastOne;    lastThree = item; break;
            case 4: parent = lastThree;  lastFour = item;  break;
            case 5: parent = lastFour;   lastFive = item;  break;
            case 7: parent = lastFive;    break;
            }
            // need to be reparented
            if (!parent) {
                qWarning() << item->code() << "no parent";
            } else {
                item->setParent(parent);
                parent->addChildren(item);
            }
            last = item;
            parent = 0;
        }
        q->reset();
    }

    AtcItem *getItem(const QModelIndex &index) const
    {
        if (index.isValid()) {
            AtcItem *item = static_cast<AtcItem *>(index.internalPointer());
            if (item)
                return item;
        }
        return m_Root;
    }

public:
    AtcItem *m_Root;

private:
    AtcTreeModel *q;
};
}  // End namespace Internal
}  // End namespace DrugsDB


AtcTreeModel::AtcTreeModel(QObject *parent) :
        QAbstractItemModel(parent),
        d(new Internal::AtcTreeModelPrivate(this))
{
}

AtcTreeModel::~AtcTreeModel()
{}

void AtcTreeModel::init()
{
    d->getTree();
}

QModelIndex AtcTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

//     if (!parent.isValid())
//         return QModelIndex();

    Internal::AtcItem *parentItem = d->getItem(parent);
    Internal::AtcItem *childItem = 0;
    childItem = parentItem->child(row);

    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex AtcTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    Internal::AtcItem *childItem = d->getItem(index);
    Internal::AtcItem *parentItem = childItem->parent();

    if (parentItem == d->m_Root)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int AtcTreeModel::rowCount(const QModelIndex &parent) const
{
    Internal::AtcItem *item = d->getItem(parent);
    if (item) {
        return item->childCount();
    }
    return 0;
}

int AtcTreeModel::columnCount(const QModelIndex &) const
{
    return 2; // code and label
}

QVariant AtcTreeModel::data(const QModelIndex & item, int role) const
{
    if (!item.isValid())
        return QVariant();

    const Internal::AtcItem *it = d->getItem(item);

    switch (role)
    {
    case Qt::EditRole :
    case Qt::DisplayRole :
        {
            return it->data(item.column());
        }
    case Qt::ToolTipRole :
        {
            return it->english();
        }
    case Qt::ForegroundRole :
        {
//            if (it->isTemplate()) {
//                return QColor(settings()->value(Constants::S_FOREGROUND_TEMPLATES, "#000").toString());
//            } else {
//                return QColor(settings()->value(Constants::S_FOREGROUND_CATEGORIES, "darkblue").toString());
//            }
            break;
        }
    case Qt::BackgroundRole :
        {
//            QColor c;
//            if (it->isTemplate()) {
//                c = QColor(settings()->value(Constants::S_BACKGROUND_TEMPLATES, "white").toString());
//            } else {
//                c = QColor(settings()->value(Constants::S_BACKGROUND_CATEGORIES, "white").toString());
//            }
//            if (Utils::isDebugCompilation()) {
//                if (it->isNewlyCreated()) {
//                    c = QColor(Qt::blue);
//                } else
//                    if (it->isModified()) {
//                    c = QColor(Qt::red);
//                }
//            }
//            if (c.name()=="#ffffff")
//                return QVariant();
//            c.setAlpha(125);
//            return c;
            break;
        }
    }
    return QVariant();
}

Qt::ItemFlags AtcTreeModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QVariant AtcTreeModel::headerData(int section, Qt::Orientation orientation, int) const
{
     if (orientation == Qt::Horizontal) {
        if (section==0)
            return tr("Label");
        if (section==1)
            return tr("Code");
    }
    return QVariant();
}
