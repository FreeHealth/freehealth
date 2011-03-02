/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
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
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 03 Mar 2011
 * \internal
*/

namespace Core {
class IContext;
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

private:
    void updateActions();

protected:
    QAction *aAddReceipts;
    QAction *aReceipts;
    QAction *aLegder;
    QAction *aMovements;
    QAction *aAssets;

    QPointer<AccountContextualWidget> m_CurrentView;
};

}  // End Internal
}  // End Account


namespace Account {

class ACCOUNT_EXPORT AccountWidgetManager : public Internal::AccountActionHandler
{
    Q_OBJECT
public:
    static AccountWidgetManager *instance();
    ~AccountWidgetManager() {}

    Account::AccountContextualWidget *currentView() const;

private Q_SLOTS:
    void updateContext(Core::IContext *object);

private:
    AccountWidgetManager(QObject *parent = 0);
    static AccountWidgetManager *m_Instance;
};

}  // End Account

#endif // ACCOUNTWIDGETMANAGERMANAGER_H
