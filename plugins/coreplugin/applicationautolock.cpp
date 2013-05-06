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
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Core::ApplicationAutoLock
 * \brief short description of class
 *
 * Long description of class
 * \sa Core::
 */

#include "applicationautolock.h"

#include <QTimer>
#include <QApplication>

#include <QDebug>

using namespace Core;
using namespace Internal;

namespace Core {
namespace Internal {
class ApplicationAutoLockPrivate
{
public:
    ApplicationAutoLockPrivate(ApplicationAutoLock *parent) :
        _initialized(false),
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~ApplicationAutoLockPrivate()
    {
    }
    
public:
    QTimer _timer;
    bool _initialized;
    
private:
    ApplicationAutoLock *q;
};
} // namespace Internal
} // end namespace Core


/*! Constructor of the Core::ApplicationAutoLock class */
ApplicationAutoLock::ApplicationAutoLock(QObject *parent) :
    QObject(parent),
    d(new ApplicationAutoLockPrivate(this))
{
}

/*! Destructor of the Core::ApplicationAutoLock class */
ApplicationAutoLock::~ApplicationAutoLock()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ApplicationAutoLock::initialize()
{
    if (d->_initialized)
        return true;
    qApp->installEventFilter(this);
    d->_timer.setSingleShot(true);
    d->_timer.setInterval(2000);
    connect(&d->_timer, SIGNAL(timeout()), this, SLOT(timerTimeOut()), Qt::UniqueConnection);
    d->_initialized = true;
    return true;
}

void ApplicationAutoLock::setTimeBeforeLocking(int durationInMs)
{
    d->_timer.setInterval(durationInMs);
}

/** Restart a listening cycle */
void ApplicationAutoLock::startListening()
{
    d->_timer.start();
}

/** Returns true if the application must be in a lock state */
bool ApplicationAutoLock::isLocked() const
{
#if QT_VERSION >= 0x050000
    return (d->_timer.remainingTime() <= 0);
#else
    return d->_timer.isActive();
#endif
}

/*! Listen to application events and manage the lock timer. */
bool ApplicationAutoLock::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::Wheel:
    case QEvent::DragEnter:
    case QEvent::DragMove:
    case QEvent::DragLeave:
    case QEvent::Drop:
    case QEvent::ContextMenu:
    case QEvent::TabletPress:
    case QEvent::TabletRelease:
//        TouchBegin = 194,
//        TouchUpdate = 195,
//        TouchEnd = 196,
        d->_timer.start();
        break;
    default: break;
    }
    return QObject::eventFilter(obj, event);
}

void ApplicationAutoLock::timerTimeOut()
{
    d->_timer.stop();
    Q_EMIT lockRequired();
}
