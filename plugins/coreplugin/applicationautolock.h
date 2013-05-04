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
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CORE_APPLICATIONAUTOLOCK_H
#define CORE_APPLICATIONAUTOLOCK_H

#include <coreplugin/core_exporter.h>
#include <QObject>

/**
 * \file applicationautolock.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 04 May 2013
*/

namespace Core {
namespace Internal {
class ApplicationAutoLockPrivate;
} // namespace Internal

class CORE_EXPORT ApplicationAutoLock : public QObject
{
    Q_OBJECT
    
public:
    explicit ApplicationAutoLock(QObject *parent = 0);
    ~ApplicationAutoLock();
    bool initialize();
    void setTimeBeforeLocking(int durationInMs);
    void startListening();

    bool isLocked() const;

Q_SIGNALS:
    void lockRequired();

private:
    bool eventFilter(QObject *obj, QEvent *event);

private Q_SLOTS:
    void timerTimeOut();

private:
    Internal::ApplicationAutoLockPrivate *d;
};

} // namespace Core

#endif  // CORE_APPLICATIONAUTOLOCK_H

