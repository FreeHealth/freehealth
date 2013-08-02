/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "movementmodel.h"
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

class MovementModelPrivate
{
public:
    MovementModelPrivate(MovementModel *parent) : m_SqlTable(0), m_IsDirty(false), q(parent)
    {
        m_SqlTable = new QSqlTableModel(q, QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        m_SqlTable->setTable(AccountDB::AccountBase::instance()->table(Constants::Table_Movement));
    }
    ~MovementModelPrivate () {}

public:
    QSqlTableModel *m_SqlTable;
    bool m_IsDirty;

private:
    MovementModel *q;
};

}  // End namespace Internal
}  // End namespace AccountDB



MovementModel::MovementModel(QObject *parent) :
        QAbstractTableModel(parent),
        m_UserUid(user()->uuid()),
        d(new Internal::MovementModelPrivate(this))
{
    //d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    setUserUuid(m_UserUid);
}

MovementModel::~MovementModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int MovementModel::rowCount(const QModelIndex &parent) const
{
    // prevent trees
    if (parent.isValid()) return 0;

    return d->m_SqlTable->rowCount(parent);
}

int MovementModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}

void MovementModel::setUserUuid(const QString &uuid)
{
    QHash<int, QString> where;
    where.insert(Constants::MOV_USER_UID, QString("='%1'").arg(uuid));
    d->m_SqlTable->setFilter(AccountBase::instance()->getWhereClause(Constants::Table_Movement, where));
    d->m_SqlTable->select();
}

QVariant MovementModel::data(const QModelIndex &index, int role) const
{
    return d->m_SqlTable->data(index, role);
}

bool MovementModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = d->m_SqlTable->setData(index, value, role);
    d->m_IsDirty = d->m_SqlTable->isDirty(index);
    return ret;
}


QVariant MovementModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return d->m_SqlTable->headerData(section,orientation,role);
}

bool MovementModel::setHeaderData(int section,
                                  Qt::Orientation orientation,
                                  const QVariant & value,
                                  int role){
    bool ret = true;
    ret = d->m_SqlTable->setHeaderData(section,orientation,value,role);
    return ret;
}

bool MovementModel::insertRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnRowChange);
    bool ret = d->m_SqlTable->insertRows(row, count, parent);
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    return ret;
}

bool MovementModel::removeRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
    bool ret = d->m_SqlTable->removeRows(row, count, parent);
    if (ret == true)
    {
    	  ret = d->m_SqlTable->submitAll();
        }
    else
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to submit removeRows" ;
        }
    return ret;
}

bool MovementModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        d->m_IsDirty = false;
        return true;
    }
    return false;
}

void MovementModel::revert()
{
    d->m_IsDirty = false;
    d->m_SqlTable->revert();
}

bool MovementModel::isDirty() const
{
    return d->m_IsDirty;
}

QSqlError MovementModel::lastError(){
    return d->m_SqlTable->lastError();
}

void MovementModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
    d->m_SqlTable->select();
}

QString MovementModel::filter(){
    return d->m_SqlTable->filter();
}

bool MovementModel::canFetchMore ( const QModelIndex & parent  ) const  {
    return d->m_SqlTable->canFetchMore ( parent  );
}

void MovementModel::fetchMore ( const QModelIndex & parent ) {
    d->m_SqlTable->fetchMore ( parent  );
}


void MovementModel::clearFilters(){
    QString filter = QString();
    d->m_SqlTable->setFilter(filter);
    d->m_SqlTable->select();
}
bool MovementModel::select(){
    return d->m_SqlTable->select();
}

void MovementModel::setDatesBeginEndAndUserFilter(QDateTime &start, QDateTime &end, const QString &uuid ){
    QString filter;
    filter = QString("%1='%2'").arg("USER_UID",uuid);
    filter += " AND ";
    filter += QString("DATE BETWEEN '%1' AND '%2'").arg(start.toString("yyyy-MM-dd"),end.toString("yyyy-MM-dd"));
    setFilter(filter);

}
