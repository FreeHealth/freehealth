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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Utils::waitForSignal
 * Wait for a signal to be emitted by a specified QObject. Creates an internal eventloop.
 */

#include "waitforsignal.h"

#include <QEventLoop>
#include <QTimer>

namespace Utils {

bool waitForSignal(QObject *sender, const char *signal, int timeoutMs)
{
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(timeoutMs);
    timer.setSingleShot(true);

    loop.connect(sender, signal, SLOT(quit()));
    loop.connect(&timer, SIGNAL(timeout()), SLOT(quit()));
    timer.start();
    loop.exec();

    return timer.isActive();
}

}

