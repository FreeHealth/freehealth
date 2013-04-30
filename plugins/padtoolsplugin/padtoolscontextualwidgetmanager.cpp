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
 *       Eric Maeker <>                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class PadTools::Internal::PadToolsContextualWidgetManager
 * \brief Manages contextualized widget and action handler for the PadTools plugin.
 * \sa Core::IContext, Core::ContextManager, Core::ICore::contextManager(), Core::ICore::actionManager()
 */

#include "padtoolscontextualwidgetmanager.h"
//#include "padtoolscontextualwidget.h"
#include "constants.h"
#include "padwriter.h"

#ifdef FREEDIAMS
#   include <drugsplugin/constants.h>
#endif

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

using namespace PadTools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

// Register an existing Core action
//static QAction *registerAction(const QString &id, const Core::Context &ctx, QObject *parent)
//{
//    QAction *a = new QAction(parent);
//    Core::Command *cmd = Core::ICore::instance()->actionManager()->registerAction(a, Core::Id(id), ctx);
//    Q_UNUSED(cmd);
//    return a;
//}

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
PadToolsContextualWidgetManager::PadToolsContextualWidgetManager(QObject *parent) :
    PadToolsActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*,Core::Context)));
    setObjectName("PadToolsContextualWidgetManager");
}

PadToolsContextualWidgetManager::~PadToolsContextualWidgetManager()
{
}

/**
 * Slot called when the Core::ContextManager context changed.
 * If the context is a PadTools::PadToolsContextualWidget the action handler current view is updated.
 * \sa PadTools::Internal::PadToolsActionHandler
 */
void PadToolsContextualWidgetManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(additionalContexts);
    //    qWarning() << "PadToolsContextualWidgetManager::updateContext(Core::IContext *object)";
    //    if (object)
    //        qWarning() << "PadToolsContextualWidgetManager::updateContext(Core::IContext *object)" << object->widget();

    PadTools::Internal::PadWriter *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;
            break;
        }
        QWidget *testParents = object->widget();
        while (!view && testParents) {
            view = qobject_cast<PadTools::Internal::PadWriter *>(testParents);
            if (!view)
                testParents = testParents->parentWidget();
        }
        if (!view) {
            if (!m_CurrentView)
                return;
            break;
        }

        if (view == m_CurrentView) {
            return;
        }

    } while (false);

    if (view)
        PadToolsActionHandler::setCurrentView(view);
}

PadWriter *PadToolsContextualWidgetManager::currentView() const
{
    return PadToolsActionHandler::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
PadToolsActionHandler::PadToolsActionHandler(QObject *parent) :
    QObject(parent),
    aShowSource(0),
    aViewOutput(0),
    m_CurrentView(0)
{
    setObjectName("PadToolsActionHandler");

//    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context ctx(PadTools::Constants::C_PADTOOLS_PLUGINS);
//    Core::Context global(Core::Constants::C_GLOBAL);

    // Create the plugin specific menu
    Core::ActionContainer *menu = actionManager()->actionContainer(PadTools::Constants::M_PLUGIN_PADTOOLS);
    if (!menu) {
        menu = actionManager()->createMenu(PadTools::Constants::M_PLUGIN_PADTOOLS);
        //        menu->appendGroup(PadTools::Constants::G_PLUGINS_VIEWS);
        //        menu->appendGroup(PadTools::Constants::G_PLUGINS_MODES);
        //        menu->appendGroup(PadTools::Constants::G_PLUGINS_SEARCH);
        //        menu->appendGroup(PadTools::Constants::G_PLUGINS_DRUGS);
        //        menu->appendGroup(PadTools::Constants::G_PLUGINS_INTERACTIONS);
        menu->setTranslations(PadTools::Constants::PADTOOLS_TEXT);

        // Add the menu to the menubar or to the plugin menu
#ifdef FREEDIAMS
        actionManager()->actionContainer(Core::Id(DrugsWidget::Constants::M_PLUGINS_DRUGS))->addMenu(menu, Core::Constants::G_PLUGINS_PADTOOLS);
#else
#   ifdef FREEMEDFORMS
        actionManager()->actionContainer(Core::Id(Core::Constants::M_PLUGINS))->addMenu(menu, Core::Constants::G_PLUGINS_PADTOOLS);
#   else
        actionManager()->actionContainer(Core::Id(Core::Constants::MENUBAR))->addMenu(menu, Core::Constants::G_PLUGINS_PADTOOLS);
#   endif
#endif
    }
    Q_ASSERT(menu);

    // Example: register an existing Core action
    //    aClear = registerAction(Core::Constants::A_LIST_CLEAR, ctx, this);
    //    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    //    a = aClear = new QAction(this);
    //    a->setObjectName("PadTools.aClear");
    //    a->setIcon(theme()->icon(Core::Constants::ICONCLEAR));
    //    cmd = actionManager()->registerAction(a, Core::Constants::A_LIST_CLEAR, ctx);
    //    cmd->setTranslations(Trans::Constants::LISTCLEAR_TEXT);
    //    menu->addAction(cmd, Core::Constants::G_DEFAULT_ONE);
    //    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    aShowSource = createAction(this, "aShowSource", Core::Constants::ICONHELP,
                               Constants::A_PADTOOLS_SHOWSOURCE,
                               ctx,
                               Constants::SHOW_SOURCE, Constants::PADWRITER_TRANS_CONTEXT,
                               cmd,
                               0, "",
                               QKeySequence::UnknownKey, false);
    connect(aShowSource, SIGNAL(triggered()), this, SLOT(onShowSourceRequested()));

    aViewOutput = createAction(this, "aViewOutput", Core::Constants::ICONHELP,
                                     Constants::A_PADTOOLS_VIEWOUTPUT,
                                     ctx,
                                     Constants::VIEW_OUTPUT, Constants::PADWRITER_TRANS_CONTEXT,
                                     cmd,
                                     0, "",
                                     QKeySequence::UnknownKey, false);
    connect(aViewOutput, SIGNAL(triggered()), this, SLOT(onViewOutputRequested()));

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
}

/** Define the current view, update and connect actions */
void PadToolsActionHandler::setCurrentView(PadWriter *view)
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

void PadToolsActionHandler::updateActions()
{
    // Proceed actions dis/enableing
}

void PadToolsActionHandler::onViewOutputRequested()
{
    if (m_CurrentView)
        m_CurrentView->onViewOutputRequested();
}

void PadToolsActionHandler::onShowSourceRequested()
{
    if (m_CurrentView)
        m_CurrentView->onShowSourceRequested();
}


