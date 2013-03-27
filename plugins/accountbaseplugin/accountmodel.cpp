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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
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

// TODO: Manage user change
class AccountModelPrivate
{
public:
    AccountModelPrivate(AccountModel *parent) :
            m_SqlTable(0), m_IsDirty(false),
            //m_StartDate(QDate::currentDate()), m_EndDate(QDate::currentDate()),
            q(parent)
    {   //qDebug() << __FILE__ << QString::number(__LINE__) << " m_UserUid =  " << m_UserUid;
        m_SqlTable = new QSqlTableModel(q, QSqlDatabase::database(Constants::DB_ACCOUNTANCY));
        m_SqlTable->setTable(accountBase()->table(Constants::Table_Account));
        if (user())
            m_UserUid = user()->uuid();
        //setFilterUserUuid();
        //refreshFilter();
    }
    ~AccountModelPrivate () {}
    void setFilterUserUuid()
   {
    QString uuid = m_UserUid;
    qDebug() << __FILE__ << QString::number(__LINE__) << " uuid =" << uuid  ;
    QString filter = QString("%1 = '%2'").arg("USER_UID",uuid);
    m_SqlTable->setFilter(filter);
    m_SqlTable->select();
    }

    /*void refreshFilter()
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
    }*/

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
    // prevent trees
    if (parent.isValid()) return 0;

    int rows = 0;
    //d->m_SqlTable->setFilter("");
    rows = d->m_SqlTable->rowCount(parent);
    return rows;
}

int AccountModel::columnCount(const QModelIndex &parent) const
{
    return d->m_SqlTable->columnCount(parent);
}

QVariant AccountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    int row = index.row();
    /*if (role==Qt::DisplayRole) {
        if (index.column()==Constants::ACCOUNT_DATE) {
            QDate date = d->m_SqlTable->data(index, role).toDate();
            return date.toString(settings()->value(Core::Constants::S_DATEFORMAT, QLocale().dateFormat(QLocale::LongFormat)).toString());
        }
        return d->m_SqlTable->data(index, role);
    }
    return QVariant();*/
    switch(role){
        case Qt::DisplayRole :
            if (index.column()==Constants::ACCOUNT_DATE) {
            QDate date = d->m_SqlTable->data(index, role).toDate();
            return date.toString(settings()->value(Core::Constants::S_DATEFORMAT, QLocale().dateFormat(QLocale::LongFormat)).toString());
                }
            return d->m_SqlTable->data(index, role);
            break;
        case Qt::BackgroundRole :
            if(getDoublesRows.contains(row)){
                QColor color("blue");
                color.setAlpha(025);
                return color;
                }
            else{
                return QColor("white");
                }
            break;
        default :
            return QVariant();
            break;
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
    /*if (orientation==Qt::Horizontal) {
        switch (section) {
        //todo remove tr translations use tkTr
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
    }*/

    return d->m_SqlTable->headerData(section, orientation, role);
}

bool AccountModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant & value,
                                 int role){
    return d->m_SqlTable->setHeaderData(section,orientation,value,role);
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
    //d->refreshFilter();
}

void AccountModel::setEndDate(const QDate &date)
{
    d->m_EndDate = date;
    //d->refreshFilter();
}

double AccountModel::sum(const int &fieldRef)
{
    // construct query == SELECT total(FIELD) FROM TABLE WHERE...
    QSqlQuery query(accountBase()->totalSqlCommand(Constants::Table_Account, fieldRef) + " WHERE " + d->m_SqlTable->filter(), d->m_SqlTable->database());
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
    if (user()) {
        d->m_UserUid = user()->uuid();
        //d->refreshFilter();
    }
}

QSqlError AccountModel::lastError(){
    return d->m_SqlTable->lastError();
}

bool AccountModel::canFetchMore ( const QModelIndex & parent  ) const  {
    return d->m_SqlTable->canFetchMore ( parent  );
}

void AccountModel::fetchMore ( const QModelIndex & parent ) {
    d->m_SqlTable->fetchMore ( parent  );
}

void AccountModel::setFilter(const QString & filter){
    d->m_SqlTable->setFilter(filter);
    d->m_SqlTable->select();
}

QString AccountModel::filter(){
    return d->m_SqlTable->filter();
}

void AccountModel::select(){
    d->m_SqlTable->select();
}

QString AccountModel::getUserUuid(){
    QString uuid = d->m_UserUid;
    return uuid;
}

QSqlRecord AccountModel::record(int row){
    return d->m_SqlTable->record(row);
}
