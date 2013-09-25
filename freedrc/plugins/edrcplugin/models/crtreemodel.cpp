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

using namespace eDRC;
using namespace Internal;

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
        QStandardItem *item = new QStandardItem;
        list << item;
        return item;
    }

    void createTree()
    {
        // Sort list by date of examination
        qSort(_list.begin(), _list.end(), ConsultResult::lessThanByDate);

        QDate currentDate;
        QStandardItem *dateBranch;

        // Add the historic item
        QStandardItem *historic;

        // Manage the tree
        for(int i = 0; i < _list.count(); ++i) {
            const ConsultResult &cr = _list.at(i);
            if (currentDate != cr.dateOfExamination()) {
                // New date branch
            }
            // Add the CR to the current date branch
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
    QAbstractTableModel(parent),
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
    d->createTree();
    endResetModel();
}

void CrTreeModel::updateConsultResult(const QModelIndex &crIndex, const ConsultResult &crToUpdate)
{}

const QList<ConsultResult> &CrTreeModel::consultResultList() const
{
    return d->_list;
}
