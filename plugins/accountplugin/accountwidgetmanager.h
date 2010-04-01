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

#include <coreplugin/contextmanager/icontext.h>

#include <QWidget>
#include <QObject>
#include <QAction>
#include <QPointer>

/**
 * \file accountwidgetmanager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 20 Mar 2010
 * \internal
*/

namespace Account {
namespace Internal {

class AccountContext : public Core::IContext
{
public:
    AccountContext(QWidget *w) : Core::IContext(w), wgt(w) {}

    void setContext(QList<int> c) { ctx = c; }

    QList<int> context() const { return ctx; }
    QWidget *widget() { return wgt; }
private:
    QWidget *wgt;
    QList<int> ctx;
};

class AccountActionHandler : public QObject
{
    Q_OBJECT
public:
    AccountActionHandler(QObject *parent = 0);
    virtual ~AccountActionHandler() {}

    void setCurrentView(QWidget *view);

//private Q_SLOTS:
//    void moveUp();
//    void moveDown();
//    void sortDrugs();
//    void removeItem();
//    void clear();
//    void viewInteractions();
//    void listViewItemChanged();
//    void searchActionChanged(QAction *a);
//    void printPrescription();
//    void toggleTestingDrugs();
//    void changeDuration();
//    void createTemplate();
//    void printPreview();
//    void showDatabaseInformations();
//    void modeActionChanged(QAction *a);

private:
//    bool canMoveUp();
//    bool canMoveDown();
    void updateActions();

protected:
    QAction *aAddRow;
    QAction *aRemoveRow;
    QAction *aDown;
    QAction *aUp;
    QAction *aSort;
    QAction *aEdit;
    QAction *aClear;
    QAction *aViewInteractions;
    QActionGroup *gSearchMethod;
    QAction *aSearchCommercial;
    QAction *aSearchMolecules;
    QAction *aSearchInn;
    QAction *aPrintPrescription;
    QAction *aPrintPreview;
    QAction *aToggleTestingDrugs;
    QAction *aChangeDuration;
    QAction *aToTemplate;
    QAction *aDatabaseInformations;
    QActionGroup *gModes;
    QAction *aPrescriberMode;
    QAction *aSelectOnlyMode;

//    QPointer<DrugsCentralWidget> m_CurrentView;
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

//    AccountWidget::DrugsCentralWidget  *currentView() const;
//    DrugsDB::DrugsModel          *currentDrugsModel() const { return DrugsDB::DrugsModel::activeModel(); }
//    DrugsDB::InteractionsManager *currentInteractionManager() const { return currentView()->currentDrugsModel()->currentInteractionManger(); }

private Q_SLOTS:
    void updateContext(Core::IContext *object);

private:
    AccountWidgetManager(QObject *parent = 0);
    static AccountWidgetManager *m_Instance;
};

}  // End Account

#endif // ACCOUNTWIDGETMANAGERMANAGER_H
