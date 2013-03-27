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
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SCRIPT_INTERNAL_SCRIPTLOG_H
#define SCRIPT_INTERNAL_SCRIPTLOG_H

#include <QObject>

/**
 * \file scriptlog.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 2013-01-05
*/

namespace Script {
namespace Internal {
class ScriptLog : public QObject
{
    Q_OBJECT
public:
    explicit ScriptLog(QObject *parent = 0);
    ~ScriptLog();

public Q_SLOTS:
    void message(const QString &owner, const QString &message);
    void error(const QString &owner, const QString &message);
};

} // namespace Internal
} // namespace Script

#endif // SCRIPT_INTERNAL_SCRIPTLOG_H

