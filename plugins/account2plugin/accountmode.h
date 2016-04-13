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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers: Eric MAEKER, <eric.maeker@gmail.com>                  *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNT2_ACCOUNTMODE_H
#define ACCOUNT2_ACCOUNTMODE_H

#include <coreplugin/modemanager/imode.h>

#include <QStackedWidget>

/**
 * \file accountmode.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 17 Mar 2013
*/

namespace Account2 {
namespace Internal {

class AccountMode : public Core::IMode
{
    Q_OBJECT
public:
    explicit AccountMode(QObject *parent = 0);
    ~AccountMode();

    void setCentralWidget(QWidget *widget);

//public Q_SLOTS:
//    void showAddReceipts();
//    void showReceipts();
//    void showLedger();
//    void showMovements();
//    void showAssets();
//    void showAccount();

private Q_SLOTS:
    void postCoreInitialization();
    void modeActivated(Core::IMode *mode);

private:
    QStackedWidget *m_Stack;
};

}  // End namespace Internal
}  // End namespace Account2

#endif // ACCOUNT2_ACCOUNTMODE_H
