/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre, GP               *
 *   pm.desombre@medsyn.fr                                                 *
 *   original from Eric Maeker, MD.                                        *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>                     *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "insurancemodel.h"
#include "constants.h"
#include "accountbase.h"

#include <translationutils/constanttranslations.h>

#include <QAbstractTableModel>
#include <QSqlTableModel>

#include <QDebug>



using namespace AccountDB;

using namespace Trans::ConstantTranslations;


namespace AccountDB {
namespace Internal {
class InsuranceModelPrivate
{
public:
    InsuranceModelPrivate(InsuranceModel *parent) : m_SqlTable(0), q(parent)
    {
        m_SqlTable = new QSqlTableModel(q,QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        m_SqlTable->setTable(AccountBase::instance()->table(Constants::Table_Insurance));
    }

    ~InsuranceModelPrivate () {}

public:
    QSqlTableModel *m_SqlTable;


private:
    InsuranceModel *q;
};
}
}



InsuranceModel::InsuranceModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::InsuranceModelPrivate(this))
{
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    d->m_SqlTable->select();
}

InsuranceModel::~InsuranceModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int InsuranceModel::rowCount(const QModelIndex &parent) const
{
    int rows = 0;
    d->m_SqlTable->setFilter("");
    d->m_SqlTable->select();
    rows = d->m_SqlTable->rowCount(parent);
    return rows;
}

int InsuranceModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}


QVariant InsuranceModel::data(const QModelIndex &index, int role) const
{
    QVariant data ;
    if(!index.isValid()){
       qWarning() << __FILE__ << QString::number(__LINE__) << "index is not valid" ;
       data = QVariant();
       }
    if(role == Qt::DisplayRole){
        data = d->m_SqlTable->data(index, Qt::DisplayRole);
        }	
    if (role == Qt::EditRole)
    {
    	data = d->m_SqlTable->data(index, Qt::EditRole);
        }
    return data;
}

bool InsuranceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return d->m_SqlTable->setData(index, value, role);
}


QVariant InsuranceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

bool InsuranceModel::insertRows(int row, int count, const QModelIndex &parent)
{
      bool ret = d->m_SqlTable->insertRows(row, count, parent);
      return ret;
}

bool InsuranceModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return d->m_SqlTable->removeRows(row, count, parent);
}

bool InsuranceModel::isDirty() const
{
    return true;
}

bool InsuranceModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        return true;
    }
    return false;
}

void  InsuranceModel::revert()
{
    d->m_SqlTable->revert();
}

QSqlError InsuranceModel::lastError(){
    return d->m_SqlTable->lastError();
}

void InsuranceModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
    d->m_SqlTable->select();
}

