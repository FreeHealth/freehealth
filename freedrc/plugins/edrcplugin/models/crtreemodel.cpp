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

    QList<QStandardItem *> crToItem(const ConsultResult &cr)
    {
        QList<QStandardItem *> list;
        for(int i = 0; i < CrTreeModel::ColumnCount; ++i) {
            list << new QStandardItem("-----");
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
            if (currentDate.isNull() && cr.dateOfExamination().isNull() && !dateBranch) {
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
                q->invisibleRootItem()->appendRow(branch);
            }
            // Add the CR to the current date branch
            dateBranch->appendRow(crToItem(cr));
        }
    }

public:
    QList<ConsultResult> _list;

private:
    CrTreeModel *q;
};
} // namespace Internal
} // namespace eDRC

CrTreeModel::CrTreeModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new CrTreeModelPrivate(this))
{
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
void CrTreeModel::setCrList(const QList<ConsultResult> &list)
{
    beginResetModel();
    d->_list = list;
    clear();
    d->createTree();
    endResetModel();
}

/**
 * Add the \e cr to the model list of CR.
 * The model will be automatically resetted and sorted.
 */
void CrTreeModel::addConsultResult(const ConsultResult &cr)
{
    beginResetModel();
    d->_list << cr;
    clear();
    d->createTree();
    endResetModel();
}

void CrTreeModel::updateConsultResult(const QModelIndex &crIndex, const ConsultResult &crToUpdate)
{}

const QList<ConsultResult> &CrTreeModel::consultResultList() const
{
    return d->_list;
}
