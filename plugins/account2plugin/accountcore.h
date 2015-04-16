/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef ACCOUNT2PLUGIN_ACCOUNTCORE_H
#define ACCOUNT2PLUGIN_ACCOUNTCORE_H

#include <QObject>
#include <account2plugin/account2_exporter.h>

/**
 * \file accountcore.h
 * \author Eric Maeker
 * \version 0.8.6
 * \date 17 Mar 2013
*/

namespace DataPack {
class Pack;
}

namespace Account2 {
namespace Internal {
class AccountCorePrivate;
class Account2Plugin;
class AccountBase;
} // namespace Internal

class ACCOUNT2_EXPORT AccountCore : public QObject
{
    Q_OBJECT
    friend class Account2::Internal::Account2Plugin;

protected:
    AccountCore(QObject *parent = 0);
    bool initialize();

public:
    static AccountCore *instance();
    ~AccountCore();

    bool isDatabaseInitialized() const;

    Internal::AccountBase *accountBase() const;

protected:
    void postCoreInitialization();

private Q_SLOTS:
    void packInstalled(const DataPack::Pack &pack);
    void packRemoved(const DataPack::Pack &pack);

private:
    static AccountCore *_instance;
    Internal::AccountCorePrivate *d;
};

}  // namespace Account2

#endif  // ACCOUNT2PLUGIN_ACCOUNTCORE_H

