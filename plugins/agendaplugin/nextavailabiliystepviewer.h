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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDA_NEXTAVAILABILIYSTEPVIEWER_H
#define AGENDA_NEXTAVAILABILIYSTEPVIEWER_H

#include <QWidget>
#include <QDateTime>
#include <QRect>

namespace Agenda {
namespace Internal {
namespace Ui {
    class NextAvailabiliyStepViewer;
}

class NextAvailabiliyStepViewer : public QWidget
{
    Q_OBJECT

public:
    explicit NextAvailabiliyStepViewer(QWidget *parent = 0);
    ~NextAvailabiliyStepViewer();

    void setAvailabilities(const QList<QRect> &avs);
    void setAppointments(const QList<QRect> &avs);

    void addFoundFreeTime(const QList<QRect> &free);

    void drawStep();

//    QSize sizeHint() const;

private:
    void paintEvent(QPaintEvent *);

private:
    Ui::NextAvailabiliyStepViewer *ui;
    QList<QRect> avs;
    QList<QRect> aps;
    QList<QRect> free;
};


class NextAvailabiliyManager
{
public:
    NextAvailabiliyManager() : m_ReachedNextAppointment(false) {}
    ~NextAvailabiliyManager() {}

    static bool rectLessThan(const QRect &r1, const QRect &r2);
    static QRect dateToRect(const QDateTime &date, const int durationInMinutes);
    static QRect simplifiedDateToRect(const int weekDay, const QTime &start, const QTime &end);
    static QRect dateToRect(const QDateTime &begin, const QDateTime &end);
    static QDateTime rectToDateStart(const QRect &rect);
    static QDateTime rectToDateEnd(const QRect &rect);
    static bool isInAvailabilities(const QList<QRect> &avList, const QRect &testDate);
    static int minutesToNextAvailability(const QList<QRect> &avList, const QRect &testDate);

    void setAvaibilitiesToRect(const QList<QRect> &avs);

    QList<QDateTime> nextAvailableTime(const QDateTime &startSearch,
                                       const int durationInMinutes,
                                       const int calendarDurationInMinutes,
                                       const QRect &nextAppointment,
                                       const int numberOfDates);

    bool hasReachedNextAppointment() const {return m_ReachedNextAppointment;}
    QDateTime requestingNewAppointmentLaterThan() const {return m_NeedLaterThan;}

private:
    QList<QRect> avs;
    bool m_ReachedNextAppointment;
    QDateTime m_NeedLaterThan;
};

}  // End namespace Internal
}  // End namespace Agenda

#endif // AGENDA_NEXTAVAILABILIYSTEPVIEWER_H
