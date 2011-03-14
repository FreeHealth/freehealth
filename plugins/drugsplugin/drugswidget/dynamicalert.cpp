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
#include "dynamicalert.h"

#include <drugsbaseplugin/druginteractionresult.h>
#include <drugsbaseplugin/druginteractioninformationquery.h>
#include <drugsbaseplugin/idruginteractionalert.h>

#include "ui_dynamicalert.h"

#include <QTabWidget>
#include <QDialogButtonBox>
#include <QLabel>

using namespace DrugsWidget;

DynamicAlert::DynamicAlert(const DrugsDB::DrugInteractionInformationQuery &query, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DynamicAlert)
{
    ui->setupUi(this);
    QVector<DrugsDB::IDrugInteractionAlert *> alerts = query.result->alerts(query);
    QVector<int> alertsToUse;
    for(int i=0; i<alerts.count(); ++i) {
        DrugsDB::IDrugInteractionAlert *alert = alerts.at(i);
        if (alert->hasDynamicAlertWidget(query))
            alertsToUse << i;
    }

    // No alerts -> should never be the case ==> assert
    Q_ASSERT(!alertsToUse.isEmpty());

    if (alertsToUse.count()==1) {
        // No tabwidget
        DrugsDB::IDrugInteractionAlert *alert = alerts.at(alertsToUse.at(0));
        QLabel *label = new QLabel(this);
        label->setWordWrap(true);
        label->setTextFormat(Qt::RichText);
        DrugsDB::DrugInteractionInformationQuery myQuery(query);
        myQuery.messageType = DrugsDB::DrugInteractionInformationQuery::InformationAlert;
        label->setText(alert->message(myQuery.relatedDrug, myQuery));
        ui->layout->addWidget(label, 0, 0);
    } else {
        // With tabwidget
        QTabWidget *tab = new QTabWidget(this);
        for(int i=0; i < alertsToUse.count(); ++i) {
            DrugsDB::IDrugInteractionAlert *alert = alerts.at(alertsToUse.at(i));
            QLabel *label = new QLabel(this);
            label->setWordWrap(true);
            label->setTextFormat(Qt::RichText);
            DrugsDB::DrugInteractionInformationQuery myQuery(query);
            myQuery.messageType = DrugsDB::DrugInteractionInformationQuery::InformationAlert;
            label->setText(alert->message(myQuery.relatedDrug, myQuery));
            tab->addTab(label, alert->engineUid());
        }
        ui->layout->addWidget(tab, 0, 0);
    }

    // Add buttons
    QDialogButtonBox *box = new QDialogButtonBox(this);
    QPushButton *accept = box->addButton(tr("Accept alert and cancel last action"), QDialogButtonBox::AcceptRole);
    QPushButton *override = box->addButton(tr("Override alert and go on"), QDialogButtonBox::RejectRole);
    connect(box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(box, SIGNAL(rejected()), this, SLOT(reject()));
    ui->layout->addWidget(box);
}

DynamicAlert::~DynamicAlert()
{
    delete ui;
}

DynamicAlert::DialogResult DynamicAlert::executeDynamicAlert(const DrugsDB::DrugInteractionInformationQuery &query, QWidget *parent)
{
    if (!query.result)
        return NoDynamicAlert;
    // test all alerts for dynamic alerts
    const QVector<DrugsDB::IDrugInteractionAlert *> &alerts = query.result->alerts(query);
    bool hasAlert = false;
    for(int i=0; i<alerts.count(); ++i) {
        if (alerts.at(i)->hasDynamicAlertWidget(query)) {
            hasAlert = true;
            break;
        }
    }
    if (!hasAlert)
        return NoDynamicAlert;

    DynamicAlert dlg(query, parent);
    dlg.setModal(true);
    if (dlg.exec()==QDialog::Accepted) {
        return DynamicAlertAccepted;
    }
    /** \todo create an interface class like IDrugDynamicAlertListener and get object in the plugin manager pool. To allow plugins to implement alert overridden logging. */
    return DynamicAlertOverridden;
}

void DynamicAlert::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
