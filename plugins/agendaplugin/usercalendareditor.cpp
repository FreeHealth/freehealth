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
 *   Main Developers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
  \class Agenda::UserCalendarEditorWidget
  Is a data widget mapper to the Agenda::UserCalendarModel.
*/

#include "usercalendareditor.h"
#include "usercalendarmodel.h"
#include "usercalendar.h"
#include "availabilitycreatordialog.h"

#include <utils/log.h>
#include <utils/global.h>

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
    ui->clearAvail->setVisible(false);
    ui->tabWidget->setCurrentIndex(0);
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(setFocus()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(submit()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(revert()));
//    connect(ui->clearAvail, SIGNAL(clicked()), this, SLOT(clearAvailabilities()));
    connect(ui->removeAvail, SIGNAL(clicked()), this, SLOT(removeAvailabilities()));
    connect(ui->addAvailability, SIGNAL(clicked()), this, SLOT(addAvailability()));
}

UserCalendarEditorWidget::~UserCalendarEditorWidget()
{
    delete ui;
    if (m_AvailabilityModel)
        delete m_AvailabilityModel;
    m_AvailabilityModel = 0;
}

void UserCalendarEditorWidget::clear()
{
    // TODO: code here
    revert();
}

/** Define the Agenda::UserCalendarModel to use. */
void UserCalendarEditorWidget::setUserCalendarModel(UserCalendarModel *model)
{
    m_UserCalendarModel = model;
    ui->userCalendarDelegatesWidget->setUserCalendarModel(model);
}

/** Define the \e index to use. You must firstly define the Agenda::UserCalendarModel with setUserCalendarModel(). */
void UserCalendarEditorWidget::setCurrentIndex(const QModelIndex &index)
{
//    Q_ASSERT(m_UserCalendarModel);
    if (!m_UserCalendarModel)
        return;

    // Create mapper
    if (!m_Mapper) {
        m_Mapper = new QDataWidgetMapper(this);
        m_Mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
        m_Mapper->setModel(m_UserCalendarModel);
        m_Mapper->addMapping(ui->calendarLabel, UserCalendarModel::Label);
        m_Mapper->addMapping(ui->defaultLocationEdit, UserCalendarModel::LocationUid);
        m_Mapper->addMapping(ui->description, UserCalendarModel::Description);
        m_Mapper->addMapping(ui->defaultDuration, UserCalendarModel::DefaultDuration);
        // TODO: wrong mapping with QCheckBoxes
        m_Mapper->addMapping(ui->isDefaultCheck, UserCalendarModel::IsDefault, "checked");
        m_Mapper->addMapping(ui->isPrivateCheck, UserCalendarModel::IsPrivate, "checked");
        m_Mapper->addMapping(ui->passwordEdit, UserCalendarModel::Password);
    }
    if (index.isValid())
        m_Mapper->setCurrentIndex(index.row());
    else {
        m_Mapper->setCurrentIndex(-1);
        clear();
    }

    // Availability
    if (m_AvailabilityModel) {
        delete m_AvailabilityModel;
        m_AvailabilityModel = 0;
    }
    m_AvailabilityModel = m_UserCalendarModel->availabilityModel(index, this);
    ui->availabilityView->setModel(m_AvailabilityModel);

    // Set delegates
    ui->userCalendarDelegatesWidget->setUserCalendarIndex(index.row());
}

void UserCalendarEditorWidget::addAvailability()
{
    if (!m_Mapper)
        return;
    if (!m_AvailabilityModel)
        return;
    AvailabilityCreatorDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        // save availabilities to the userCalendar or cache them
        const QList<DayAvailability> &av = dlg.getAvailability();
        for(int i = 0 ; i < av.count(); ++i) {
            m_AvailabilityModel->addAvailability(av.at(i));
        }
//        m_UserCalendarModel->updateUserCalendarChanged(m_Mapper->currentIndex());
    }
}

void UserCalendarEditorWidget::clearAvailabilities()
{
//    if (m_AvailabilityModel)
//        m_AvailabilityModel->clearAvailabilities();
}

void UserCalendarEditorWidget::removeAvailabilities()
{
    if (m_AvailabilityModel) {
        QModelIndex idx = ui->availabilityView->currentIndex();
        if (idx.parent()==QModelIndex()) {
            if (m_AvailabilityModel->rowCount()==1) {
                Utils::warningMessageBox(tr("Can not delete all availabilities"),
                                         tr("Agenda must have at least one availability. "
                                            "You can not delete all its availabilities."));
                return;
            }
            bool yes = Utils::yesNoMessageBox(tr("Delete full day."),
                                              tr("You are about to delete all recorded time range for one specific day of the week (%1). "
                                                 "Do you really want to delete all time range?").arg(idx.data().toString()));
            if (!yes)
                return;
        } else {
            if ((m_AvailabilityModel->rowCount()==1) && (m_AvailabilityModel->rowCount(idx)==1)) {
                Utils::warningMessageBox(tr("Can not delete all availabilities"),
                                         tr("Agenda must have at least one availability. You can not delete all its availabilities."));
                return;
            }
        }
        m_AvailabilityModel->removeAvailability(idx);
    }
}


/** Submit changes to the model. */
void UserCalendarEditorWidget::submit()
{
    ui->userCalendarDelegatesWidget->submit();
    if (m_AvailabilityModel)
        m_AvailabilityModel->submit();
    if (m_Mapper)
        m_Mapper->submit();
}

/** Submit changes to the model. */
void UserCalendarEditorWidget::revert()
{
    if (m_Mapper)
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
