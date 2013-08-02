/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  The FreeAccount plugins are free, open source FreeMedForms' plugins.   *
 *  (C) 2010-2011 by Pierre-Marie Desombre, MD <pm.desombre@medsyn.fr>     *
 *  and Eric Maeker, MD <eric.maeker@gmail.com>                            *
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
 *  Main Developers:  Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>,      *
 *                     Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
#ifndef ACCOUNTWIDGETMANAGERMANAGER_H
#define ACCOUNTWIDGETMANAGERMANAGER_H

#include <accountplugin/account_exporter.h>

#include <QWidget>
#include <QObject>
#include <QAction>
#include <QPointer>

/**
 * \file accountwidgetmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 22 Sept 2012
 * \internal
*/

namespace Core {
class IContext;
class Context;
}

namespace Account {
class AccountContextualWidget;
namespace Internal {

class AccountActionHandler : public QObject
{
    Q_OBJECT
public:
    AccountActionHandler(QObject *parent = 0);
    virtual ~AccountActionHandler() {}

    void setCurrentView(AccountContextualWidget *view);

private Q_SLOTS:
    void addReceipts();
    void receipts();
    void ledger();
    void movements();
    void assets();
    void account();

private:
    void updateActions();

protected:
    QAction *aAddReceipts;
    QAction *aReceipts;
    QAction *aLegder;
    QAction *aMovements;
    QAction *aAssets;
    QAction *aAccount;

    QPointer<AccountContextualWidget> m_CurrentView;

    // TODO: to remove. This is provided only for testing purpose of FreeAccount
    QVector<AccountContextualWidget *> m_OpenedViews;
};

}  // End Internal
}  // End Account


namespace Account {

class ACCOUNT_EXPORT AccountWidgetManager : public Internal::AccountActionHandler
{
    Q_OBJECT
public:
    static AccountWidgetManager *instance();
    ~AccountWidgetManager() ;

    Account::AccountContextualWidget *currentView() const;

private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);

private:
    AccountWidgetManager(QObject *parent = 0);
    static AccountWidgetManager *m_Instance;
};

}  // End Account

#endif // ACCOUNTWIDGETMANAGERMANAGER_H
