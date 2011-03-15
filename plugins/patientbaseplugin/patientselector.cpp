/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Patients::PatientSelector
  \brief Selector Widget for the recorded patients.
  Allow user to search and select patient from the complete database.
*/

#include "patientselector.h"
#include "patientmodel.h"
#include "patientbar.h"
#include "constants_menus.h"
#include "constants_settings.h"

#include "ui_patientselector.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/constants_menus.h>

#include <translationutils/constanttranslations.h>

#include <QToolButton>

#include <QDebug>

using namespace Patients;
using namespace Trans::ConstantTranslations;


/**
  \todo Two modes of searching : small db --> update on typing, big db --> update on validation
*/

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ActionManager *actionManager() { return Core::ICore::instance()->actionManager(); }


namespace Patients {
namespace Internal {
class PatientSelectorPrivate
{
public:
    PatientSelectorPrivate(PatientSelector *parent) :
            ui(new Ui::PatientSelector),
            m_Model(0),
            q(parent)
    {
    }

    ~PatientSelectorPrivate()
    {
        delete ui;
    }

    void createSearchToolButtons()
    {
        m_SearchToolButton = new QToolButton(ui->searchLine);   // parent object will be redefined
        m_SearchToolButton->setPopupMode(QToolButton::InstantPopup);
        m_SearchToolButton->setIcon(theme()->icon(Core::Constants::ICONSEARCH));

        Core::Command *cmd;
        QStringList actions;
        actions
                << Constants::A_SEARCH_PATIENTS_BY_NAME
                << Constants::A_SEARCH_PATIENTS_BY_FIRSTNAME
                << Constants::A_SEARCH_PATIENTS_BY_NAMEFIRSTNAME
                << Constants::A_SEARCH_PATIENTS_BY_DOB;

        QList<QAction *> l;
        foreach(const QString &a, actions) {
            cmd = actionManager()->command(a);
            m_SearchToolButton->addAction(cmd->action());
            l << cmd->action();
        }

        // add buttons to search line
        ui->searchLine->setLeftButton(m_SearchToolButton);

        int id = settings()->value(Constants::S_SEARCHMETHOD, 0).toInt();
        if (id < l.count() && id >= 0) {
            l.at(id)->trigger();
            l.at(id)->setChecked(true);
        }

        // add action to the navigation button
        m_NavigationToolButton = new QToolButton(ui->searchLine);   // parent object will be redefined
        m_NavigationToolButton->setPopupMode(QToolButton::InstantPopup);
        m_NavigationToolButton->setIcon(theme()->icon(Core::Constants::ICONPATIENT));
        m_NavigationMenu = new QMenu(m_NavigationToolButton);
        cmd = actionManager()->command(Core::Constants::A_PATIENT_NEW);
        m_NavigationMenu->addAction(cmd->action());
        m_NavigationMenu->addSeparator();
        m_NavigationToolButton->setMenu(m_NavigationMenu);

        // add buttons to search line
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

private:
    PatientSelector *q;
};


}  // End namespace Internal
}  // End namespace Patients


/** \brief Create a PatientSelector with \e fields to show. \sa PatientSelector::FieldsToShow */
PatientSelector::PatientSelector(QWidget *parent, const FieldsToShow fields) :
        QWidget(parent), d(0)
{
    d = new Internal::PatientSelectorPrivate(this);
    d->ui->setupUi(this);
    d->ui->searchLine->setDelayedSignals(true);

    if (fields == None) {
        d->m_Fields = FieldsToShow(settings()->value(Constants::S_SELECTOR_FIELDSTOSHOW, Default).toInt());
    } else {
        d->m_Fields = fields;
    }

    // Get the active model
    if (!PatientModel::activeModel()) {
        PatientModel *model = new PatientModel(this);
        PatientModel::setActiveModel(model);
        setPatientModel(model);
    } else {
        setPatientModel(PatientModel::activeModel());
    }

    // Some connections
    connect(d->ui->searchLine, SIGNAL(textChanged(QString)), this, SLOT(refreshFilter(QString)));
    connect(d->ui->tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(changeIdentity(QModelIndex,QModelIndex)));
    connect(d->ui->tableView, SIGNAL(activated(QModelIndex)), this, SLOT(onPatientSelected(QModelIndex)));
}

PatientSelector::~PatientSelector()
{
    d->saveSettings();
    if (d) {
        delete d;
        d = 0;
    }
}

/** \brief Initialize view and actions. */
void PatientSelector::init()
{
    d->createSearchToolButtons();
    // connect navigationButton pressed -> update navigation actions
    connect(d->m_NavigationToolButton->menu(), SIGNAL(aboutToShow()), this, SLOT(updateNavigationButton()));
}

void PatientSelector::updateNavigationButton()
{
    // remove all actions from the second
    for(int i = 2; i < d->m_NavigationToolButton->actions().count(); ++i) {
        d->m_NavigationMenu->removeAction(d->m_NavigationMenu->actions().at(i));
    }
    // add navigation actions
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
    d->ui->tableView->resizeColumnsToContents();
    d->ui->numberOfPatients->setText(QString::number(m->numberOfFilteredPatients()));
    d->ui->identity->setCurrentPatientModel(m);

    connect(m, SIGNAL(patientChanged(QModelIndex)), this, SLOT(setSelectedPatient(QModelIndex)));
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
    if (fields & PatientSelector::BirthName) {
        d->ui->tableView->showColumn(Core::IPatient::BirthName);
    }
    if (fields & PatientSelector::SecondName) {
        d->ui->tableView->showColumn(Core::IPatient::SecondName);
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
    if (fields & PatientSelector::FullAdress) {
        d->ui->tableView->showColumn(Core::IPatient::FullAddress);
    }

    // debug
    d->ui->tableView->showColumn(Core::IPatient::PractitionnerLkID);
}

/** \brief Define the selected patient (use this if patient was selected from outside the selector). */
void PatientSelector::setSelectedPatient(const QModelIndex &index)
{
    d->ui->tableView->selectRow(index.row());
}

/** \brief Update the IdentityWidget with the new current identity. */
void PatientSelector::changeIdentity(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    d->ui->identity->setCurrentIndex(current);
}

/** \brief Refresh the search filter of the Patient::PatientModel */
void PatientSelector::refreshFilter(const QString &)
{
    if (!d->m_Model)
        return;
    QString text = d->ui->searchLine->searchText();
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

/** \brief Slot activated when the user select a patient from the selector. \sa setSelectedPatient()*/
void PatientSelector::onPatientSelected(const QModelIndex &index)
{
    // Inform Core::IPatient model wrapper
    if (!d->m_Model)
        PatientModel::activeModel()->setCurrentPatient(index);
    else
        d->m_Model->setCurrentPatient(index);
}

bool PatientSelector::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        break;
    case QEvent::Show:
        PatientBar::instance()->hide();
        break;
    case QEvent::Hide:
        PatientBar::instance()->show();
        break;
    default:
        break;
    }
    return QWidget::event(event);
}
