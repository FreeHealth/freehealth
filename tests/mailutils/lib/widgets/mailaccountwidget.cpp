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
 * \class Mail::Internal::MailAccountWidget
 */

#include "mailaccountwidget.h"
#include "../models/mailaccountmodel.h"

//#include <translationutils/constants.h>

#include <QDebug>

using namespace Mail;
using namespace Internal;
//using namespace Trans::ConstantTranslations;

namespace Mail {
namespace Internal {
class MailAccountWidgetPrivate
{
public:
    MailAccountWidgetPrivate(MailAccountWidget *parent) :
        q(parent)
    {
    }
    
    ~MailAccountWidgetPrivate()
    {
    }
    
public:
    MailAccountModel *_accountModel;
    
private:
    MailAccountWidget *q;
};
} // namespace Internal
} // end namespace Mail


/*! Constructor of the Mail::Internal::MailAccountWidget class */
MailAccountWidget::MailAccountWidget(QObject *parent) :
    QObject(parent),
    d(new MailAccountWidgetPrivate(this))
{
}

/*! Destructor of the Mail::Internal::MailAccountWidget class */
MailAccountWidget::~MailAccountWidget()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool MailAccountWidget::initialize()
{
    return true;
}

void MailAccountWidget::setMailAccount(const MailAccount &account)
{

}

void MailAccountWidget::setMailAccountModel(MailAccountModel *model)
{
    d->_accountModel = model;
}

void MailAccountWidget::setMailAccountModelIndex(MailAccountModel *model)
{

}
