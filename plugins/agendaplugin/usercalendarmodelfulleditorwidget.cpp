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
/**
  \class Agenda::UserCalendarModelFullEditorWidget
  Is a full editor widget for Agenda::UserCalendar. A view on the left allow the user to select
  the calendar to edit in the right part of the widget.
*/

#include "usercalendarmodelfulleditorwidget.h"
#include "agendabase.h"
#include "agendacore.h"
#include "usercalendarmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>
#include <coreplugin/modemanager/modemanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/constants_menus.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_agenda.h>

#include "ui_usercalendarmodelfulleditorwidget.h"

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ModeManager *modeManager() {return Core::ICore::instance()->modeManager();}
static inline Core::ActionManager *actionManager() {return Core::ICore::instance()->actionManager();}
static inline Agenda::Internal::AgendaBase &base() {return Agenda::AgendaCore::instance().agendaBase();}

UserCalendarModelFullEditorWidget::UserCalendarModelFullEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserCalendarModelFullEditorWidget),
    m_UserCalendarModel(0)
{
//    QAction *a= aCreateCalendar = new QAction(this);
//    a->setObjectName("aCreateCalendar");
//    a->setText(tr("Create a new calendar"));
//    a->setToolTip(a->text());

    ui->setupUi(this);
    ui->editor->setEnabled(false);

    connect(ui->agendaNamesCombo, SIGNAL(currentIndexChanged(QModelIndex)), this, SLOT(setCurrentIndex(QModelIndex)));
    // INFO: User calendars are added/removed using the Combos actions' connected to the view model
}

UserCalendarModelFullEditorWidget::~UserCalendarModelFullEditorWidget()
{
    delete ui;
}

void UserCalendarModelFullEditorWidget::setUserCalendarModel(UserCalendarModel *model)
{
    Q_ASSERT(model);
    if (!model)
        return;
    if (m_UserCalendarModel==model)
        return;
    m_UserCalendarModel = model;
    ui->editor->setUserCalendarModel(m_UserCalendarModel);
    ui->agendaNamesCombo->setModel(m_UserCalendarModel);
    ui->agendaNamesCombo->setModelColumn(UserCalendarModel::Label);
}

void UserCalendarModelFullEditorWidget::setCurrentIndex(const QModelIndex &index)
{
    // inform UserCalendarEditor
    ui->editor->setEnabled(index.isValid());
    ui->editor->setCurrentIndex(index);
}

void UserCalendarModelFullEditorWidget::clear()
{
    ui->editor->clear();
    ui->editor->setEnabled(false);
    ui->agendaNamesCombo->setModel(0);
    m_UserCalendarModel = 0;
}

bool UserCalendarModelFullEditorWidget::submit()
{
    ui->editor->submit();
    if (m_UserCalendarModel)
        return m_UserCalendarModel->submit();
    return false;
}

void UserCalendarModelFullEditorWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        aCreateCalendar->setText(tr("Create a new calendar"));
        aCreateCalendar->setToolTip(aCreateCalendar->text());
    }
}

