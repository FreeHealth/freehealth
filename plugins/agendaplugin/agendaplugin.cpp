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
 ***************************************************************************/
#include "agendaplugin.h"
#include "agendabase.h"
#include "icalendarevent.h"
#include "iusercalendar.h"
#include "agendawidgetmanager.h"

// TEST
#include "eventeditorwidget.h"
#include <utils/randomizer.h>
#include <coreplugin/isettings.h>
#include <QDir>
#include <QFileInfo>
// END TEST

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>
#include <QDialog>
#include <QGridLayout>
#include <QDebug>

using namespace Agenda;
using namespace Internal;

// TEST
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}
// END TEST

AgendaPlugin::AgendaPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating AgendaPlugin";
}

AgendaPlugin::~AgendaPlugin()
{
}

bool AgendaPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "AgendaPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("AgendaPlugin");

    return true;
}

void AgendaPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "AgendaPlugin::extensionsInitialized";

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Initialize database
    Internal::AgendaBase::instance();

    // Initialize ActionHandler and WidgetManager
    AgendaWidgetManager::instance();

    testDatabase();
}

void AgendaPlugin::testDatabase()
{
    qWarning() << "\n\n\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx AGENDA BASE TEST";
    Internal::AgendaBase *base = Internal::AgendaBase::instance();

    // Try to get calendar(s) for the current user
    QList<IUserCalendar *> cals = base->getUserCalendars();
    qWarning() << "Number of user calendars" << cals.count();

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");
    QDir pix(settings()->path(Core::ISettings::SmallPixmapPath));

    IUserCalendar *ucal = 0;
    if (cals.count() == 0) {
        for(int i=0; i < 5; ++i) {
            ucal = new IUserCalendar();
            // Create a calendar for the current user
            ucal->setData(IUserCalendar::DbOnly_IsValid, 1);
            ucal->setData(IUserCalendar::Password, r.getRandomString(r.randomInt(0,10)));
            ucal->setData(IUserCalendar::Label, r.randomWords(r.randomInt(2,5)));
            ucal->setData(IUserCalendar::IsPrivate, r.randomInt(0,1));
            ucal->setData(IUserCalendar::ThemedIcon, r.randomFile(pix, QStringList() << "*.png").fileName());
            if (base->saveUserCalendar(ucal))
                qWarning() << "user calendar correctly saved to database";
            cals << ucal;
        }
        // one must be the default
        ucal = cals.at(r.randomInt(0,4));
        ucal->setData(IUserCalendar::IsDefault, 1);
        base->saveUserCalendar(ucal);
    }
    ucal = cals.at(0);

    // Create events in the calendar
    QDateTime start = QDateTime::currentDateTime();
    start.setTime(QTime(QTime::currentTime().hour()+1, 0, 0));
    QDateTime end = start.addSecs(60*15);
    ICalendarEvent *ev = new ICalendarEvent;
    ev->setData(ICalendarEvent::DbOnly_CalId, ucal->data(IUserCalendar::DbOnly_CalId));
    ev->setData(ICalendarEvent::DbOnly_IsValid, 1);
    ev->setData(ICalendarEvent::DbOnly_EvId, -1);
    ev->setData(ICalendarEvent::DbOnly_CalId, 1);
//    ev->setData(ICalendarEvent::DbOnly_ComId, );
    ev->setData(ICalendarEvent::PatientUid, r.getRandomString(45));
    ev->setData(ICalendarEvent::DateStart, start);
    ev->setData(ICalendarEvent::DateEnd, end);
    ev->setData(ICalendarEvent::DbOnly_CatId, -1);
    ev->setData(ICalendarEvent::TypeId, 1);
    ev->setData(ICalendarEvent::StatusId, 2);
    ev->setData(ICalendarEvent::SiteUid, "siteId");
    ev->setData(ICalendarEvent::IsPrivate, r.randomInt(0,1));
    ev->setData(ICalendarEvent::Password, "nopass");
    ev->setData(ICalendarEvent::IsBusy, r.randomInt(0,1));
    ev->setData(ICalendarEvent::IsAGroupEvent, r.randomInt(0,1));
    ev->setData(ICalendarEvent::Label, r.randomWords(r.randomInt(2, 15)));
    ev->setData(ICalendarEvent::FullContent, r.randomWords(r.randomInt(10, 500)));
    ev->setData(ICalendarEvent::TextualSite, r.getRandomString(r.randomInt(1,145)));
    ev->setData(ICalendarEvent::ThemedIcon, r.randomFile(pix, QStringList() << "*.png").fileName());
    ev->setData(ICalendarEvent::DbOnly_XmlViewOptions, "XmlViewOptions");
    ev->setData(ICalendarEvent::DbOnly_XmlOptions, "XmlOptions");
    if (base->saveCalendarEvent(ev))
        qWarning() << "event correctly saved";

    // Test event dialog
    QDialog dlg;
    QGridLayout lay(&dlg);
    EventEditorWidget w(&dlg);
    w.setCalendarEvent(ev);
    w.setAvailableUserCalendar(cals);
    lay.addWidget(&w);
    dlg.setLayout(&lay);
    dlg.exec();

    // Try to get events now
    CalendarEventQuery q;
    q.setDateRangeForCurrentWeek();
    QList<ICalendarEvent*> list = base->getCalendarEvents(q);
    qWarning() << "Retreived" << list.count() << "events from the database for user" << ucal->data(IUserCalendar::UserOwnerUid).toString() << "dateRange" << q.dateStart().toString(Qt::ISODate)<< q.dateEnd().toString(Qt::ISODate);

    qDeleteAll(list);
    list.clear();

    qWarning() << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx AGENDA BASE TEST END\n\n\n";
}

Q_EXPORT_PLUGIN(AgendaPlugin)
