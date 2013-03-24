/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developers:                                                      *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *   Contributors:                                                         *
 *       Christian A. Reiter <christian.a.reiter@gmail.com>                *
 ***************************************************************************/
#include <QSqlTableModel>

#include "medicalproceduremodel.h"
#include "database/accountbase.h"
#include "database/constants_db.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <account2plugin/database/constants_db.h>
#include <account2plugin/accountcore.h>
#include <account2plugin/accountitems.h>
#include <account2plugin/database/accountbase.h>

#include <translationutils/constanttranslations.h>

/*!
 * \class Account2::MedicalProcedureModel
 *
 * The MedicalProcedureModel provides an editable data model
 * for the "Medical Procedures" that are used in the accounting.
 *
 * It is a high-level interface for editing database records of Medical
 * Procedures. It internally uses a SqlTableModel to save and recieve the
 * data from the database.
 */



using namespace Account2;
using namespace Internal;
using namespace Trans::ConstantTranslations;
using namespace Constants;

enum { WarnDebugMessage = false };

static inline Account2::AccountCore *accountCore() {return Account2::AccountCore::instance();}
static inline Account2::Internal::AccountBase *accountBase() {return Account2::AccountCore::instance()->accountBase();}

namespace Account2 {
namespace Internal {

class MedicalProcedureModelPrivate
{
public:
    MedicalProcedureModelPrivate(MedicalProcedureModel *parent) :
        _sql(0),
        m_isDirty(false),
        q(parent)
    {
        _sql = new QSqlTableModel(q, QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        _sql->setTable(accountBase()->table(Constants::Table_MedicalProcedure));
        _sql->setEditStrategy(QSqlTableModel::OnManualSubmit);
    }

    ~MedicalProcedureModelPrivate ()
    {}

    void refreshMedicalProceduresFromDatabase(const BasicFilter &filter)
    {
        AccountBaseQuery query(filter);
        query.setRetrieveObjects(AccountBaseQuery::MedicalProcedure);
        AccountBaseResult result = accountBase()->query(query);
        _medicalProcedures = result.medicalProcedures();
    }

    QSqlTableModel *_sql;
    QList<MedicalProcedure> _medicalProcedures;
    bool m_isDirty;

private:
    MedicalProcedureModel *q;
};


}  // End namespace Internal
}  // End namespace AccountDB


MedicalProcedureModel::MedicalProcedureModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::MedicalProcedureModelPrivate(this))
{
}

MedicalProcedureModel::~MedicalProcedureModel()
{
    if (d) {
        delete d;
        d = 0;
    }
}

int MedicalProcedureModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(parent == QModelIndex());
    return d->_sql->rowCount();
}

int MedicalProcedureModel::columnCount(const QModelIndex &parent) const
{
    Q_ASSERT(parent == QModelIndex());
    return d->_sql->columnCount(parent);
}

QVariant MedicalProcedureModel::data(const QModelIndex &index, int role) const
{
    return d->_sql->data(index, role);
}

bool MedicalProcedureModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = d->_sql->setData(index, value, role);
    d->m_isDirty = d->_sql->isDirty(index);
    return ret;
}

QVariant MedicalProcedureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return d->_sql->headerData(section, orientation, role);
}

bool MedicalProcedureModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return d->_sql->setHeaderData(section, orientation, value, role )  ;
}

bool MedicalProcedureModel::insertRows(int row, int count, const QModelIndex &parent)
{
    //    d->m_isDirty = true;
    return true;
}

bool MedicalProcedureModel::removeRows(int row, int count, const QModelIndex &parent)
{
//    d->m_IsDirty = true;

    return true;

//    while(d->_sql->canFetchMore())
//        d->_sql->fetchMore();
//    return d->_sql->removeRows(row, count, parent);
}

void MedicalProcedureModel::setFilter(const QString & filter)
{
    d->_sql->setFilter(filter);
    //d->m_SqlTable->select();
}

QString MedicalProcedureModel::filter()
{
    return d->_sql->filter();
}

bool MedicalProcedureModel::submit()
{
    bool success = d->_sql->submit();
    if (success)
        d->m_isDirty = false;
    return success;
}

void MedicalProcedureModel::revert()
{
    d->m_isDirty = false;
    d->_sql->revert();
}

bool MedicalProcedureModel::isDirty() const
{
    return d->m_isDirty;
}

QSqlError MedicalProcedureModel::lastError()
{
    return d->_sql->lastError();
}

void MedicalProcedureModel::clear()
{
    d->_sql->clear();
    d->m_isDirty = false;
}

bool MedicalProcedureModel::select()
{
    return d->_sql->select();
}
