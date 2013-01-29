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
/**
 * \class Patients::Internal::PatientWidgetManager
 * \internal
 * Manage the contextual objects of the plugin.
 * Instance is owned and managed by the Patients::PatientCore
*/
#include "patientwidgetmanager.h"
#include "constants_menus.h"
#include "constants_trans.h"
#include "patientbase.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/databaseinformationdialog.h>
#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_database.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <QDialog>
#include <QWidget>
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>

using namespace Patients::Constants;
using namespace Patients::Internal;
using namespace Patients;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////      MANAGER      ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
PatientWidgetManager::PatientWidgetManager(QObject *parent) : PatientActionHandler(parent)
{
    connect(Core::ICore::instance()->contextManager(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(updateContext(Core::IContext*,Core::Context)));
    setObjectName("PatientWidgetManager");
}

void PatientWidgetManager::updateContext(Core::IContext *object, const Core::Context &additionalContexts)
{
    Q_UNUSED(additionalContexts);
    PatientSelector *view = 0;
    do {
        if (!object) {
            if (!m_CurrentView)
                return;
            break;
        }
        view = qobject_cast<PatientSelector *>(object->widget());
        if (!view) {
            if (!m_CurrentView)
                return;
            break;
        }

        if (view == m_CurrentView) {
            return;
        }

    } while (false);
    if (view) {
        PatientActionHandler::setCurrentView(view);
    }
}

void PatientWidgetManager::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    m_CurrentView->initialize();
}

PatientSelector *PatientWidgetManager::selector() const
{
    return PatientActionHandler::m_CurrentView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ACTION HANDLER   ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Actions :
// Toggle search patient view
// Search method ?
// Limit database usage ?
// View patients information
// Patient selection history
PatientActionHandler::PatientActionHandler(QObject *parent) :
        QObject(parent),
        aSearchName(0),
        aSearchFirstname(0),
        aSearchNameFirstname(0),
        aSearchDob(0),
        aViewPatientInformation(0),
        aPrintPatientInformation(0),
        aShowPatientDatabaseInformation(0),
        gSearchMethod(0)
{
    setObjectName("PatientActionHandler");

    Core::ITheme *th = Core::ICore::instance()->theme();

    QAction *a = 0;
    Core::Command *cmd = 0;
    Core::Context ctx(Patients::Constants::C_PATIENTS);
    Core::Context searchcontext(Patients::Constants::C_PATIENTS_SEARCH, Core::Constants::C_GLOBAL);
    Core::Context globalcontext(Core::Constants::C_GLOBAL);

    Core::ActionContainer *menu = actionManager()->actionContainer(Core::Id(Core::Constants::M_PATIENTS));
    if (!menu) {
        Utils::warningMessageBox(tr("Fatal Error"), tr("%1: Unable to retreive patients menu.").arg(objectName()));
        return;
    }
    Q_ASSERT(menu);
    if (!menu)
        return;

    menu->appendGroup(Core::Id(Constants::G_PATIENTS));
    menu->appendGroup(Core::Id(Constants::G_PATIENTS_NEW));
    menu->appendGroup(Core::Id(Constants::G_PATIENTS_SEARCH));
    menu->appendGroup(Core::Id(Constants::G_PATIENTS_HISTORY));
    menu->appendGroup(Core::Id(Constants::G_PATIENTS_INFORMATION));

//    actionManager()->actionContainer(Core::Constants::M_PATIENTS)->addMenu(menu, Core::Constants::G_PATIENTS);

    // Search method menu
    Core::ActionContainer *searchmenu = actionManager()->actionContainer(Core::Id(Constants::M_PATIENTS_SEARCH));
    if (!searchmenu) {
        searchmenu = actionManager()->createMenu(Constants::M_PATIENTS_SEARCH);
        searchmenu->appendGroup(Core::Id(Constants::G_PATIENTS_SEARCH));
        searchmenu->setTranslations(Trans::Constants::SEARCHMENU_TEXT);
        menu->addMenu(searchmenu, Core::Id(Constants::G_PATIENTS_SEARCH));
    }
    Q_ASSERT(searchmenu);

    // TODO: create search icons
    gSearchMethod = new QActionGroup(this);
    a = aSearchName = new QAction(this);
    a->setObjectName("aSearchName");
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(Core::Constants::ICONSEARCH));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_SEARCH_PATIENTS_BY_NAME), searchcontext);
    cmd->setTranslations(Constants::SEARCHBYNAME_TEXT, Constants::SEARCHBYNAME_TOOLTIP, Constants::TRANS_CONTEXT);
    searchmenu->addAction(cmd, Core::Id(Constants::G_PATIENTS_SEARCH));
    gSearchMethod->addAction(a);

    a = aSearchFirstname = new QAction(this);
    a->setObjectName("aSearchFirstname");
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(Core::Constants::ICONSEARCH));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_SEARCH_PATIENTS_BY_FIRSTNAME), searchcontext);
    cmd->setTranslations(Constants::SEARCHBYFIRSTNAME_TEXT, Constants::SEARCHBYFIRSTNAME_TOOLTIP, Constants::TRANS_CONTEXT);
    searchmenu->addAction(cmd, Core::Id(Constants::G_PATIENTS_SEARCH));
    gSearchMethod->addAction(a);

    a = aSearchNameFirstname = new QAction(this);
    a->setObjectName("aSearchNameFirstname");
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(Core::Constants::ICONSEARCH));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_SEARCH_PATIENTS_BY_NAMEFIRSTNAME), searchcontext);
    cmd->setTranslations(Constants::SEARCHBYNAMEFIRSTNAME_TEXT, Constants::SEARCHBYNAMEFIRSTNAME_TOOLTIP, Constants::TRANS_CONTEXT);
    searchmenu->addAction(cmd, Core::Id(Constants::G_PATIENTS_SEARCH));
    gSearchMethod->addAction(a);

    a = aSearchDob = new QAction(this);
    a->setObjectName("aSearchDob");
    a->setCheckable(true);
    a->setChecked(false);
    a->setIcon(th->icon(Core::Constants::ICONSEARCH));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_SEARCH_PATIENTS_BY_DOB), searchcontext);
    cmd->setTranslations(Constants::SEARCHBYDOB_TEXT, Constants::SEARCHBYDOB_TOOLTIP, Constants::TRANS_CONTEXT);
    searchmenu->addAction(cmd, Core::Id(Constants::G_PATIENTS_SEARCH));
    gSearchMethod->addAction(a);

    connect(gSearchMethod, SIGNAL(triggered(QAction*)), this, SLOT(searchActionChanged(QAction*)));

//    a = aViewPatientInformation = new QAction(this);
//    a->setObjectName("aViewPatientInformation");
////    a->setIcon(th->icon(Core::Constants::ICONCLEAR));
//    cmd = actionManager()->registerAction(a, Constants::A_VIEWPATIENT_INFOS, globalcontext);
//    cmd->setTranslations(Trans::Constants::PATIENT_INFORMATION);
//    menu->addAction(cmd, Constants::G_PATIENTS_INFORMATION);
////    connect(a, SIGNAL(triggered()), this, SLOT(clear()));

    // Databases information
    Core::ActionContainer *hmenu = actionManager()->actionContainer(Core::Id(Core::Constants::M_HELP_DATABASES));
    if (!hmenu)
        return;

    a = aShowPatientDatabaseInformation = new QAction(this);
    a->setObjectName("aShowPatientDatabaseInformation");
    a->setIcon(th->icon(Core::Constants::ICONHELP));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_VIEWPATIENTDATABASE_INFOS), globalcontext);
    cmd->setTranslations(Trans::Constants::PATIENT_DATABASE);
    cmd->retranslate();
    hmenu->addAction(cmd, Core::Id(Core::Constants::G_HELP_DATABASES));

    connect(aShowPatientDatabaseInformation,SIGNAL(triggered()), this, SLOT(showPatientDatabaseInformation()));

//    contextManager()->updateContext();
//    actionManager()->retranslateMenusAndActions();
}

void PatientActionHandler::updateActions()
{}

void PatientActionHandler::setCurrentView(PatientSelector *view)
{
    m_CurrentView = view;
}

void PatientActionHandler::searchActionChanged(QAction *action)
{
    if (action==aSearchName) {
        if (m_CurrentView)
            m_CurrentView->setSearchMode(PatientSelector::SearchByName);
    }
    if (action==aSearchFirstname) {
        if (m_CurrentView)
            m_CurrentView->setSearchMode(PatientSelector::SearchByFirstname);
    }
    if (action==aSearchNameFirstname) {
        if (m_CurrentView)
            m_CurrentView->setSearchMode(PatientSelector::SearchByNameFirstname);
    }
    if (action==aSearchDob) {
        if (m_CurrentView)
            m_CurrentView->setSearchMode(PatientSelector::SearchByDOB);
    }
}

void PatientActionHandler::viewPatientInformation()
{}

void PatientActionHandler::printPatientsInformation()
{}

void PatientActionHandler::showPatientDatabaseInformation()
{
    Utils::DatabaseInformationDialog dlg(Core::ICore::instance()->mainWindow());
    dlg.setTitle(tkTr(Trans::Constants::PATIENT_DATABASE_INFORMATION));
    dlg.setDatabase(*PatientBase::instance());
    Utils::resizeAndCenter(&dlg);
    dlg.exec();
}
