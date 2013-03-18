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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "bankaccountmodel.h"

#include <account2plugin/accountcore.h>
#include <account2plugin/accountitems.h>
#include <account2plugin/database/accountbase.h>
#include <account2plugin/database/constants_db.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>

#include <translationutils/constanttranslations.h>

#include <QSqlTableModel>
#include <QDebug>

enum { WarnDebugMessage = false };

using namespace Account2;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Account2::AccountCore *accountCore() {return Account2::AccountCore::instance();}
static inline Account2::Internal::AccountBase *accountBase() {return Account2::AccountCore::instance()->accountBase();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}

namespace Account2 {
namespace Internal {
class BankAccountModelPrivate
{
public:
    BankAccountModelPrivate(BankAccountModel *parent) :
        q(parent)
    {
        _sql = new QSqlTableModel(q, accountBase()->database());
        _sql->setTable(accountBase()->table(Constants::Table_BankDetails));
        _sql->setEditStrategy(QSqlTableModel::OnManualSubmit);
    }

    ~BankAccountModelPrivate()
    {}

    void refreshFilter(const QString &userUid)
    {
        QHash<int, QString> where;
        if (userUid.isEmpty()) {
            where.insert(Constants::BANKDETAILS_USER_UID, QString("='%1'").arg(user()->uuid()));
        } else {
            if (userUid == "%")
                where.insert(Constants::BANKDETAILS_USER_UID, QString("LIKE '%'"));
            else
                where.insert(Constants::BANKDETAILS_USER_UID, QString("='%1'").arg(userUid));
        }
        _sql->setFilter(accountBase()->getWhereClause(Constants::Table_BankDetails, where));
        _sql->select();
    }

public:
    QSqlTableModel *_sql;

private:
    BankAccountModel *q;
};

} // namespace Internal
} // namespace Account2

BankAccountModel::BankAccountModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::BankAccountModelPrivate(this))
{
}

BankAccountModel::~BankAccountModel()
{
    if (d) {
        delete d;
        d=0;
    }
}

int BankAccountModel::rowCount(const QModelIndex &) const
{
    return d->_sql->rowCount();
}

QVariant BankAccountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
       return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        int sqlCol = -1;
        switch (index.column()) {
        case Id: sqlCol = Constants::BANKDETAILS_ID; break;
        case UserUid: sqlCol = Constants::BANKDETAILS_USER_UID; break;
        case UserFullName: return user()->fullNameOfUser(d->_sql->data(d->_sql->index(index.row(), Constants::BANKDETAILS_USER_UID)).toString());
        case Label: sqlCol = Constants::BANKDETAILS_LABEL; break;
        case OwnerFullName: sqlCol = Constants::BANKDETAILS_OWNER; break;
        case OwnerFullAddress: sqlCol = Constants::BANKDETAILS_OWNERADRESS; break;
        case BankAccountNumber: sqlCol = Constants::BANKDETAILS_ACCOUNTNUMBER; break;
        case IBAN: sqlCol = Constants::BANKDETAILS_IBAN; break;
        case Comment: sqlCol = Constants::BANKDETAILS_COMMENT; break;
        case IsDefault: sqlCol = Constants::BANKDETAILS_DEFAULT; break;
        default: return QVariant();
        }
        if (sqlCol!=-1)
            return d->_sql->data(d->_sql->index(index.row(), sqlCol));
    }

    return QVariant();
}

bool BankAccountModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

QVariant BankAccountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Id: return "Id";
        case UserUid: return "UserUid";
        case UserFullName: return "UserFullName";
        case Label: return "Label";
        case OwnerFullName: return "OwnerFullName";
        case OwnerFullAddress: return "OwnerFullAddress";
        case BankAccountNumber: return "Number";
        case IBAN: return "IBAN";
        case Comment: return "Comment";
        case IsDefault: return "Default";
        default: return QVariant();
        }
    }
    return QVariant();
}

bool BankAccountModel::setFilter(const QString &userUid)
{
    beginResetModel();
    d->refreshFilter(userUid);
    endResetModel();
    return true;
}

bool BankAccountModel::insertRows(int row, int count, const QModelIndex &parent)
{
      return true;
}

bool BankAccountModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return true;
}

bool BankAccountModel::isDirty() const
{
    return true;
}

bool BankAccountModel::submit()
{
    return d->_sql->submit();
}

void BankAccountModel::revert()
{
    d->_sql->revert();
}

