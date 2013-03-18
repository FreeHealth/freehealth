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
#include "paymentmodel.h"

#include <account2plugin/accountcore.h>
#include <account2plugin/accountitems.h>
#include <account2plugin/database/accountbase.h>

#include <translationutils/constanttranslations.h>

#include <QDebug>

enum { WarnDebugMessage = false };

using namespace Account2;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Account2::AccountCore *accountCore() {return Account2::AccountCore::instance();}
static inline Account2::Internal::AccountBase *accountBase() {return Account2::AccountCore::instance()->accountBase();}

namespace Account2 {
namespace Internal {
class PaymentModelPrivate
{
public:
    PaymentModelPrivate(PaymentModel *parent) :
        q(parent)
    {
    }

    ~PaymentModelPrivate()
    {}

    void refreshPaymentsFromDatabase(const BasicFilter &filter)
    {
        // Query database
        AccountBaseQuery query(filter);
        query.setRetrieveObjects(AccountBaseQuery::Payment);
        AccountBaseResult result = accountBase()->query(query);
        _payments = result.payments();
        // Create the tree
        // Payment
        //   +-- Fee label (xx%)
        //   +-- Fee label (xx%)
    }

public:
    QList<Payment> _payments;

private:
    PaymentModel *q;
};

} // namespace Internal
} // namespace Account2

PaymentModel::PaymentModel(QObject *parent) :
    QStandardItemModel(parent),
    d(new Internal::PaymentModelPrivate(this))
{
}

PaymentModel::~PaymentModel()
{
    if (d) {
        delete d;
        d=0;
    }
}

int PaymentModel::rowCount(const QModelIndex &) const
{
    return d->_payments.count();
}

QVariant PaymentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
       return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const Payment &pay = d->_payments.at(index.row());
        switch (index.column()) {
        case Id: return pay.id();
        case Amount: return pay.amount();
        case Type: return pay.type();
        case Date_Creation: return pay.date(VariableDatesItem::Date_Creation);
        case Date_Execution: return pay.date(VariableDatesItem::Date_MedicalRealisation);
        case Comment: return pay.comment();
        default: return QVariant();
        }
    }

    return QVariant();
}

bool PaymentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}


QVariant PaymentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Id: return "Id";
        case Amount: return "Amount";
        case Type: return "Type";
        case Date_Creation: return "DateCreation";
        case Date_Execution: return "DateExec";
        case Comment: return "Comment";
        default: return QVariant();
        }
    }
    return QVariant();
}

bool PaymentModel::setFilter(const BasicFilter &filter)
{
    beginResetModel();
    d->refreshPaymentsFromDatabase(filter);
    endResetModel();
    return true;
}

bool PaymentModel::insertRows(int row, int count, const QModelIndex &parent)
{
      return true;
}

bool PaymentModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return true;
}

bool PaymentModel::isDirty() const
{
    return true;
}

bool PaymentModel::submit()
{
    return true;
}

void PaymentModel::revert()
{
}

