/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#include "medicalproceduremodel.h"
#include "accountbase.h"
#include "constants.h"
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QSqlTableModel>
#include <QUuid>

using namespace AccountDB;
using namespace Trans::ConstantTranslations;

enum {WarnFilter=true};

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline AccountDB::AccountBase *accountBase() {return AccountDB::AccountBase::instance();}
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }

namespace AccountDB {
namespace Internal {

class MedicalProcedureModelPrivate
{
public:
    MedicalProcedureModelPrivate(MedicalProcedureModel *parent) : m_SqlTable(0), 
                                                                  m_IsDirty(false),
                                                                  m_UserUid(user()->value(Core::IUser::Uuid).toString()),
                                                                  q(parent)
    {
        m_SqlTable = new QSqlTableModel(q, QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        m_SqlTable->setTable(AccountDB::AccountBase::instance()->table(Constants::Table_MedicalProcedure));
        refreshFilter();
    }
    ~MedicalProcedureModelPrivate () {}
    
    void refreshFilter()
    {
        if (!m_SqlTable)
            return;
        QHash<int, QString> where;
        where.insert(AccountDB::Constants::MP_USER_UID, QString("='%1'").arg(m_UserUid));
        if (!m_TypeFilter.isEmpty()) {
            where.insert(AccountDB::Constants::MP_TYPE, QString("='%1'").arg(m_TypeFilter));
        }
        m_SqlTable->setFilter(accountBase()->getWhereClause(Constants::Table_MedicalProcedure, where));
        if (WarnFilter){
            qWarning() << m_SqlTable->filter() << __FILE__ << __LINE__;
            }
        //q->reset();
     }

public:
    QSqlTableModel *m_SqlTable;
    bool m_IsDirty;
    QString m_UserUid, m_TypeFilter;

private:
    MedicalProcedureModel *q;
};

}  // End namespace Internal
}  // End namespace AccountDB



MedicalProcedureModel::MedicalProcedureModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::MedicalProcedureModelPrivate(this))
{
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);
    d->m_SqlTable->select();
}

MedicalProcedureModel::~MedicalProcedureModel()
{
    if (d) {
        delete d;
        d=0;
    }
}


int MedicalProcedureModel::rowCount(const QModelIndex &parent) const
{ 
    int rows = 0;
    d->m_SqlTable->setFilter("");
    d->m_SqlTable->select();
    rows = d->m_SqlTable->rowCount(parent);
    //qDebug() << __FILE__ << QString::number(__LINE__) << " rows = " << QString::number(rows);

    return rows;
}

int MedicalProcedureModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}

QStringList MedicalProcedureModel::distinctAvailableType() const
{
    QStringList toReturn;
    QSqlQuery query(accountBase()->database());
    query.exec(accountBase()->selectDistinct(AccountDB::Constants::Table_MedicalProcedure, AccountDB::Constants::MP_TYPE));
    if (query.isActive()) {
        while (query.next()) {
            toReturn << query.value(0).toString();
        }
    } else {
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
    }
    query.finish();
    return toReturn;
}

void MedicalProcedureModel::setUserUuid(const QString &uuid)
{
    QHash<int, QString> where;
    where.insert(Constants::BANKDETAILS_USER_UID, QString("='%1'").arg(uuid));
    d->m_SqlTable->setFilter(AccountBase::instance()->getWhereClause(Constants::Table_MedicalProcedure, where));
}

QVariant MedicalProcedureModel::data(const QModelIndex &index, int role) const
{   
    return d->m_SqlTable->data(index, role);
}

bool MedicalProcedureModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = d->m_SqlTable->setData(index, value, role);
    d->m_IsDirty = d->m_SqlTable->isDirty(index);
    return ret;
}


QVariant MedicalProcedureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

bool MedicalProcedureModel::setHeaderData(int section,Qt::Orientation orientation,
                                          QVariant & value,int role ){
    return d->m_SqlTable->setHeaderData(section, orientation,value,role )  ;                                   
}

bool MedicalProcedureModel::insertRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnRowChange);
    bool ok = true;
    for(int i = 0 ; i < count ; ++i) {
//        QSqlQuery query(accountBase()->database());
//        query.prepare(accountBase()->prepareInsertQuery(AccountDB::Constants::Table_MedicalProcedure));
//        query.bindValue(AccountDB::Constants::MP_ABSTRACT, QVariant());
//        query.bindValue(AccountDB::Constants::MP_AMOUNT, QVariant());
//        query.bindValue(AccountDB::Constants::MP_DATE, QVariant());
//        query.bindValue(AccountDB::Constants::MP_ID, QVariant());
//        query.bindValue(AccountDB::Constants::MP_NAME, QVariant());
//        query.bindValue(AccountDB::Constants::MP_REIMBOURSEMENT, QVariant());
//        query.bindValue(AccountDB::Constants::MP_TYPE, QVariant());
//        query.bindValue(AccountDB::Constants::MP_UID, QUuid::createUuid().toString());
//        query.bindValue(AccountDB::Constants::MP_USER_UID, user()->value(Core::IUser::Uuid));
//        if (!query.exec()) {
//            Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
//        }
//        query.finish();

        if (!d->m_SqlTable->insertRow(row+i, parent)) {
            qWarning() << d->m_SqlTable->database().lastError().text();
            ok = false;
        } else {
            // Set CurrentUser UUID
            QModelIndex userUid = d->m_SqlTable->index(row+i, Constants::MP_USER_UID, parent);
            if (!d->m_SqlTable->setData(userUid, user()->value(Core::IUser::Uuid))) {
                qWarning() << d->m_SqlTable->database().lastError().text();
                ok = false;
            }
            // Create MP UUID
            QModelIndex mpUid = d->m_SqlTable->index(row+i, Constants::MP_UID, parent);
            if (!d->m_SqlTable->setData(mpUid, QUuid::createUuid().toString())) {
                qWarning() << d->m_SqlTable->database().lastError().text();
                ok = false;
            }
        }

    }
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
//    d->m_SqlTable->select();
//    reset();
    return ok;
}

bool MedicalProcedureModel::removeRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
    return d->m_SqlTable->removeRows(row, count, parent);
}

void MedicalProcedureModel::setTypeFilter(const QString &type)
{
    d->m_TypeFilter = type;
    d->refreshFilter();
    d->m_SqlTable->select();
    reset();
}

bool MedicalProcedureModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        d->m_IsDirty = false;
        return true;
    }
    return false;
}

void MedicalProcedureModel::revert()
{
    d->m_IsDirty = false;
    d->m_SqlTable->revert();
}

void MedicalProcedureModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
}

bool MedicalProcedureModel::isDirty() const
{
    return d->m_IsDirty;
}

QSqlError MedicalProcedureModel::lastError(){
    return d->m_SqlTable->lastError();
}
