/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef MFPRESCRIPTIONVIEWERMANAGER_H
#define MFPRESCRIPTIONVIEWERMANAGER_H

#include <drugswidget/mfDrugsCentralWidget.h>
#include <drugsmodel/mfDrugsModel.h>
class mfInteractionsManager;

#include <tkContext.h>
#include <tkUniqueIdentifier.h>

#include <QWidget>
#include <QObject>
#include <QAction>
#include <QPointer>

/**
 * \file mfDrugsManager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.0.5
 * \date 23 July  2009
 * \internal
*/

class mfDrugsContext : public tkContext
{
public:
    mfDrugsContext(mfDrugsCentralWidget *w) : tkContext(w), wgt(w) {}

    void setContext(QList<int> c) { ctx = c; }

    QList<int> context() const { return ctx; }
    QWidget *widget() { return wgt; }
private:
    mfDrugsCentralWidget *wgt;
    QList<int> ctx;
};


class mfDrugsActionHandler : public QObject
{
    Q_OBJECT
public:
    mfDrugsActionHandler(QObject *parent = 0);
    virtual ~mfDrugsActionHandler() {}

    void setCurrentView(mfDrugsCentralWidget *view);

public Q_SLOTS:
    void drugsModelChanged();

private Q_SLOTS:
    void moveUp();
    void moveDown();
    void sortDrugs();
    void removeItem();
    void clear();
    void viewInteractions();
    void listViewItemChanged();
    void searchActionChanged(QAction *a);
    void printPrescription();
    void toogleTestingDrugs();

private:
    bool canMoveUp();
    bool canMoveDown();
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
    QAction *aToogleTestingDrugs;

    QPointer<mfDrugsCentralWidget> m_CurrentView;
};


#define DRUGMODEL  mfDrugsManager::instance()->currentDrugsModel()

class mfDrugsManager : public mfDrugsActionHandler
{
    Q_OBJECT
public:
    static mfDrugsManager *instance();
    ~mfDrugsManager() {}

    mfDrugsCentralWidget  *currentView() const;
    mfDrugsModel          *currentDrugsModel() const { return currentView()->currentDrugsModel(); }
    mfInteractionsManager *currentInteractionManager() const { return currentView()->currentDrugsModel()->currentInteractionManger(); }

private Q_SLOTS:
    void updateContext(tkContext *object);

private:
    mfDrugsManager(QObject *parent = 0);
    static mfDrugsManager *m_Instance;
};


#endif // MFPRESCRIPTIONVIEWER_H
