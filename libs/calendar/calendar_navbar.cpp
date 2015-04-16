/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "calendar_navbar.h"
#include "calendar_theme.h"

#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_datetime.h>
#include <translationutils/trans_agenda.h>

#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QDate>
#include <QMenu>
#include <QComboBox>

#include <QDebug>

using namespace Calendar;
using namespace Trans::ConstantTranslations;

static inline Calendar::CalendarTheme *theme() {return Calendar::CalendarTheme::instance();}

/**
  \class Calendar::CalendarNavbar
  Assume the navigation in the calendar view.
*/

/**
  \fn void Calendar::CalendarNavbar::granularityChanged(int);
  Signal sent when user select a new day granularity. Time is sent in minutes.
*/

CalendarNavbar::CalendarNavbar(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(180, 180, 255));
    this->setPalette(palette);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(createNavigationButtons());
    layout->addStretch();
    layout->addWidget(createCurrentDateViewButton());
    layout->addStretch();
    layout->addWidget(createNavigationModeButton());
    layout->addWidget(createRefreshModelButton());

    // signal/slot connections
    connect(m_todayButton, SIGNAL(clicked()), this, SLOT(todayPage()));
    connect(m_previousPageButton, SIGNAL(clicked()), this, SLOT(previousPage()));
    connect(m_nextPageButton, SIGNAL(clicked()), this, SLOT(nextPage()));
    connect(m_viewModeNav, SIGNAL(triggered(QAction*)), this, SLOT(changeViewMode(QAction*)));
    connect(aForceModelRefresh, SIGNAL(triggered()), this, SIGNAL(forceModelRefresh()));
}

QWidget *CalendarNavbar::createNavigationButtons()
{
    QWidget *w = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(w);
    layout->setSpacing(0);
    layout->setMargin(0);

    // Add prev/next
    m_previousPageButton = new QToolButton(this);
    QString icon = theme()->iconFileName(CalendarTheme::NavigationPrevious);
    if (icon.isEmpty())
        m_previousPageButton->setText("<<");
    else
        m_previousPageButton->setIcon(QIcon(icon));

    m_nextPageButton = new QToolButton(this);
    icon = theme()->iconFileName(CalendarTheme::NavigationNext);
    if (icon.isEmpty())
        m_nextPageButton->setText(">>");
    else
        m_nextPageButton->setIcon(QIcon(icon));

    layout->addWidget(m_previousPageButton);
    layout->addWidget(m_nextPageButton);

    // Add day nav
    layout->addWidget(m_todayButton = createTodayButton());
    w->setLayout(layout);
    return w;
}

QToolButton *CalendarNavbar::createNavigationModeButton()
{
    QString icon;
    m_viewModeNav = new QToolButton(this);
    icon = theme()->iconFileName(CalendarTheme::NavigationViewMode);
    if (!icon.isEmpty())
        m_viewModeNav->setIcon(QIcon(icon));
    m_viewModeNav->setPopupMode(QToolButton::InstantPopup);
    aDayView = new QAction(tkTr(Trans::Constants::DAY), this);
    icon = theme()->iconFileName(CalendarTheme::NavigationDayViewMode);
    if (!icon.isEmpty())
        aDayView->setIcon(QIcon(icon));
    aWeekView = new QAction(tkTr(Trans::Constants::WEEK), this);
    icon = theme()->iconFileName(CalendarTheme::NavigationDayViewMode);
    if (!icon.isEmpty())
        aWeekView->setIcon(QIcon(icon));
    aMonthView = new QAction(tkTr(Trans::Constants::MONTH), this);
    icon = theme()->iconFileName(CalendarTheme::NavigationDayViewMode);
    if (!icon.isEmpty())
        aMonthView->setIcon(QIcon(icon));
    m_viewModeNav->addAction(aDayView);
    m_viewModeNav->addAction(aWeekView);
    m_viewModeNav->addAction(aMonthView);
    return m_viewModeNav;
}

QToolButton *CalendarNavbar::createRefreshModelButton()
{
    QToolButton *but = new QToolButton(this);
    aForceModelRefresh = new QAction(this);
    const QString &icon = theme()->iconFileName(CalendarTheme::NavigationForceModelRefreshing);
    if (!icon.isEmpty())
        aForceModelRefresh->setIcon(QIcon(icon));
    but->addAction(aForceModelRefresh);
    but->setDefaultAction(aForceModelRefresh);
    connect(aForceModelRefresh, SIGNAL(triggered()), this, SLOT(refreshModel()));
    return but;
}

QToolButton *CalendarNavbar::createTodayButton()
{
    QToolButton *button = new QToolButton(this);
    QString icon = theme()->iconFileName(CalendarTheme::NavigationBookmarks);
    if (icon.isEmpty())
        button->setText("^");
    else
        button->setIcon(QIcon(icon));

    QMenu *menu = new QMenu(this);
    aToday = menu->addAction(tkTr(Trans::Constants::TODAY), this, SLOT(todayPage()));
    aTomorrow = menu->addAction(tkTr(Trans::Constants::TOMORROW), this, SLOT(tomorrowPage()));
    aYesterday = menu->addAction(tkTr(Trans::Constants::YESTERDAY), this, SLOT(yesterdayPage()));
    menu->addSeparator();
    aCurrentWeek = menu->addAction(tkTr(Trans::Constants::CURRENT_WEEK), this, SLOT(currentWeekPage()));
    aNextWeek = menu->addAction(tkTr(Trans::Constants::NEXT_WEEK), this, SLOT(nextWeekPage()));
    menu->addSeparator();
    aCurrentMonth = menu->addAction(tkTr(Trans::Constants::CURRENT_MONTH), this, SLOT(currentMonthPage()));
    aNextMonth = menu->addAction(tkTr(Trans::Constants::NEXT_MONTH), this, SLOT(nextMonthPage()));
    menu->addSeparator();

    mViewRange = menu->addMenu(tr("View range"));
    for(int i = 1; i < 19; ++i) {
        QAction *action = mViewRange->addAction(QString("%1 %2").arg(i*5).arg(tkTr(Trans::Constants::MINUTES)));
        action->setData(i);
    }
    connect(mViewRange, SIGNAL(triggered(QAction*)), this, SLOT(changeGranularity(QAction*)));

    button->setMenu(menu);
    button->setPopupMode(QToolButton::InstantPopup);
//    button->setDefaultAction(action);
    return button;
}

QToolButton *CalendarNavbar::createCurrentDateViewButton()
{
    m_currentDateViewButton = new QToolButton(this);
    QString icon = theme()->iconFileName(CalendarTheme::NavigationCurrentDateView);
    if (!icon.isEmpty()) {
        m_currentDateViewButton->setIcon(QIcon(icon));
        m_currentDateViewButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    QFont bold;
    bold.setBold(true);
    m_currentDateViewButton->setFont(bold);
    m_currentDateViewButton->setPopupMode(QToolButton::InstantPopup);

    QAction *a = 0;
    QMenu *generalMenu = new QMenu(this);

    // Add month menu
    mMonths = generalMenu->addMenu(tkTr(Trans::Constants::MONTHS));
    for(int i = 1 ; i < 13; ++i) {
        QString t = QLocale().toString(QDate(2012, i, 1), "MMMM");
        t = Utils::firstLetterUpperCase(t);
        a = mMonths->addAction(t);
        a->setData(i);
    }
    connect(mMonths, SIGNAL(triggered(QAction*)), this, SLOT(changeMonths(QAction*)));

    // Add weeks menu
    // go to the first monday of the year
    QDate date = QDate(QDate::currentDate().year(), 1, 1);
    if (date.dayOfWeek() != 1)
        date = date.addDays(8 - date.dayOfWeek());
    QDate endWeek;
    QString dateFormat = QLocale().dateFormat(QLocale::ShortFormat);
    mWeeks = generalMenu->addMenu(tkTr(Trans::Constants::WEEKS));
    QMenu *menu = mWeeks->addMenu(QString("1 - 10"));
    for(int i = 1 ; i < 53; ++i) {
        if (i % 10 == 0) {
            menu = mWeeks->addMenu(QString("%1 - %2").arg(i).arg(i + 9));
        }
        endWeek = date.addDays(6);
        // FIXME: use tkTr(Trans::Constants::_1_COLON_2)
        a = menu->addAction(QString("%1: %2 - %3")
                             .arg(i)
                             .arg(QLocale().toString(date, dateFormat))
                             .arg(QLocale().toString(endWeek, dateFormat)));
        a->setData(date);
        date = date.addDays(7);
    }
    connect(mWeeks, SIGNAL(triggered(QAction*)), this, SLOT(changeWeek(QAction*)));

    m_currentDateViewButton->setMenu(generalMenu);

    return m_currentDateViewButton;
}

void CalendarNavbar::setViewType(ViewType viewType)
{
	if (viewType == m_viewType)
		return;

	m_viewType = viewType;

	refreshInfos();

	emit viewTypeChanged();
}

void CalendarNavbar::setDate(const QDate &date)
{
	QDate firstDate = getFirstDateByRandomDate(m_viewType, date);
	if (firstDate == m_firstDate)
		return;

	m_firstDate = firstDate;
	refreshInfos();
	emit firstDateChanged();
}

void CalendarNavbar::setDayGranularity(const int durationInMinutes)
{
    Q_UNUSED(durationInMinutes);
//    // Find the index (every 5 minutes)
//    int index = -1;
//    if (durationInMinutes%5)
//        index = (durationInMinutes/5);
//    else
//        index = (durationInMinutes/5 - 1);
//    m_granularity->setCurrentIndex(index);
}

void CalendarNavbar::refreshInfos()
{
	// TODO (refresh label, etc...)
	switch (m_viewType){
	case View_Day:
//		m_dateLabel->setText(m_firstDate.toString());
                m_currentDateViewButton->setText(QLocale().toString(m_firstDate));
		break;
	case View_Week:
//		m_dateLabel->setText(getDateIntervalString());
                m_currentDateViewButton->setText(getDateIntervalString());
                break;
	case View_Month:
//		m_dateLabel->setText(m_firstDate.toString("MMMM yyyy"));
                m_currentDateViewButton->setText(QLocale().toString(m_firstDate, "MMMM yyyy"));
                break;
	default:; // should never happends
	}
}

void CalendarNavbar::todayPage()
{
	setDate(QDate::currentDate());
}

void CalendarNavbar::yesterdayPage()
{
	setDate(QDate::currentDate().addDays(-1));
}

void CalendarNavbar::tomorrowPage()
{
	setDate(QDate::currentDate().addDays(1));
}

void CalendarNavbar::currentWeekPage()
{
    setViewType(View_Week);
    setDate(QDate::currentDate());
}

void CalendarNavbar::nextWeekPage()
{
    setViewType(View_Week);
    setDate(QDate::currentDate().addDays(7));
}

void CalendarNavbar::currentMonthPage()
{
    setViewType(View_Month);
    setDate(QDate::currentDate());
}

void CalendarNavbar::nextMonthPage()
{
    setViewType(View_Month);
    setDate(QDate::currentDate().addDays(QDate::currentDate().daysInMonth()));
}

void CalendarNavbar::previousPage()
{
	switch (m_viewType) {
	case View_Day:
		setDate(m_firstDate.addDays(-1));
		break;
	case View_Week:
		setDate(m_firstDate.addDays(-7));
		break;
	case View_Month:
		setDate(m_firstDate.addMonths(-1));
		break;
	default: // should never happend
		break;
	}
}

void CalendarNavbar::nextPage()
{
	switch (m_viewType) {
	case View_Day:
		setDate(m_firstDate.addDays(1));
		break;
	case View_Week:
		setDate(m_firstDate.addDays(7));
		break;
	case View_Month:
		setDate(m_firstDate.addMonths(1));
		break;
	default: // should never happend
		break;
	}
}

void CalendarNavbar::changeViewMode(QAction *action)
{
    if (action==aDayView)
        dayMode();
    else if (action==aWeekView)
        weekMode();
    else if (action==aMonthView)
        monthMode();
}

void CalendarNavbar::changeMonths(QAction *action)
{
    QDate monday = QDate(QDate::currentDate().year(), action->data().toInt(), 1);
    if (monday.dayOfWeek() != 1) {
        monday = monday.addDays(8 - monday.dayOfWeek());
    }
    setViewType(View_Month);
    setDate(monday);
}

void CalendarNavbar::changeWeek(QAction *action)
{
    QDate monday = action->data().toDate();
    setViewType(View_Week);
    setDate(monday);
}

void CalendarNavbar::dayMode()
{
	setViewType(Calendar::View_Day);
}

void CalendarNavbar::weekMode()
{
	setViewType(Calendar::View_Week);
}

void CalendarNavbar::monthMode()
{
	setViewType(Calendar::View_Month);
}

void CalendarNavbar::changeGranularity(QAction *action)
{
    Q_EMIT granularityChanged(action->data().toInt()*5);
}

QString CalendarNavbar::getDateIntervalString()
{
	QDate lastDate;
	switch (m_viewType) {
	case View_Day:
        return QLocale().toString(m_firstDate);
	case View_Week:
		lastDate = m_firstDate.addDays(6);
		if (m_firstDate.month() != lastDate.month() && m_firstDate.year() != lastDate.year())
            return QString("%1 - %2").arg(
                        QLocale().toString(m_firstDate, tr("d MMM yyyy")),
                        QLocale().toString(lastDate, tr("d MMM yyyy")));
		else if (m_firstDate.month() != lastDate.month())
            return QString("%1 - %2").arg(
                        QLocale().toString(m_firstDate, tr("d MMM")),
                        QLocale().toString(lastDate, tr("d MMM yyyy")));
		else
            return QString("%1 - %2").arg(
                        QLocale().toString(m_firstDate, tr("d")),
                        QLocale().toString(lastDate, tr("d MMM yyyy")));
	case View_Month:
//			return randomDate.addDays(- randomDate.day() + 1);
	default: // should never happend
		return "";
	}
}

void CalendarNavbar::refreshModel()
{
    QDate save = m_firstDate;
    setDate(QDate());
    setDate(save);
}

void CalendarNavbar::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        aDayView->setText(Utils::firstLetterUpperCase(tkTr(Trans::Constants::DAY)));
        aWeekView->setText(Utils::firstLetterUpperCase(tkTr(Trans::Constants::WEEK)));
        aMonthView->setText(Utils::firstLetterUpperCase(tkTr(Trans::Constants::MONTH)));
        aToday->setText(tkTr(Trans::Constants::TODAY));
        aTomorrow->setText(tkTr(Trans::Constants::TOMORROW));
        aYesterday->setText(tkTr(Trans::Constants::YESTERDAY));
        aCurrentWeek->setText(tkTr(Trans::Constants::CURRENT_WEEK));
        aNextWeek->setText(tkTr(Trans::Constants::NEXT_WEEK));
        aCurrentMonth->setText(tkTr(Trans::Constants::CURRENT_MONTH));
        aNextMonth->setText(tkTr(Trans::Constants::NEXT_MONTH));
        mMonths->clear();
        mMonths->setTitle(Utils::firstLetterUpperCase(tkTr(Trans::Constants::MONTHS)));
        for(int i = 1 ; i < 13; ++i) {
            QString t = QLocale().toString(QDate(2012, i, 1), "MMMM");
            t = Utils::firstLetterUpperCase(t);
            QAction *a = mMonths->addAction(t);
            a->setData(i);
        }
        mWeeks->setTitle(Utils::firstLetterUpperCase(tkTr(Trans::Constants::WEEKS)));
        mViewRange->clear();
        mViewRange->setTitle(tr("View range"));
        for(int i = 1; i < 19; ++i) {
            QAction *action = mViewRange->addAction(QString("%1 %2").arg(i*5).arg(tkTr(Trans::Constants::MINUTES)));
            action->setData(i);
        }
    }
}

