/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
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
#include "mfDrugsManager.h"

// include drugs widget headers
#include <mfDrugsConstants.h>
#include <drugswidget/mfDrugsCentralWidget.h>

// include toolkit headers
#include <tkTheme.h>
#include <tkLog.h>
#include <tkActionManager.h>
#include <tkContextManager.h>

using namespace mfDrugsConstants;
Q_TK_USING_CONSTANTS

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
mfDrugsManager *mfDrugsManager::m_Instance = 0;

mfDrugsManager *mfDrugsManager::instance()
{
    if (!m_Instance)
        m_Instance = new mfDrugsManager(qApp);
    return m_Instance;
}

mfDrugsManager::mfDrugsManager(QObject *parent) : mfDrugsActionHandler(parent)
{
    connect(tkContextManager::instance(), SIGNAL(contextChanged(tkContext*)),
            this, SLOT(updateContext(tkContext*)));
    setObjectName("mfDrugsManager");
    tkLog::addMessage(this, "Instance created");
}

void mfDrugsManager::updateContext(tkContext *object)
{
//    if (object)
//        qWarning() << "CONTEXT" << object->widget();

    mfDrugsCentralWidget *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

//            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
            break;
        }
        view = qobject_cast<mfDrugsCentralWidget *>(object->widget());
        if (!view) {
            if (!m_CurrentView)
                return;

//            m_CurrentView = 0;   // keep trace of the last active view (we need it in dialogs)
            break;
        }

        if (view == m_CurrentView) {
            return;
        }

    } while (false);
    if (view) {
        mfDrugsActionHandler::setCurrentView(view);
    }
}

mfDrugsCentralWidget *mfDrugsManager::currentView() const
{
    return mfDrugsActionHandler::m_CurrentView;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
mfDrugsActionHandler::mfDrugsActionHandler(QObject *parent) :
        QObject(parent),
        aAddRow(0),
        aRemoveRow(0),
        aDown(0),
        aUp(0),
        aSort(0),
        aEdit(0),
        aClear(0),
        aViewInteractions(0),
        gSearchMethod(0),
        aSearchCommercial(0),
        aSearchMolecules(0),
        aSearchInn(0),
        aPrintPrescription(0),
        aToogleTestingDrugs(0),
        m_CurrentView(0)
{
    setObjectName("mfDrugsActionHandler");
    tkLog::addMessage(this, "Instance created");

    tkActionManager *am = tkActionManager::instance();
    QAction *a = 0;
    tkCommand *cmd = 0;
    QList<int> ctx = QList<int>() << tkUID->uniqueIdentifier(mfDrugsConstants::C_DRUGS_PLUGINS);

    tkActionContainer *menu = am->actionContainer(mfDrugsConstants::M_PLUGINS_DRUGS);
    if (!menu) {
        menu = am->createMenu(mfDrugsConstants::M_PLUGINS_DRUGS);
        menu->appendGroup(mfDrugsConstants::G_PLUGINS_SEARCH);
        menu->appendGroup(mfDrugsConstants::G_PLUGINS_DRUGS);
        menu->setTranslations(mfDrugsConstants::DRUGSMENU_TEXT);
    }
    Q_ASSERT(menu);
#ifdef DRUGS_INTERACTIONS_STANDALONE
    am->actionContainer(tkConstants::MENUBAR)->addMenu(menu, mfDrugsConstants::G_PLUGINS_DRUGS);
#else
    am->actionContainer(tkConstants::M_PLUGINS)->addMenu(menu, mfDrugsConstants::G_PLUGINS_DRUGS);
#endif

    // Create local actions
    a = aClear = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONCLEAR));
    cmd = am->registerAction(a, tkConstants::A_LIST_CLEAR, ctx);
    cmd->setTranslations(tkConstants::LISTCLEAR_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    a = aRemoveRow = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONREMOVE));
    cmd = am->registerAction(a, tkConstants::A_LIST_REMOVE, ctx);
    cmd->setTranslations(tkConstants::LISTREMOVE_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(removeItem()));

    a = aDown = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONMOVEDOWN));
    cmd = am->registerAction(a, tkConstants::A_LIST_MOVEDOWN, ctx);
    cmd->setTranslations(tkConstants::LISTMOVEDOWN_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(moveDown()));

    a = aUp = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONMOVEUP));
    cmd = am->registerAction(a, tkConstants::A_LIST_MOVEUP, ctx);
    cmd->setTranslations(tkConstants::LISTMOVEUP_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(moveUp()));

    a = aSort = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONSORT));
    cmd = am->registerAction(a, tkConstants::A_LIST_SORT, ctx);
    cmd->setTranslations(tkConstants::LISTSORT_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(sortDrugs()));

    a = aViewInteractions = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONEYES));
    cmd = am->registerAction(a, tkConstants::A_VIEW_INTERACTIONS, ctx);
    cmd->setTranslations(tkConstants::VIEWINTERACTIONS_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(viewInteractions()));

    a = aToogleTestingDrugs = new QAction(this);
    a->setIcon(tkTheme::icon(mfDrugsConstants::ICONTOOGLETESTINGDRUGS));
    a->setCheckable(true);
    a->setChecked(true);
    cmd = am->registerAction(a, mfDrugsConstants::A_TOOGLE_TESTINGDRUGS, ctx);
    cmd->setTranslations(mfDrugsConstants::TOOGLETESTINGDRUGS_TEXT);
    menu->addAction(cmd, mfDrugsConstants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(toogleTestingDrugs()));

    // Search method menu
    tkActionContainer *searchmenu = am->actionContainer(mfDrugsConstants::M_PLUGINS_SEARCH);
    if (!searchmenu) {
        searchmenu = am->createMenu(mfDrugsConstants::M_PLUGINS_SEARCH);
        searchmenu->appendGroup(mfDrugsConstants::G_PLUGINS_SEARCH);
        searchmenu->setTranslations(mfDrugsConstants::SEARCHMENU_TEXT, DRUGCONSTANTS_TR_CONTEXT);
        menu->addMenu(searchmenu, mfDrugsConstants::G_PLUGINS_SEARCH);
    }
    Q_ASSERT(searchmenu);

    gSearchMethod = new QActionGroup(this);
    a = aSearchCommercial = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(tkTheme::icon(mfDrugsConstants::MFDRUGS_ICONSEARCHCOMMERCIAL));
    cmd = am->registerAction(a, mfDrugsConstants::A_SEARCH_COMMERCIAL, ctx);
    cmd->setTranslations(mfDrugsConstants::SEARCHCOMMERCIAL_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
    searchmenu->addAction(cmd, mfDrugsConstants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchMolecules = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(tkTheme::icon(mfDrugsConstants::MFDRUGS_ICONSEARCHMOLS));
    cmd = am->registerAction(a, mfDrugsConstants::A_SEARCH_MOLECULES, ctx);
    cmd->setTranslations(mfDrugsConstants::SEARCHMOLECULES_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
    searchmenu->addAction(cmd, mfDrugsConstants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);

    a = aSearchInn = new QAction(this);
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(tkTheme::icon(mfDrugsConstants::MFDRUGS_ICONSEARCHINN));
    cmd = am->registerAction(a, mfDrugsConstants::A_SEARCH_INN, ctx);
    cmd->setTranslations(mfDrugsConstants::SEARCHINN_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
    searchmenu->addAction(cmd, mfDrugsConstants::G_PLUGINS_SEARCH);
    gSearchMethod->addAction(a);
    connect(gSearchMethod,SIGNAL(triggered(QAction*)),this,SLOT(searchActionChanged(QAction*)));

    tkActionContainer *filemenu = am->actionContainer(tkConstants::M_FILE);
    a = aPrintPrescription = new QAction(this);
    a->setIcon(tkTheme::icon(tkConstants::ICONPRINT));
    a->setShortcut(tkTr(K_PRINT_PRESCRIPTION));
    cmd = am->registerAction(a, mfDrugsConstants::A_PRINT_PRESCRIPTION, ctx);
    cmd->setTranslations(mfDrugsConstants::PRINTPRESCRIPTION_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
    cmd->setKeySequence(tkTr(K_PRINT_PRESCRIPTION));
    cmd->retranslate();
    if (filemenu) {
        filemenu->addAction(cmd, tkConstants::G_FILE_PRINT);
    }
    connect(aPrintPrescription,SIGNAL(triggered()), this, SLOT(printPrescription()));

    menu->retranslate();
    searchmenu->retranslate();
}

void mfDrugsActionHandler::searchActionChanged(QAction *a)
{
    if (!m_CurrentView)
        return;
    if (a==aSearchCommercial)
        m_CurrentView->setCurrentSearchMethod(mfDrugsConstants::SearchCommercial);
    else if (a==aSearchMolecules)
        m_CurrentView->setCurrentSearchMethod(mfDrugsConstants::SearchMolecules);
    else if (a==aSearchInn)
        m_CurrentView->setCurrentSearchMethod(mfDrugsConstants::SearchInn);
}

void mfDrugsActionHandler::setCurrentView(mfDrugsCentralWidget *view)
{
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        return;
    }
    // disconnect old view
    if (m_CurrentView) {
        if (view == m_CurrentView.data())
            return;
        m_CurrentView->disconnect();
        disconnect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                   this, SLOT(listViewItemChanged()));
        disconnect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
                   this, SLOT(drugsModelChanged()));
    }
    m_CurrentView = view;
    // reconnect some actions
    m_CurrentView->createConnections();
    connect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(listViewItemChanged()));
    connect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
            this, SLOT(drugsModelChanged()));
    updateActions();
}

void mfDrugsActionHandler::listViewItemChanged()
{
    aUp->setEnabled(canMoveUp());
    aDown->setEnabled(canMoveDown());
}

void mfDrugsActionHandler::drugsModelChanged()
{
    if (!m_CurrentView)
        return;
    aViewInteractions->setEnabled(m_CurrentView->currentDrugsModel()->prescriptionHasInteractions());
}

void mfDrugsActionHandler::updateActions()
{
    listViewItemChanged();
    drugsModelChanged();
}

bool mfDrugsActionHandler::canMoveUp()
{
    if (!m_CurrentView)
        return false;
    QModelIndex idx = m_CurrentView->prescriptionListView()->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() >= 1)
        return true;
    return false;
}

bool mfDrugsActionHandler::canMoveDown()
{
    if (!m_CurrentView)
        return false;
    QModelIndex idx = m_CurrentView->prescriptionListView()->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.row() < (m_CurrentView->prescriptionListView()->model()->rowCount()-1))
        return true;
    return false;
}

void mfDrugsActionHandler::moveUp()
{
    if (m_CurrentView)
        m_CurrentView->prescriptionView()->moveUp();
}

void mfDrugsActionHandler::moveDown()
{
    if (m_CurrentView)
        m_CurrentView->prescriptionView()->moveDown();
}

void mfDrugsActionHandler::sortDrugs()
{
    if (m_CurrentView)
       m_CurrentView->currentDrugsModel()->sort(0);
}

void mfDrugsActionHandler::removeItem()
{
    if (m_CurrentView)
        m_CurrentView->prescriptionView()->removeTriggered();
}

void mfDrugsActionHandler::clear()
{
    if (m_CurrentView)
        m_CurrentView->prescriptionView()->clearTriggered();
}

void mfDrugsActionHandler::viewInteractions()
{
    if (m_CurrentView)
        m_CurrentView->prescriptionView()->viewInteractions();
}

void mfDrugsActionHandler::printPrescription()
{
    if (m_CurrentView)
        m_CurrentView->printPrescription();
}

void mfDrugsActionHandler::toogleTestingDrugs()
{
    if (m_CurrentView)
       m_CurrentView->currentDrugsModel()->showTestingDrugs(aToogleTestingDrugs->isChecked());
}
