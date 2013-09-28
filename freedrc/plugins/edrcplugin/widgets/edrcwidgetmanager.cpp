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
 *       Eric Maeker <eric.maeker@gmail.com>                               *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class eDRC::Internal::EdrcWidgetManager
 * \brief Manages contextualized widget and action handler for the eDRC plugin.
 * \sa Core::IContext, Core::ContextManager, Core::ICore::contextManager(), Core::ICore::actionManager()
 */

#include "edrcwidgetmanager.h"
#include "edrcwontextualwidget.h"
#include <edrcplugin/constants.h>

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

using namespace eDRC;
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
static inline QAction *createAction(QObject *parent, const QString &actionName, const QString &icon,
                                    const QString &commandName,
                                    const Core::Context &context,
                                    const QString &trans, const QString &transContext,
                                    Core::Command *cmd,
                                    Core::ActionContainer *menu,
                                    const QString &group,
                                    QKeySequence::StandardKey key = QKeySequence::UnknownKey,
                                    bool checkable = false)
{
    QAction *a = new QAction(parent);
    a->setObjectName(actionName);
    if (!icon.isEmpty())
        a->setIcon(theme()->icon(icon));
    if (checkable) {
        a->setCheckable(true);
        a->setChecked(false);
    }
    cmd = actionManager()->registerAction(a, Core::Id(commandName), context);
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

static EdrcContextualWidget *parentContextualWidget(QWidget  *widget)
{
    QWidget *parent = widget->parentWidget();
    while (parent) {
        EdrcContextualWidget *w = qobject_cast<EdrcContextualWidget *>(parent);
        if (w)
            return w;
        parent = parent->parentWidget();
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
EdrcWidgetManager::EdrcWidgetManager(QObject *parent) :
    EdrcActionManager(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*,Core::Context)));
    setObjectName("EdrcWidgetManager");
}

EdrcWidgetManager::~EdrcWidgetManager()
{
}

/**
 * Slot called when the Core::ContextManager context changed.
 * If the context is a eDRC::eDRCContextualWidget the action handler current view is updated.
 * \sa eDRC::Internal::EdrcActionManager
 */
void EdrcWidgetManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(additionalContexts);
//    if (object)
//        qWarning() << "\nEdrcWidgetManager::updateContext(Core::IContext *object)" << object->widget();
    
    EdrcContextualWidget *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;
            //            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
            break;
        }
        view = qobject_cast<EdrcContextualWidget *>(object->widget());
        if (!view) {
            // if widget is inside a FormContextualWidget add the form context
            view = parentContextualWidget(object->widget());
            int contextId = Core::Id(Constants::C_EDRC_PLUGIN).uniqueIdentifier();
            if (view) {
                if (!contextManager()->hasContext(contextId)) {
                    //                    qWarning() << "    Adding context";
                    contextManager()->updateAdditionalContexts(Core::Context(), Core::Context(Constants::C_EDRC_PLUGIN));
                    break;
                }
            } else {
                // !view
                if (contextManager()->hasContext(contextId)) {
                    //                    qWarning() << "    Removing context";
                    contextManager()->updateAdditionalContexts(Core::Context(Constants::C_EDRC_PLUGIN), Core::Context());
                    break;
                }
                if (!m_CurrentView)
                    return;
            }
            break;
        }

        if (view == m_CurrentView) {
            return;
        }
        
    } while (false);
    if (view) {
        EdrcActionManager::setCurrentView(view);
    }
}

EdrcContextualWidget *EdrcWidgetManager::currentView() const
{
    return EdrcActionManager::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
EdrcActionManager::EdrcActionManager(QObject *parent) :
    QObject(parent),
    aClear(0), aFileOpen(0), aFileSave(0), aFileSaveAs(0), aFileSavePDF(0), aFilePrint(0), aFilePrintPreview(0),
    aAddItem(0), aRemoveItem(0), aEditItem(0),
    aShowDatabaseInformation(0),
    m_CurrentView(0)
{
    setObjectName("EdrcActionManager");
    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context ctx(eDRC::Constants::C_EDRC_PLUGIN);
    Core::Context global(Core::Constants::C_GLOBAL);
    
    // Create the plugin specific menu
//    Core::ActionContainer *menu = actionManager()->actionContainer(eDRC::Constants::M_PLUGIN_eDRC);
//    if (!menu) {
//        menu = actionManager()->createMenu(eDRC::Constants::M_PLUGIN_eDRC);
//        //        menu->appendGroup(eDRC::Constants::G_PLUGINS_VIEWS);
//        //        menu->appendGroup(eDRC::Constants::G_PLUGINS_MODES);
//        //        menu->appendGroup(eDRC::Constants::G_PLUGINS_SEARCH);
//        //        menu->appendGroup(eDRC::Constants::G_PLUGINS_DRUGS);
//        //        menu->appendGroup(eDRC::Constants::G_PLUGINS_INTERACTIONS);
//        //        menu->setTranslations(eDRC::Constants::DRUGSMENU_TEXT);
        
        // Add the menu to the menubar or to the plugin menu
//#ifndef FREEMEDFORMS
//        actionManager()->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, Core::Constants::G_PLUGINS_eDRC);
//#else
//        actionManager()->actionContainer(Core::Constants::MENUBAR)->addMenu(menu, Core::Constants::G_PLUGINS);
//#endif
//    }
//    Q_ASSERT(menu);
    
    // Register existing Core action
    a = aFileOpen = registerAction(Core::Constants::A_FILE_OPEN, ctx, this);
    connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
    a = aFileSave = registerAction(Core::Constants::A_FILE_SAVE, ctx, this);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
    a = aFileSaveAs = registerAction(Core::Constants::A_FILE_SAVEAS, ctx, this);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    a = aFilePrint = registerAction(Core::Constants::A_FILE_PRINT, ctx, this);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
    a = aFilePrintPreview = registerAction(Core::Constants::A_FILE_PRINTPREVIEW, ctx, this);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrintPreview()));

    a = aAddItem = registerAction(Core::Constants::A_LIST_ADD, ctx, this);
    connect(a, SIGNAL(triggered()), this, SLOT(addItem()));
    a = aRemoveItem = registerAction(Core::Constants::A_LIST_REMOVE, ctx, this);
    connect(a, SIGNAL(triggered()), this, SLOT(removeItem()));
    a = aClear = registerAction(Core::Constants::A_LIST_CLEAR, ctx, this);
    connect(a, SIGNAL(triggered()), this, SLOT(clearItems()));

    // Register your own actions
    aEditItem = createAction(this, "eDRC.aEditItem", Core::Constants::ICONEDIT,
                                Constants::A_LIST_EDIT,
                                ctx,
                                Trans::Constants::M_EDIT_TEXT, Constants::TR_CONTEXT,
                                cmd,
                                0, "",
                                QKeySequence::UnknownKey, false);
    connect(aEditItem, SIGNAL(triggered()), this,  SLOT(editItem()));

    aFileSavePDF = createAction(this, "eDRC.aFileSavePDF", Core::Constants::ICONSAVEAS,
                                Constants::A_FILE_SAVEASPDF,
                                ctx,
                                Constants::SAVEAS_PDF_TEXT, Constants::TR_CONTEXT,
                                cmd,
                                0, "",
                                QKeySequence::UnknownKey, false);
    connect(aFileSavePDF, SIGNAL(triggered()), this,  SLOT(fileSavePDF()));

    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    aShowDatabaseInformation = createAction(this, "eDRC.aShowDatabaseInformation", Core::Constants::ICONHELP,
                                            Constants::A_SHOW_DBINFO,
                                            global,
                                            Constants::EDRC_DATABASE_INFORMATION_TEXT, Constants::TR_CONTEXT,
                                            cmd,
                                            hmenu, Core::Constants::G_HELP_DATABASES,
                                            QKeySequence::UnknownKey, false);
    connect(aShowDatabaseInformation,SIGNAL(triggered()), this, SLOT(showDatabaseInformation()));
    
    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
    //    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(updateActions()));
}

/** Define the current view, update and connect actions */
void EdrcActionManager::setCurrentView(EdrcContextualWidget *view)
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

void EdrcActionManager::updateActions()
{
    // Proceed actions dis/enableing
}

void EdrcActionManager::fileOpen()
{
    if (m_CurrentView)
        m_CurrentView->fileOpen();
}

void EdrcActionManager::fileSave()
{
    if (m_CurrentView)
        m_CurrentView->fileSave();
}

void EdrcActionManager::fileSaveAs()
{
    if (m_CurrentView)
        m_CurrentView->fileSaveAs();
}

void EdrcActionManager::fileSavePDF()
{
    if (m_CurrentView)
        m_CurrentView->fileSavePDF();
}

void EdrcActionManager::filePrint()
{
    if (m_CurrentView)
        m_CurrentView->filePrint();
}

void EdrcActionManager::filePrintPreview()
{
    if (m_CurrentView)
        m_CurrentView->filePrintPreview();
}

void EdrcActionManager::editItem()
{
    if (m_CurrentView)
        m_CurrentView->editItem();
}

void EdrcActionManager::addItem()
{
    if (m_CurrentView)
        m_CurrentView->addItem();
}

void EdrcActionManager::removeItem()
{
    if (m_CurrentView)
        m_CurrentView->removeItem();
}

void EdrcActionManager::clearItems()
{
    if (m_CurrentView)
        m_CurrentView->clearItems();
}

void EdrcActionManager::showDatabaseInformation()
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


