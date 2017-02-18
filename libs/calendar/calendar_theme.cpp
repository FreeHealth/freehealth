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
#include "calendar_theme.h"

#include <QDir>

using namespace Calendar;

CalendarTheme *CalendarTheme::m_Instance = 0;

CalendarTheme *CalendarTheme::instance()
{
    if (!m_Instance)
        m_Instance = new CalendarTheme();
    return m_Instance;
}

void CalendarTheme::setPath(const PathReference ref, const QString &absPath)
{
    if (QDir(absPath).exists())
        m_path.insert(ref, QDir::cleanPath(absPath));
}

void CalendarTheme::setIconFileName(const IconReference ref, const QString &fileName)
{
    m_icons.insert(ref, fileName);
}

QString CalendarTheme::iconFileName(const IconReference ref, const IconSize size) const
{
    QString path;
    switch (size) {
    case SmallSize: path = m_path.value(SmallIconPath); break;
    case MediumSize: path = m_path.value(MediumIconPath); break;
    case BigSize: path = m_path.value(BigIconPath); break;
    }
    if (path.isEmpty())
        return QString();
    QFileInfo fi(path + QDir::separator() + m_icons.value(ref));
    if (fi.exists() && fi.isFile())
        return fi.absoluteFilePath();
    return QString();
}

void CalendarTheme::setColor(const ColorInUse colorRef, const QColor &color)
{
    m_colors.insert(colorRef, color);
}

QColor CalendarTheme::color(const ColorInUse colorRef) const
{
    return m_colors.value(colorRef);
}

void CalendarTheme::populateWithDefault()
{
    // TODO: Guillaume: code here
    // populate colors
}


