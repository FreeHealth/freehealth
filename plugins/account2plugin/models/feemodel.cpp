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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "feemodel.h"

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
class FeeModelPrivate
{
public:
    FeeModelPrivate(FeeModel *parent) :
        q(parent)
    {
    }

    ~FeeModelPrivate()
    {}

    void refreshFeesFromDatabase(const BasicFilter &filter)
    {
        AccountBaseQuery query(filter);
        query.setRetrieveObjects(AccountBaseQuery::Fee);
        AccountBaseResult result = accountBase()->query(query);
        _fees = result.fees();
    }

public:
    QList<Fee> _fees;

private:
    FeeModel *q;
};

} // namespace Internal
} // namespace Account2

FeeModel::FeeModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new Internal::FeeModelPrivate(this))
{
}

FeeModel::~FeeModel()
{
    if (d) {
        delete d;
        d=0;
    }
}

int FeeModel::rowCount(const QModelIndex &) const
{
    return d->_fees.count();
}

QVariant FeeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
       return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const Fee &fee = d->_fees.at(index.row());
        switch (index.column()) {
        case Id: return fee.id();
        case Label: return fee.label();
        case Amount: return fee.amount();
        case UserUid: return fee.userUid();
        case PatientUid: return fee.patientUid();
        case Type: return fee.type();
        case Date_Creation: return fee.date(VariableDatesItem::Date_Creation);
        case Date_Execution: return fee.date(VariableDatesItem::Date_MedicalRealisation);
        case TaxRate: return fee.taxRate();
        case Comment: return fee.comment();
        default: return QVariant();
        }
    }

    return QVariant();
}

bool FeeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}


QVariant FeeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Id: return "Id";
        case Label: return "Label";
        case Amount: return "Amount";
        case UserUid: return "User";
        case PatientUid: return "Patient";
        case Type: return "Type";
        case Date_Creation: return "DateCreation";
        case Date_Execution: return "DateExec";
        case TaxRate: return "TaxRate";
        case Comment: return "Comment";
        default: return QVariant();
        }
    }
    return QVariant();
}

bool FeeModel::setFilter(const BasicFilter &filter)
{
    beginResetModel();
    d->refreshFeesFromDatabase(filter);
    endResetModel();
    return true;
}

bool FeeModel::insertRows(int row, int count, const QModelIndex &parent)
{
      return true;
}

bool FeeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return true;
}

bool FeeModel::isDirty() const
{
    return true;
}

bool FeeModel::submit()
{
    return true;
}

void FeeModel::revert()
{
}

