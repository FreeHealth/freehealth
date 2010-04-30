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
        q->connect(ui->searchName, SIGNAL(textChanged(QString)), q, SLOT(refreshFilter(QString)));
        q->connect(ui->searchSurname, SIGNAL(textChanged(QString)), q, SLOT(refreshFilter(QString)));
    }

    ~PatientSelectorPrivate()
    {
        delete ui;
    }

//    void createSearchToolButtons()
//    {
//        m_SearchToolButton = new QToolButton(searchLine);   // parent object will be redefined
//        m_SearchToolButton->setPopupMode(QToolButton::InstantPopup);
//        m_SearchToolButton->setIcon(theme()->icon(Core::Constants::ICONSEARCH));
//
//        Core::Command *cmd = actionManager()->command(Constants::A_SEARCH_BY_NAME);
//        m_SearchToolButton->addAction(cmd->action());
//        cmd->action()->trigger();
//
//        cmd = am->command(Constants::A_SEARCH_BY_NAME_SURNAME);
//        m_SearchToolButton->addAction(cmd->action());
//        cmd = am->command(Constants::A_SEARCH_INN);
//        m_SearchToolButton->addAction(cmd->action());
//
//        m_DrugsHistoricButton = new QToolButton(searchLine);
//        m_DrugsHistoricButton->setPopupMode(QToolButton::InstantPopup);
//        m_DrugsHistoricButton->setToolTip(tr("Selected drugs historic."));
//        m_DrugsHistoricButton->setIcon(Core::ICore::instance()->theme()->icon(Core::Constants::ICONEDIT));
//
//        // add buttons to search line
//        searchLine->setLeftButton(m_SearchToolButton);
//        searchLine->setRightButton(m_DrugsHistoricButton);
//    }


public:
    Ui::PatientSelector *ui;
    PatientModel *m_Model;
    PatientSelector::FieldsToShow m_Fields;
    QToolButton *m_SearchToolButton;

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
    if (d) {
        delete d;
        d = 0;
    }
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
    for(int i=0; i < PatientModel::NumberOfColumns; ++i) {
        d->ui->tableView->hideColumn(i);
    }
    // Show selected columns
    if (fields & PatientSelector::BirthName) {
        d->ui->tableView->showColumn(PatientModel::BirthName);
    }
    if (fields & PatientSelector::SecondName) {
        d->ui->tableView->showColumn(PatientModel::SecondName);
    }
    if (fields & PatientSelector::SurName) {
        d->ui->tableView->showColumn(PatientModel::Surname);
    }
    if (fields & PatientSelector::FullName) {
        d->ui->tableView->showColumn(PatientModel::FullName);
    }
    if (fields & PatientSelector::Gender) {
        d->ui->tableView->showColumn(PatientModel::IconizedGender);
    }
    if (fields & PatientSelector::Title) {
        d->ui->tableView->showColumn(PatientModel::Title);
    }
    if (fields & PatientSelector::DateOfBirth) {
        d->ui->tableView->showColumn(PatientModel::DateOfBirth);
    }
    if (fields & PatientSelector::FullAdress) {
        d->ui->tableView->showColumn(PatientModel::FullAddress);
    }
}

void PatientSelector::changeIdentity(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    d->ui->identity->setCurrentIndex(current);
}


void PatientSelector::refreshFilter(const QString &text)
{
    if (!d->m_Model)
        return;
    if (text.length() < 3)
        return;
    const QString &name = d->ui->searchName->text();
    const QString &sur = d->ui->searchSurname->text();
    if (name.length() >= 3 || sur.length() >= 3)
        d->m_Model->setFilter(name, sur);
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

//#include "patientselector.moc"
