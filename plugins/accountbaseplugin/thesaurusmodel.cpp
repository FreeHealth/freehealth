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
#include "thesaurusmodel.h"
#include "constants.h"
#include "accountbase.h"

#include <translationutils/constanttranslations.h>

#include <QAbstractTableModel>
#include <QSqlTableModel>

using namespace AccountDB;

using namespace Trans::ConstantTranslations;


namespace AccountDB {
namespace Internal {
class ThesaurusModelPrivate
{
public:
    ThesaurusModelPrivate(ThesaurusModel *parent) : m_SqlTable(0), q(parent)
    {
        m_SqlTable = new QSqlTableModel(q,QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
    }

    ~ThesaurusModelPrivate () {}

public:
    QSqlTableModel *m_SqlTable;


private:
    ThesaurusModel *q;
};
}
}



ThesaurusModel::ThesaurusModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::ThesaurusModelPrivate(this))
{
    d->m_SqlTable->setTable(AccountBase::instance()->table(Constants::Table_Thesaurus));
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    //    d->m_SqlTable->setFilter( user );
    d->m_SqlTable->select();
}

ThesaurusModel::~ThesaurusModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int ThesaurusModel::rowCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->rowCount(parent);
}

int ThesaurusModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}


QVariant ThesaurusModel::data(const QModelIndex &index, int role) const
{
    return d->m_SqlTable->data(index, role);
}

bool ThesaurusModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return d->m_SqlTable->setData(index, value, role);
}


QVariant ThesaurusModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

bool ThesaurusModel::insertRows(int row, int count, const QModelIndex &parent)
{
      bool ret = d->m_SqlTable->insertRows(row, count, parent);
      return ret;
}

bool ThesaurusModel::removeRows(int row, int count, const QModelIndex &parent)
{
      bool ret = d->m_SqlTable->removeRows(row, count, parent);
      return ret;
}

bool ThesaurusModel::isDirty() const
{
    return true;
}

bool ThesaurusModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        return true;
    }
    return false;
}

void ThesaurusModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
}

bool ThesaurusModel::select(){
    return d->m_SqlTable->select();
}


