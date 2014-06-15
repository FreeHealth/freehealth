/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "availablemovementmodel.h"
#include "constants.h"
#include "accountbase.h"

#include <translationutils/constanttranslations.h>

#include <QAbstractTableModel>
#include <QSqlTableModel>

using namespace AccountDB;
using namespace Trans::ConstantTranslations;

static inline AccountDB::AccountBase *accountBase() {return AccountDB::AccountBase::instance();}

namespace AccountDB {
namespace Internal {
class AvailableMovementModelPrivate
{
public:
    AvailableMovementModelPrivate(AvailableMovementModel *parent) : m_SqlTable(0), q(parent)
    {
    }

    ~AvailableMovementModelPrivate () {}

public:
    QSqlTableModel *m_SqlTable;


private:
    AvailableMovementModel *q;
};
}
}



AvailableMovementModel::AvailableMovementModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::AvailableMovementModelPrivate(this))
{
    d->m_SqlTable = new QSqlTableModel(this, accountBase()->database());
    d->m_SqlTable->setTable(accountBase()->table(Constants::Table_AvailableMovement));
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    //    d->m_SqlTable->setFilter( user );
    d->m_SqlTable->select();
}

AvailableMovementModel::~AvailableMovementModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int AvailableMovementModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return d->m_SqlTable->rowCount(parent);
}

int AvailableMovementModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}


QVariant AvailableMovementModel::data(const QModelIndex &index, int role) const
{
    return d->m_SqlTable->data(index, role);
}

bool AvailableMovementModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return d->m_SqlTable->setData(index, value, role);
}


QVariant AvailableMovementModel::headerData(int section, Qt::Orientation orientation, int role) const{
   return d->m_SqlTable->headerData(section,orientation,role);
}

bool AvailableMovementModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool b = d->m_SqlTable->insertRows(row, count, parent);
    return b;
}

bool AvailableMovementModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool b = d->m_SqlTable->removeRows(row, count, parent);
    return b;
}

bool AvailableMovementModel::isDirty() const
{
    return true;
}

void AvailableMovementModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
    d->m_SqlTable->select();
}

bool AvailableMovementModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        return true;
    }
    return false;
}

QSqlError AvailableMovementModel::lastError(){
    return d->m_SqlTable->lastError();
}

