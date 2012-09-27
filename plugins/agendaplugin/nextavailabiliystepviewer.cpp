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
#include "nextavailabiliystepviewer.h"
#include "ui_nextavailabiliystepviewer.h"

#include <utils/global.h>

#include <QPainter>

#include <QDebug>

using namespace Agenda;
using namespace Internal;

enum { WarnDebugs = false };

namespace {
    const int scaleFactor = 2;
    //    // 1px = 1minute
    //    // linear Monday to Sunday
    //    // 1 day = 1440 minutes
    const int dayHeight = 1440;
    const int heightHint = dayHeight*8/scaleFactor;
}

NextAvailabiliyStepViewer::NextAvailabiliyStepViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NextAvailabiliyStepViewer)
{
    ui->setupUi(this);
    setMinimumSize(250, heightHint);
    setMaximumSize(250, heightHint);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

NextAvailabiliyStepViewer::~NextAvailabiliyStepViewer()
{
    delete ui;
}

void NextAvailabiliyStepViewer::setAvailabilities(const QList<QRect> &_avs)
{
    avs = _avs;
    update();
}

void NextAvailabiliyStepViewer::setAppointments(const QList<QRect> &_aps)
{
    aps = _aps;
    update();
}

void NextAvailabiliyStepViewer::addFoundFreeTime(const QList<QRect> &_free)
{
    free << _free;
    update();
}


//QSize NextAvailabiliyStepViewer::sizeHint() const
//{
//    return QSize(250, heightHint);
//}

void NextAvailabiliyStepViewer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen;
    // Draw hours & half hour
    pen.setColor(QColor("gray"));
    painter.setPen(pen);
    for(int i = 0; i < 7*24; ++i) {
        int y = i*60/scaleFactor;
        painter.drawLine(0, y, 50, y);
        painter.drawText(25, y + 20, QString::number(i%24).rightJustified(2, '0'));
    }
    for(int i = 0; i < 7*24*2; ++i) {
        int y = i*30/scaleFactor;
        painter.drawLine(0, y, 20, y);
        painter.drawLine(45, y, 50, y);
    }
    // Draw day limits && day name
    pen.setColor(QColor("darkblue"));
    painter.setPen(pen);
    for(int i = 0; i < 8; ++i) {
        int y = i*1440/scaleFactor;
        painter.drawLine(0, y, 200, y);
        painter.drawText(150, y + 40, QDate::shortDayName(i+1));
    }

    // Draw avails
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    for(int i = 0; i < avs.count(); ++i) {
        const QRect &r = avs.at(i);
        int top = r.top()/scaleFactor;
        QRect drawMe(0, top, r.width(), r.height()/scaleFactor);
        painter.fillRect(drawMe, QColor("yellow")); //QColor(220, 220, 220));
        painter.drawRect(drawMe);
        painter.drawText(5, top+10, QString::number(i));
    }
    pen.setStyle(Qt::SolidLine);

    // Draw appointemnts
    pen.setColor(QColor("black"));
    painter.setPen(pen);
    for(int i = 0; i < aps.count(); ++i) {
        const QRect &r = aps.at(i);
        int top = r.top()/scaleFactor;
        QRect drawMe(0, top, r.width(), r.height()/scaleFactor+1);
        painter.fillRect(drawMe, QColor("gray")); //QColor(220, 220, 220));
        painter.drawRect(drawMe);
//        painter.drawText(30, top+10, QString::number(i));
    }

    // Draw free
    for(int i = 0; i < free.count(); ++i) {
        const QRect &r = free.at(i);
        int top = r.top()/scaleFactor;
        QRect drawMe(55, top, r.width(), r.height()/scaleFactor+1);
        painter.fillRect(drawMe, QColor("red")); //QColor(220, 220, 220));
        painter.drawRect(drawMe);
    }

    // Draw now
    QRect now = NextAvailabiliyManager::dateToRect(QDateTime::currentDateTime(), 1);
    pen.setColor("black");
    painter.drawLine(100, now.top()/scaleFactor, 500, now.top()/scaleFactor);
}




///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool NextAvailabiliyManager::rectLessThan(const QRect &r1, const QRect &r2)
{
    return rectToDateStart(r1) < rectToDateStart(r2);
}

QRect NextAvailabiliyManager::dateToRect(const QDateTime &date, const int durationInMinutes)
{
    // TOP : day of week
    //   1px = 1minute
    //   linear Monday to Sunday
    //   1 day = 1440 minutes
    // X = month + day
    // WIDTH = year
    // HEIGHT = durationInMinutes
    int day = (date.date().dayOfWeek()-1) * 1440;
    int year = date.date().year();
    int month = date.date().month() * 31 + date.date().day();
    int top = date.time().hour() * 60 + date.time().minute() + day;
    return QRect(month, top, year-2000, durationInMinutes);
}

QRect NextAvailabiliyManager::simplifiedDateToRect(const int weekDay, const QTime &start, const QTime &end)
{
    // TOP : day of week
    //   1px = 1minute
    //   linear Monday to Sunday
    //   1 day = 1440 minutes
    // X = month + day
    // WIDTH = year
    // HEIGHT = durationInMinutes
    int day = (weekDay-1) * 1440;
    int top = start.hour() * 60 + start.minute() + day;
    int durationInMinutes = start.secsTo(end) / 60;
    return QRect(0, top, 11, durationInMinutes);
}

QRect NextAvailabiliyManager::dateToRect(const QDateTime &begin, const QDateTime &end)
{
    return dateToRect(begin, begin.secsTo(end)/60);
}

QDateTime NextAvailabiliyManager::rectToDateStart(const QRect &rect)
{
    int day = rect.topLeft().x() % 31;
    int month = rect.topLeft().x() / 31;
    int year = rect.width() + 2000;
    int timeStart = rect.top() % 1440;
    QTime t(timeStart/60, timeStart%60);
    return QDateTime(QDate(year,month,day), t);
}

QDateTime NextAvailabiliyManager::rectToDateEnd(const QRect &rect)
{
    return rectToDateStart(rect).addSecs(rect.height()*60);
}

bool NextAvailabiliyManager::isInAvailabilities(const QList<QRect> &avList, const QRect &testDate)
{
    for(int i = 0; i < avList.count(); ++i) {
        const QRect &av = avList.at(i);
        if (testDate.top() >= av.top() && testDate.bottom() <= av.bottom()) {
//            if (av.contains(testDate))
                return true;
        }
    }
    return false;
}

int NextAvailabiliyManager::minutesToNextAvailability(const QList<QRect> &avList, const QRect &testDate)
{
    if (avList.isEmpty())
        return 0;
    QRect next;
    const int dateTop = testDate.top();
    if (WarnDebugs)
        qWarning() << "        minToNextAv" << rectToDateStart(testDate);

    int firstAv = -1;
    int minDiff = 10080; // next week
    if (WarnDebugs)
        qWarning()<<"        xxxxxxxxxxxxxxxx";
    for(int i = 0; i < avList.count(); ++i) {
        const QRect &av = avList.at(i);
        int diff = av.top()-dateTop;
        if (diff < 0) // go next week
            diff += 10080;

        // In case the list is not correctly sorted, fing the first availability of the week
        if (WarnDebugs)
            qWarning() <<"        " << i << rectToDateStart(av) << diff << minDiff << (diff < minDiff);

        if (diff > 0 && diff < minDiff) {
            minDiff = diff;
            firstAv = i;
        }
    }
    if (firstAv == -1) // should never be the case
        return 0;

    next = avList.at(firstAv);
    if (WarnDebugs)
        qWarning() << "        nextAvail in" << minDiff << "minutes; avDate=" << rectToDateStart(avList.at(firstAv));
    return minDiff;//(next.top() - testDate.bottom() - 1);
}


void NextAvailabiliyManager::setAvaibilitiesToRect(const QList<QRect> &_avs)
{
    avs = _avs;
//    qSort(avs.begin(), avs.end(), rectLessThan);
}





/** Return the next available appointments by searching for
  - \e startSearch date time,
  - a duration of \e duration minutes
  - for the agenda \e calendar
  - limiting search to \e numberOfDates appointments
*/
QList<QDateTime> NextAvailabiliyManager::nextAvailableTime(const QDateTime &startSearch,
                                                           const int durationInMinutes,
                                                           const int calendarDurationInMinutes,
                                                           const QRect &nextAppointment,
                                                           const int numberOfDates)
{
    QList<QDateTime> toReturn;
    m_ReachedNextAppointment = false;
    m_NeedLaterThan = QDateTime();

    // Some checkings
    if (durationInMinutes <= 0)
        return toReturn;
    if (numberOfDates <= 0)
        return toReturn;
    if ((!nextAppointment.isNull()) && (rectToDateStart(nextAppointment) < startSearch)) {
        if (WarnDebugs)
            qWarning() << "NextApp < StartSearch" << rectToDateStart(nextAppointment) << startSearch;
        m_ReachedNextAppointment = true;
        QDateTime nextEnd = rectToDateEnd(nextAppointment);
        if (nextEnd > startSearch)
            m_NeedLaterThan = nextEnd;//, calendarDurationInMinutes;
        else
            m_NeedLaterThan = Utils::roundDateTime(startSearch, calendarDurationInMinutes);
        return toReturn;
    }

    // Here we can go on
    QDateTime start;
    start = Utils::roundDateTime(startSearch, calendarDurationInMinutes);

    if (WarnDebugs)
        qWarning() << "-------------------\nSTART" << start << "nextApp" << rectToDateStart(nextAppointment);

    // get the first event and make tests

    int nbFound = 0;
    int durationInSeconds = durationInMinutes * 60;
    // TODO: manage algorythm error here
    int limitComputation = 10000;//224640; // 10min, 6day a week, 12hours a day :: 1 full year == 12*6 *10 *6 *52 == 224640

    while (nbFound < numberOfDates) {
        --limitComputation;
        if (limitComputation <= 0)
            break;
        if (start.isNull())
            break;

        QRect testDate = dateToRect(start, durationInMinutes);

        if (WarnDebugs)
            qWarning() << "TEST" << testDate.intersect(nextAppointment).height();

        // rect does not intersect the next recorded appointment ? -> go next appointment
        while ((testDate.intersect(nextAppointment).height() < 1) || nextAppointment.isNull()) {

            if (WarnDebugs)
                qWarning() << "---\ntest" << testDate << "next" << nextAppointment << testDate.intersect(nextAppointment).height() << "topTest" << (nextAppointment.top() <= testDate.top());

            if (nbFound == numberOfDates) {
                return toReturn;
            }

            // rect in availabilities ?
            bool inAvail = isInAvailabilities(avs, testDate);

            if (inAvail) {
                // Add date
//                if (WarnNextAvailableTimeWarnings)
//                    qWarning() << "Adding" << start;
                toReturn << start;
                ++nbFound;
                // Add duration to start and/or go to beginning of next availability and cycle
                start = start.addSecs(durationInSeconds);
                testDate = dateToRect(start, durationInMinutes);
            } else {
                if (WarnDebugs)
                    qWarning() << "Not In Availabilities";
                // go to the next avail beginning
                int minutesToNextAvail = minutesToNextAvailability(avs, testDate);
                start = start.addSecs(minutesToNextAvail*60);// + testDate.height()*60);
                if (WarnDebugs)
                    qWarning() << "minToAv" << minutesToNextAvail << start << dateToRect(start, durationInMinutes);
                // did we go after the next appointment ? -> require new appointment
                if (!nextAppointment.isNull()) {
                    QDateTime nextStart = rectToDateStart(nextAppointment);
                    if (start > nextStart) {
                        if (WarnDebugs)
                            qWarning() << "start" << start << "nextApp" << nextStart;
                        break;
                    }
                }
                testDate = dateToRect(start, durationInMinutes);
            }
        }

        // requiere next appointment
        m_ReachedNextAppointment = true;
        QDateTime nextAppEnd = rectToDateEnd(nextAppointment);
        if (nextAppEnd > start)
            m_NeedLaterThan = nextAppEnd;
        else
            m_NeedLaterThan = start;
        return toReturn;
    }

    if (WarnDebugs)
        qWarning() << "No available time found";


    return toReturn;
}
