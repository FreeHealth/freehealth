/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CALENDAR_THEME_H
#define CALENDAR_THEME_H

#include <calendar/calendar_exporter.h>

#include <QHash>
#include <QString>
#include <QColor>

/**
 * \file calendar_theme.h
 * \author Eric Maeker
 * \version 0.6.0
 * \date 12 Aug 2011
*/

namespace Calendar {

class CALENDAR_EXPORT CalendarTheme
{
public:
    enum PathReference {
        SmallIconPath = 0,  // 16x16
        MediumIconPath,     // 32x32
        BigIconPath          // 64x64
    };

    enum IconReference {
        NavigationNext = 0,
        NavigationPrevious,
        NavigationCurrentDateView,
        NavigationBookmarks,
        NavigationViewMode,
        NavigationDayViewMode,
        NavigationWeekViewMode,
        NavigationMonthViewMode,
        NavigationForceModelRefreshing
    };

    enum IconSize {
        SmallSize = 0,
        MediumSize,
        BigSize
    };

    enum ColorInUse {
        ColorStatusWaiting = 0,
        ColorStatusApproved,
        ColorStatusArrived,
        ColorStatusChanged,
        ColorStatusCancelled,
        ColorStatusMissed
    };

    static CalendarTheme *instance();
    ~CalendarTheme() {}

    void setPath(const PathReference ref, const QString &absPath);
    void setIconFileName(const IconReference ref, const QString &fileName);

    QString iconFileName(const IconReference ref, const IconSize size = SmallSize) const;

    void setColor(const ColorInUse colorRef, const QColor &color);
    QColor color(const ColorInUse colorRef) const;

private:
    CalendarTheme() {}
    void populateWithDefault();

    QHash<PathReference, QString> m_path;
    QHash<IconReference, QString> m_icons;
    QHash<ColorInUse, QColor> m_colors;
    static CalendarTheme *m_Instance;
};

}  // End namespace Calendar

#endif // CALENDAR_THEME_H
