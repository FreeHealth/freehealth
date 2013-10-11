/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "druginteractortablemodel.h"
#include <ddiplugin/ddicore.h>
#include <ddiplugin/constants.h>
#include <ddiplugin/database/ddidatabase.h>
#include <ddiplugin/interactors/druginteractor.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QFont>
#include <QLocale>
#include <QDate>
#include <QColor>
#include <QSqlTableModel>

#include <QDebug>

using namespace DDI;
using namespace Internal;

static inline DDI::Internal::DDIDatabase *ddiBase() {return DDI::DDICore::instance()->database();}

namespace DDI {
namespace Internal {
class DrugInteractorTableModelPrivate
{
public:
    DrugInteractorTableModelPrivate(DrugInteractorTableModel *parent) :
        _sql(0),
        q(parent)
    {
    }

    ~DrugInteractorTableModelPrivate()
    {
    }

    // Reconstruct the complete tree
//    void filter(const QString &interactor = QString::null)
//    {
//        delete rootItem;
//        rootItem = new DITreeItem(0,0);
//        QFont bold;
//        bold.setBold(true);
//        QHash<QString, DITreeItem *> categories;
//        if (interactor.isEmpty()) {

//            // first pass == create categories
//            if (m_ShowData == DrugInteractorTableModel::InteractingClasses ||
//                    m_ShowData == DrugInteractorTableModel::All) {
//                for(int i=0; i < m_interactors.count(); ++i) {
//                    DrugInteractor *di = m_interactors.at(i);
//                    if (!di->isValid())
//                        continue;
//                    // if is class -> create a category
//                    if (di->isClass()) {
//                        if (!categories.keys().contains(di->data(DrugInteractor::InitialLabel).toString())) {
//                            DITreeItem *cat = new DITreeItem(di, rootItem);
//                            categories.insert(di->data(DrugInteractor::InitialLabel).toString(), cat);
//                        }
//                    }
//                }
//            }

//            // second pass = add interactors to categories or rootItem
//            for(int i=0; i < m_interactors.count(); ++i) {
//                DrugInteractor *di = m_interactors.at(i);
//                if (!di->isValid())
//                    continue;
//                // add molecule item to root
//                if (!di->isClass() &&
//                    (m_ShowData == DrugInteractorTableModel::All || m_ShowData==DrugInteractorTableModel::InteractingMolecules)) {
//                    new DITreeItem(di, rootItem);
//                }

//                // find parent class
//                foreach(const QString &id, di->parentIds()) {
//                    DITreeItem *cat = categories.value(id, 0);
//                    if (!cat)
//                        continue;
//                    new DITreeItem(di, cat);
//                }
//            }
//        }
//        rootItem->sortChildren();
//    }

//    QString getTooltip(DITreeItem *item)
//    {
//        QStringList msg;
//        DrugInteractor *di = item->di();
//        msg << QString("<b><u>UID:</b></u><br />&nbsp;&nbsp;%1").arg(di->data(DrugInteractor::InitialLabel).toString());

//        // check errors
//        QStringList errors;
//        if (!di->isClass() && di->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty()) {
//            errors << QString("Missing ATC link");
//        }
//        if (di->isClass()) {
//            for(int i=0; i < item->childCount();++i) {
//                if (item->child(i)->di()->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty()) {
//                    errors << QString("One child at least is missing ATC link");
//                }
//            }
//        }
//        if (!di->isReviewed()) {
//            errors << QString("Item is not reviewed");
//        } else if (di->isClass()) {
//            for(int i=0; i < item->childCount();++i) {
//                if (!item->child(i)->di()->isReviewed())
//                    errors << QString("One child at least is not reviewed");
//            }
//        }
//        if (errors.count())
//            msg << QString("<span style=\"color:#FF2020\"><b><u>Errors:</b></u><br />&nbsp;&nbsp;* %1</span>").arg(errors.join("<br />&nbsp;&nbsp;* "));


//        // get related DDI
//        QStringList directDdis;
//        QMultiHash<QString, QString> parentDdis;  // key=parent value=second interactor
//        for(int i=0; i < m_ddis.count(); ++i) {
//            // direct ddis
//            DrugDrugInteraction *ddi = m_ddis.at(i);
//            const QString &id = di->data(DrugInteractor::InitialLabel).toString();
//            if (ddi->firstInteractor()==id) {
//                directDdis << QString("&nbsp;&nbsp;* %1 (%2)")
//                       .arg(ddi->data(DrugDrugInteraction::SecondInteractorName).toString())
//                       .arg(ddi->data(DrugDrugInteraction::LevelName).toString());
//            } else if (ddi->secondInteractor()==id) {
//                directDdis << QString("&nbsp;&nbsp;* %1 (%2)")
//                        .arg(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())
//                        .arg(ddi->data(DrugDrugInteraction::LevelName).toString());
//            }
//            // parent ddis
//            if (di->parentIds().contains(ddi->firstInteractor())) {
//                parentDdis.insertMulti(ddi->firstInteractor(),
//                                       QString("%1 (%2)")
//                                       .arg(ddi->secondInteractor())
//                                       .arg(ddi->levelName()));
//            } else if (di->parentIds().contains(ddi->secondInteractor())) {
//                parentDdis.insertMulti(ddi->data(DrugDrugInteraction::SecondInteractorName).toString(),
//                                       QString("%1 (%2)")
//                                       .arg(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())
//                                       .arg(ddi->data(DrugDrugInteraction::LevelName).toString()));
//            }
//        }
//        QString ddisText;
//        if (directDdis.count()) {
//            ddisText = QString("<span style=\"color:#AA10AA\"><b><u>DDI (own):</u></b><br />%1").arg(directDdis.join("<br />"));
//        } // <br /><span style=\"color:#FF2020\"><b><u>DDI:</b></u><br />%1</span>
//        if (parentDdis.count()) {
//            ddisText = QString("<span style=\"color:#AA10AA\"><b><u>DDI (from parents):</u></b><br />");
//            foreach(const QString &parent, parentDdis.uniqueKeys()) {
//                const QStringList &l = parentDdis.values(parent);
//                ddisText += QString("&nbsp;&nbsp;<b>* %1</b><br/>&nbsp;&nbsp;&nbsp;&nbsp;* %2<br/>").arg(parent).arg(l.join("<br/>&nbsp;&nbsp;&nbsp;&nbsp;* "));
//            }
//        }
//        if (!ddisText.isEmpty()) {
//            ddisText += "</span>";
//            msg << ddisText;
//        }

//        // TODO: get pims */

//        return msg.join("<br />");
//    }

//    void onInteractorCreated()
//    {
//        filter();
//        q->reset();
//    }


public:
    QSqlTableModel *_sql;
    DrugInteractorTableModel::ShowData m_ShowData;

private:
    DrugInteractorTableModel *q;
};
}  // End namespace Internal
}  // End namespace DDI


DrugInteractorTableModel::DrugInteractorTableModel(ShowData show, QObject *parent) :
    QSqlTableModel(parent),
    d(new Internal::DrugInteractorTableModelPrivate(this))
{
    setObjectName("DrugInteractorTableModel");
    d->_sql = new QSqlTableModel(this, ddiBase()->database());
    d->_sql->setTable(ddiBase()->table(Constants::Table_Interactors));
    d->_sql->select();
}

DrugInteractorTableModel::~DrugInteractorTableModel()
{
    if (d)
        delete d;
    d = 0;
}

int DrugInteractorTableModel::rowCount(const QModelIndex &parent) const
{
    return d->_sql->rowCount(parent);
}

int DrugInteractorTableModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant DrugInteractorTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

//    DrugInteractor *di = item->di();
//    if (!di) {
//        return QVariant();
//    }

//    if (role == Qt::DisplayRole || role == Qt::EditRole) {
//        switch (index.column()) {
//        case Uuid: return di->data(DrugInteractor::InitialLabel);
//        case TrLabel : return di->data(DrugInteractor::TranslatedLabel);
//        case EnLabel : return di->data(DrugInteractor::EnLabel);
//        case FrLabel : return di->data(DrugInteractor::FrLabel);
//        case DeLabel : return di->data(DrugInteractor::DeLabel);
//        case EsLabel : return di->data(DrugInteractor::EsLabel);
//        case IsInteractingClass : return di->data(DrugInteractor::IsClass);
//        case DoNotWarnDuplicated: return di->data(DrugInteractor::DoNotWarnDuplicated);
//        case ClassInformationFr : return di->data(DrugInteractor::ClassInformationFr).toString();
//        case ClassInformationEn : return di->data(DrugInteractor::ClassInformationEn).toString();
//        case ClassInformationDe : return di->data(DrugInteractor::ClassInformationDe).toString();
//        case ATCCodeStringList : return di->data(DrugInteractor::ATCCodeStringList);
//        case IsReviewed : return di->data(DrugInteractor::IsReviewed);
//        case DateOfCreation : return di->data(DrugInteractor::DateOfCreation);
//        case DateLastUpdate : return di->data(DrugInteractor::DateLastUpdate);
//        case IsAutoFound : return di->data(DrugInteractor::IsAutoFound);
//        case Comment : return di->data(DrugInteractor::Comment);
//        case PMIDStringList : return di->data(DrugInteractor::PMIDsStringList);
//        case ChildrenUuid: return di->childrenIds();
//        case Reference: return di->data(DrugInteractor::Reference);
//        } // End switch
//    } else if (role == Qt::ToolTipRole) {
//        return d->getTooltip(item);
//    } else if (role==Qt::ForegroundRole) {
//        if (!di->isClass() && di->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty()) {
//            return QColor(255,50,50,150);
//        }
//        if (di->isClass()) {
//            for(int i=0; i < item->childCount();++i) {
//                if (item->child(i)->di()->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty()) {
//                    return QColor(255,50,50,150);
//                }
//            }
//        }
//        if (!di->isReviewed()) {
//            return QColor(50,250,50,150);
//        } else if (di->isClass()) {
//            for(int i=0; i < item->childCount();++i) {
//                if (!item->child(i)->di()->isReviewed())
//                    return QColor(50,255,50,150);
//            }
//        }
//    } else if (role == Qt::DecorationRole) {
////        if (item->childCount() == 0)
////            return ddi->levelIcon();
//    } else if (role == Qt::FontRole) {
//        if (di->isClass()) {
//            QFont bold;
//            bold.setBold(true);
//            return bold;
//        }
//    }
    return QVariant();
}

bool DrugInteractorTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
//    DITreeItem *item = d->getItem(index);
//    if (!item) {
//        return false;
//    }

//    DrugInteractor *di = item->di();
//    if (!di || index.column()==0) {
//        return false;
//    }

//    if (role==Qt::EditRole) {
//        switch (index.column()) {
//        case EnLabel :
//        {
//            di->setData(DrugInteractor::EnLabel, value);
//            QModelIndex trLab = this->index(index.row(), TrLabel, index.parent());
//            Q_EMIT dataChanged(trLab, trLab);
//            break;
//        }
//        case FrLabel :
//        {
//            di->setData(DrugInteractor::FrLabel, value);
//            QModelIndex trLab = this->index(index.row(), TrLabel, index.parent());
//            Q_EMIT dataChanged(trLab, trLab);
//            break;
//        }
//        case DeLabel :
//        {
//            di->setData(DrugInteractor::DeLabel, value);
//            QModelIndex trLab = this->index(index.row(), TrLabel, index.parent());
//            Q_EMIT dataChanged(trLab, trLab);
//            break;
//        }
//        case EsLabel :
//        {
//            di->setData(DrugInteractor::EsLabel, value);
//            QModelIndex trLab = this->index(index.row(), TrLabel, index.parent());
//            Q_EMIT dataChanged(trLab, trLab);
//            break;
//        }
//        case IsInteractingClass : di->setData(DrugInteractor::IsClass, value); break;
//        case DoNotWarnDuplicated: di->setData(DrugInteractor::DoNotWarnDuplicated, value); break;
//        case ClassInformationFr : di->setData(DrugInteractor::ClassInformationFr, value.toString().replace("\"", "'")); break;
//        case ClassInformationEn : di->setData(DrugInteractor::ClassInformationEn, value.toString().replace("\"", "'")); break;
//        case ClassInformationDe : di->setData(DrugInteractor::ClassInformationDe, value.toString().replace("\"", "'")); break;
//        case ATCCodeStringList :
//            di->setData(DrugInteractor::ATCCodeStringList, value);
//            Q_EMIT unlinkedCountChanged();
//            break;
//        case IsReviewed :
//            di->setData(DrugInteractor::IsReviewed, value);
//            Q_EMIT unreviewedCountChanged();
//            break;
//        case DateOfCreation : di->setData(DrugInteractor::DateOfCreation, value); break;
//        case DateLastUpdate : di->setData(DrugInteractor::DateLastUpdate, value); break;
//        case IsAutoFound : di->setData(DrugInteractor::IsAutoFound, value); break;
//        case Comment : di->setData(DrugInteractor::Comment, value); break;
//        case PMIDStringList : di->setData(DrugInteractor::PMIDsStringList, value); break;
//        case ChildrenUuid:
//        {
//            QStringList list = value.toStringList();
//            list.sort();
//            di->setChildId(list);
//            // TODO: update DI parent */
//            beginRemoveRows(index, 0, rowCount(index));
//            item->clearChildren();
//            endRemoveRows();
//            beginInsertRows(index, 0, list.count());
//            foreach(const QString &childId, list) {
//                // find the related interactor
//                for(int i = 0; i < d->m_interactors.count(); ++i) {
//                    DrugInteractor *__di = d->m_interactors.at(i);
//                    if (__di->data(DrugInteractor::InitialLabel).toString()==childId.toUpper()) {
//                        new DITreeItem(__di, item);
//                        if (!__di->parentIds().contains(childId, Qt::CaseInsensitive)) {
//                            __di->addParentId(di->data(DrugInteractor::InitialLabel).toString());
//                        }
//                        break;
//                    }
//                }
//            }
//            endInsertRows();
//            break;
//        }
//        case Reference: di->setData(DrugInteractor::Reference, value); break;

//        default:
//            return false;
//        }
//        // DateLastUpdate -> automatic
//        di->setData(DrugInteractor::DateLastUpdate, QDate::currentDate());
//        QModelIndex date = this->index(index.row(), DateLastUpdate, index.parent());
//        QModelIndex trLabel = this->index(index.row(), TrLabel, index.parent());
//        Q_EMIT dataChanged(index, index);
//        Q_EMIT dataChanged(date, date);
//        Q_EMIT dataChanged(trLabel, trLabel);
//        if (item->parent()!=d->rootItem) {
//            trLabel = this->index(index.parent().row(), TrLabel, index.parent().parent());
//            Q_EMIT dataChanged(trLabel, trLabel);
//        }
//        return true;
//    }

    return false;
}

bool DrugInteractorTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // insert row to categories only
//    if (!parent.isValid())
//        return false;
//    DITreeItem *parentItem = d->getItem(parent);
//    if (parentItem->childCount()==0)  // parent is an interactor not a category
//        return false;
//    beginInsertRows(parent, row, row+count);
//    for(int i = 0; i < count; ++i) {
//        DrugInteractor *di = new DrugInteractor;
//        d->m_interactors.append(di);
//        di->setData(DrugInteractor::Id, core()->createInternalUuid());
//        DITreeItem *item = new DITreeItem(di, parentItem);
//        parentItem->removeChild(item);
//        parentItem->insertChild(row+i, item);
//    }
//    endInsertRows();
    return true;
}

// todo remove mirrored
bool DrugInteractorTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // only remove rows with a category parent
//    if (!parent.isValid())
//        return false;
//    beginRemoveRows(parent, row, row+count);
//    DITreeItem *parentItem = d->getItem(parent);
//    for(int i = 0; i < count; ++i) {
////        const QString &interactor = parentItem->child(row+i)->ddi()->data(DrugDrugInteraction::IsValid, false);
//        parentItem->child(row+i)->di()->setData(DrugInteractor::IsValid, false);
//        parentItem->removeChild(row+i);
//        // remove mirrored
//    }
//    endRemoveRows();
    return true;
}

Qt::ItemFlags DrugInteractorTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    return f;
}

/**
 * Creates a new drug interacting class and store it in the model.
 * In case of success, return the created DrugsDB::DrugInteractor pointer
 * else return 0.
 */
DrugInteractor *DrugInteractorTableModel::createInteractingClass(const QString &initialLabel)
{
//    for(int i=0; i < d->m_interactors.count(); ++i) {
//        if (d->m_interactors.at(i)->data(DrugInteractor::InitialLabel).toString() == initialLabel.toUpper()) {
//            return 0;
//        }
//    }
//    DrugInteractor *di = new DrugInteractor;
//    di->setData(DrugInteractor::IsValid, true);
//    di->setData(DrugInteractor::InitialLabel, Utils::removeAccents(initialLabel.toUpper()));
//    di->setData(DrugInteractor::FrLabel, initialLabel.toUpper());
//    di->setData(DrugInteractor::IsClass, true);
//    di->setData(DrugInteractor::DateOfCreation, QDate::currentDate());
//    di->setData(DrugInteractor::IsDuplicated, false);
//    d->m_interactors.append(di);
//    d->onInteractorCreated();
//    return di;
}

/**
 * Creates a new interactor and store it in the model.
 * In case of success, return the created DrugsDB::DrugInteractor pointer
 * else return 0.
 */
DrugInteractor *DrugInteractorTableModel::createInteractor(const QString &initialLabel)
{
//    for(int i=0; i < d->m_interactors.count(); ++i) {
//        if (d->m_interactors.at(i)->data(DrugInteractor::InitialLabel).toString() == initialLabel.toUpper()) {
//            return 0;
//        }
//    }
//    DrugInteractor *di = new DrugInteractor;
//    di->setData(DrugInteractor::IsValid, true);
//    di->setData(DrugInteractor::InitialLabel, Utils::removeAccents(initialLabel.toUpper()));
//    di->setData(DrugInteractor::FrLabel, initialLabel.toUpper());
//    di->setData(DrugInteractor::IsClass, false);
//    di->setData(DrugInteractor::DateOfCreation, QDate::currentDate());
//    di->setData(DrugInteractor::IsDuplicated, false);
//    d->m_interactors.append(di);
//    d->onInteractorCreated();
//    return di;
}

int DrugInteractorTableModel::numberOfUnreviewed() const
{
//    int result = 0;
//    for(int i=0; i < d->m_interactors.count();++i) {
//        if (d->m_interactors.at(i)->isValid() && !d->m_interactors.at(i)->isReviewed())
//            ++result;
//    }
//    return result;
    return 0;
}

int DrugInteractorTableModel::numberOfUnlinked() const
{
//    int result = 0;
//    for(int i=0; i < d->m_interactors.count();++i) {
//        if (d->m_interactors.at(i)->isValid() && d->m_interactors.at(i)->data(DrugInteractor::ATCCodeStringList).toStringList().isEmpty())
//            ++result;
//    }
//    return result;
    return 0;
}

bool DrugInteractorTableModel::submit()
{
//    core()->saveCompleteList(d->m_interactors);
    return true;
}
