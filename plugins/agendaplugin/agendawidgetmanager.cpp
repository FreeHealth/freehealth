/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "agendawidgetmanager.h"
#include <agendaplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <QDialog>
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>

using namespace Agenda::Constants;
using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
AgendaWidgetManager *AgendaWidgetManager::m_Instance = 0;

AgendaWidgetManager *AgendaWidgetManager::instance()
{
    if (!m_Instance)
        m_Instance = new AgendaWidgetManager(qApp);
    return m_Instance;
}

AgendaWidgetManager::AgendaWidgetManager(QObject *parent) :
    AgendaActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*,Core::Context)));
    setObjectName("AgendaWidgetManager");
}

void AgendaWidgetManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(object);
    //    qWarning() << "DrugsManager::updateContext(Core::IContext *object)";
    //    if (object)
    //        qWarning() << "DrugsManager::updateContext(Core::IContext *object)" << object->widget();

    QWidget *view = 0;
//    do {
//        if (!object) {
//            if (!m_CurrentView)
//                return;

//            //            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
//            break;
//        }
//        view = qobject_cast<DrugsCentralWidget *>(object->widget());
//        if (!view) {
//            if (!m_CurrentView)
//                return;

//            //            m_CurrentView = 0;   // keep trace of the last active view (we need it in dialogs)
//            break;
//        }

//        if (view == m_CurrentView) {
//            return;
//        }

//    } while (false);
    if (view) {
        AgendaActionHandler::setCurrentView(view);
    }
}

QWidget *AgendaWidgetManager::currentView() const
{
    return AgendaActionHandler::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
AgendaActionHandler::AgendaActionHandler(QObject *parent) :
        QObject(parent),
        aClear(0),
        aNewEvent(0),
        aPrintSelection(0),
        aPrintPreviewSelection(0),
        aAgendaDatabaseInformation(0),
        m_CurrentView(0)
{
    setObjectName("AgendaActionHandler");
    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context ctx(Agenda::Constants::C_AGENDA_PLUGIN);
    Core::Context globalcontext(Core::Constants::C_GLOBAL);

//    Core::ActionContainer *menu = actionManager()->actionContainer(Constants::M_PLUGINS_AGENDA);
//    if (!menu) {
//        menu = actionManager()->createMenu(Constants::M_PLUGINS_AGENDA);
//        menu->appendGroup(Constants::G_AGENDA_VIEW);
//        menu->appendGroup(Constants::G_AGENDA_EDIT);
//        menu->appendGroup(Constants::G_AGENDA_OPTIONS);
//        menu->setTranslations(Trans::Constants::AGENDA);
//    }
//    Q_ASSERT(menu);
//    actionManager()->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, Core::Constants::G_PLUGINS_CALENDAR);

    // Create local actions
//    a = aClear = new QAction(this);
//    a->setIcon(theme()->icon(Core::Constants::ICONCLEAR));
//    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_CLEAR, ctx);
//    cmd->setTranslations(Trans::Constants::LISTCLEAR_TEXT);
//    menu->addAction(cmd, Constants::G_AGENDA_EDIT);
//    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    // Search method menu
//    Core::ActionContainer *searchmenu = actionManager()->actionContainer(DrugsWidget::Constants::M_PLUGINS_SEARCH);
//    if (!searchmenu) {
//        searchmenu = actionManager()->createMenu(DrugsWidget::Constants::M_PLUGINS_SEARCH);
//        searchmenu->appendGroup(DrugsWidget::Constants::G_PLUGINS_SEARCH);
//        searchmenu->setTranslations(Trans::Constants::SEARCHMENU_TEXT);
//        menu->addMenu(searchmenu, DrugsWidget::Constants::G_PLUGINS_SEARCH);
//    }
//    Q_ASSERT(searchmenu);

//    gSearchMethod = new QActionGroup(this);
//    int m = settings()->value(Constants::S_SEARCHMETHOD).toInt();
//    a = aSearchCommercial = new QAction(this);
//    a->setCheckable(true);
//    a->setChecked(m==Constants::SearchCommercial);
//    a->setIcon(theme()->icon(DrugsDB::Constants::I_SEARCHCOMMERCIAL));
//    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SEARCH_COMMERCIAL, ctx);
//    cmd->setTranslations(DrugsWidget::Constants::SEARCHCOMMERCIAL_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
//    cmd->setAttribute(Core::Command::CA_UpdateText);
//    searchmenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_SEARCH);
//    gSearchMethod->addAction(a);

//    a = aSearchMolecules = new QAction(this);
//    a->setCheckable(true);
//    a->setChecked(m==Constants::SearchMolecules);
//    a->setIcon(theme()->icon(DrugsDB::Constants::I_SEARCHMOLS));
//    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SEARCH_MOLECULES, ctx);
//    cmd->setTranslations(DrugsWidget::Constants::SEARCHMOLECULES_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
//    cmd->setAttribute(Core::Command::CA_UpdateText);
//    searchmenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_SEARCH);
//    gSearchMethod->addAction(a);

//    a = aSearchInn = new QAction(this);
//    a->setCheckable(true);
//    a->setChecked(m==Constants::SearchInn);
//    a->setIcon(theme()->icon(DrugsDB::Constants::I_SEARCHINN));
//    cmd = actionManager()->registerAction(a, DrugsWidget::Constants::A_SEARCH_INN, ctx);
//    cmd->setTranslations(DrugsWidget::Constants::SEARCHINN_TEXT, "", DRUGCONSTANTS_TR_CONTEXT);
//    cmd->setAttribute(Core::Command::CA_UpdateText);
//    searchmenu->addAction(cmd, DrugsWidget::Constants::G_PLUGINS_SEARCH);
//    gSearchMethod->addAction(a);
//    connect(gSearchMethod,SIGNAL(triggered(QAction*)),this,SLOT(searchActionChanged(QAction*)));

    // New agenda event
    Core::ActionContainer *fmenu = actionManager()->actionContainer(Core::Constants::M_GENERAL_NEW);
    a = aNewEvent = new QAction(this);
    QIcon icon;
    icon.addFile(theme()->iconFullPath(Constants::I_NEW_AGENDAEVENT, Core::ITheme::SmallIcon), QSize(16, 16));
    icon.addFile(theme()->iconFullPath(Constants::I_NEW_AGENDAEVENT, Core::ITheme::MediumIcon), QSize(32, 32));
    a->setIcon(icon);
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_NEW_AGENDAEVENT), globalcontext);
    cmd->setTranslations(Trans::Constants::AGENDA_EVENT, Trans::Constants::AGENDA_EVENT);
//    cmd->setKeySequence(QKeySequence::Print);
    cmd->retranslate();
    if (fmenu) {
        fmenu->addAction(cmd, Core::Id(Core::Constants::G_GENERAL_NEW));
    }
//    connect(aNewEvent,SIGNAL(triggered()), this, SLOT(newEvent()));

    a = aPrintSelection = new QAction(this);
    a->setIcon(theme()->icon(Core::Constants::ICONPRINT));
    //    a->setShortcut(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_PRINT_SELECTION), ctx);
    cmd->setTranslations(Constants::PRINTSELECTION_TEXT, Constants::PRINTSELECTION_TEXT, AGENDA_TR_CONTEXT);
//    cmd->setKeySequence(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION));
    cmd->retranslate();
//    if (fmenu) {
//        fmenu->addAction(cmd, Core::Id(Core::Constants::G_FILE_PRINT));
//    }
    connect(aPrintSelection,SIGNAL(triggered()), this, SLOT(printSelection()));

//    a = aPrintPreview = new QAction(this);
//    a->setIcon(theme()->icon(Core::Constants::ICONPRINTPREVIEW));
//    //    a->setShortcut(tkTr(Trans::Constants::K_PRINT_PRESCRIPTION));
//    cmd = actionManager()->registerAction(a, Core::Constants::A_FILE_PRINTPREVIEW, ctx);
//    cmd->setTranslations(Trans::Constants::PRINTPREVIEW_TEXT, Trans::Constants::PRINTPREVIEW_TEXT);
//    cmd->retranslate();
//    if (fmenu) {
//        fmenu->addAction(cmd, Core::Constants::G_FILE_PRINT);
//    }
//    connect(aPrintPreview,SIGNAL(triggered()), this, SLOT(printPreview()));


    // Databases information
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Id(Core::Constants::M_HELP_DATABASES));
    a = aAgendaDatabaseInformation = new QAction(this);
    a->setIcon(theme()->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_AGENDADATABASE_INFORMATION), globalcontext);
    cmd->setTranslations(Trans::Constants::AGENDA_DATABASE_INFORMATION);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Id(Core::Constants::G_HELP_DATABASES));
    }
    connect(aAgendaDatabaseInformation,SIGNAL(triggered()), this, SLOT(showAgendaDatabaseInformation()));

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
}

//void AgendaActionHandler::searchActionChanged(QAction *a)
//{
//    if (!m_CurrentView)
//        return;
//    if (a==aSearchCommercial)
//        m_CurrentView->setCurrentSearchMethod(DrugsWidget::Constants::SearchCommercial);
//    else if (a==aSearchMolecules)
//        m_CurrentView->setCurrentSearchMethod(DrugsWidget::Constants::SearchMolecules);
//    else if (a==aSearchInn)
//        m_CurrentView->setCurrentSearchMethod(DrugsWidget::Constants::SearchInn);
//}

void AgendaActionHandler::setCurrentView(QWidget *view)
{
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        LOG_ERROR("setCurrentView: no view");
        return;
    }
    //    qWarning() << "AgendaActionHandler::setCurrentView(DrugsCentralWidget *view)";

    // disconnect old view
//    if (m_CurrentView) {
//        if (view == m_CurrentView.data())
//            return;
//        m_CurrentView->disconnect();
//        disconnect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//                   this, SLOT(listViewItemChanged()));
//        disconnect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
//                   this, SLOT(drugsModelChanged()));
//        m_CurrentView->drugSelector()->disconnectFilter();
//    }
//    m_CurrentView = view;

//    DrugsDB::DrugsModel::setActiveModel(view->currentDrugsModel());
//    // reconnect some actions
//    m_CurrentView->createConnections();
//    connect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//            this, SLOT(listViewItemChanged()));
//    connect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
//            this, SLOT(drugsModelChanged()));
//    m_CurrentView->drugSelector()->connectFilter();
//    updateActions();
}

void AgendaActionHandler::updateActions()
{
}

void AgendaActionHandler::clear()
{
//    if (m_CurrentView) {
//        Q_ASSERT(m_CurrentView->prescriptionView());
//        m_CurrentView->prescriptionView()->clearTriggered();
//    }
}

//void AgendaActionHandler::newEvent()
//{
//}

void AgendaActionHandler::printSelection()
{
//    if (m_CurrentView)
//        m_CurrentView->printPrescription();
}

void AgendaActionHandler::printPreviewSelection()
{
//    if (m_CurrentView) {
//        Q_ASSERT(m_CurrentView->currentDrugsModel());
//        m_CurrentView->currentDrugsModel()->showTestingDrugs(aToggleTestingDrugs->isChecked());
//    }
}

void AgendaActionHandler::showAgendaDatabaseInformation()
{
    QDialog dlg(qApp->activeWindow(), Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    QGridLayout lay(&dlg);
    QTreeWidget tree(&dlg);
    tree.setColumnCount(2);
    tree.header()->hide();

    lay.addWidget(&tree);
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}
