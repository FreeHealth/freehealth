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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "usercalendarmodelfulleditorwidget.h"
#include "agendabase.h"
#include "usercalendarmodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/iuser.h>
#include <coreplugin/itheme.h>

#include <usermanagerplugin/usermodel.h>

#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include "ui_usercalendarmodelfulleditorwidget.h"

#include <QDataWidgetMapper>

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Agenda::Internal::AgendaBase *base() {return Agenda::Internal::AgendaBase::instance();}

UserCalendarModelFullEditorWidget::UserCalendarModelFullEditorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserCalendarModelFullEditorWidget),
    m_UserCalendarModel(0)
{
    QAction *a= aCreateCalendar = new QAction(this);
    a->setObjectName("aCreateCalendar");
    a->setText(tr("Create a new calendar"));
    a->setToolTip(a->text());

    ui->setupUi(this);
    ui->listView->setActions(Views::Constants::AddRemove);

    connect(ui->listView, SIGNAL(activated(QModelIndex)), ui->editor, SLOT(setCurrentIndex(QModelIndex)));
//    connect(ui->listView, SIGNAL(addRequested), ui->editor, SLOT(addUserCalendar()));

}

UserCalendarModelFullEditorWidget::~UserCalendarModelFullEditorWidget()
{
    delete ui;
}

void UserCalendarModelFullEditorWidget::setUserCalendarModel(UserCalendarModel *model)
{
    Q_ASSERT(model);
    qWarning() << Q_FUNC_INFO;
    m_UserCalendarModel = model;
    ui->listView->setModel(m_UserCalendarModel);
    ui->editor->setUserCalendarModel(m_UserCalendarModel);
}

//void UserCalendarModelFullEditorWidget::addUserCalendar()
//{
//    qWarning() << Q_FUNC_INFO;
//}

void UserCalendarModelFullEditorWidget::clear()
{
    ui->listView->setModel(0);
    if (m_UserCalendarModel)
        delete m_UserCalendarModel;
    m_UserCalendarModel = 0;
}

bool UserCalendarModelFullEditorWidget::submit()
{
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

