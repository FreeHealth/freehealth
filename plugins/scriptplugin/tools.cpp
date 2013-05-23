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
 ***************************************************************************/
#include "tools.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
#include <utils/global.h>

#include <QDesktopServices>
#include <QUrl>
#include <QDir>

using namespace Script;
using namespace Internal;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

Tools::Tools(QObject *parent) :
    QObject(parent)
{
    setObjectName("Script::Tools");
}

QString Tools::userDocumentPath()
{
    return QString(settings()->path(Core::ISettings::UserDocumentsPath) + QDir::separator());
}

bool Tools::checkDirCreateIfNotExists(const QString &absolutePath)
{
    return Utils::checkDir(absolutePath, true, "ScriptTools");
}

QString Tools::dateToString(const QVariant &date, const QString &format)
{
    if (date.canConvert(QVariant::DateTime))
        return date.toDateTime().toString(format);
    else if (date.canConvert(QVariant::Date))
        return date.toDate().toString(format);
    return format;
}

QDate Tools::addDays(const QDate &date, int days)
{
    return date.addDays(days);
}

QDate Tools::addWeeks(const QDate &date, int weeks)
{
    return date.addDays(weeks*7);
}

int Tools::daysTo(const QDate &from, const QDate &to)
{
    return from.daysTo(to);
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
