/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
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
#include <utils/widgets/databaseinformationdialog.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_menu.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <formmanagerplugin/constants_db.h>

#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>

using namespace PMH;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::IMainWindow *mainWindow() { return Core::ICore::instance()->mainWindow(); }
static inline PMH::PmhCore *pmhCore() { return PMH::PmhCore::instance(); }
static inline PMH::Internal::PmhBase *pmhBase() { return PMH::Internal::PmhBase::instance(); }
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::ITheme *theme() { return Core::ICore::instance()->theme(); }

static PmhContextualWidget *parentPmhWidget(QWidget  *widget)
{
    QWidget *parent = widget->parentWidget();
    while (parent) {
        PmhContextualWidget *w = qobject_cast<PmhContextualWidget *>(parent);
        if (w)
            return w;
        parent = parent->parentWidget();
    }
    return 0;
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
PmhWidgetManager::PmhWidgetManager(QObject *parent) :
    PmhActionHandler(parent),
    _contextObject(0)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*,Core::Context)));
    setObjectName("PmhWidgetManager");
}

void PmhWidgetManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(additionalContexts)
    if (_contextObject==object)
        return;
    _contextObject=object;

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
            view = parentPmhWidget(object->widget());
            int formContextId = Core::Id(Constants::C_PMH_PLUGINS).uniqueIdentifier();
            if (view) {
                if (!contextManager()->hasContext(formContextId)) {
                    //                    qWarning() << "    Adding context";
                    contextManager()->updateAdditionalContexts(Core::Context(), Core::Context(Constants::C_PMH_PLUGINS));
                    break;
                }
            } else {
                // !view
                if (contextManager()->hasContext(formContextId)) {
                    //                    qWarning() << "    Removing context";
                    contextManager()->updateAdditionalContexts(Core::Context(Constants::C_PMH_PLUGINS), Core::Context());
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
    Core::ITheme *th = Core::ICore::instance()->theme();
    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context ctx(Constants::C_PMH_PLUGINS);
    Core::Context globalcontext(Core::Constants::C_GLOBAL);

    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Id(Core::Constants::M_PATIENTS));
    Core::ActionContainer *newmenu = actionManager()->actionContainer(Core::Id(Core::Constants::M_GENERAL_NEW));
    Core::ActionContainer *pmhMenu = actionManager()->createMenu(Constants::M_PMH);
    Q_ASSERT(menu);
    if (!menu) {
        LOG_ERROR("Menu Patient not created");
        return;
    } else {
//        menu->appendGroup(Core::Constants::G_PATIENTS_NAVIGATION);
        pmhMenu->appendGroup(Core::Id(Constants::G_PMH_NEW));
        pmhMenu->appendGroup(Core::Id(Constants::G_PMH_EDITION));
        pmhMenu->setTranslations(Trans::Constants::PMHMENU_TEXT);
        menu->addMenu(pmhMenu, Core::Id(Core::Constants::G_PATIENTS));
    }

    // Create local actions
    a = aAddPmh = new QAction(this);
    aAddPmh->setEnabled(false);
    a->setObjectName("aAddPmh");
    a->setIcon(th->icon(Core::Constants::ICONADD));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_PMH_NEW), globalcontext);
    cmd->setTranslations(Constants::CREATEPMH_TEXT, Constants::CREATEPMH_TEXT, Constants::PMHCONSTANTS_TR_CONTEXT);
    pmhMenu->addAction(cmd, Core::Id(Constants::G_PMH_NEW));
    if (newmenu)
        newmenu->addAction(cmd, Core::Id(Core::Constants::G_GENERAL_NEW));
//    connect(a, SIGNAL(triggered()), this, SLOT(createPmh()));

    a = aRemovePmh= new QAction(this);
    a->setObjectName("aRemovePmh");
    a->setIcon(th->icon(Core::Constants::ICONREMOVE));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_PMH_REMOVE), ctx);
    cmd->setTranslations(Constants::REMOVEPMH_TEXT, Constants::REMOVEPMH_TEXT, Constants::PMHCONSTANTS_TR_CONTEXT);
    pmhMenu->addAction(cmd, Core::Id(Constants::G_PMH_NEW));
//    connect(a, SIGNAL(triggered()), this, SLOT(createPmh()));

    a = aAddCat = new QAction(this);
    a->setObjectName("aAddCat");
    a->setIcon(th->icon(Core::Constants::ICONCATEGORY_ADD));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_PMH_NEWCATEGORY), ctx);
    cmd->setTranslations(Constants::CREATECATEGORY_TEXT, Constants::CREATECATEGORY_TEXT, Constants::PMHCONSTANTS_TR_CONTEXT);
    pmhMenu->addAction(cmd, Core::Id(Constants::G_PMH_NEW));
//    connect(a, SIGNAL(triggered()), this, SLOT(createPmh()));

    a = aCategoryManager= new QAction(this);
    a->setObjectName("aCategoryManager");
    a->setIcon(th->icon(Core::Constants::ICONCATEGORY_MANAGER));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_PMH_CATEGORYMANAGER), globalcontext);
    cmd->setTranslations(Constants::CATEGORYMANAGER_TEXT, Constants::CATEGORYMANAGER_TEXT, Constants::PMHCONSTANTS_TR_CONTEXT);
    pmhMenu->addAction(cmd, Core::Id(Constants::G_PMH_EDITION));
    connect(a, SIGNAL(triggered()), this, SLOT(categoryManager()));

    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Id(Core::Constants::M_HELP_DATABASES));
    a = aPmhDatabaseInformation = new QAction(this);
    a->setObjectName("aPmhDatabaseInformation");
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_PMH_SHOWDBINFOS), globalcontext);
    cmd->setTranslations(Trans::Constants::PMH_DATABASE_INFORMATION);
    cmd->retranslate();
    if (hmenu) {
        hmenu->addAction(cmd, Core::Id(Core::Constants::G_HELP_DATABASES));
    }
    connect(aPmhDatabaseInformation, SIGNAL(triggered()), this, SLOT(showPmhDatabaseInformation()));

    // create forms actions
    aCreateEpisode = createAction(this, "aCreateEpisode", Core::Constants::ICONADD,
                                  Form::Constants::A_ADDEPISODE,
                                  ctx,
                                  Form::Constants::ADDEPISODE_TEXT, Form::Constants::FORM_TR_CONTEXT,
                                  cmd,
                                  0, "",
                                  QKeySequence::UnknownKey, false);
    connect(aCreateEpisode, SIGNAL(triggered()), this, SLOT(onCreateEpisodeRequested()));

    contextManager()->updateContext();
    actionManager()->retranslateMenusAndActions();

    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
}

void PmhActionHandler::setCurrentView(PmhContextualWidget *view)
{
    Q_ASSERT(view);
    if (!view) { // this should never be the case
        LOG_ERROR("setCurrentView: no view");
        return;
    }
    m_CurrentView = view;
    updateActions();
}

void PmhActionHandler::updateActions()
{
    onActionEnabledStateUpdated(PMH::Internal::PmhContextualWidget::Action_Clear);
    onActionEnabledStateUpdated(PMH::Internal::PmhContextualWidget::Action_CreateEpisode);
    onActionEnabledStateUpdated(PMH::Internal::PmhContextualWidget::Action_ValidateCurrentEpisode);
    onActionEnabledStateUpdated(PMH::Internal::PmhContextualWidget::Action_SaveCurrentEpisode);
    onActionEnabledStateUpdated(PMH::Internal::PmhContextualWidget::Action_RemoveCurrentEpisode);
    onActionEnabledStateUpdated(PMH::Internal::PmhContextualWidget::Action_RenewCurrentEpisode);
    onActionEnabledStateUpdated(PMH::Internal::PmhContextualWidget::Action_TakeScreenShot);
    onActionEnabledStateUpdated(PMH::Internal::PmhContextualWidget::Action_PrintCurrentFormEpisode);
}

void PmhActionHandler::onActionEnabledStateUpdated(PMH::Internal::PmhContextualWidget::WidgetAction action)
{
    if (m_CurrentView) {
        QAction *a = 0;
        switch (action) {
        case PMH::Internal::PmhContextualWidget::Action_Clear: /*a = aClear;*/ break;
        case PMH::Internal::PmhContextualWidget::Action_CreateEpisode: a = aCreateEpisode; break;
        case PMH::Internal::PmhContextualWidget::Action_ValidateCurrentEpisode: /*a = aValidateEpisode;*/ break;
        case PMH::Internal::PmhContextualWidget::Action_SaveCurrentEpisode: /*a = aSaveEpisode;*/ break;
        case PMH::Internal::PmhContextualWidget::Action_RenewCurrentEpisode: /*a = aRenewEpisode;*/ break;
        case PMH::Internal::PmhContextualWidget::Action_RemoveCurrentEpisode: /*a = aRemoveEpisode;*/ break;
        case PMH::Internal::PmhContextualWidget::Action_TakeScreenShot: /*a = aTakeScreenshot;*/ break;
        case PMH::Internal::PmhContextualWidget::Action_PrintCurrentFormEpisode: /*a = aPrintForm;*/ break;
        }  // end switch
        if(a)
            if(m_CurrentView)
                a->setEnabled(m_CurrentView->enableAction(action));
    }
}

void PmhActionHandler::onCurrentPatientChanged()
{
    // enable addpmh action only once when the first patient is activated
    if (aAddPmh->isEnabled()) {
        disconnect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
    }
    aAddPmh->setEnabled(true);
}

void PmhActionHandler::showPmhDatabaseInformation()
{
    Utils::DatabaseInformationDialog dlg(Core::ICore::instance()->mainWindow());
    dlg.setTitle(tkTr(Trans::Constants::PMH_DATABASE_INFORMATION));
    dlg.setDatabase(*pmhBase());
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}

void PmhActionHandler::categoryManager()
{
    Category::CategoryDialog dlg(mainWindow());
    dlg.setCategoryModel(pmhCore()->pmhCategoryModel(), PmhCategoryModel::Label);
    dlg.exec();
}

void PmhActionHandler::onClearRequested()
{
    if (m_CurrentView) {
        m_CurrentView->clear();
    }
}

void PmhActionHandler::onSaveEpisodeRequested()
{
    if (m_CurrentView) {
        m_CurrentView->saveCurrentEpisode();
    }
}

void PmhActionHandler::onCreateEpisodeRequested()
{
    if (m_CurrentView) {
        m_CurrentView->createEpisode();
    }
}

void PmhActionHandler::onValidateEpisodeRequested()
{
    if (m_CurrentView) {
        m_CurrentView->validateCurrentEpisode();
    }
}

void PmhActionHandler::onRenewEpisodeRequested()
{
    if (m_CurrentView) {
        m_CurrentView->renewEpisode();
    }
}

void PmhActionHandler::onRemoveEpisodeRequested()
{
#ifdef WITH_EPISODE_REMOVAL
    if (m_CurrentView) {
        m_CurrentView->removeCurrentEpisode();
    }
#endif
}

void PmhActionHandler::onTakeScreenshotRequested()
{
    if (m_CurrentView) {
        m_CurrentView->takeScreenshotOfCurrentEpisode();
    }
}

void PmhActionHandler::onPrintFormRequested()
{
    if (m_CurrentView) {
        m_CurrentView->printFormOrEpisode();
    }
}
