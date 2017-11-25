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
 * \class Mail::MailAccount
 * \brief short description of class
 *
 * Long description of class
 * \sa Mail::
 */

#include "mailaccount.h"

//#include <utils/log.h>
//#include <translationutils/constants.h>

#include <QDomDocument>
#include <QDebug>

using namespace Mail;
using namespace Internal;
//using namespace Trans::ConstantTranslations;
namespace {
const char * const XML_ROOT_TAG = "MailAccount";
const char * const XML_POP3ROOT_TAG = "Pop3";
}

namespace Mail {
namespace Internal {
class MailAccountPrivate
{
public:
    MailAccountPrivate(MailAccount *parent) :
        _hostPort(110),
        _usesSsl(false),
        _usesStartTls(true),
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~MailAccountPrivate()
    {
    }
    
public:
    QString _descr, _userName, _userLog, _userPass, _userAddress, _host;
    int _hostPort;
    bool _usesSsl, _usesStartTls;
    QDateTime _lastConnection;
    
private:
    MailAccount *q;
};
} // namespace Internal
} // end namespace Mail


/*! Constructor of the Mail::MailAccount class */
MailAccount::MailAccount() :
    d(new MailAccountPrivate(this))
{
}

/*! Copy constructor of the Mail::MailAccount class */
MailAccount::MailAccount(const MailAccount &copy) :
    d(new MailAccountPrivate(this))
{
    d->_userName = copy.d->_userName;
    d->_userLog = copy.d->_userLog;
    d->_userPass = copy.d->_userPass;
    d->_userAddress = copy.d->_userAddress;
    d->_host = copy.d->_host;
    d->_hostPort = copy.d->_hostPort;
    d->_usesSsl = copy.d->_usesSsl;
    d->_usesStartTls = copy.d->_usesStartTls;
    d->_lastConnection = copy.d->_lastConnection;
}

/*! Destructor of the Mail::MailAccount class */
MailAccount::~MailAccount()
{
    if (d)
        delete d;
    d = 0;
}

void MailAccount::setAccountDescription(const QString &descr)
{
    d->_descr = descr;
}

void MailAccount::setUserName(const QString &name)
{
    d->_userName = name;
}

void MailAccount::setUserMailAddress(const QString &mailAddress)
{
    d->_userAddress = mailAddress;
}

void MailAccount::setHostName(const QString &hostName)
{
    d->_host = hostName;
}

void MailAccount::setHostPort(int port)
{
    d->_hostPort = port;
}

void MailAccount::setHostUsesSsl(bool useSsl)
{
    d->_usesSsl = useSsl;
}

void MailAccount::setHostUsesStartTls(bool useTls)
{
    d->_usesStartTls = useTls;
}

void MailAccount::setHostLogin(const QString &hostLogin)
{
    d->_userLog = hostLogin;
}

void MailAccount::setHostPassword(const QString &password)
{
    d->_userPass = password;
}

void MailAccount::setLastSucceededConnection(const QDateTime &dt)
{
    d->_lastConnection = dt;
}

QString MailAccount::accountDescription() const
{
    return d->_descr;
}

QString MailAccount::userName() const
{
    return d->_userName;
}

QString MailAccount::userMailAddress() const
{
    return d->_userAddress;
}

QString MailAccount::hostName() const
{
    return d->_host;
}

int MailAccount::hostPort() const
{
    return d->_hostPort;
}

bool MailAccount::hostUsesSsl() const
{
    return d->_usesSsl;
}

bool MailAccount::hostUsesStartTls() const
{
    return d->_usesStartTls;
}

QString MailAccount::hostLogin() const
{
    return d->_userLog;
}

QString MailAccount::hostPassword() const
{
    return d->_userPass;
}

QDateTime MailAccount::lastSucceededConnection() const
{
    return d->_lastConnection;
}

QString MailAccount::toXml() const
{
    return QString("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                   "<!DOCTYPE FreeMedForms>\n"
                   "<%1>\n"
                   "  <%2>\n"
                   "    <Description label=\"%11\"/>\n"
                   "    <Host name=\"%3\" port=\"%4\" ssl=\"%5\" tsl=\"%6\"/>\n"
                   "    <User fullName=\"%7\" address=\"%8\" login=\"%9\"/>\n"
                   "    <LastConnection date=\"%10\"/>\n"
                   "  </%2>\n"
                   "</%1>\n")
            .arg(::XML_ROOT_TAG)
            .arg(::XML_POP3ROOT_TAG)
            .arg(d->_host)
            .arg(d->_hostPort)
            .arg(d->_usesSsl)
            .arg(d->_usesStartTls)
            .arg(d->_userName)
            .arg(d->_userAddress)
            .arg(d->_userLog)
            .arg(d->_lastConnection.toString(Qt::ISODate))
            .arg(d->_descr)
            ;
}

bool MailAccount::fromXml(const QString &xmlContent)
{
    QDomDocument doc;
    QString err;
    int l, c;
    if (!doc.setContent(xmlContent, &err, &l, &c)) {
//        LOG_ERROR_FOR("MailAccount", QString("XML Error (%1,%2): %3\n%4").arg(l).arg(c).arg(err).arg(xmlContent));
        return false;
    }
    QDomElement root = doc.firstChildElement(::XML_ROOT_TAG);
    if (root.isNull())
        return false;
    root = root.firstChildElement(::XML_POP3ROOT_TAG);
    if (root.isNull())
        return false;

    // Description
    QDomElement desc = root.firstChildElement("Description");
    if (desc.isNull())
        return false;
    d->_descr = desc.attribute("label");

    // Host
    QDomElement host = root.firstChildElement("Host");
    if (host.isNull())
        return false;
    d->_host = host.attribute("name");
    d->_hostPort = host.attribute("port").toInt();
    d->_usesSsl = host.attribute("ssl").toInt();
    d->_usesStartTls = host.attribute("tsl").toInt();

    // Use
    QDomElement user = root.firstChildElement("User");
    if (user.isNull())
        return false;
    d->_userName = user.attribute("fullName");
    d->_userAddress = user.attribute("address");
    d->_userLog = user.attribute("login");

    // LastConnection
    QDomElement last = root.firstChildElement("LastConnection");
    if (last.isNull())
        return false;
    d->_lastConnection = QDateTime::fromString(last.attribute("date"), Qt::ISODate);

    return true;
}

MailAccount &MailAccount::operator=(const MailAccount &other)
{
    if (this == &other)
        return *this;
    d->_userName = other.d->_userName;
    d->_userLog = other.d->_userLog;
    d->_userPass = other.d->_userPass;
    d->_userAddress = other.d->_userAddress;
    d->_host = other.d->_host;
    d->_hostPort = other.d->_hostPort;
    d->_usesSsl = other.d->_usesSsl;
    d->_usesStartTls = other.d->_usesStartTls;
    d->_lastConnection = other.d->_lastConnection;
    return *this;
}

bool MailAccount::operator==(const MailAccount &other)
{
    return d->_userName == other.d->_userName
            && d->_userLog == other.d->_userLog
            && d->_userPass == other.d->_userPass
            && d->_userAddress == other.d->_userAddress
            && d->_host == other.d->_host
            && d->_hostPort == other.d->_hostPort
            && d->_usesSsl == other.d->_usesSsl
            && d->_usesStartTls == other.d->_usesStartTls
            && d->_lastConnection == other.d->_lastConnection;
}
