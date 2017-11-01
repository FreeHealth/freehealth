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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MAILUTILS_INTERNAL_MAILACCOUNTWIDGET_H
#define MAILUTILS_INTERNAL_MAILACCOUNTWIDGET_H

#include <QObject>

/**
 * \file mailaccountwidget.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 13 May 2013
*/

namespace Mail {
namespace Internal {
class MailAccountModel;
class MailAccount;
class MailAccountWidgetPrivate;

class MailAccountWidget : public QObject
{
    Q_OBJECT
    
public:
    explicit MailAccountWidget(QObject *parent = 0);
    ~MailAccountWidget();
    bool initialize();

    void setMailAccount(const MailAccount &account);

    void setMailAccountModel(MailAccountModel *model);
    void setMailAccountModelIndex(MailAccountModel *model);

Q_SIGNALS:
    
public Q_SLOTS:
    
private:
    MailAccountWidgetPrivate *d;
};

} // namespace Internal
} // namespace Mail

#endif // MAILUTILS_INTERNAL_MAILACCOUNTWIDGET_H

