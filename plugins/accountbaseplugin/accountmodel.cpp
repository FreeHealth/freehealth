/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "accountmodel.h"
#include "accountbase.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <translationutils/constanttranslations.h>

#include <QSqlTableModel>

using namespace AccountDB;
using namespace Trans::ConstantTranslations;

enum {WarnFilter=true};

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline AccountDB::AccountBase *accountBase() {return AccountDB::AccountBase::instance();}
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }


namespace AccountDB {
namespace Internal {

/** \todo Manage user change */
class AccountModelPrivate
{
public:
    AccountModelPrivate(AccountModel *parent) :
            m_SqlTable(0), m_IsDirty(false),
            m_StartDate(QDate::currentDate()), m_EndDate(QDate::currentDate()),
            m_UserUid(user()->value(Core::IUser::Uuid).toString()),
            q(parent)
    {   //qDebug() << __FILE__ << QString::number(__LINE__) << " m_UserUid =  " << m_UserUid;
        m_SqlTable = new QSqlTableModel(q, QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        m_SqlTable->setTable(accountBase()->table(Constants::Table_Account));
        refreshFilter();
    }
    ~AccountModelPrivate () {}

    void refreshFilter()
    {
        if (!m_SqlTable)
            return;
        QHash<int, QString> where;
        if (m_EndDate==m_StartDate) {
            where.insert(AccountDB::Constants::ACCOUNT_DATE, QString("='%1'").arg(m_EndDate.toString(Qt::ISODate)));
        } else {
            where.insertMulti(AccountDB::Constants::ACCOUNT_DATE, QString(">='%1'").arg(m_StartDate.toString(Qt::ISODate)));
            where.insertMulti(AccountDB::Constants::ACCOUNT_DATE, QString("<='%1'").arg( m_EndDate.toString(Qt::ISODate)));
        }
        where.insert(AccountDB::Constants::ACCOUNT_USER_UID, QString("='%1'").arg(m_UserUid));
        m_SqlTable->setFilter(accountBase()->getWhereClause(Constants::Table_Account, where));
        if (WarnFilter)
            qWarning() << m_SqlTable->filter() << __FILE__ << __LINE__;
        q->reset();
    }

public:
    QSqlTableModel *m_SqlTable;
    bool m_IsDirty;
    QDate m_StartDate, m_EndDate;
    QString m_UserUid;

private:
    AccountModel *q;
};

}  // End namespace Internal
}  // End namespace AccountDB



AccountModel::AccountModel(QObject *parent) :
        QAbstractTableModel(parent), d(new Internal::AccountModelPrivate(this))
{
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnFieldChange);

    connect(user(), SIGNAL(userChanged()), this, SLOT(userChanged()));
    userChanged();

    d->m_SqlTable->select();
}

AccountModel::~AccountModel()
{
    if (d) {
        delete d;
        d=0;
    }
}

int AccountModel::rowCount(const QModelIndex &parent) const
{
    int rows = 0;
    d->m_SqlTable->setFilter("");
    d->m_SqlTable->select();
    rows = d->m_SqlTable->rowCount(parent);
    return rows;
}

int AccountModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}

void AccountModel::setUserUuid(const QString &uuid)
{
    d->m_UserUid = uuid;
    d->refreshFilter();
}


QVariant AccountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role==Qt::DisplayRole) {
        if (index.column()==Constants::ACCOUNT_DATE) {
            QDate date = d->m_SqlTable->data(index, role).toDate();
            return date.toString(settings()->value(Core::Constants::S_DATEFORMAT, QLocale().dateFormat(QLocale::LongFormat)).toString());
        }
        return d->m_SqlTable->data(index, role);
    }
    return QVariant();
}

bool AccountModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = d->m_SqlTable->setData(index, value, role);
    d->m_IsDirty = d->m_SqlTable->isDirty(index);
    return ret;
}


QVariant AccountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal) {
        switch (section) {
        /** \todo remove tr translations use tkTr*/
        case Constants::ACCOUNT_CASHAMOUNT : return tr(Constants::CASH, Constants::ACCOUNTCONSTANTS_TR_CONTEXT);
        case Constants::ACCOUNT_CHEQUEAMOUNT : return tr(Constants::CHEQUES, Constants::ACCOUNTCONSTANTS_TR_CONTEXT);
        case Constants::ACCOUNT_COMMENT : return tr("Comments");
        case Constants::ACCOUNT_DATE : return tr("Date");
        case Constants::ACCOUNT_DUEAMOUNT : return tr( Constants::DUE, Constants::ACCOUNTCONSTANTS_TR_CONTEXT);
        case Constants::ACCOUNT_DUEBY : return tr("Due by");
        case Constants::ACCOUNT_ID : return "ID";
        case Constants::ACCOUNT_INSURANCEAMOUNT : return tr(Constants::INSURANCE, Constants::ACCOUNTCONSTANTS_TR_CONTEXT);
        case Constants::ACCOUNT_INSURANCE_ID : return "Assurance Id";
        case Constants::ACCOUNT_ISVALID : return "is valid";
        case Constants::ACCOUNT_OTHERAMOUNT : return tr(Constants::OTHER, Constants::ACCOUNTCONSTANTS_TR_CONTEXT);
        case Constants::ACCOUNT_MEDICALPROCEDURE_TEXT : return "MP";
        case Constants::ACCOUNT_MEDICALPROCEDURE_XML : return "MP";
        case Constants::ACCOUNT_PATIENT_NAME : return tr(Trans::Constants::PATIENT);
        case Constants::ACCOUNT_PATIENT_UID : return "Patient uuid";
        case Constants::ACCOUNT_SITE_ID : return "Site Id";
//        case Constants::ACCOUNT_TRACE : return "trace";
        case Constants::ACCOUNT_USER_UID : return tkTr(Trans::Constants::USER);
        case Constants::ACCOUNT_VISAAMOUNT : return tr(Constants::VISA, Constants::ACCOUNTCONSTANTS_TR_CONTEXT);
        }
    }

    return QVariant();
}

bool AccountModel::insertRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnRowChange);
    bool ret = d->m_SqlTable->insertRows(row, count, parent);
//    d->m_SqlTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
    return ret;
}

bool AccountModel::removeRows(int row, int count, const QModelIndex &parent)
{
    d->m_IsDirty = true;
    return d->m_SqlTable->removeRows(row, count, parent);
}

bool AccountModel::submit()
{
    if (d->m_SqlTable->submitAll()) {
        d->m_IsDirty = false;
        return true;
    }
    return false;
}

void AccountModel::revert()
{
    d->m_IsDirty = false;
    d->m_SqlTable->revert();
}

bool AccountModel::isDirty() const
{
    return d->m_IsDirty;
}

void AccountModel::setStartDate(const QDate &date)
{
    d->m_StartDate = date;
    d->refreshFilter();
}

void AccountModel::setEndDate(const QDate &date)
{
    d->m_EndDate = date;
    d->refreshFilter();
}

double AccountModel::sum(const int &fieldRef)
{
    // construct query == SELECT total(FIELD) FROM TABLE WHERE...
    QSqlQuery query(accountBase()->total(Constants::Table_Account, fieldRef) + " WHERE " + d->m_SqlTable->filter(), d->m_SqlTable->database());
    if (query.isActive()) {
        if (query.next())
            return query.value(0).toDouble();
    } else {
        Utils::Log::addQueryError(this, query);
    }
    return 0.0;
}

void AccountModel::userChanged()
{
    d->m_UserUid = user()->value(Core::IUser::Uuid).toString();
    d->refreshFilter();
}

QSqlError AccountModel::lastError(){
    return d->m_SqlTable->lastError();
}

bool AccountModel::canFetchMore ( const QModelIndex & parent  ) const  {
    return d->m_SqlTable->canFetchMore ( parent  );
}
    
void AccountModel::fetchMore ( const QModelIndex & parent ) {
    d->m_SqlTable->canFetchMore ( parent  );
}

