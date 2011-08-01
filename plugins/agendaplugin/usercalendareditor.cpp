/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developpers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class Agenda::UserCalendarEditorWidget
  Is a data widget mapper to the Agenda::UserCalendarModel.
*/

#include "usercalendareditor.h"
#include "usercalendarmodel.h"
#include "usercalendar.h"

#include "ui_usercalendareditor.h"

#include <QDebug>

using namespace Agenda;

UserCalendarEditorWidget::UserCalendarEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserCalendarEditorWidget),
    m_UserCalendarModel(0),
    m_AvailabilityModel(0),
    m_Mapper(0)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(setFocus()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(submit()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(revert()));
}

UserCalendarEditorWidget::~UserCalendarEditorWidget()
{
    delete ui;
    if (m_AvailabilityModel)
        delete m_AvailabilityModel;
    m_AvailabilityModel = 0;
}

/** Define the Agenda::UserCalendarModel to use. */
void UserCalendarEditorWidget::setUserCalendarModel(UserCalendarModel *model)
{
    m_UserCalendarModel = model;
}

/** Define the \e index to use. You must firstly define the Agenda::UserCalendarModel with setUserCalendarModel(). */
void UserCalendarEditorWidget::setCurrentIndex(const QModelIndex &index)
{
    Q_ASSERT(m_UserCalendarModel);
    if (!m_UserCalendarModel)
        return;

    // Create mapper
    if (!m_Mapper) {
        m_Mapper = new QDataWidgetMapper(this);
        m_Mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
        m_Mapper->setModel(m_UserCalendarModel);
        m_Mapper->addMapping(ui->calendarLabel, UserCalendarModel::Label);
        m_Mapper->addMapping(ui->defaultLocation, UserCalendarModel::LocationUid);
        m_Mapper->addMapping(ui->description, UserCalendarModel::Description);
        m_Mapper->addMapping(ui->defaultDuration, UserCalendarModel::DefaultDuration);
        m_Mapper->addMapping(ui->isDefaultCheck, UserCalendarModel::IsDefault, "checked");
        m_Mapper->addMapping(ui->isPrivateCheck, UserCalendarModel::IsPrivate, "checked");
        m_Mapper->addMapping(ui->password, UserCalendarModel::Password);
    }
    m_Mapper->setCurrentIndex(index.row());

    // Availability
    if (m_AvailabilityModel) {
        delete m_AvailabilityModel;
        m_AvailabilityModel = 0;
    }
    m_AvailabilityModel = m_UserCalendarModel->availabilityModel(index, this);
    ui->availabilityView->setModel(m_AvailabilityModel);
}

/** Submit changes to the model. */
void UserCalendarEditorWidget::submit()
{
    m_Mapper->submit();
}

/** Submit changes to the model. */
void UserCalendarEditorWidget::revert()
{
    m_Mapper->revert();
}

void UserCalendarEditorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
