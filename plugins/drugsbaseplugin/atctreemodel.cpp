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
#include "atctreemodel.h"

#include <drugsbaseplugin/drugbasecore.h>
#include <drugsbaseplugin/drugsbase.h>
#include <drugsbaseplugin/drugsbaseinfo.h>
#include <drugsbaseplugin/constants.h>

#include <coreplugin/translators.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QLocale>

#include <QDebug>

using namespace DrugsDB;
using namespace Trans::ConstantTranslations;

static inline Core::Translators *translators() {return Core::ICore::instance()->translators();}
static inline DrugsDB::DrugsBase &drugsBase() {return DrugsDB::DrugBaseCore::instance().drugsBase();}

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

    void setData(const int id, const QVariant &value) {m_Data.insert(id, value.toString());}

    // Access to data
    QString data(const int id) const { return m_Data.value(id); }

    QString code() const {return m_Data.value(AtcTreeModel::ATC_Code);}

    // For sort functions
    static bool lessThan(AtcItem *item1, AtcItem *item2)
    {
        return item1->code() < item2->code();
    }

private:
    AtcItem *m_Parent;
    QList<AtcItem*> m_Children;
    QHash<int, QString> m_Data;
};


class AtcTreeModelPrivate
{
public:
    AtcTreeModelPrivate(AtcTreeModel *parent) :
            m_Root(0), q(parent)
    {
    }

    ~AtcTreeModelPrivate()
    {
        delete m_Root;
    }

    void getTree()
    {
        if (!drugsBase().actualDatabaseInformation() || !drugsBase().actualDatabaseInformation()->atcCompatible) {
            q->beginResetModel();
            if (m_Root)
                delete m_Root;
            m_Root = 0;
            q->endResetModel();
            return;
        }

        if (m_Language==QLocale().name().left(2)) {
//            qWarning() << "atc tree not rebuilded";
            return;
        }

        q->beginResetModel();
        // ATC tree is available for en, fr and de
        m_Language = QLocale().name().left(2);
        if (!(m_Language=="fr" || m_Language=="de" || m_Language=="en"))
            m_Language = "en";
        Utils::FieldList get;
        get     << Utils::Field(Constants::Table_ATC, Constants::ATC_CODE)
                << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LABEL);
        Utils::JoinList joins;
        joins   << Utils::Join(Constants::Table_ATC_LABELS, Constants::ATC_LABELS_ATCID, Constants::Table_ATC, Constants::ATC_ID)
                << Utils::Join(Constants::Table_LABELSLINK, Constants::LABELSLINK_MASTERLID, Constants::Table_ATC_LABELS, Constants::ATC_LABELS_MASTERLID)
                << Utils::Join(Constants::Table_LABELS, Constants::LABELS_LID, Constants::Table_LABELSLINK, Constants::LABELSLINK_LID);
        Utils::FieldList cond;
        cond    << Utils::Field(Constants::Table_LABELS, Constants::LABELS_LANG, QString("='%1'").arg(m_Language));
        cond    << Utils::Field(Constants::Table_ATC, Constants::ATC_ID, " < 100000");
        QSqlQuery query(drugsBase().select(get, joins, cond), QSqlDatabase::database(Constants::DB_DRUGS_NAME));
        QList<AtcItem *> list;
        if (query.isActive()) {
            while (query.next()) {
                AtcItem *item = new AtcItem();
                item->setData(AtcTreeModel::ATC_Code, query.value(0));
                item->setData(AtcTreeModel::ATC_Label, query.value(1));
                list.append(item);
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
        }
        query.finish();

        if (m_Root) {
            delete m_Root;
            m_Root = 0;
        }
        m_Root = new AtcItem;
        m_Root->setData(0, "ATC_ROOT_ITEM");

//        AtcItem *last = 0;
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
//            last = item;
            parent = 0;
        }
        q->endResetModel();
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
    QString m_Language;

private:
    AtcTreeModel *q;
};
}  // End namespace Internal
}  // End namespace DrugsDB


AtcTreeModel::AtcTreeModel(QObject *parent) :
        QAbstractItemModel(parent),
        d(new Internal::AtcTreeModelPrivate(this))
{
    connect(&drugsBase(), SIGNAL(drugsBaseHasChanged()), this, SLOT(onDrugsBaseChanged()));
    connect(translators(), SIGNAL(languageChanged()), this, SLOT(init()));
}

AtcTreeModel::~AtcTreeModel()
{
    if (d)
        delete d;
    d = 0;
}

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
            return tkTr(Trans::Constants::_1_COLON_2).arg(it->data(ATC_Label)).arg(it->data(ATC_Code));
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

void AtcTreeModel::onDrugsBaseChanged()
{
    d->m_Language.clear();
    d->getTree();
}
