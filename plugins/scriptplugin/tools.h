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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SCRIPT_INTERNAL_TOOLS_H
#define SCRIPT_INTERNAL_TOOLS_H

#include <QObject>
#include <QDate>
#include <QString>

/**
 * \file tools.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 06 Jan 2012
*/

namespace Script {
namespace Internal {

class Tools : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userDocumentPath READ userDocumentPath)

public:
    explicit Tools(QObject *parent = 0);
    
public Q_SLOTS:
    QString userDocumentPath();

    QString dateToString(const QDate &date, const QString &format);
    QDate addDays(const QDate &date, int days);
    QDate addWeeks(const QDate &date, int weeks);
    int daysTo(const QDate &from, const QDate &to);

    void openUrl(const QString &url);
    QString lineWrapString(const QString &text, int lineLength);

};

} // namespace Internal
} // namespace Script

#endif // SCRIPT_INTERNAL_TOOLS_H
