/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Mail::MailAccountModel
 */

#include "mailaccountmodel.h"
#include "../mailaccount.h"

//#include <translationutils/constants.h>

#include <QDebug>

using namespace Mail;
using namespace Internal;
//using namespace Trans::ConstantTranslations;

namespace Mail {
namespace Internal {
class MailAccountModelPrivate
{
public:
    MailAccountModelPrivate(MailAccountModel *parent) :
        q(parent)
    {
    }
    
    ~MailAccountModelPrivate()
    {
    }
    
public:
    QList<MailAccount> _accounts;
    
private:
    MailAccountModel *q;
};
} // namespace Internal
} // end namespace Mail


/*! Constructor of the Account::MailAccountModel class */
MailAccountModel::MailAccountModel(QObject *parent) :
    QAbstractTableModel(parent),
    d(new MailAccountModelPrivate(this))
{
}

/*! Destructor of the Account::MailAccountModel class */
MailAccountModel::~MailAccountModel()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool MailAccountModel::initialize()
{
    return true;
}

int MailAccountModel::rowCount(const QModelIndex &) const
{
    return d->_accounts.count();
}

QVariant MailAccountModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

bool MailAccountModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

QVariant MailAccountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

void MailAccountModel::addMailAccount(const MailAccount &account)
{
    beginResetModel();
    d->_accounts << account;
    endResetModel();
}

void MailAccountModel::addMailAccounts(const QList<MailAccount> &accounts)
{
    beginResetModel();
    endResetModel();
}

const QList<MailAccount> &MailAccountModel::mailAccounts() const
{
    return d->_accounts;
}
