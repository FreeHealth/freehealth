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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Script::Internal::ScriptLog
 * Script wrapper over the Utils::Log static class
 * \sa Utils::Log
 */

#include "scriptlog.h"

#include <utils/log.h>

#include <QDebug>

using namespace Script;
using namespace Internal;

/*! Constructor of the Script::Internal::ScriptLog class */
ScriptLog::ScriptLog(QObject *parent) :
    QObject(parent)
{
}

/*! Destructor of the Script::Internal::ScriptLog class */
ScriptLog::~ScriptLog()
{
}

void ScriptLog::message(const QString &owner, const QString &message)
{
    LOG_FOR(owner, message);
}

void ScriptLog::error(const QString &owner, const QString &message)
{
    LOG_ERROR_FOR(owner, message);
}
