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
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "usercalendar.h"

#include <QStandardItem>

using namespace Calendar;

UserCalendar::UserCalendar() :
        m_Modified(false)
{
}

UserCalendar::~UserCalendar()
{}

bool UserCalendar::isValid() const
{
    /** \todo code here */
    return true;
}

bool UserCalendar::isNull() const
{
    /** \todo code here */
    return false;
}

QVariant UserCalendar::data(const int ref) const
{
    return m_Datas.value(ref);
}

bool UserCalendar::setData(const int ref, const QVariant &value)
{
    m_Datas.insert(ref, value);
    m_Modified = true;
    return true;
}

bool UserCalendar::isModified() const
{
    return m_Modified;
}

void UserCalendar::setModified(const bool state)
{
    m_Modified=state;
}

QStandardItem *UserCalendar::toStandardItem() const
{
    QStandardItem *it = new QStandardItem;
    if (m_Datas.keys().contains(AbsPathIcon))
        it->setIcon(QIcon(m_Datas.value(AbsPathIcon).toString()));
    it->setText(m_Datas.value(Label).toString());
    it->setToolTip(it->text());
    return it;
}

QString UserCalendar::xmlOptions() const
{
    return QString();
}
