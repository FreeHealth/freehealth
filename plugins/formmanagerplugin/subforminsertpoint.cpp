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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "subforminsertionpoint.h"

#include <QStringList>
#include <QDebug>

using namespace Form;

void SubFormPoint::setReceiverUid(const QString &uid)
{
    if (uid.contains("@[]@")) {
        QStringList uids = uid.split("@[]@", QString::KeepEmptyParts);
        if (uids.count() == 2) {
            m_ReceiverFormUid = uids.at(0);
            m_ModeUid = uids.at(1);
        }
    } else {
        m_ReceiverFormUid = uid;
    }
}

const QString &SubFormInsertionPoint::receiverUidForDatabase() const
{
    if (modeUid().isEmpty())
        m_DbReceiver = receiverUid();
    else
        m_DbReceiver = receiverUid() + "@[]@" + modeUid();
    return m_DbReceiver;
}

QDebug operator<<(QDebug dbg, const Form::SubFormInsertionPoint &c)
{
    dbg.nospace() << QString("SubFormInsertionPoint(subForm: %1; receiverMode: %2; receiverForm: %3; asChild: %4; append: %5")
                     .arg(c.subFormUid())
                     .arg(c.modeUid())
                     .arg(c.receiverUid())
                     .arg(c.addAsChild())
                     .arg(c.appendToForm())
                     ;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Form::SubFormInsertionPoint *c)
{
    if (!c) {
        dbg.nospace() << "SubFormInsertionPoint(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}

