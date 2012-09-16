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
 *       Eric MAEKER <eric.maeker@gmail.com>                             *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class Form::Internal::FormContextualWidgetManager
 * \brief Manages contextualized widget and action handler for the Form plugin.
 * \sa Form::Internal::FormContextualWidget, Core::IContext, Core::ContextManager, Core::ICore::contextManager(), Core::ICore::actionManager()
 */

#include "formcontextualwidgetmanager.h"
#include "formcontextualwidget.h"
#include "constants_db.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/uniqueidmanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_database.h>

#include <QDialog>
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>

#include <QDebug>

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }

// Register an existing Core action
static QAction *registerAction(const QString &id, const QList<int> &ctx, QObject *parent)
{
    QAction *a = new QAction(parent);
    Core::Command *cmd = Core::ICore::instance()->actionManager()->registerAction(a, id, ctx);
    Q_UNUSED(cmd);
    return a;
}

// Create an action
static inline QAction *createAction(QObject *parent, const QString &name, const QString &icon,
                                    const QString &actionName,
                                    const QList<int> &context,
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
    cmd = actionManager()->registerAction(a, actionName, context);
    if (!transContext.isEmpty())
        cmd->setTranslations(trans, trans, transContext);
    else
        cmd->setTranslations(trans); // use the Trans::Constants tr context (automatic)
    if (key != QKeySequence::UnknownKey)
        cmd->setDefaultKeySequence(key);
    if (menu)
        menu->addAction(cmd, group);
    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
FormContextualWidgetManager::FormContextualWidgetManager(QObject *parent) :
    FormActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*)),
            this, SLOT(updateContext(Core::IContext*)));
    setObjectName("FormContextualWidgetManager");
}

FormContextualWidgetManager::~FormContextualWidgetManager()
{
}

/**
 * Slot called when the Core::ContextManager context changed.
 * If the context is a Form::FormContextualWidget the action handler current view is updated.
 * \sa Form::FormActionHandler
 */
void FormContextualWidgetManager::updateContext(Core::IContext *object)
{
    //    qWarning() << "FormContextualWidgetManager::updateContext(Core::IContext *object)";
    //    if (object)
    //        qWarning() << "FormContextualWidgetManager::updateContext(Core::IContext *object)" << object->widget();
    
    FormContextualWidget *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;
            
            //            m_CurrentView = 0;  // keep trace of the last active view (we need it in dialogs)
            break;
        }
        view = qobject_cast<FormContextualWidget *>(object->widget());
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
        FormActionHandler::setCurrentView(view);
    }
}

FormContextualWidget *FormContextualWidgetManager::currentView() const
{
    return FormActionHandler::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
FormActionHandler::FormActionHandler(QObject *parent) :
    QObject(parent),
    aClear(0),
    aShowDatabaseInformation(0),
    aAddEpisode(0), aValidateEpisode(0), aRemoveEpisode(0),
    aTakeScreenshot(0),
    aAddForm(0),
    aPrintForm(0),
    m_CurrentView(0)
{
    setObjectName("FormActionHandler");
    
    Core::UniqueIDManager *uid = Core::ICore::instance()->uniqueIDManager();
    
    QAction *a = 0;
    Core::Command *cmd = 0;
    QList<int> ctx = QList<int>() << uid->uniqueIdentifier(Form::Constants::C_FORM_PLUGINS);
    QList<int> allContexts = QList<int>() << uid->uniqueIdentifier(Form::Constants::C_FORM_PLUGINS) << Core::Constants::C_GLOBAL_ID;

    // Create the plugin specific menu
    Core::ActionContainer *menu = actionManager()->actionContainer(Form::Constants::M_PLUGIN_FORM);
    if (!menu) {
        menu = actionManager()->createMenu(Form::Constants::M_PLUGIN_FORM);
        //        menu->appendGroup(Form::Constants::G_PLUGINS_VIEWS);
        //        menu->appendGroup(Form::Constants::G_PLUGINS_MODES);
        //        menu->appendGroup(Form::Constants::G_PLUGINS_SEARCH);
        //        menu->appendGroup(Form::Constants::G_PLUGINS_DRUGS);
        //        menu->appendGroup(Form::Constants::G_PLUGINS_INTERACTIONS);
        //        menu->setTranslations(Form::Constants::DRUGSMENU_TEXT);
        
        // Add the menu to the menubar or to the plugin menu
#ifndef FREEMEDFORMS
        actionManager()->actionContainer(Core::Constants::M_PLUGINS)->addMenu(menu, Core::Constants::G_PLUGINS_FORM);
#else
        actionManager()->actionContainer(Core::Constants::MENUBAR)->addMenu(menu, Core::Constants::G_PLUGINS);
#endif
    }
    Q_ASSERT(menu);
    
    // Create local actions
    // Example: register existing Core actions
    aClear = registerAction(Core::Constants::A_LIST_CLEAR, ctx, this);
    connect(aClear, SIGNAL(triggered()), this, SLOT(clear()));
    
    a = aAddEpisode = new QAction(this);
    a->setObjectName("aAddEpisode");
    a->setIcon(theme()->icon(Core::Constants::ICONADD));
    cmd = actionManager()->registerAction(a, Constants::A_ADDEPISODE, ctx);
    cmd->setTranslations(Constants::ADDEPISODE_TEXT, Constants::ADDEPISODE_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
//    connect(aAddEpisode, SIGNAL(triggered()), this, SLOT(onAddEpisodeRequested()));

    a = aValidateEpisode = new QAction(this);
    a->setObjectName("aValidateEpisode");
    a->setIcon(theme()->icon(Core::Constants::ICONVALIDATEDARK));
    cmd = actionManager()->registerAction(a, Constants::A_VALIDATEEPISODE, ctx);
    cmd->setTranslations(Constants::VALIDATEEPISODE_TEXT, Constants::VALIDATEEPISODE_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
//    connect(aValidateEpisode, SIGNAL(triggered()), this, SLOT(onValidateEpisodeRequested()));

    aRemoveEpisode = createAction(this, "aRemoveEpisode", Core::Constants::ICONREMOVE,
                                  Constants::A_REMOVEEPISODE,
                                  ctx,
                                  Trans::Constants::REMOVE_EPISODE, "",
                                  cmd,
                                  0, "",
                                  QKeySequence::UnknownKey, false);
    //    connect(aRemoveEpisode, SIGNAL(triggered()), this, SLOT(onRemoveEpisodeRequested()));

    aTakeScreenshot = createAction(this, "aTakeScreenshot", Core::Constants::ICONTAKESCREENSHOT,
                                   Constants::A_TAKESCREENSHOT,
                                   ctx,
                                   Trans::Constants::TAKE_SCREENSHOT, "",
                                   cmd,
                                   0, "",
                                   QKeySequence::UnknownKey, false);
    //    connect(aTakeScreenshot, SIGNAL(triggered()), this, SLOT(onTakeScreenshotRequested()));

    a = aAddForm = new QAction(this);
    a->setObjectName("aAddForm");
    a->setIcon(theme()->icon(Core::Constants::ICONFORMS_ADD));
    cmd = actionManager()->registerAction(a, Constants::A_ADDFORM, ctx);
    cmd->setTranslations(Constants::ADDFORM_TEXT, Constants::ADDFORM_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
//    connect(aAddForm, SIGNAL(triggered()), this, SLOT(onAddFormRequested()));

    aPrintForm = registerAction(Core::Constants::A_FILE_PRINT, ctx, this);
    connect(aPrintForm, SIGNAL(triggered()), this, SLOT(onPrintFormRequested()));

    // Databases information
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Constants::M_HELP_DATABASES);
    aShowDatabaseInformation = createAction(this, "aShowDatabaseInformation", Core::Constants::ICONHELP,
                                            Constants::A_DATABASE_INFORMATION,
                                            allContexts,
                                            Trans::Constants::FORM_DATABASE_INFORMATION, "",
                                            cmd,
                                            hmenu, Core::Constants::G_HELP_DATABASES,
                                            QKeySequence::UnknownKey, false);
    aShowDatabaseInformation->setEnabled(false);
    connect(aShowDatabaseInformation,SIGNAL(triggered()), this, SLOT(showDatabaseInformation()));
    
    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(updateActions()));
}

/** Define the current view, update and connect actions */
void FormActionHandler::setCurrentView(FormContextualWidget *view)
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

void FormActionHandler::updateActions()
{
    // Proceed actions dis/enableing
}

// Eg of action interacting with the currentview
void FormActionHandler::clear()
{
    if (m_CurrentView) {
//        m_CurrentView->clear();
    }
}

void FormActionHandler::onAddEpisodeRequested()
{
    if (m_CurrentView) {
//        m_CurrentView->clear();
    }
}

void FormActionHandler::onValidateEpisodeRequested()
{
    if (m_CurrentView) {
//        m_CurrentView->clear();
    }
}

void FormActionHandler::onRemoveEpisodeRequested()
{
    if (m_CurrentView) {
//        m_CurrentView->clear();
    }
}

void FormActionHandler::onTakeScreenshotRequested()
{
    if (m_CurrentView) {
//        m_CurrentView->clear();
    }
}

void FormActionHandler::onAddFormRequested()
{
    if (m_CurrentView) {
//        m_CurrentView->clear();
    }
}

void FormActionHandler::onPrintFormRequested()
{
    if (m_CurrentView) {
//        m_CurrentView->clear();
    }
}

void FormActionHandler::showDatabaseInformation()
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


