/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#ifndef MFPRESCRIPTIONVIEWERMANAGER_H
#define MFPRESCRIPTIONVIEWERMANAGER_H

#include <drugsplugin/drugswidget/mfDrugsCentralWidget.h>
#include <drugsplugin/drugsmodel/mfDrugsModel.h>

#include <coreplugin/contextmanager/icontext.h>

#include <QWidget>
#include <QObject>
#include <QAction>
#include <QPointer>

/**
 * \file mfDrugsManager.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.0.8
 * \date 14 Sept  2009
 * \internal
*/

namespace Drugs {
namespace Internal {
class InteractionsManager;

/** \todo should be outside Internal */

class DrugsContext : public Core::IContext
{
public:
    DrugsContext(DrugsCentralWidget *w) : Core::IContext(w), wgt(w) {}

    void setContext(QList<int> c) { ctx = c; }

    QList<int> context() const { return ctx; }
    QWidget *widget() { return wgt; }
private:
    DrugsCentralWidget *wgt;
    QList<int> ctx;
};


class DrugsActionHandler : public QObject
{
    Q_OBJECT
public:
    DrugsActionHandler(QObject *parent = 0);
    virtual ~DrugsActionHandler() {}

    void setCurrentView(DrugsCentralWidget *view);

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

    QPointer<DrugsCentralWidget> m_CurrentView;
};


#define DRUGMODEL  DrugsManager::instance()->currentDrugsModel()

class DrugsManager : public DrugsActionHandler
{
    Q_OBJECT
public:
    static DrugsManager *instance();
    ~DrugsManager() {}

    DrugsCentralWidget  *currentView() const;
    DrugsModel          *currentDrugsModel() const { return currentView()->currentDrugsModel(); }
    InteractionsManager *currentInteractionManager() const { return currentView()->currentDrugsModel()->currentInteractionManger(); }

private Q_SLOTS:
    void updateContext(Core::IContext *object);

private:
    DrugsManager(QObject *parent = 0);
    static DrugsManager *m_Instance;
};

}  // End Internal
}  // End Drugs

#endif // MFPRESCRIPTIONVIEWER_H
