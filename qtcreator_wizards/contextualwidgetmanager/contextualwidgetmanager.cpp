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
 *   Main Developers:                                                      *
 *       %Author% <%AuthorEmail%>                             *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class %PluginName%::Internal::%PluginName%ContextualWidgetManager
 * \brief Manages contextualized widget and action handler for the %PluginName% plugin.
 * \sa Core::IContext, Core::ContextManager, Core::ICore::contextManager(), Core::ICore::actionManager()
 */

#include "%PluginName:l%contextualwidgetmanager.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/uniqueidmanager.h>

#include <utils/log.h>
#include <utils/global.h>

#include <QDialog>
#include <QGridLayout>
#include <QTreeWidget>

#include <QDebug>

using namespace %PluginName%;
using namespace Internal;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
%PluginName%ContextualWidgetManager::%PluginName%ContextualWidgetManager(QObject *parent) :
    %PluginName%ActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
    setObjectName("%PluginName%ContextualWidgetManager");
}

void %PluginName%ContextualWidgetManager::updateContext(Core::IContext *object)
{
    //    qWarning() << "%PluginName%ContextualWidgetManager::updateContext(Core::IContext *object)";
    //    if (object)
    //        qWarning() << "%PluginName%ContextualWidgetManager::updateContext(Core::IContext *object)" << object->widget();

    %PluginName%ContextualWidget *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;

            //            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
            break;
        }
        view = qobject_cast<%PluginName%ContextualWidget *>(object->widget());
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
        %PluginName%ActionHandler::setCurrentView(view);
    }
}

%PluginName%ContextualWidget *%PluginName%ContextualWidget::currentView() const
{
    return %PluginName%ActionHandler::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
%PluginName%ActionHandler::%PluginName%ActionHandler(QObject *parent) :
    QObject(parent),
    aClear(0),
    aShowDatabaseInformation(0),
    m_CurrentView(0)
{
    setObjectName("%PluginName%ActionHandler");

    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();

    QAction *a = 0;
    Core::Command *cmd = 0;
    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(%PluginName%::Constants::C_%PluginName:u%_PLUGINS);

    // Create the plugin specific menu
    Core::ActionContainer *menu = actionManager()->actionContainer(%PluginName%::Constants::M_PLUGIN_%PluginName:u%);
    if (!menu) {
        menu = actionManager()->createMenu(DrugsWidget::Constants::M_PLUGINS_%PluginName:u%);
//        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_VIEWS);
//        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_MODES);
//        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_SEARCH);
//        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_DRUGS);
//        menu->appendGroup(DrugsWidget::Constants::G_PLUGINS_INTERACTIONS);
//        menu->setTranslations(DrugsWidget::Constants::DRUGSMENU_TEXT);

    // Add the menu to the menubar or to the plugin menu
#ifndef FREEMEDFORMS
        actionManager()->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, Core::Constants::G_PLUGINS_%PluginName:u%);
#else
        actionManager()->actionContainer(Core::Constants::MENUBAR)->addMenu(menu, Core::Constants::G_PLUGINS);
#endif
    }
    Q_ASSERT(menu);

    // Create local actions
    a = aClear = new QAction(this);
    a->setIcon(theme()->icon(Core::Constants::ICONCLEAR));
    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_CLEAR, ctx);
    cmd->setTranslations(Trans::Constants::LISTCLEAR_TEXT);
    menu->addAction(cmd, %PluginName%::Constants::G_PLUGINS_DRUGS);
    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    // Databases information
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    a = aShowDatabaseInformation = new QAction(this);
    a->setIcon(theme()->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, %PluginName%::Constants::A_DATABASE_INFORMATION, QList<int>() << Core::Constants::C_GLOBAL_ID);
    cmd->setTranslations(Trans::Constants::%PluginName:u%_DATABASE_INFORMATION);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Constants::G_HELP_DATABASES);
    }
    connect(aShowDatabaseInformation,SIGNAL(triggered()), this, SLOT(showDatabaseInformation()));

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
}

void %PluginName%ActionHandler::setCurrentView(%PluginName%ContextualWidget *view)
{
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        LOG_ERROR("setCurrentView: no view");
        return;
    }

    // disconnect old view
    if (m_CurrentView) {
//        if (view == m_CurrentView.data())
//            return;
//        m_CurrentView->disconnect();
//        disconnect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//                   this, SLOT(listViewItemChanged()));
//        disconnect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
//                   this, SLOT(drugsModelChanged()));
//        m_CurrentView->drugSelector()->disconnectFilter();
    }
    m_CurrentView = view;

    // connect new view
//    DrugsDB::DrugsModel::setActiveModel(view->currentDrugsModel());
//    m_CurrentView->createConnections();
//    connect(m_CurrentView->prescriptionListView()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
//            this, SLOT(listViewItemChanged()));
//    connect(m_CurrentView->currentDrugsModel(), SIGNAL(numberOfRowsChanged()),
//            this, SLOT(drugsModelChanged()));
//    m_CurrentView->drugSelector()->connectFilter();

    // update actions according to the current view
    updateActions();
}

void %PluginName%ActionHandler::updateActions()
{
    // Proceed actions dis/enableing
}

// Eg of action interacting with the currentview
void %PluginName%ActionHandler::clear()
{
    if (m_CurrentView) {
        m_CurrentView->clear();
    }
}

void %PluginName%ActionHandler::showDatabaseInformation()
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

