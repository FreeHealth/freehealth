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
#include "bankaccountmodel.h"
#include "accountbase.h"
#include "constants.h"

#include <utils/log.h>
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <QSqlTableModel>

using namespace AccountDB;
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }
namespace AccountDB {
namespace Internal {

class BankAccountModelPrivate
{
public:
    BankAccountModelPrivate(BankAccountModel *parent) : m_SqlTable(0), m_IsDirty(false), q(parent)
    {
        m_SqlTable = new QSqlTableModel(q, QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        m_SqlTable->setTable(AccountDB::AccountBase::instance()->table(Constants::Table_BankDetails));
    }
    ~BankAccountModelPrivate () {}

public:
    QSqlTableModel *m_SqlTable;
    bool m_IsDirty;

private:
    BankAccountModel *q;
};

}  // End namespace Internal
}  // End namespace AccountDB



BankAccountModel::BankAccountModel(QObject *parent) : QAbstractTableModel(parent), 
                                                      m_UserUid(user()->uuid()),
                                                      d(new Internal::BankAccountModelPrivate(this))
{
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    setUserUuid(m_UserUid);
}

BankAccountModel::~BankAccountModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int BankAccountModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return d->m_SqlTable->rowCount(parent);
}

int BankAccountModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}

void BankAccountModel::setUserUuid(const QString &uuid)
{
    QHash<int, QString> where;
    where.insert(Constants::BANKDETAILS_USER_UID, QString("='%1'").arg(uuid));
    d->m_SqlTable->setFilter(AccountBase::instance()->getWhereClause(Constants::Table_BankDetails, where));
    d->m_SqlTable->select();
}

QVariant BankAccountModel::data(const QModelIndex &index, int role) const
{
    return d->m_SqlTable->data(index, role);
}

bool BankAccountModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = d->m_SqlTable->setData(index, value, role);
    d->m_IsDirty = d->m_SqlTable->isDirty(index);
    return ret;
}


QVariant BankAccountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return d->m_SqlTable->headerData(section,orientation,role);
}

bool BankAccountModel::insertRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
    bool ret = d->m_SqlTable->insertRows(row, count, parent);
    return ret;
}

bool BankAccountModel::removeRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
    return d->m_SqlTable->removeRows(row, count, parent);
}

bool BankAccountModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        d->m_IsDirty = false;
        return true;
    }
    return false;
}

void BankAccountModel::revert()
{
    d->m_IsDirty = false;
    d->m_SqlTable->revert();
}

bool BankAccountModel::isDirty() const
{
    return d->m_IsDirty;
}

void BankAccountModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
    d->m_SqlTable->select();
}

QString BankAccountModel::filter(){
    return d->m_SqlTable->filter();
}
