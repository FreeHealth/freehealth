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
 *  Main Developers:                                                       *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ICALENDARITEMDATAWIDGET_H
#define ICALENDARITEMDATAWIDGET_H

#include <calendar/calendar_exporter.h>

#include <QObject>

/**
 * \file icalendaritemdatawidget.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 11 Aug 2011
*/


namespace Calendar {
class CalendarItem;
class AbstractCalendarModel;

class CALENDAR_EXPORT ICalendarItemDataWidget : public QObject
{
    Q_OBJECT
public:
    enum InsertionPlaces {
        Beginning = 0,              //!< top of dialog
        Ending,                     //!< bottom of dialog
        BeforeGeneralInformation,   //!< in the "extra section", before "general Information"
        AfterGeneralInformation,    //!< in the "extra section", after "general Information"
        BeforeDateTime,             //!< before date/time section
        AfterDateTime,              //!< after the date/time section
        BeforeDescription,          //!< before description
        AfterDescription            //!< after description
    };

    ICalendarItemDataWidget(QObject *parent = 0) : QObject(parent) { }
    virtual ~ICalendarItemDataWidget() {}

    virtual int insertionPlace() const = 0;
    virtual QWidget *createWidget(QWidget *parent = 0) = 0;

    virtual bool setCalendarItemModel(Calendar::AbstractCalendarModel *model) = 0;
    virtual bool setCalendarItem(const Calendar::CalendarItem &item) = 0;

    virtual bool clear() = 0;
    virtual bool submitChangesToCalendarItem(const Calendar::CalendarItem &item) = 0;
};

}  // End namespace Calendar


#endif // ICALENDARITEMDATAWIDGET_H
