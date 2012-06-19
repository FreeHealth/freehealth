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
 *   Main developers : Christian A. Reiter, <christian.a.reiter@gmail.com> *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef BIRTHDAYEDIT_H
#define BIRTHDAYEDIT_H

#include <QLineEdit>
#include <utils/global_exporter.h>

#include <QDate>
#include <QDateTimeEdit>
#include <QKeyEvent>


namespace Utils {

/**
 * \class Utils::BirthDayEdit
 * \brief Replacement class for QDateEdit to better handle birthday entering
 *
 * Inherits QLineEdit and accepts an input format that can be freely defined for
 * each translation. It parses the input and tries to make a date out of it, using
 * the available masks (user provided, system QLocale()->dateFormat(QLocale::ShortFormat),
 * FMF provided). When the focus is lost, it displays the date in the standard way.
 */
class UTILS_EXPORT BirthDayEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged USER true)
//    Q_PROPERTY(bool calendarPopup READ calendarPopup WRITE setCalendarPopup)

public:
    explicit BirthDayEdit(QWidget *parent);
    explicit BirthDayEdit(const QDate & date, QWidget *parent = 0);
    ~BirthDayEdit();

    QDate date() const;
//    void setDateFormats(QString formats);

private:
    void init(const QDate& date = QDate(), const QDate& maximumDate = QDate(), const QDate& minimumDate = QDate());

signals:
    void dateChanged (const QDate& date);

public slots:
    virtual void clear();
    void setDisplayedDateString();
    void setDateString(const QString& dateString);
    void setDate(const QDate &date);

protected slots:
    void updateDisplayText();

private:
    QDate m_date;
    QDate m_maximumDate;
    QDate m_minimumDate;
};

} // end Utils

#endif // BIRTHDAYEDIT_H
