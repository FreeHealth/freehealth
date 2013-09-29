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
 *  This program is distributed in the hope that it will be useful, *
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
/**
 * \class eDRC::CrTreeModel
 * Provides a basic CR tree read-only model.
 */

#include "crtreemodel.h"
#include <edrcplugin/edrccore.h>
#include <edrcplugin/consultresult.h>
#include <edrcplugin/database/constants_db.h>
#include <edrcplugin/database/edrcbase.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_datetime.h>

using namespace eDRC;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline eDRC::EdrcCore &edrcCore() {return eDRC::EdrcCore::instance();}
static inline eDRC::Internal::DrcDatabase &edrcBase() {return eDRC::EdrcCore::instance().edrcBase();}

namespace eDRC {
namespace Internal {
class CrTreeModelPrivate
{
public:
    CrTreeModelPrivate(CrTreeModel *parent) :
        q(parent)
    {}

    QList<QStandardItem *> crToItem(const ConsultResult &cr, int listIndex)
    {
        QList<QStandardItem *> list;
        for(int i = 0; i < CrTreeModel::ColumnCount; ++i) {
            QStandardItem *item = new QStandardItem(" ");
            list << item;
            _itemToListIndex.insert(item, listIndex);
        }
        list[CrTreeModel::Label]->setText(edrcBase().getCrLabel(cr.consultResultId()));
        list[CrTreeModel::Label]->setToolTip(edrcCore().toHtml(cr));
        list[CrTreeModel::DateOfExamination]->setText(QLocale().toString(cr.dateOfExamination(), QLocale::ShortFormat));
        list[CrTreeModel::Validity]->setText(cr.isValid()?tkTr(Trans::Constants::ISVALID):tkTr(Trans::Constants::ISNOTVALID));
        list[CrTreeModel::DiagnosisPosition]->setText(cr.diagnosisPositionToHumanReadable());
        list[CrTreeModel::MedicalFollowUp]->setText(cr.medicalFollowUpToHumanReadable());
//        list[CrTreeModel::Html]->setText(edrcCore().toHtml(cr));
        list[CrTreeModel::Id]->setText(QString::number(cr.consultResultId()));
        list[CrTreeModel::Empty1]->setText("");
        list[CrTreeModel::Empty2]->setText("");
        list[CrTreeModel::Empty3]->setText("");
        return list;
    }

    void createTree()
    {
        _itemToListIndex.clear();
        // Sort list by date of examination
        qSort(_list.begin(), _list.end(), ConsultResult::lessThanByDate);

        QDate currentDate;
        QStandardItem *dateBranch = 0;

        // Add the historic item
        QList<QStandardItem *> history;
        history << new QStandardItem(tkTr(Trans::Constants::HISTORY));
        for(int i = 1; i < CrTreeModel::ColumnCount; ++i) {
            history << new QStandardItem;
        }
        q->invisibleRootItem()->appendRow(history);

        // Manage the tree
        for(int i = 0; i < _list.count(); ++i) {
            const ConsultResult &cr = _list.at(i);
            if (currentDate.isNull()
                    && cr.dateOfExamination().date().isNull()
                    && !dateBranch) {
                dateBranch = new QStandardItem(tkTr(Trans::Constants::NO_DATE));
                QList<QStandardItem *> branch;
                branch << dateBranch;
                for(int i = 1; i < CrTreeModel::ColumnCount; ++i) {
                    branch << new QStandardItem;
                }
                q->invisibleRootItem()->appendRow(branch);
            } else if (currentDate != cr.dateOfExamination().date()) {
                // New date branch
                currentDate = cr.dateOfExamination().date();
                dateBranch = new QStandardItem(QLocale().toString(currentDate, QLocale::LongFormat));
                QList<QStandardItem *> branch;
                branch << dateBranch;
                for(int i = 1; i < CrTreeModel::ColumnCount; ++i) {
                    branch << new QStandardItem;
                }
                branch[CrTreeModel::DateOfExamination]->setData(QDateTime(currentDate, QTime(0,0,0)), Qt::DisplayRole);
                q->invisibleRootItem()->appendRow(branch);
            }
            // Add the CR to the current date branch
            dateBranch->appendRow(crToItem(cr, i));
        }
    }

public:
    QList<ConsultResult> _list;
    QHash<QStandardItem*, int> _itemToListIndex;

private:
    CrTreeModel *q;
};
} // namespace Internal
} // namespace eDRC

CrTreeModel::CrTreeModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new CrTreeModelPrivate(this))
{
    setObjectName("CrTreeModel");
}

CrTreeModel::~CrTreeModel()
{
    if (d)
        delete d;
    d = 0;
}


/**
 * Populate the model with the selected \e list of CR.
 * The model will be automatically resetted and sorted.
 */
bool CrTreeModel::setCrList(const QList<ConsultResult> &list)
{
    beginResetModel();
    d->_list = list;
    clear();
    d->createTree();
    endResetModel();
    return true;
}

/**
 * Add the \e cr to the model list of CR.
 * The model will be automatically resetted and sorted.
 */
bool CrTreeModel::addConsultResult(const ConsultResult &cr)
{
    beginResetModel();
    d->_list << cr;
    clear();
    d->createTree();
    endResetModel();
    return true;
}

void CrTreeModel::updateConsultResult(const QModelIndex &crIndex, const ConsultResult &crToUpdate)
{}

bool CrTreeModel::removeItems(const QModelIndex &index)
{
    // History can not be removed
    if (isHistoryIndex(index))
        return false;

    // Remove a single CR
    if (isConsultResult(index)) {
        int listId = d->_itemToListIndex.value(itemFromIndex(index), -1);
        if (listId == -1) {
            LOG_ERROR("No id?");
            return false;
        }
        // Remove cache
        d->_list.removeAt(listId);
        for(int i=0; i < ColumnCount; ++i) {
            QStandardItem *item = itemFromIndex(this->index(index.row(), i, index.parent()));
            d->_itemToListIndex.remove(item);
        }
        // Remove index in model
        removeRow(index.row(), index.parent());
        if (rowCount(index.parent()) == 0) {
            QModelIndex parent = index.parent();
            removeRow(parent.row(), parent.parent());
        }
        return true;
    }

    // Remove a date-branch
    for(int i=0; i < rowCount(index); ++i) {
        QModelIndex cr = this->index(i, Label, index);
        int listId = d->_itemToListIndex.value(itemFromIndex(cr), -1);
        if (listId == -1)
            continue;
        d->_list.removeAt(listId);
        for(int i=0; i < ColumnCount; ++i) {
            QStandardItem *item = itemFromIndex(this->index(cr.row(), i, cr.parent()));
            d->_itemToListIndex.remove(item);
        }
        removeRow(cr.row(), cr.parent());
        if (rowCount(cr.parent()) == 0) {
            QModelIndex parent = cr.parent();
            removeRow(parent.row(), parent.parent());
        }
    }

    return true;
}

bool CrTreeModel::isConsultResult(const QModelIndex &index) const
{
    return index.parent().isValid();
}

bool CrTreeModel::isHistoryIndex(const QModelIndex &index) const
{
    return (index.row() == 0 && index.parent() == QModelIndex());
}

QString CrTreeModel::htmlContent(const QModelIndex &index) const
{
    // Return history?
    if (isHistoryIndex(index)) {
        QString html;
        foreach(const ConsultResult &cr, d->_list) {
            html += edrcCore().toHtml(cr);
        }
        return html;
    }

    // Return only a CR
    if (isConsultResult(index)) {
        int listId = d->_itemToListIndex.value(itemFromIndex(index), -1);
        if (listId == -1)
            return QString::null;
        return edrcCore().toHtml(d->_list.at(listId));
    }

    // Return a history from a date
    QString html;
    for(int i=0; i < rowCount(index); ++i) {
        QModelIndex cr = this->index(i, Label, index);
        int listId = d->_itemToListIndex.value(itemFromIndex(cr), -1);
        if (listId == -1)
            continue;
        html += edrcCore().toHtml(d->_list.at(listId));
    }
    return html;
}

const QList<ConsultResult> &CrTreeModel::consultResultList() const
{
    return d->_list;
}
