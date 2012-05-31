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
#include "alertitemeditorwidget.h"
#include "ui_alertitemeditorwidget.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

using namespace Alert;
using namespace Trans::ConstantTranslations;

AlertItemEditorWidget::AlertItemEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlertItemEditorWidget)
{
    ui->setupUi(this);
    // set up combo
    ui->priority->addItem(tkTr(Trans::Constants::HIGH));
    ui->priority->addItem(tkTr(Trans::Constants::MEDIUM));
    ui->priority->addItem(tkTr(Trans::Constants::LOW));

    // DynamicAlert = 0,
    // StaticAlert,
    ui->viewType->addItem(tr("Dynamic alert"));
    ui->viewType->addItem(tr("Static alert"));

    // ApplicationNotification = 0,
    // PatientCondition,
    // UserNotification
    ui->contentType->addItem(tr("Application notification"));
    ui->contentType->addItem(tr("Patient bar notification"));
    ui->contentType->addItem(tr("Status bar notification"));

    // RelatedToPatient = 0,
    // RelatedToFamily,
    // RelatedToAllPatients,
    // RelatedToUser,
    // RelatedToUserGroup,
    // RelatedToApplication
    ui->relatedTo->addItem(tkTr(Trans::Constants::RELATED_TO_CURRENT_PATIENT));
//    ui->relatedTo->addItem(tkTr(Trans::Constants::RELATED_TO_PATIENT_FAMILY_1).arg(""));
    ui->relatedTo->addItem(tkTr(Trans::Constants::RELATED_TO_ALL_PATIENTS));
    ui->relatedTo->addItem(tkTr(Trans::Constants::RELATED_TO_CURRENT_USER));
//    ui->relatedTo->addItem(tkTr(Trans::Constants::RELATED_TO_USER_GROUP_1));
    ui->relatedTo->addItem(tkTr(Trans::Constants::RELATED_TO_APPLICATION));

    ui->cycleCombo->addItem(tr("Not cycling"));
    ui->cycleCombo->addItem(tr("Cycle"));
//    ui->cyclingEvery;
}

AlertItemEditorWidget::~AlertItemEditorWidget()
{
    delete ui;
}
