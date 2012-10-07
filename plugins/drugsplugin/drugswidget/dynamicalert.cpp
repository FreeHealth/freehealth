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
#include "dynamicalert.h"

#include <drugsplugin/drugswidget/interactionsynthesisdialog.h>

#include <drugsbaseplugin/druginteractionresult.h>
#include <drugsbaseplugin/druginteractioninformationquery.h>
#include <drugsbaseplugin/idruginteractionalert.h>
#include <drugsbaseplugin/constants.h>
#include <drugsbaseplugin/idrugengine.h>
#include <drugsbaseplugin/drugsmodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/imainwindow.h>

#include <utils/global.h>

#include "ui_dynamicalert.h"

#include <QTabWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QToolButton>

#include <QDebug>

using namespace DrugsWidget;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline DrugsDB::DrugsModel *drugModel() { return DrugsDB::DrugsModel::activeModel(); }

static inline QWidget *engineTitle(QWidget *parent, DrugsDB::IDrugEngine *engine)
{
    QWidget *w = new QWidget(parent);
    QHBoxLayout *lay = new QHBoxLayout(w);
    w->setLayout(lay);

    QLabel *icon = new QLabel(w);
    icon->setPixmap(engine->icon(Core::ITheme::MediumIcon).pixmap(32,32));

    QLabel *name = new QLabel(w);
    name->setText("<b>"+engine->name()+"</b>");

    lay->addWidget(icon);
    lay->addWidget(name, 10);
    return w;
}

DynamicAlert::DynamicAlert(const DrugsDB::DrugInteractionInformationQuery &query, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DynamicAlert)
{
    ui->setupUi(this);
    setWindowTitle(tr("Drug interaction alert"));
    setWindowIcon(theme()->icon(DrugsDB::Constants::I_DRUGENGINE));
    setWindowModality(Qt::WindowModal);
    ui->generalIconLabel->setPixmap(theme()->icon(DrugsDB::Constants::I_DRUGENGINE, Core::ITheme::BigIcon).pixmap(64,64));
    QVector<DrugsDB::IDrugInteractionAlert *> alerts = query.result->alerts(query);
    QVector<int> alertsToUse;
    for(int i=0; i<alerts.count(); ++i) {
        DrugsDB::IDrugInteractionAlert *alert = alerts.at(i);
        if (alert->hasDynamicAlertWidget(query))
            alertsToUse << i;
    }

    // No alerts -> should never be the case ==> assert
    Q_ASSERT(!alertsToUse.isEmpty());

//    qWarning() << "nbOfDynAlerts" << alertsToUse.count();

    if (alertsToUse.count()==1) {
        // No tabwidget
        DrugsDB::IDrugInteractionAlert *alert = alerts.at(alertsToUse.at(0));

        QLabel *label = new QLabel(this);
        label->setTextFormat(Qt::RichText);
        label->setWordWrap(true);
//        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        DrugsDB::DrugInteractionInformationQuery myQuery(query);
        myQuery.messageType = DrugsDB::DrugInteractionInformationQuery::InformationAlert;
        myQuery.iconSize = DrugsDB::DrugInteractionInformationQuery::BigSize;
        myQuery.levelOfWarningStaticAlert = myQuery.levelOfWarningDynamicAlert;
        label->setText(alert->message(myQuery.relatedDrug, myQuery));

        ui->centralLayout->addWidget(engineTitle(this, alert->engine()), 0, 0);
        ui->centralLayout->addWidget(label, 1, 0);
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

            QWidget *w = new QWidget(this);
            QVBoxLayout *lay = new QVBoxLayout(w);
            lay->setMargin(0);
            lay->setSpacing(5);
            w->setLayout(lay);
            lay->addWidget(engineTitle(this, alert->engine()));
            lay->addWidget(label);

            tab->addTab(w, alert->engine()->icon(Core::ITheme::SmallIcon), alert->engine()->shortName());
        }

        ui->centralLayout->addWidget(tab, 0, 0);
    }

    // Add buttons
    QDialogButtonBox *box = new QDialogButtonBox(this);
    QToolButton *accept = new QToolButton(this);
    accept->setText(tr("Accept alert and cancel last action"));
    accept->setIcon(theme()->icon(DrugsDB::Constants::I_DRUGALERT_ACCEPT, Core::ITheme::MediumIcon));
    accept->setIconSize(QSize(32,32));
    accept->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    box->addButton(accept, QDialogButtonBox::AcceptRole);

    QToolButton *override = new QToolButton(this);
    override->setText(tr("Override alert and go on"));
    override->setIcon(theme()->icon(DrugsDB::Constants::I_DRUGALERT_OVERRIDE, Core::ITheme::MediumIcon));
    override->setIconSize(QSize(32,32));
    override->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    box->addButton(override, QDialogButtonBox::RejectRole);

    QToolButton *showSynthesis = new QToolButton(this);
    showSynthesis->setText(tr("Show full interactions information"));
    showSynthesis->setIcon(theme()->icon(DrugsDB::Constants::I_DRUGENGINE, Core::ITheme::MediumIcon));
    showSynthesis->setIconSize(QSize(32,32));
    showSynthesis->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    box->addButton(showSynthesis, QDialogButtonBox::HelpRole);

    connect(box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(box, SIGNAL(rejected()), this, SLOT(reject()));
    connect(showSynthesis, SIGNAL(clicked()), this, SLOT(showInteractionSynthesisDialog()));
    ui->buttonLayout->addWidget(box);

    adjustSize();
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
    if (dlg.exec()==QDialog::Accepted) {
        return DynamicAlertAccepted;
    }
    // TODO: create an interface class like IDrugDynamicAlertListener and get object in the plugin manager pool. To allow plugins to implement alert overridden logging.
    return DynamicAlertOverridden;
}

void DynamicAlert::showInteractionSynthesisDialog()
{
    InteractionSynthesisDialog dlg(drugModel(), this);
    Utils::resizeAndCenter(&dlg, Core::ICore::instance()->mainWindow());
    dlg.exec();
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
