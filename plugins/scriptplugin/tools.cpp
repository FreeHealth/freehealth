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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "tools.h"

#include <utils/log.h>
#include <utils/global.h>

#include <QDesktopServices>
#include <QUrl>

using namespace Script;
using namespace Internal;

Tools::Tools(QObject *parent) :
    QObject(parent)
{
    setObjectName("Script::Tools");
}

QString Tools::dateToString(const QDate &date, const QString &format)
{
    return date.toString(format);
}

void Tools::openUrl(const QString &url)
{
    if (!QDesktopServices::openUrl(QUrl(url)))
        LOG_ERROR("When requested openUrl with " + url);
    else
        LOG("URL opened: " + url);
}

QString Tools::lineWrapString(const QString &text, int lineLength)
{
   return Utils::lineWrapString(text, lineLength);
}
