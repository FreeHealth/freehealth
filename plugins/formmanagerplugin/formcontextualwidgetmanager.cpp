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
#include "episodebase.h"
#include "constants_db.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
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

#include <QDialog>
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QAction>

#include <QDebug>

using namespace Form;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
static inline Form::Internal::EpisodeBase *episodeBase() { return Form::Internal::EpisodeBase::instance(); }

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

static FormContextualWidget *parentFormWidget(QWidget  *widget)
{
    QWidget *parent = widget->parentWidget();
    while (parent) {
        FormContextualWidget *w = qobject_cast<FormContextualWidget *>(parent);
        if (w)
            return w;
        parent = parent->parentWidget();
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
FormContextualWidgetManager::FormContextualWidgetManager(QObject *parent) :
    FormActionHandler(parent),
    _contextObject(0)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*,Core::Context)));
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
void FormContextualWidgetManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(additionalContexts);
//    qWarning() << "updateContext(Core::IContext *object)" << object << _contextObject;
    if (_contextObject==object)
        return;
    _contextObject = object;

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
            // if widget is inside a FormContextualWidget add the form context
            view = parentFormWidget(object->widget());
            int formContextId = Core::Id(Constants::C_FORM_PLUGINS).uniqueIdentifier();
            if (view) {
                if (!contextManager()->hasContext(formContextId)) {
                    //                    qWarning() << "    Adding context";
                    contextManager()->updateAdditionalContexts(Core::Context(), Core::Context(Constants::C_FORM_PLUGINS));
                    break;
                }
            } else {
                // !view
                if (contextManager()->hasContext(formContextId)) {
                    //                    qWarning() << "    Removing context";
                    contextManager()->updateAdditionalContexts(Core::Context(Constants::C_FORM_PLUGINS), Core::Context());
                    break;
                }
            }
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
FormActionHandler::FormActionHandler(QObject *parent) :
    QObject(parent),
    aClear(0),
    aShowDatabaseInformation(0),
    aCreateEpisode(0), aValidateEpisode(0), aRemoveEpisode(0), aSaveEpisode(0),
    aTakeScreenshot(0),
    aAddForm(0),
    aPrintForm(0),
    m_CurrentView(0)
{
    setObjectName("FormActionHandler");

    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context ctx(Form::Constants::C_FORM_PLUGINS);
    Core::Context allContexts(Form::Constants::C_FORM_PLUGINS, Core::Constants::C_GLOBAL);

    // Create the plugin specific menu
    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Id(Form::Constants::M_PLUGIN_FORM));
    if (!menu) {
        menu = actionManager()->createMenu(Form::Constants::M_PLUGIN_FORM);
        //        menu->appendGroup(Core::Id(Form::Constants::G_PLUGINS_VIEWS));
        //        menu->appendGroup(Core::Id(Form::Constants::G_PLUGINS_MODES));
        //        menu->appendGroup(Core::Id(Form::Constants::G_PLUGINS_SEARCH));
        //        menu->appendGroup(Core::Id(Form::Constants::G_PLUGINS_DRUGS));
        //        menu->appendGroup(Core::Id(Form::Constants::G_PLUGINS_INTERACTIONS));
        //        menu->setTranslations(Form::Constants::DRUGSMENU_TEXT);

        // Add the menu to the menubar or to the plugin menu
#ifndef FREEMEDFORMS
        actionManager()->actionContainer(Core::Id(Core::Constants::M_PLUGINS))->addMenu(menu, Core::Constants::G_PLUGINS_FORM);
#else
        actionManager()->actionContainer(Core::Id(Core::Constants::MENUBAR))->addMenu(menu, Core::Constants::G_PLUGINS);
#endif
    }
    Q_ASSERT(menu);

    // Create local actions
    // Example: register existing Core actions
    aClear = registerAction(Core::Constants::A_LIST_CLEAR, ctx, this);
    connect(aClear, SIGNAL(triggered()), this, SLOT(onClearRequested()));

    aSaveEpisode = registerAction(Core::Constants::A_FILE_SAVE, ctx, this);
    connect(aSaveEpisode, SIGNAL(triggered()), this, SLOT(onSaveEpisodeRequested()));

    aCreateEpisode = createAction(this, "aCreateEpisode", Core::Constants::ICONADD,
                                  Constants::A_ADDEPISODE,
                                  ctx,
                                  Constants::ADDEPISODE_TEXT, Constants::FORM_TR_CONTEXT,
                                  cmd,
                                  0, "",
                                  QKeySequence::UnknownKey, false);
    connect(aCreateEpisode, SIGNAL(triggered()), this, SLOT(onCreateEpisodeRequested()));

    a = aValidateEpisode = new QAction(this);
    a->setObjectName("aValidateEpisode");
    a->setIcon(theme()->icon(Core::Constants::ICONVALIDATEDARK));
    cmd = actionManager()->registerAction(a, Constants::A_VALIDATEEPISODE, ctx);
    cmd->setTranslations(Constants::VALIDATEEPISODE_TEXT, Constants::VALIDATEEPISODE_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
    connect(aValidateEpisode, SIGNAL(triggered()), this, SLOT(onValidateEpisodeRequested()));

    aRemoveEpisode = createAction(this, "aRemoveEpisode", Core::Constants::ICONREMOVE,
                                  Constants::A_REMOVEEPISODE,
                                  ctx,
                                  Trans::Constants::REMOVE_EPISODE, "",
                                  cmd,
                                  0, "",
                                  QKeySequence::UnknownKey, false);
    connect(aRemoveEpisode, SIGNAL(triggered()), this, SLOT(onRemoveEpisodeRequested()));

    aTakeScreenshot = createAction(this, "aTakeScreenshot", Core::Constants::ICONTAKESCREENSHOT,
                                   Constants::A_TAKESCREENSHOT,
                                   ctx,
                                   Trans::Constants::TAKE_SCREENSHOT, "",
                                   cmd,
                                   0, "",
                                   QKeySequence::UnknownKey, false);
    connect(aTakeScreenshot, SIGNAL(triggered()), this, SLOT(onTakeScreenshotRequested()));

    a = aAddForm = new QAction(this);
    a->setObjectName("aAddForm");
    a->setIcon(theme()->icon(Core::Constants::ICONFORMS_ADD));
    cmd = actionManager()->registerAction(a, Constants::A_ADDFORM, ctx);
    cmd->setTranslations(Constants::ADDFORM_TEXT, Constants::ADDFORM_TEXT, Constants::FORM_TR_CONTEXT);
//    cmenu->addAction(cmd, Core::Constants::G_EDIT_LIST);
    connect(aAddForm, SIGNAL(triggered()), this, SLOT(onAddFormRequested()));

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
//    qWarning() << "SET VIEW" << view << "hasContext" << contextManager()->hasContext(Core::Id(Constants::C_FORM_PLUGINS).uniqueIdentifier());
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        LOG_ERROR("setCurrentView: no view");
        return;
    }

    // disconnect old view
    if (m_CurrentView) {
        disconnect(m_CurrentView, SIGNAL(actionsEnabledStateChanged()), this, SLOT(updateActions()));
        disconnect(m_CurrentView, SIGNAL(actionEnabledStateChanged(Form::Internal::FormContextualWidget::WidgetAction)), this, SLOT(onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::WidgetAction)));
    }
    m_CurrentView = view;

    // connect new view
    connect(m_CurrentView, SIGNAL(actionsEnabledStateChanged()), this, SLOT(updateActions()));
    connect(m_CurrentView, SIGNAL(actionEnabledStateChanged(Form::Internal::FormContextualWidget::WidgetAction)), this, SLOT(onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::WidgetAction)));
    // update actions according to the current view
    updateActions();
}

void FormActionHandler::updateActions()
{
    // Proceed actions dis/enableing
    onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::Action_Clear);
    onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::Action_CreateEpisode);
    onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::Action_ValidateCurrentEpisode);
    onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::Action_SaveCurrentEpisode);
    onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::Action_RemoveCurrentEpisode);
    onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::Action_TakeScreenShot);
    onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::Action_AddForm);
    onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::Action_PrintCurrentFormEpisode);
}

void FormActionHandler::onActionEnabledStateUpdated(Form::Internal::FormContextualWidget::WidgetAction action)
{
    if (m_CurrentView) {
        QAction *a = 0;
        switch (action) {
        case Form::Internal::FormContextualWidget::Action_Clear: a = aClear; break;
        case Form::Internal::FormContextualWidget::Action_CreateEpisode: a = aCreateEpisode; break;
        case Form::Internal::FormContextualWidget::Action_ValidateCurrentEpisode: a = aValidateEpisode; break;
        case Form::Internal::FormContextualWidget::Action_SaveCurrentEpisode: a = aSaveEpisode; break;
        case Form::Internal::FormContextualWidget::Action_RemoveCurrentEpisode: a = aRemoveEpisode; break;
        case Form::Internal::FormContextualWidget::Action_TakeScreenShot: a = aTakeScreenshot; break;
        case Form::Internal::FormContextualWidget::Action_AddForm: a = aAddForm; break;
        case Form::Internal::FormContextualWidget::Action_PrintCurrentFormEpisode: a = aPrintForm; break;
        }  // end switch
        if(a)
            a->setEnabled(m_CurrentView->enableAction(action));
    }
}

// Eg of action interacting with the currentview
void FormActionHandler::onClearRequested()
{
    if (m_CurrentView) {
        m_CurrentView->clear();
    }
}

void FormActionHandler::onSaveEpisodeRequested()
{
    qWarning() << "FormActionHandler::onSaveEpisodeRequested" << m_CurrentView;
    if (m_CurrentView) {
        m_CurrentView->saveCurrentEpisode();
    }
}

void FormActionHandler::onCreateEpisodeRequested()
{
    if (m_CurrentView) {
        m_CurrentView->createEpisode();
    }
}

void FormActionHandler::onValidateEpisodeRequested()
{
    if (m_CurrentView) {
        m_CurrentView->validateCurrentEpisode();
    }
}

void FormActionHandler::onRemoveEpisodeRequested()
{
    if (m_CurrentView) {
        m_CurrentView->removeCurrentEpisode();
    }
}

void FormActionHandler::onTakeScreenshotRequested()
{
    if (m_CurrentView) {
        m_CurrentView->takeScreenshotOfCurrentEpisode();
    }
}

void FormActionHandler::onAddFormRequested()
{
    if (m_CurrentView) {
        m_CurrentView->addForm();
    }
}

void FormActionHandler::onPrintFormRequested()
{
    if (m_CurrentView) {
        m_CurrentView->printFormOrEpisode();
    }
}

void FormActionHandler::showDatabaseInformation()
{
    QDialog dlg(qApp->activeWindow(), Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint);
    QGridLayout lay(&dlg);
    QTreeWidget tree(&dlg);
    tree.setColumnCount(2);
    episodeBase()->toTreeWidget(&tree);
    tree.header()->hide();
    lay.addWidget(&tree);
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}


