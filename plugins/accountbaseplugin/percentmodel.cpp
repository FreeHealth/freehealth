/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "percentmodel.h"
#include "accountbase.h"
#include "constants.h"

#include <utils/log.h>
#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <QSqlTableModel>

using namespace AccountDB;

enum {WarnFilter=false};

static inline AccountDB::AccountBase *accountBase() {return AccountDB::AccountBase::instance();}
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }

namespace AccountDB {
namespace Internal {

class PercentModelPrivate
{
public:
    PercentModelPrivate(PercentModel *parent) : m_SqlTable(0),
                                                m_UserUid(user()->uuid()),
                                                m_IsDirty(false),
                                                q(parent)
    {
        if(WarnFilter)
        qDebug() << __FILE__ << QString::number(__LINE__) << " m_UserUid =  " << m_UserUid;
        m_SqlTable = new QSqlTableModel(q, QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        m_SqlTable->setTable(AccountDB::AccountBase::instance()->table(Constants::Table_Percent));
        refreshFilter();
    }
    ~PercentModelPrivate () {}

    void refreshFilter()
    {
        if (!m_SqlTable)
            return;
        QHash<int, QString> where;
        where.insert(AccountDB::Constants::PERCENT_USER_UID, QString("='%1'").arg(m_UserUid));
        m_SqlTable->setFilter(accountBase()->getWhereClause(Constants::Table_Percent, where));
        if (WarnFilter)
            qWarning() << m_SqlTable->filter() << __FILE__ << __LINE__;
        //q->reset();
    }

public:
    QSqlTableModel *m_SqlTable;
    QString m_UserUid;
    bool m_IsDirty;

private:
    PercentModel *q;
};

}  // End namespace Internal
}  // End namespace AccountDB



PercentModel::PercentModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::PercentModelPrivate(this))
{
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    d->m_SqlTable->select();
}

PercentModel::~PercentModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int PercentModel::rowCount(const QModelIndex &parent) const
{
    int rows = 0;
    d->m_SqlTable->setFilter("");
    d->m_SqlTable->select();
    rows = d->m_SqlTable->rowCount(parent);
    return rows;
}

int PercentModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}

/*void PercentModel::setUserUuid(const QString &uuid)
{
    QHash<int, QString> where;
    where.insert(Constants::PERCENT_USER_UID, QString("='%1'").arg(uuid));
    d->m_SqlTable->setFilter(AccountBase::instance()->getWhereClause(Constants::Table_Percent, where));
}*/

QVariant PercentModel::data(const QModelIndex &index, int role) const
{
    return d->m_SqlTable->data(index, role);
}

bool PercentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = d->m_SqlTable->setData(index, value, role);
    d->m_IsDirty = d->m_SqlTable->isDirty(index);
    return ret;
}


QVariant PercentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

bool PercentModel::setHeaderData(int section, Qt::Orientation orientation,
                                          const QVariant &value, int role ){
    return d->m_SqlTable->setHeaderData(section, orientation,value,role )  ;
}

bool PercentModel::insertRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnRowChange);
    bool ret = d->m_SqlTable->insertRows(row, count, parent);
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    return ret;
}

bool PercentModel::removeRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
    return d->m_SqlTable->removeRows(row, count, parent);
}

bool PercentModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        d->m_IsDirty = false;
        return true;
    }
    return false;
}

void PercentModel::revert()
{
    d->m_IsDirty = false;
    d->m_SqlTable->revert();
}

bool PercentModel::isDirty() const
{
    return d->m_IsDirty;
}

QSqlError PercentModel::lastError(){
    return d->m_SqlTable->lastError();
}
