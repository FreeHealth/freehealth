/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Agenda::Internal::UserCalendarWizardCreatorPage
 * Is an Internal class of the Agenda plugin and an extension
 * for the UserPlugin::UserCreatorWizard (user creator wizard).
*/

#include "usercalendarwizardcreatorpage.h"
#include "usercalendar.h"
#include "constants.h"
#include "agendabase.h"
#include "agendacore.h"

#include <utils/global.h>

#include "ui_usercalendarwizardcreatorpage.h"

using namespace Agenda;
using namespace Internal;

static inline Agenda::Internal::AgendaBase &base() {return Agenda::AgendaCore::instance().agendaBase();}


/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// UserCalendarWizardCreatorPage ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserCalendarWizardCreatorPage::UserCalendarWizardCreatorPage(QObject *parent) :
    IUserWizardPage(parent),
    page(0)
{
    setObjectName("UserCalendarWizardCreatorPage");
}

UserCalendarWizardCreatorPage::~UserCalendarWizardCreatorPage()
{}

QString UserCalendarWizardCreatorPage::displayName() const
{
    return tr("User's agendas");
}

QString UserCalendarWizardCreatorPage::title() const
{
    return tr("User's agendas");
}

QString UserCalendarWizardCreatorPage::category() const
{
    return tr("User's agendas");
}

int UserCalendarWizardCreatorPage::sortIndex() const
{
    return 20;
}

// UserPlugin::IUserWizardPage
QWizardPage *UserCalendarWizardCreatorPage::createWizardPage(QWidget *parent)
{
    page = new UserCalendarWizardPage(parent);
    return page;
}

void UserCalendarWizardCreatorPage::submit(const QString &userUid)
{
    if (page) {
        UserCalendar *u = page->getUserCalendar(userUid);
        base().saveUserCalendar(u);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// WizardPage //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserCalendarWizardPage::UserCalendarWizardPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::UserCalendarWizardCreatorWidget)
{
  ui->setupUi(this);
  ui->createAgenda->setChecked(false);
  connect(ui->createAgenda, SIGNAL(clicked(bool)), this, SLOT(toggleAgendaEditing(bool)));
  retranslate();
}

UserCalendarWizardPage::~UserCalendarWizardPage()
{
    delete ui;
}

UserCalendar *UserCalendarWizardPage::getUserCalendar(const QString &userUid)
{
    UserCalendar *u = base().createEmptyCalendar(userUid);
    u->setData(UserCalendar::Label, ui->calendarLabel->text());
    u->setData(UserCalendar::DefaultDuration, ui->defaultDuration->value());
    u->setData(UserCalendar::Description, ui->description->toHtml());
    u->setData(UserCalendar::LocationUid, ui->defaultLocation->text());
    u->setData(UserCalendar::IsDefault, 1);
    return u;
}

bool UserCalendarWizardPage::validatePage()
{
    if (ui->createAgenda->isChecked()) {
        QStringList msg;
        if (ui->calendarLabel->text().simplified().isEmpty()) {
            msg << tr("Please specify a label for this agenda.");
        }
        if (ui->defaultDuration->value()==0) {
            msg << tr("The default duration can not be null.");
        }
        if (!msg.isEmpty()) {
            Utils::warningMessageBox(tr("Error detected"), msg.join("\n"));
            return false;
        }
    }
    return true;
}

void UserCalendarWizardPage::retranslate()
{
    ui->retranslateUi(this);
    setTitle(tr("Manage an agenda."));
    setSubTitle(tr("You can quickly define an agenda for this user. This agenda will be the default one."));
}

void UserCalendarWizardPage::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::LanguageChange) {
        retranslate();
    }
}

void UserCalendarWizardPage::toggleAgendaEditing(bool state)
{
    if (state)
        ui->calendarLabel->setFocus();
}

