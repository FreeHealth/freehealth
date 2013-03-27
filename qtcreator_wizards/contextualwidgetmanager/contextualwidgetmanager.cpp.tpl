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
 *  Main Developers:                                                       *
 *       %Author% <%AuthorEmail%>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class %PluginName%::Internal::%PluginName%ContextualWidgetManager
 * \brief Manages contextualized widget and action handler for the %PluginName% plugin.
 * \sa Core::IContext, Core::ContextManager, Core::ICore::contextManager(), Core::ICore::actionManager()
 */

#include "%PluginName:l%contextualwidgetmanager.h"
#include "%PluginName:l%contextualwidget.h"
#include "constants.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_database.h>

#include <QAction>
#include <QDialog>
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>

#include <QDebug>

using namespace %PluginName%;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

// Register an existing Core action
static QAction *registerAction(const QString &id, const Core::Context &ctx, QObject *parent)
{
    QAction *a = new QAction(parent);
    Core::Command *cmd = Core::ICore::instance()->actionManager()->registerAction(a, Core::Id(id), ctx);
    Q_UNUSED(cmd);
    return a;
}

// Create an action
static inline QAction *createAction(QObject *parent, const QString &name, const QString &icon,
                                    const QString &actionName,
                                    const Core::Context &context,
                                    const QString &trans, const QString &transContext,
                                    Core::Command *cmd,
                                    Core::ActionContainer *menu,
                                    const QString &group,
                                    QKeySequence::StandardKey key = QKeySequence::UnknownKey,
                                    bool checkable = false)
{
    QAction *a = new QAction(parent);
    a->setObjectName(name);
    if (!icon.isEmpty())
        a->setIcon(theme()->icon(icon));
    if (checkable) {
        a->setCheckable(true);
        a->setChecked(false);
    }
    cmd = actionManager()->registerAction(a, Core::Id(actionName), context);
    if (!transContext.isEmpty())
        cmd->setTranslations(trans, trans, transContext);
    else
        cmd->setTranslations(trans, trans); // use the Trans::Constants tr context (automatic)
    if (key != QKeySequence::UnknownKey)
        cmd->setDefaultKeySequence(key);
    if (menu)
        menu->addAction(cmd, Core::Id(group));
    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
%PluginName%ContextualWidgetManager::%PluginName%ContextualWidgetManager(QObject *parent) :
    %PluginName%ActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*,Core::Context)));
    setObjectName("%PluginName%ContextualWidgetManager");
}

%PluginName%ContextualWidgetManager::~%PluginName%ContextualWidgetManager()
{
}

/**
 * Slot called when the Core::ContextManager context changed.
 * If the context is a %PluginName%::%PluginName%ContextualWidget the action handler current view is updated.
 * \sa %PluginName%::Internal::%PluginName%ActionHandler
 */
void %PluginName%ContextualWidgetManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(additionalContexts);
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

%PluginName%ContextualWidget *%PluginName%ContextualWidgetManager::currentView() const
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
    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context ctx(%PluginName%::Constants::C_%PluginName:u%_PLUGINS);
    Core::Context global(Core::Constants::C_GLOBAL);

    // Create the plugin specific menu
    Core::ActionContainer *menu = actionManager()->actionContainer(%PluginName%::Constants::M_PLUGIN_%PluginName:u%);
    if (!menu) {
        menu = actionManager()->createMenu(%PluginName%::Constants::M_PLUGIN_%PluginName:u%);
//        menu->appendGroup(%PluginName%::Constants::G_PLUGINS_VIEWS);
//        menu->appendGroup(%PluginName%::Constants::G_PLUGINS_MODES);
//        menu->appendGroup(%PluginName%::Constants::G_PLUGINS_SEARCH);
//        menu->appendGroup(%PluginName%::Constants::G_PLUGINS_DRUGS);
//        menu->appendGroup(%PluginName%::Constants::G_PLUGINS_INTERACTIONS);
//        menu->setTranslations(%PluginName%::Constants::DRUGSMENU_TEXT);

    // Add the menu to the menubar or to the plugin menu
#ifndef FREEMEDFORMS
        actionManager()->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, Core::Constants::G_PLUGINS_%PluginName:u%);
#else
        actionManager()->actionContainer(Core::Constants::MENUBAR)->addMenu(menu, Core::Constants::G_PLUGINS);
#endif
    }
    Q_ASSERT(menu);

    // Example: register an existing Core action
    aClear = registerAction(Core::Constants::A_LIST_CLEAR, ctx, this);
    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

//    a = aClear = new QAction(this);
//    a->setObjectName("%PluginName%.aClear");
//    a->setIcon(theme()->icon(Core::Constants::ICONCLEAR));
//    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_CLEAR, ctx);
//    cmd->setTranslations(Trans::Constants::LISTCLEAR_TEXT);
//    menu->addAction(cmd, Core::Constants::G_DEFAULT_ONE);
//    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    // Example: register your own actions: db info in the help menu
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    aShowDatabaseInformation = createAction(this, "aShowDatabaseInformation", Core::Constants::ICONHELP,
                                            Constants::A_SHOWPATIENTLASTEPISODES,
                                            allContexts,
                                            Trans::Constants::%PluginName:u%_DATABASE_INFORMATION, "",
                                            cmd,
                                            hmenu, Core::Constants::G_HELP_DATABASES,
                                            QKeySequence::UnknownKey, false);
    aShowDatabaseInformation->setEnabled(false);
    connect(aShowDatabaseInformation,SIGNAL(triggered()), this, SLOT(showDatabaseInformation()));

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
//    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(updateActions()));
}

/** Define the current view, update and connect actions */
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

