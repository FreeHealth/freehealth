/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
/**
 * \class Patients::Internal::PatientWidgetManager
 * \internal
 * Manage the contextual objects of the plugin.
 * Instance is owned and managed by the Patients::PatientCore
*/
#include "patientwidgetmanager.h"
#include "constants_menus.h"
#include "constants_trans.h"
#include "constants_settings.h"
#include "patientbase.h"
#include "patientcore.h"
#include "patientdataextractordialog.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/databaseinformationdialog.h>
#include <translationutils/constants.h>
#include <translationutils/trans_patient.h>
#include <translationutils/trans_menu.h>
#include <translationutils/trans_database.h>

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/filemanager.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <QDialog>
#include <QWidget>
#include <QGridLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>

using namespace Patients::Constants;
using namespace Patients::Internal;
using namespace Patients;
using namespace Trans::ConstantTranslations;

static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Core::ContextManager *contextManager() { return Core::ICore::instance()->contextManager(); }
static inline Core::IPatient *patient() { return Core::ICore::instance()->patient(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::IUser *user() { return Core::ICore::instance()->user(); }
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }
static inline Patients::PatientCore *patientCore() {return Patients::PatientCore::instance();}

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
    m_RecentPatients(0),
    aSearchName(0),
    aSearchFirstname(0),
    aSearchNameFirstname(0),
    aSearchDob(0),
    aViewPatientInformation(0),
    aPrintPatientInformation(0),
    aShowPatientDatabaseInformation(0),
    aViewCurrentPatientData(0),
    aExportPatientFile(0),
    gSearchMethod(0)
{
    setObjectName("PatientActionHandler");
    m_RecentPatients = new Core::FileManager(this);

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
    connect(actionManager()->command(Core::Constants::A_PATIENT_REMOVE)->action(), SIGNAL(triggered()), this, SLOT(removePatient()));

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

    // Export patient file
    a = aExportPatientFile = new QAction(this);
    a->setObjectName("aExportPatientFile");
    a->setIcon(th->icon(Core::Constants::ICONEXPORTPATIENTFILE));
    cmd = actionManager()->registerAction(a, Core::Id(Constants::A_EXPORTPATIENTFILE), ctx);
    cmd->setTranslations(Trans::Constants::EXPORTPATIENTFILE);
    cmd->retranslate();
    menu->addAction(cmd, Core::Id(Constants::G_PATIENTS_INFORMATION));
    connect(aExportPatientFile, SIGNAL(triggered()), this, SLOT(onExportPatientFileRequested()));

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

    if (!Utils::isReleaseCompilation()) {
        a = aViewCurrentPatientData = new QAction(this);
        a->setObjectName("aViewCurrentPatientData");
        a->setIcon(th->icon(Core::Constants::ICONPATIENTHISTORY));
        cmd = actionManager()->registerAction(a, Core::Id(Constants::A_VIEWCURRENTPATIENTDATA), globalcontext);
        cmd->setTranslations(Trans::Constants::PATIENT_OVERVIEW);
        cmd->retranslate();
        menu->addAction(cmd, Core::Id(Constants::G_PATIENTS_HISTORY));
        connect(aViewCurrentPatientData,SIGNAL(triggered()), this, SLOT(viewCurrentPatientData()));
    }

    Core::ActionContainer *recentsMenu = actionManager()->actionContainer(Core::Constants::M_PATIENTS_NAVIGATION);
    if (!recentsMenu)
        return;
    if (!recentsMenu->menu())
        return;
    connect(recentsMenu->menu(), SIGNAL(aboutToShow()), this, SLOT(aboutToShowRecentPatients()));

    refreshSettings();
    connect(patient(), SIGNAL(currentPatientChanged()), this, SLOT(onCurrentPatientChanged()));
    connect(user(), SIGNAL(userChanged()), this, SLOT(refreshSettings()));
}

void PatientActionHandler::onCurrentPatientChanged()
{
    // Store the uuids of the patient in the recent manager
    const QString &uuid = patient()->uuid();
    m_RecentPatients->setCurrentFile(uuid);
    m_RecentPatients->addToRecentFiles(uuid);

    // Force settings to be saved
    // TODO: improve this
    settings()->setValue(Constants::S_RECENTPATIENT_LIST, m_RecentPatients->recentFiles());
    settings()->sync();

    // refresh the navigation menu
    aboutToShowRecentPatients();
}

/** \brief Rebuild the patients' history menu */
void PatientActionHandler::aboutToShowRecentPatients()
{
    // update patient history
    Core::ActionContainer *recentsMenu = actionManager()->actionContainer(Core::Constants::M_PATIENTS_NAVIGATION);
    if (!recentsMenu)
        return;
    if (!recentsMenu->menu())
        return;
    recentsMenu->menu()->clear();

    bool hasRecentFiles = false;
    const QStringList &uuids = m_RecentPatients->recentFiles();
    const QHash<QString, QString> &names = patient()->fullPatientName(uuids);
    for(int i = 0; i < uuids.count(); ++i) {
        hasRecentFiles = true;
        QAction *action = recentsMenu->menu()->addAction(QString("%1: %2").arg(i).arg(names.value(uuids.at(i))));
        action->setData(uuids.at(i));
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentPatient()));
    }

    recentsMenu->menu()->setEnabled(hasRecentFiles);
}

/**
 * Opens a recent patient selected from the patient history.
 * This slot is called by a recent patients' menu's action.
 */
void PatientActionHandler::openRecentPatient()
{
    // get the uuid of the sender
    QAction *a = qobject_cast<QAction*>(sender());
    if (!a)
        return;
    const QString &uuid = a->data().toString();
    if (uuid.isEmpty())
        return;

    // get the corresponding to the uuid
    patientCore()->setCurrentPatientUuid(uuid);
}

void PatientActionHandler::updateActions()
{}

void PatientActionHandler::setCurrentView(PatientSelector *view)
{
    m_CurrentView = view;
}

void PatientActionHandler::refreshSettings()
{
    // Update recent patient max
    m_RecentPatients->setMaximumRecentFiles(settings()->value(Constants::S_RECENTPATIENT_MAX, 10).toInt());
    QStringList uuids = settings()->value(Constants::S_RECENTPATIENT_LIST).toStringList();
    uuids.removeAll("");
    m_RecentPatients->setRecentFiles(uuids);
    // Refresh the patient navigation history
    aboutToShowRecentPatients();
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

void PatientActionHandler::removePatient()
{
    // TODO: move this in the PatientCore and force all patientmodel refreshing
    if (!patient()->currentPatientIndex().isValid()) {
        Utils::warningMessageBox(tr("Remove the patient"),
                                 tr("If you want to remove a patient, you must firstly "
                                    "select it as the current one and then remove it. <br /><br />"
                                    "<b>Warning: No current patient. Select a patient first.</b>")
                                 );
        return;
    }

    // message box
    bool yes = Utils::yesNoMessageBox(tr("Remove the current patient"),
                                      tr("You are about to remove the current patient:<br /><br/>"
                                         "    <b>%1</b><br/>"
                                         "    <b>%2</b><br/>"
                                         "    <b>%3</b><br/><br/>"
                                         "Do you really want to remove this patient?")
                                      .arg(patient()->data(Core::IPatient::FullName).toString())
                                      .arg(patient()->data(Core::IPatient::DateOfBirth).toString())
                                      .arg(patient()->data(Core::IPatient::FullAddress).toString())
                                      );
    if (!yes)
        return;

    // set validity flag to false
    if (!patientCore()->removePatient(patient()->uuid())) {
        LOG_ERROR("Unable to remove current patient");
        return;
    }
    // set current mode to patient selector is done by MainWindow::onCurrentPatientChanged
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

void PatientActionHandler::viewCurrentPatientData()
{
    for(int i=0; i < Core::IPatient::NumberOfColumns; ++i) {
        qWarning() << patient()->enumToString(Core::IPatient::PatientDataRepresentation(i)) << patient()->data(i);
    }
}

void PatientActionHandler::onExportPatientFileRequested()
{
    PatientDataExtractorDialog dlg(Core::ICore::instance()->mainWindow());
    dlg.initialize();
    dlg.exec();
}
