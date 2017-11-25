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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Patients::PatientSelector
 *
 * \brief Selector Widget for the recorded patients.
 * Allow user to search and select patient from the complete database. \n
 * Automatically removes the search filter on user changed to null.
 *
 * Bugs and evolutions:
 * In versions <= 0.8.0, the Core::IPatient was directly used in the patient view
 * and was filtered with the search text. The Core::IPatient was filtered using a SQL
 * query causing indexes to be lost after a search.
 * This was not a good behavior (because of model persistence lose),
 * so in later version we introduced an internal model and linked it to the Core::IPatient
 * model.
 *
 * \sa Patient::PatientModel
 */

#include "patientselector.h"
#include "patientmodel.h"
#include "patientbar.h"
#include "constants_menus.h"
#include "constants_settings.h"
#include "patientcore.h"

#include "ui_patientselector.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/isettings.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/modemanager/modemanager.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/datetimedelegate.h>
#include <translationutils/constanttranslations.h>

#include <QToolButton>
#include <QMenu>

#include <QDebug>

using namespace Patients;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::IPatient *patient() {return Core::ICore::instance()->patient();}
static inline Core::IMainWindow *mainWindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }
static inline Patients::PatientCore *patientCore() {return Patients::PatientCore::instance();}

namespace Patients {
namespace Internal {
class PatientSelectorPrivate
{
public:
    PatientSelectorPrivate(PatientSelector *parent) :
            ui(new Ui::PatientSelector),
            m_Model(0),
            m_SearchToolButton(0),
            m_NavigationToolButton(0),
            m_NavigationMenu(0),
            m_SearchMethod(-1),
            m_LastSearch(QString("_##_")),  // Force a first refresh when calling refreshFilter() with an empty QString
            m_refreshMethod(PatientSelector::WhileTyping),
            m_SetActivatedPatientAsCurrent(true),
            q(parent)
    {
    }

    ~PatientSelectorPrivate()
    {
        delete ui;
    }

    void createSearchToolButtons()
    {
        m_SearchToolButton = new QToolButton();   // parent object will be redefined
        m_SearchToolButton->setPopupMode(QToolButton::InstantPopup);
        m_SearchToolButton->setIcon(theme()->icon(Core::Constants::ICONSEARCH));

        QStringList actions;
        actions
                << Constants::A_SEARCH_PATIENTS_BY_NAME
                << Constants::A_SEARCH_PATIENTS_BY_FIRSTNAME
                << Constants::A_SEARCH_PATIENTS_BY_NAMEFIRSTNAME
                << Constants::A_SEARCH_PATIENTS_BY_DOB;

        QList<QAction *> actionList;
        foreach(const QString &a, actions) {
            Core::Command *cmd = actionManager()->command(Core::Id(a));
            m_SearchToolButton->addAction(cmd->action());
            actionList << cmd->action();
        }

        int id = settings()->value(Constants::S_SEARCHMETHOD, 0).toInt();
        if (id < actionList.count() && id >= 0) {
            actionList.at(id)->trigger();
            actionList.at(id)->setChecked(true);
            m_SearchToolButton->setDefaultAction(actionList.at(id));
            m_SearchMethod = id;
        } else {
            m_SearchMethod = 0;
        }

        // add buttons to search line
        ui->searchLine->setLeftButton(m_SearchToolButton);

//        // add action to the navigation button
        m_NavigationToolButton = new QToolButton(q);   // parent object will be redefined
        m_NavigationToolButton->setPopupMode(QToolButton::InstantPopup);
        m_NavigationToolButton->setIcon(theme()->icon(Core::Constants::ICONPATIENT));
        m_NavigationMenu = new QMenu(m_NavigationToolButton);
        m_NavigationToolButton->setMenu(m_NavigationMenu);

//        // add buttons to search line
        ui->searchLine->setRightButton(m_NavigationToolButton);
    }

    void saveSettings()
    {
        settings()->setValue(Constants::S_SEARCHMETHOD, m_SearchMethod);
    }

public:
    Ui::PatientSelector *ui;
    PatientModel *m_Model;
    PatientSelector::FieldsToShow m_Fields;
    QToolButton *m_SearchToolButton, *m_NavigationToolButton;
    QMenu *m_NavigationMenu;
    int m_SearchMethod;
    QString m_LastSearch;
    PatientSelector::RefreshSearchResult m_refreshMethod;
    bool m_SetActivatedPatientAsCurrent;

private:
    PatientSelector *q;
};
}  // End namespace Internal
}  // End namespace Patients


/** \brief Create a PatientSelector with \e fields to show. \sa PatientSelector::FieldsToShow */
PatientSelector::PatientSelector(QWidget *parent, const FieldsToShow fields) :
    QWidget(parent),
    d(new Internal::PatientSelectorPrivate(this))
{
    d->ui->setupUi(this);
    d->ui->searchLine->setDelayedSignals(true);

    // Create internal patient model
    PatientModel *model = new PatientModel(this);
    setPatientModel(model);
    patientCore()->registerPatientModel(model);

    // datetime delegate
    d->ui->tableView->setItemDelegateForColumn(Core::IPatient::DateOfBirth, new Utils::DateTimeDelegate(this, true));

    d->createSearchToolButtons();

    // Some connections
    connect(d->m_NavigationToolButton->menu(), SIGNAL(aboutToShow()), this, SLOT(updateNavigationButton()));
    connect(d->ui->tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(changeIdentity(QModelIndex,QModelIndex)));
    connect(d->ui->tableView, SIGNAL(activated(QModelIndex)), this, SLOT(onPatientActivated(QModelIndex)));

    updatePatientActions(QModelIndex());
    if (fields == None) {
        d->m_Fields = FieldsToShow(settings()->value(Constants::S_SELECTOR_FIELDSTOSHOW, Default).toInt());
    } else {
        d->m_Fields = fields;
    }
    connect(user(), SIGNAL(userChanged()), this, SLOT(onUserChanged()));

    // Manage settings
    if (settings()->value(Constants::S_SEARCHWHILETYPING).toBool())
        setRefreshSearchResultMethod(WhileTyping);
    else
        setRefreshSearchResultMethod(ReturnPress);

    // Connect Core::IPatient model
    connect(patient(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onPatientDataChanged(QModelIndex,QModelIndex)));
}

PatientSelector::~PatientSelector()
{
    if (d) {
        d->saveSettings();
        delete d;
        d = 0;
    }
}

/** \brief Initialize view and actions, select the first available patient. */
void PatientSelector::initialize()
{
    // Here we assume that the Core::IPatient model is not already filtered
//    QModelIndex current;
//    if (!d->m_Model->currentPatient().isValid()) {
//        current = d->m_Model->index(0,0);
//        d->m_Model->blockSignals(true);
//        d->m_Model->setCurrentPatient(current);
//        d->m_Model->blockSignals(false);
//    } else {
//        current = d->m_Model->currentPatient();
//    }
//    d->ui->tableView->selectRow(current.row());
//    changeIdentity(current, QModelIndex());
}

void PatientSelector::updateNavigationButton()
{
    d->m_NavigationMenu->clear();

    Core::Command *cmd = actionManager()->command(Core::Constants::A_PATIENT_NEW);
    d->m_NavigationMenu->addAction(cmd->action());
    d->m_NavigationMenu->addSeparator();

    // add navigation actions (these actions are managed by the MainWindow)
    Core::ActionContainer *navMenu = actionManager()->actionContainer(Core::Constants::M_PATIENTS_NAVIGATION);
    if (!navMenu)
        return;
    for(int i = 0; i < navMenu->menu()->actions().count(); ++i) {
        d->m_NavigationMenu->addAction(navMenu->menu()->actions().at(i));
    }
}

/** \brief Define the search mode of the selector. */
void PatientSelector::setSearchMode(const int search)
{
    d->m_SearchMethod = search;
}

/** \brief Define the PatientModel to use. */
void PatientSelector::setPatientModel(PatientModel *m)
{
    Q_ASSERT(m);
    d->m_Model = m;
    d->ui->tableView->setModel(m);
    setFieldsToShow(d->m_Fields);

    d->ui->tableView->horizontalHeader()->setStretchLastSection(false);
    d->ui->tableView->horizontalHeader()->setSectionResizeMode(Core::IPatient::UsualName, QHeaderView::Stretch);
    d->ui->tableView->horizontalHeader()->setSectionResizeMode(Core::IPatient::OtherNames, QHeaderView::Stretch);
    d->ui->tableView->horizontalHeader()->setSectionResizeMode(Core::IPatient::Firstname, QHeaderView::Stretch);
    d->ui->tableView->horizontalHeader()->setSectionResizeMode(Core::IPatient::FullName, QHeaderView::ResizeToContents);
    d->ui->tableView->horizontalHeader()->setSectionResizeMode(Core::IPatient::IconizedGender, QHeaderView::ResizeToContents);
    d->ui->tableView->horizontalHeader()->setSectionResizeMode(Core::IPatient::Title, QHeaderView::ResizeToContents);
    d->ui->tableView->horizontalHeader()->setSectionResizeMode(Core::IPatient::DateOfBirth, QHeaderView::ResizeToContents);
    d->ui->tableView->horizontalHeader()->setSectionResizeMode(Core::IPatient::FullAddress, QHeaderView::Stretch);
    d->ui->tableView->horizontalHeader()->setSectionResizeMode(Core::IPatient::PractitionerUuid, QHeaderView::ResizeToContents);

    d->ui->numberOfPatients->setText(QString::number(m->numberOfFilteredPatients()));
    d->ui->identity->setCurrentPatientModel(m);
    connect(d->m_Model, SIGNAL(currentPatientChanged(QModelIndex)), this, SLOT(setSelectedPatient(QModelIndex)));
}

/** The pointer must not be deleted or shared */
bool PatientSelector::setFilterPatientModel(const QString &name, const QString &firstName, const QDate &dateOfBirth) const
{
    Q_UNUSED(dateOfBirth);
    // TODO: manage DateOfBirth
    d->m_Model->setFilter(name, firstName);
    return true;
}

/** \brief Define the fields to show using the FieldsToShow flag */
void PatientSelector::setFieldsToShow(const FieldsToShow fields)
{
    d->m_Fields = fields;
    // Hide all columns of the table view
    for(int i=0; i < Core::IPatient::NumberOfColumns; ++i) {
        d->ui->tableView->hideColumn(i);
    }
    // Show selected columns
    if (fields & PatientSelector::UsualName) {
        d->ui->tableView->showColumn(Core::IPatient::UsualName);
    }
    if (fields & PatientSelector::OtherNames) {
        d->ui->tableView->showColumn(Core::IPatient::OtherNames);
    }
    if (fields & PatientSelector::FirstName) {
        d->ui->tableView->showColumn(Core::IPatient::Firstname);
    }
    if (fields & PatientSelector::FullName) {
        d->ui->tableView->showColumn(Core::IPatient::FullName);
    }
    if (fields & PatientSelector::Gender) {
        d->ui->tableView->showColumn(Core::IPatient::IconizedGender);
    }
    if (fields & PatientSelector::Title) {
        d->ui->tableView->showColumn(Core::IPatient::Title);
    }
    if (fields & PatientSelector::DateOfBirth) {
        d->ui->tableView->showColumn(Core::IPatient::DateOfBirth);
    }
    if (fields & PatientSelector::FullAddress) {
        d->ui->tableView->showColumn(Core::IPatient::FullAddress);
    }
}

/** Change the way the search line edit reacts while the user is typing a search string. */
void PatientSelector::setRefreshSearchResultMethod(RefreshSearchResult method)
{
    disconnect(d->ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(refreshFilter()));
    disconnect(d->ui->searchLine, SIGNAL(textEdited(QString)), this, SLOT(refreshFilter()));
    d->m_refreshMethod = method;
    if (method == WhileTyping)
        connect(d->ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(refreshFilter()));
    else
        connect(d->ui->searchLine, SIGNAL(returnPressed()), this, SLOT(refreshFilter()));
}

/**
 * When a patient is activated in the view, by default the selector changes the current
 * patient to the activated one. You can change this behavior by setting this to \e false
 */
void PatientSelector::setOnPatientActivatedSetAsCurrent(bool setAsCurrent)
{
    d->m_SetActivatedPatientAsCurrent = setAsCurrent;
}

/** Return the UUID of the currently selected patient in the view */
QString PatientSelector::selectedPatientUid() const
{
    return d->m_Model->patientUuid(d->ui->tableView->currentIndex());
}

/**
 * \brief Define the selected patient (use this if patient was selected from outside the selector).
 * The index must be a Core::Patient model index.
*/
void PatientSelector::setSelectedPatient(const QModelIndex &index)
{
    d->ui->tableView->selectRow(index.row());
    updatePatientActions(index);
}

/**
 * \brief Define the selected patient using its row inside the table view.
*/
void PatientSelector::setSelectedPatient(int row)
{
    d->ui->tableView->selectRow(row);
}

/**
 * \brief Update the IdentityWidget with the new current identity.
 * \internal
 * This slot receive indexes from the proxymodel if the proxymodel is in use, otherwise
 * it receive indexes from the Core::IPatient model.
*/
void PatientSelector::changeIdentity(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    d->ui->identity->setCurrentIndex(current);
    updatePatientActions(current);
}

/*!
 * \brief Updates (enables/disables) the corresponding QActions for the given patient
 * \internal
 * Receive index from the Core::IPatient model (if the proxy model is in use you must
 * map the proxy index to the Core::IPatient index).
 */
void PatientSelector::updatePatientActions(const QModelIndex &index)
{
    const bool enabled = index.isValid();
    actionManager()->command(Core::Constants::A_PATIENT_VIEWIDENTITY)->action()->setEnabled(enabled);
    actionManager()->command(Core::Constants::A_PATIENT_REMOVE)->action()->setEnabled(enabled);
}

/**
 * \brief Refresh the search filter of the Patient::PatientModel
 * \internal
 * Refresh the search filter.
*/
void PatientSelector::refreshFilter()
{
    if (!d->m_Model)
        return;
    QString text = d->ui->searchLine->text();
    if (text == d->m_LastSearch)
        return;
    d->m_LastSearch = text;
    QString name, firstname;
    switch (d->m_SearchMethod) {
    case SearchByName: name = text; break;
    case SearchByNameFirstname: name = text.mid(0,text.indexOf(";")).trimmed(); firstname = text.right(text.indexOf(";")); break;
    case SearchByFirstname: firstname = text; break;
    case SearchByDOB: break;
    }
    d->m_Model->setFilter(name, firstname);
    d->ui->numberOfPatients->setText(QString::number(d->m_Model->numberOfFilteredPatients()));
}

/**
 * \brief Slot activated when the user selects a patient from the selector.
 * \internal
 * This slot receive proxy index if the proxymodel is in use.
 * \sa setSelectedPatient()
 */
void PatientSelector::onPatientActivated(const QModelIndex &index)
{
    if (!d->m_SetActivatedPatientAsCurrent)
        return;

    // if user click
//    if (d->m_Model && index == d->m_Model->currentPatient()) {
//        modeManager()->activateMode(Core::Constants::MODE_PATIENT_FILE);
//        return;
//    }

    // Start the processing spinner. The spinner will be ended by another process
    mainWindow()->startProcessingSpinner();

    // Inform Core::IPatient model wrapper
    const QString &uuid = d->m_Model->patientUuid(index);
    if (!patientCore()->setCurrentPatientUuid(uuid)) {
        LOG_ERROR("Unable to select the patient: " + uuid);
        mainWindow()->endProcessingSpinner();
    }
}

/**
 * \brief Update the model/view on patient data changed
 * \internal
*/
void PatientSelector::onPatientDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    // TODO: Refresh only if the current patient is visible in the model
    if (topLeft.row() == bottomRight.row()) {
        // Limit the model refreshing to address, name and dates
        if (IN_RANGE(topLeft.column(), bottomRight.column(), Core::IPatient::FullName))
            d->m_Model->refreshModel();
        else if (IN_RANGE(topLeft.column(), bottomRight.column(), Core::IPatient::FullAddress))
            d->m_Model->refreshModel();
        else if (IN_RANGE(topLeft.column(), bottomRight.column(), Core::IPatient::DateOfBirth))
            d->m_Model->refreshModel();
        else if (IN_RANGE(topLeft.column(), bottomRight.column(), Core::IPatient::DateOfDeath))
            d->m_Model->refreshModel();
        return;
    }
    // Unable to limit? Refresh
    d->m_Model->refreshModel();
}

/**
 * \brief Update the view on user changed
 * \internal
*/
void PatientSelector::onUserChanged()
{
    // TODO: reconnect user specific patient list (using the Practionner LKID)
    // reset the search filter
    d->ui->searchLine->clear();
    refreshFilter();
    // re-initialize the view
    initialize();
}

/**
 * \brief Manages patientbar visibility and translatability
 * \internal
 */
bool PatientSelector::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        break;
    case QEvent::Show:
        patientCore()->patientBar()->hide();
        break;
    case QEvent::Hide:
        patientCore()->patientBar()->show();
        break;
    default:
        break;
    }
    return QWidget::event(event);
}
