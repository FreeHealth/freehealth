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
 * \class Account::MailAccountModel
 * \brief short description of class
 *
 * Long description of class
 * \sa Account::
 */

#include "mailaccountmodel.h"

#include <translationutils/constants.h>

#include <QDebug>

using namespace Account;
using namespace Trans::ConstantTranslations;

namespace Account {
namespace Internal {
/*!
 * \class Account::MailAccountModelPrivate
 * \brief Private implementation of the Account::MailAccountModel class.
 *
 * documentation here
 */
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
    // Put your data here
    
private:
    MailAccountModel *q;
};
} // namespace Internal
} // end namespace Account


/*! Constructor of the Account::MailAccountModel class */
MailAccountModel::MailAccountModel(QObject *parent) :
    QObject(parent),
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

