/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers : Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#include "depositmodel.h"
#include "accountbase.h"
#include "constants.h"

#include <utils/log.h>

#include <QSqlTableModel>

using namespace AccountDB;

namespace AccountDB {
namespace Internal {

class DepositModelPrivate
{
public:
    DepositModelPrivate(DepositModel *parent) : m_SqlTable(0), m_IsDirty(false), q(parent)
    {
        m_SqlTable = new QSqlTableModel(q, QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        m_SqlTable->setTable(AccountDB::AccountBase::instance()->table(Constants::Table_Deposit));
//        m_SqlTable->setFilter(USER_UID);
    }
    ~DepositModelPrivate () {}

public:
    QSqlTableModel *m_SqlTable;
    bool m_IsDirty;

private:
    DepositModel *q;
};

}  // End namespace Internal
}  // End namespace AccountDB



DepositModel::DepositModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::DepositModelPrivate(this))
{
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    d->m_SqlTable->select();
}

DepositModel::~DepositModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int DepositModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return d->m_SqlTable->rowCount(parent);
}

int DepositModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}

void DepositModel::setUserUuid(const QString &uuid)
{
    QHash<int, QString> where;
    where.insert(Constants::BANKDETAILS_USER_UID, QString("='%1'").arg(uuid));
    d->m_SqlTable->setFilter(AccountBase::instance()->getWhereClause(Constants::Table_Deposit, where));
}

QVariant DepositModel::data(const QModelIndex &index, int role) const
{
    return d->m_SqlTable->data(index, role);
}

bool DepositModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = d->m_SqlTable->setData(index, value, role);
    d->m_IsDirty = d->m_SqlTable->isDirty(index);
    return ret;
}


QVariant DepositModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

bool DepositModel::insertRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnRowChange);
    bool ret = d->m_SqlTable->insertRows(row, count, parent);
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    return ret;
}

bool DepositModel::removeRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
    return d->m_SqlTable->removeRows(row, count, parent);
}

bool DepositModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        d->m_IsDirty = false;
        return true;
    }
    return false;
}

void DepositModel::revert()
{
    d->m_IsDirty = false;
    d->m_SqlTable->revert();
}

bool DepositModel::isDirty() const
{
    return d->m_IsDirty;
}
