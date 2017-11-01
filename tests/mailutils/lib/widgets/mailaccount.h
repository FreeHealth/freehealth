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
#ifndef MAILUTILS_MAILACCOUNT_H
#define MAILUTILS_MAILACCOUNT_H

#include "../mail_exporter.h"
#include <QString>
#include <QDateTime>

/**
 * \file mailaccount.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 13 May 2013
*/

namespace Mail {
namespace Internal {
class MailAccountPrivate;
} // namespace Internal

class MAIL_EXPORT MailAccount
{    
public:
    explicit MailAccount();
    MailAccount(const MailAccount &copy);

    ~MailAccount();
    
    void setAccountDescription(const QString &descr);
    void setUserName(const QString &name);
    void setUserMailAddress(const QString &mailAddress);

    void setHostName(const QString &hostName);
    void setHostPort(int port);
    void setHostUsesSsl(bool useSsl);
    void setHostUsesStartTls(bool useTls);
    void setHostLogin(const QString &hostLogin);
    void setHostPassword(const QString &password);
    void setLastSucceededConnection(const QDateTime &dt);

    QString accountDescription() const;
    QString userName() const;
    QString userMailAddress() const;

    QString hostName() const;
    int hostPort() const;
    bool hostUsesSsl() const;
    bool hostUsesStartTls() const;
    QString hostLogin() const;
    QString hostPassword() const;
    QDateTime lastSucceededConnection() const;

    QString toXml() const;
    bool fromXml(const QString &xmlContent);

    MailAccount &operator=(const MailAccount &other);
    bool operator==(const MailAccount &other);

private:
    Internal::MailAccountPrivate *d;
};

} // namespace Mail

#endif  // MAILUTILS_MAILACCOUNT_H

