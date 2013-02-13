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
#ifndef CALENDAR_NAVBAR_H
#define CALENDAR_NAVBAR_H

#include <QWidget>
#include <QDate>

#include "common.h"

QT_BEGIN_NAMESPACE
class QPushButton;
class QToolButton;
class QLabel;
class QComboBox;
class QMenu;
QT_END_NAMESPACE

namespace Calendar {
class CalendarNavbar : public QWidget
{
    Q_OBJECT
public:
    CalendarNavbar(QWidget *parent = 0);

    void setViewType(ViewType viewType);
    ViewType viewType() const { return m_viewType; }

    const QDate &firstDate() const { return m_firstDate; }
    void setDate(const QDate &date);

public Q_SLOTS:
    void setDayGranularity(const int durationInMinutes);

Q_SIGNALS:
    void firstDateChanged();
    void viewTypeChanged();
    void granularityChanged(int);
    void forceModelRefresh();

private Q_SLOTS:
    void todayPage();
    void yesterdayPage();
    void tomorrowPage();
    void currentWeekPage();
    void nextWeekPage();
    void currentMonthPage();
    void nextMonthPage();
    void previousPage();
    void nextPage();
    void changeViewMode(QAction *action);
    void changeMonths(QAction *action);
    void changeWeek(QAction *action);
    void dayMode();
    void weekMode();
    void monthMode();
    void changeGranularity(QAction *action);
    void refreshModel();

private:
    ViewType m_viewType;
    QDate m_firstDate;
    QToolButton *m_todayButton;
    QToolButton *m_previousPageButton;
    QToolButton *m_nextPageButton;

    QAction *aDayView, *aWeekView, *aMonthView;
    QAction *aToday, *aTomorrow, *aYesterday, *aCurrentWeek, *aCurrentMonth, *aNextWeek, *aNextMonth;
    QAction *aForceModelRefresh;
    QMenu *mViewRange, *mMonths, *mWeeks;

    QToolButton *m_viewModeNav;
    QLabel *m_dateLabel; // replace with a toolButton
    QToolButton *m_currentDateViewButton;


    void refreshInfos();
    QString getDateIntervalString();
    QWidget *createNavigationButtons();
    QToolButton *createNavigationModeButton();
    QToolButton *createRefreshModelButton();
    QToolButton *createTodayButton();
    QToolButton *createCurrentDateViewButton();
    void changeEvent(QEvent *e);
};

}  // End namespace Calendar

#endif
