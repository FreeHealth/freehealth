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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "alertitemtimingeditorwidget.h"
#include "alertitem.h"
#include "ui_alertitemtimingeditorwidget.h"

#include <translationutils/constants.h>

using namespace Alert;
using namespace Trans::ConstantTranslations;

AlertItemTimingEditorWidget::AlertItemTimingEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Internal::Ui::AlertItemTimingEditorWidget)
{
    ui->setupUi(this);
    layout()->setMargin(0);

    // set up dateedits
    ui->startDate->setDisplayFormat(tkTr(Trans::Constants::DATETIMEFORMAT_FOR_EDITOR));
    ui->endDate->setDisplayFormat(tkTr(Trans::Constants::DATETIMEFORMAT_FOR_EDITOR));
    ui->cycleCombo->addItem(tr("Not cycling"));
    ui->cycleCombo->addItem(tr("Cycle every"));
    ui->cyclingEvery->addItems(Trans::ConstantTranslations::periods());

//    connect(ui->cycleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(cycleComboChanged(int)));
}

AlertItemTimingEditorWidget::~AlertItemTimingEditorWidget()
{
    delete ui;
}

void AlertItemTimingEditorWidget::clear()
{
    ui->cycleCombo->setCurrentIndex(-1);
    ui->cyclingEvery->setCurrentIndex(-1);
    ui->cycleDelayNumber->setValue(0);
    ui->cycles->setValue(0);
    ui->startDate->clear();
    ui->endDate->clear();
}

void AlertItemTimingEditorWidget::setAlertItem(const AlertItem &item)
{
    // Timings
    if (item.timings().count() > 0) {
        const AlertTiming &time = item.timingAt(0);
        ui->startDate->setDateTime(time.start());
        ui->endDate->setDateTime(time.end());
        if (time.isCycling())
            ui->cycleCombo->setCurrentIndex(1);
        else
            ui->cycleCombo->setCurrentIndex(0);
//        ui->cyclingEvery;
        ui->cycles->setValue(time.cyclingDelayInMinutes());
    }
}

bool AlertItemTimingEditorWidget::submit(AlertItem &item)
{
    return true;
}

void AlertItemTimingEditorWidget::cycleComboChanged(int index)
{
    if (index==1) {
        ui->cyclingEvery->show();
        ui->cycles->show();
        ui->cycleLabel->show();
        ui->cycleDelayNumber->show();
    } else {
        ui->cyclingEvery->hide();
        ui->cycles->hide();
        ui->cycleLabel->hide();
        ui->cycleDelayNumber->hide();
    }
}
