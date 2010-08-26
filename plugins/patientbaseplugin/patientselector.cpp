/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "patientselector.h"
#include "patientmodel.h"
#include "constants_menus.h"
#include "constants_settings.h"

#include "ui_patientselector.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/constants_icons.h>

#include <QToolButton>

#include <QDebug>

using namespace Patients;

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
        ui->setupUi(q);
        q->connect(ui->searchLine, SIGNAL(textChanged(QString)), q, SLOT(refreshFilter(QString)));
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
    }

    void saveSettings()
    {
        settings()->setValue(Constants::S_SEARCHMETHOD, m_SearchMethod);
    }


public:
    Ui::PatientSelector *ui;
    PatientModel *m_Model;
    PatientSelector::FieldsToShow m_Fields;
    QToolButton *m_SearchToolButton;
    int m_SearchMethod;

private:
    PatientSelector *q;
};


}  // End namespace Internal
}  // End namespace Patients



PatientSelector::PatientSelector(QWidget *parent, const FieldsToShow fields) :
        QWidget(parent), d(0)
{
    d = new Internal::PatientSelectorPrivate(this);
    if (fields == None) {
        d->m_Fields = FieldsToShow(settings()->value(Constants::S_SELECTOR_FIELDSTOSHOW, Default).toInt());
    } else {
        d->m_Fields = fields;
    }
    if (!PatientModel::activeModel()) {
        PatientModel *model = new PatientModel(this);
        PatientModel::setActiveModel(model);
        setPatientModel(model);
    } else {
        setPatientModel(PatientModel::activeModel());
    }
    connect(d->ui->tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(changeIdentity(QModelIndex,QModelIndex)));
    connect(d->ui->tableView, SIGNAL(activated(QModelIndex)), this, SIGNAL(patientSelected(QModelIndex)));
}

PatientSelector::~PatientSelector()
{
    d->saveSettings();
    if (d) {
        delete d;
        d = 0;
    }
}

void PatientSelector::init()
{
    d->createSearchToolButtons();
}

void PatientSelector::setSearchMode(const int search)
{
    d->m_SearchMethod = search;
}

void PatientSelector::setPatientModel(PatientModel *m)
{
    d->m_Model = m;
    d->ui->tableView->setModel(m);
    setFieldsToShow(d->m_Fields);
    d->ui->tableView->resizeColumnsToContents();
    d->ui->numberOfPatients->setText(QString::number(m->numberOfFilteredPatients()));
    d->ui->identity->setCurrentPatientModel(m);
}

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

void PatientSelector::setSelectedPatient(const QModelIndex &index)
{
    d->ui->tableView->setCurrentIndex(index);
}

void PatientSelector::changeIdentity(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    d->ui->identity->setCurrentIndex(current);
}

void PatientSelector::refreshFilter(const QString &)
{
    if (!d->m_Model)
        return;
    QString text = d->ui->searchLine->searchText();
    if (text.length() < 3)
        return;
    QString name, firstname;
    switch (d->m_SearchMethod) {
    case SearchByName: name = text; break;
    case SearchByNameFirstname: name = text.mid(0,text.indexOf(";")).trimmed(); firstname = text.right(text.indexOf(";")); break;
    case SearchByFirstname: firstname = text; break;
    case SearchByDOB: break;
    }

    if (name.length() >= 3 || firstname.length() >= 3)
        d->m_Model->setFilter(name, firstname);
    d->ui->numberOfPatients->setText(QString::number(d->m_Model->numberOfFilteredPatients()));
}

void PatientSelector::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
