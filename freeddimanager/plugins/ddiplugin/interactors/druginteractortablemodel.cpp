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
#include <QSqlRecord>

#include <QDebug>

using namespace DDI;
using namespace Internal;

static inline DDI::Internal::DDIDatabase &ddiBase() {return DDI::DDICore::instance()->database();}

namespace DDI {
namespace Internal {
class DrugInteractorTableModelPrivate
{
public:
    DrugInteractorTableModelPrivate(DrugInteractorTableModel *parent) :
        _sql(0),
        q(parent)
    {
        Q_UNUSED(q);
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
    QStringList _distinctUids;

private:
    DrugInteractorTableModel *q;
};
}  // End namespace Internal
}  // End namespace DDI

/** Table Model over the database interactor table. The model is uid sorted */
DrugInteractorTableModel::DrugInteractorTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::DrugInteractorTableModelPrivate(this))
{
    setObjectName("DrugInteractorTableModel");
    d->_sql = new QSqlTableModel(this, ddiBase().database());
    d->_sql->setTable(ddiBase().table(Constants::Table_INTERACTORS));
    d->_sql->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d->_sql->setSort(Constants::INTERACTOR_UID, Qt::AscendingOrder);
    Utils::linkSignalsFromFirstModelToSecondModel(d->_sql, this, true);
    connect(d->_sql, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(populateNewRowWithDefault(int, QSqlRecord&)));
}

DrugInteractorTableModel::~DrugInteractorTableModel()
{
    if (d)
        delete d;
    d = 0;
}

/** Initialize the model (fetch all interactors from database). */
bool DrugInteractorTableModel::initialize()
{
    // Select
    d->_sql->select();

    // Fetch all data in the source model
    while (d->_sql->canFetchMore(index(d->_sql->rowCount(), 0)))
        d->_sql->fetchMore(index(d->_sql->rowCount(), 0));

    d->_distinctUids = ddiBase().interactorDistinctUids();

    return true;
}

int DrugInteractorTableModel::rowCount(const QModelIndex &parent) const
{
    return d->_sql->rowCount(parent);
}

int DrugInteractorTableModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

void DrugInteractorTableModel::fetchMore(const QModelIndex &parent)
{
    d->_sql->fetchMore(parent);
}

bool DrugInteractorTableModel::canFetchMore(const QModelIndex &parent) const
{
    return d->_sql->canFetchMore(parent);
}

QVariant DrugInteractorTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        int sql = -1;
        switch (index.column()) {
        case Id: sql = Constants::INTERACTOR_ID; break;
        case Uuid: sql = Constants::INTERACTOR_UID; break;
        case IsValid: sql = Constants::INTERACTOR_ISVALID; break;
        case IsInteractingClass: sql = Constants::INTERACTOR_ISCLASS; break;
        case IsReviewed: sql = Constants::INTERACTOR_ISREVIEWED; break;
        case IsAutoFound: sql = Constants::INTERACTOR_ISAUTOFOUND; break;
        case DoNotWarnDuplicated: sql = Constants::INTERACTOR_WARNDUPLICATES; break;
        case TranslatedLabel:
        {
            switch (QLocale().language()) {
            case QLocale::French: sql = Constants::INTERACTOR_FR; break;
            case QLocale::English: sql = Constants::INTERACTOR_EN; break;
            case QLocale::German: sql = Constants::INTERACTOR_DE; break;
            default: sql = Constants::INTERACTOR_EN; break;
            }
            break;
        }
        case EnLabel: sql = Constants::INTERACTOR_EN; break;
        case FrLabel: sql = Constants::INTERACTOR_FR; break;
        case DeLabel: sql = Constants::INTERACTOR_DE; break;
        case ClassInformationFr: sql = Constants::INTERACTOR_INFO_FR; break;
        case ClassInformationEn: sql = Constants::INTERACTOR_INFO_EN; break;
        case ClassInformationDe: sql = Constants::INTERACTOR_INFO_DE; break;
        case ATCCodeStringList: sql = Constants::INTERACTOR_ATC; break;
        case DateOfCreation: sql = Constants::INTERACTOR_DATECREATE; break;
        case DateLastUpdate: sql = Constants::INTERACTOR_DATEUPDATE; break;
        case DateReview: sql = Constants::INTERACTOR_DATEREVIEW; break;
        case PMIDStringList: sql = Constants::INTERACTOR_PMIDS; break;
        case ChildrenUuid: sql = Constants::INTERACTOR_CHILDREN; break;
        case Reference: sql = Constants::INTERACTOR_REF; break;
        case Comment: sql = Constants::INTERACTOR_COMMENT; break;
        };

        QModelIndex sqlIndex = d->_sql->index(index.row(), sql);

        return d->_sql->data(sqlIndex, role);
    } else if (role == Qt::FontRole) {
        QModelIndex sqlIndex = d->_sql->index(index.row(), Constants::INTERACTOR_ISCLASS);
        if (d->_sql->data(sqlIndex).toBool()) {
            QFont bold;
            bold.setBold(true);
            return bold;
        }
    } else if (role==Qt::ForegroundRole) {
        QModelIndex classIndex = d->_sql->index(index.row(), Constants::INTERACTOR_ISCLASS);
        QModelIndex childIndex = d->_sql->index(index.row(), Constants::INTERACTOR_CHILDREN);
        //QModelIndex atcIndex = d->_sql->index(index.row(), Constants::INTERACTOR_ATC);
        bool isClass = d->_sql->data(classIndex).toBool();
        const QVariant &children = d->_sql->data(childIndex);
        //const QVariant &atc = d->_sql->data(atcIndex);
        if (isClass) {
            // Class without children?
            if (children.isNull()) {
                qWarning() << "DrugInteractor: class without children" << d->_sql->index(index.row(), Constants::INTERACTOR_FR).data();
                return QColor(255,50,50,150);
            }
            // Children without ATC code
            //                    return QColor(255,50,50,150);
            // TODO: here
        } else {
            // Children without being a class?
            if (!children.isNull()) {
                qWarning() << "DrugInteractor: children defined without isclass" << d->_sql->index(index.row(), Constants::INTERACTOR_FR).data();
                qWarning() << children;
                return QColor(255,50,50,150);
            }
        }
        // Not reviewed?
        QModelIndex rev = d->_sql->index(index.row(), Constants::INTERACTOR_ISREVIEWED);
        if (!d->_sql->data(rev).toBool()) {
//            qWarning() << "not reviewed" << d->_sql->index(index.row(), Constants::INTERACTOR_FR).data();
            return QColor(50,250,50,150);
        }
        // If class, all children reviewed?
    }
    return QVariant();
}

bool DrugInteractorTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    int sql = -1;
    if (role == Qt::EditRole) {
        switch (index.column()) {
        case Id: sql = Constants::INTERACTOR_ID; break;
        case Uuid: sql = Constants::INTERACTOR_UID; break;
        case IsValid: sql = Constants::INTERACTOR_ISVALID; break;
        case IsInteractingClass: sql = Constants::INTERACTOR_ISCLASS; break;
        case IsReviewed: sql = Constants::INTERACTOR_ISREVIEWED; break;
        case IsAutoFound: sql = Constants::INTERACTOR_ISAUTOFOUND; break;
        case DoNotWarnDuplicated: sql = Constants::INTERACTOR_WARNDUPLICATES; break;
        case TranslatedLabel:
        {
            switch (QLocale().language()) {
            case QLocale::French: sql = Constants::INTERACTOR_FR; break;
            case QLocale::English: sql = Constants::INTERACTOR_EN; break;
            case QLocale::German: sql = Constants::INTERACTOR_DE; break;
            default: sql = Constants::INTERACTOR_EN; break;
            }
            break;
        }
        case EnLabel: sql = Constants::INTERACTOR_EN; break;
        case FrLabel: sql = Constants::INTERACTOR_FR; break;
        case DeLabel: sql = Constants::INTERACTOR_DE; break;
        case ClassInformationFr: sql = Constants::INTERACTOR_INFO_FR; break;
        case ClassInformationEn: sql = Constants::INTERACTOR_INFO_EN; break;
        case ClassInformationDe: sql = Constants::INTERACTOR_INFO_DE; break;
        case ATCCodeStringList: sql = Constants::INTERACTOR_ATC; break;
        case DateOfCreation: sql = Constants::INTERACTOR_DATECREATE; break;
        case DateLastUpdate: sql = Constants::INTERACTOR_DATEUPDATE; break;
        case DateReview: sql = Constants::INTERACTOR_DATEREVIEW; break;
        case PMIDStringList: sql = Constants::INTERACTOR_PMIDS; break;
        case ChildrenUuid: sql = Constants::INTERACTOR_CHILDREN; break;
        case Reference: sql = Constants::INTERACTOR_REF; break;
        case Comment: sql = Constants::INTERACTOR_COMMENT; break;
        };

        bool ok = false;
        QModelIndex sqlIndex = d->_sql->index(index.row(), sql);

        switch (index.column()) {
        case IsValid:
        case IsInteractingClass:
        case IsReviewed:
        case IsAutoFound:
        case DoNotWarnDuplicated:
            ok = d->_sql->setData(sqlIndex, value.toBool()?1:0, role);
            break;
        default: ok = d->_sql->setData(sqlIndex, value, role); break;
        }
        return ok;
    }
    return false;
}

bool DrugInteractorTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return d->_sql->insertRows(row, count, parent);
}

bool DrugInteractorTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        return false;
    beginRemoveRows(parent, row, row+count);
    for(int i = 0; i < count; ++i)
        d->_sql->setData(d->_sql->index(row+i, IsValid), 0);
    endRemoveRows();
    return true;
}

QVariant DrugInteractorTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Id: return tr("Id");
        case Uuid: return tr("Uuid");
        case IsValid: return tr("Is valid");
        case IsInteractingClass: return tr("Is interaction class");
        case IsReviewed: return tr("Is reviewed");
        case IsAutoFound: return tr("Is auto-found");
        case DoNotWarnDuplicated: return tr("DoNotWarnDuplicated");
        case TranslatedLabel: return tr("Translated label");
        case EnLabel: return tr("English label");
        case FrLabel: return tr("French label");
        case DeLabel: return tr("Deustch label");
        case ClassInformationFr: return tr("Class information (french)");
        case ClassInformationEn: return tr("Class information (english)");
        case ClassInformationDe: return tr("Class information (deustch)");
        case ATCCodeStringList: return tr("ATC codes");
        case DateOfCreation: return tr("Date of creation");
        case DateLastUpdate: return tr("Date of update");
        case DateReview: return tr("Date of review");
        case PMIDStringList: return tr("PMID");
        case ChildrenUuid: return tr("Children");
        case Reference: return tr("Reference");
        case Comment: return tr("Comment");
        }
    }
    return QVariant();
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
    return 0;
}

/** Creates a new interactor and store it in the model. */
bool DrugInteractorTableModel::createInteractor(const QString &initialLabel)
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
    return true;
}

/** Returns the number of unreviewed DDI::DrugInteractor from the database. */
int DrugInteractorTableModel::numberOfUnreviewed() const
{
    // directly ask the database instead of screening the model
    QHash<int, QString> where;
    where.insert(Constants::INTERACTOR_ISREVIEWED, "=0");
    return ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where));
}

/** Returns the number of ATC unlinked DDI::DrugInteractor from the database. */
int DrugInteractorTableModel::numberOfUnlinked() const
{
    // directly ask the database instead of screening the model
    QHash<int, QString> where;
    where.insert(Constants::INTERACTOR_ISCLASS, "=0");
    where.insert(Constants::INTERACTOR_ATC, "IS NULL");
    return ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where));
}

/**
 * Returns \e true if the interactor with the \e uid exists.
 */
bool DrugInteractorTableModel::interactorUidExists(const QString &uid) const
{
    return d->_distinctUids.contains(uid);
}

/** Submit changes directly to the database */
bool DrugInteractorTableModel::submit()
{
    bool ok = d->_sql->submitAll();
    if (ok)
        d->_sql->database().commit();
    return ok;
}

/** React at primeInsert() signal, populate the newly created row with the default values */
void DrugInteractorTableModel::populateNewRowWithDefault(int row, QSqlRecord &record)
{
    Q_UNUSED(row);
    record.clearValues();
    for(int i = 0; i < d->_sql->columnCount(); ++i) {
        record.setGenerated(i, true);
        record.setValue(i, QVariant());
    }
    // We need to force the INTERACTOR_ID in the record (we can not let the db chose the ID value)
    record.setValue(Constants::INTERACTOR_ID, ddiBase().max(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID).toInt() + 1);
    record.setValue(Constants::INTERACTOR_UID, Utils::createUid());
    record.setValue(Constants::INTERACTOR_ISVALID, 1);
    record.setValue(Constants::INTERACTOR_ISREVIEWED, 0);
    record.setValue(Constants::INTERACTOR_ISAUTOFOUND, 0);
    record.setValue(Constants::INTERACTOR_ISCLASS, 0);
    record.setValue(Constants::INTERACTOR_WARNDUPLICATES, 0);
    record.setValue(Constants::INTERACTOR_DATECREATE, QDate::currentDate());
    record.setValue(Constants::INTERACTOR_DATEUPDATE, QDate::currentDate());
}
