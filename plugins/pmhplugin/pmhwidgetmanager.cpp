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
#include "pmhwidgetmanager.h"
#include "pmhcontextualwidget.h"
#include "constants.h"
#include "pmhcreatordialog.h"
#include "pmhcore.h"
#include "pmhcategorymodel.h"
#include "pmhbase.h"

#include <categoryplugin/categorydialog.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>

#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/uniqueidmanager.h>

#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>


using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

inline static Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::IMainWindow *mainWindow() { return Core::ICore::instance()->mainWindow(); }
static inline PMH::PmhCore *pmhCore() { return PMH::PmhCore::instance(); }
static inline PMH::Internal::PmhBase *pmhBase() { return PMH::Internal::PmhBase::instance(); }
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
PmhWidgetManager *PmhWidgetManager::m_Instance = 0;

PmhWidgetManager *PmhWidgetManager::instance(QObject *parent)
{
    if (!m_Instance)
        m_Instance = new PmhWidgetManager(parent);
    return m_Instance;
}

PmhWidgetManager::PmhWidgetManager(QObject *parent) : PmhActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
    setObjectName("PmhWidgetManager");
}

void PmhWidgetManager::updateContext(Core::IContext *object)
{
    PmhContextualWidget *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

            //            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
            break;
        }
        view = qobject_cast<PmhContextualWidget *>(object->widget());
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
        PmhActionHandler::setCurrentView(view);
    }
}

PmhContextualWidget *PmhWidgetManager::currentView() const
{
    return PmhActionHandler::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
PmhActionHandler::PmhActionHandler(QObject *parent) :
        QObject(parent),
        aAddPmh(0),aRemovePmh(0),
        aAddCat(0),
        aCategoryManager(0),
        aPmhDatabaseInformation(0),
        m_CurrentView(0)
{
    setObjectName("PmhActionHandler");

    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    Core::ITheme *th = Core::ICore::instance()->theme();

    QAction *a = 0;
    Core::Command *cmd = 0;
    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(Constants::C_PMH_PLUGINS);
    QList<int> globalcontext = QList<int>() << Core::Constants::C_GLOBAL_ID;

    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Constants::M_PATIENTS);
    Core::ActionContainer *newmenu = actionManager()->actionContainer(Core::Constants::M_GENERAL_NEW);
    Core::ActionContainer *pmhMenu = actionManager()->createMenu(Constants::M_PMH);
    Q_ASSERT(menu);
    if (!menu) {
        LOG_ERROR("Menu Patient not created");
        return;
    } else {
//        menu->appendGroup(Core::Constants::G_PATIENTS_NAVIGATION);
        pmhMenu->appendGroup(Constants::G_PMH_NEW);
        pmhMenu->appendGroup(Constants::G_PMH_EDITION);
        pmhMenu->setTranslations(Trans::Constants::PMHMENU_TEXT);
        menu->addMenu(pmhMenu, Core::Constants::G_PATIENTS);
    }

    // Create local actions
    a = aAddPmh = new QAction(this);
    aAddPmh->setEnabled(false);
    a->setObjectName("aAddPmh");
    a->setIcon(th->icon(Core::Constants::ICONADD));
    cmd = actionManager()->registerAction(a, Constants::A_PMH_NEW, globalcontext);
    cmd->setTranslations(Constants::CREATEPMH_TEXT, Constants::CREATEPMH_TEXT, Constants::PMHCONSTANTS_TR_CONTEXT);
    pmhMenu->addAction(cmd, Constants::G_PMH_NEW);
    if (newmenu)
        newmenu->addAction(cmd, Core::Constants::G_GENERAL_NEW);
//    connect(a, SIGNAL(triggered()), this, SLOT(createPmh()));

    a = aRemovePmh= new QAction(this);
    a->setObjectName("aRemovePmh");
    a->setIcon(th->icon(Core::Constants::ICONREMOVE));
    cmd = actionManager()->registerAction(a, Constants::A_PMH_REMOVE, ctx);
    cmd->setTranslations(Constants::REMOVEPMH_TEXT, Constants::REMOVEPMH_TEXT, Constants::PMHCONSTANTS_TR_CONTEXT);
    pmhMenu->addAction(cmd, Constants::G_PMH_NEW);
//    connect(a, SIGNAL(triggered()), this, SLOT(createPmh()));

    a = aAddCat = new QAction(this);
    a->setObjectName("aAddCat");
    a->setIcon(th->icon(Core::Constants::ICONCATEGORY_ADD));
    cmd = actionManager()->registerAction(a, Constants::A_PMH_NEWCATEGORY, ctx);
    cmd->setTranslations(Constants::CREATECATEGORY_TEXT, Constants::CREATECATEGORY_TEXT, Constants::PMHCONSTANTS_TR_CONTEXT);
    pmhMenu->addAction(cmd, Constants::G_PMH_NEW);
//    connect(a, SIGNAL(triggered()), this, SLOT(createPmh()));

    a = aCategoryManager= new QAction(this);
    a->setObjectName("aCategoryManager");
    a->setIcon(th->icon(Core::Constants::ICONCATEGORY_MANAGER));
    cmd = actionManager()->registerAction(a, Constants::A_PMH_CATEGORYMANAGER, globalcontext);
    cmd->setTranslations(Constants::CATEGORYMANAGER_TEXT, Constants::CATEGORYMANAGER_TEXT, Constants::PMHCONSTANTS_TR_CONTEXT);
    pmhMenu->addAction(cmd, Constants::G_PMH_EDITION);
    connect(a, SIGNAL(triggered()), this, SLOT(categoryManager()));

    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    a = aPmhDatabaseInformation = new QAction(this);
    a->setObjectName("aPmhDatabaseInformation");
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Constants::A_PMH_SHOWDBINFOS, globalcontext);
    cmd->setTranslations(Trans::Constants::PMH_DATABASE_INFORMATION);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);
    }
    connect(aPmhDatabaseInformation, SIGNAL(triggered()), this, SLOT(showPmhDatabaseInformation()));

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();

    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(patientChanged()));
}

void PmhActionHandler::setCurrentView(PmhContextualWidget *view)
{
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        LOG_ERROR("setCurrentView: no view");
        return;
    }
    //    qWarning() << "PmhActionHandler::setCurrentView(DrugsCentralWidget *view)";

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
    m_CurrentView = view;

//    DrugsDB::DrugsModel::setActiveModel(view->currentDrugsModel());
//    // reconnect some actions
//    m_CurrentView->createConnections();
//    connect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//            this, SLOT(listViewItemChanged()));
//    connect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
//            this, SLOT(drugsModelChanged()));
//    m_CurrentView->drugSelector()->connectFilter();
    updateActions();
}

void PmhActionHandler::updateActions()
{
}

void PmhActionHandler::patientChanged()
{
    if (aAddPmh->isEnabled()) {
        disconnect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(patientChanged()));
    }
    aAddPmh->setEnabled(true);
}

void PmhActionHandler::showPmhDatabaseInformation()
{
    QDialog dlg(mainWindow(), Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    QGridLayout lay(&dlg);
    QTreeWidget tree(&dlg);
    tree.setColumnCount(2);
    tree.header()->hide();
    pmhBase()->toTreeWidget(&tree);
    lay.addWidget(&tree);
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}

void PmhActionHandler::categoryManager()
{
    Category::CategoryDialog dlg(mainWindow());
    dlg.setCategoryModel(pmhCore()->pmhCategoryModel(), PmhCategoryModel::Label);
    dlg.exec();
}
