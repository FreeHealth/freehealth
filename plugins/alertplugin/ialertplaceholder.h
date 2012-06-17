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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IALERTPLACEHOLDER_H
#define IALERTPLACEHOLDER_H

#include <alertplugin/alertplugin_exporter.h>
#include <QWidget>
#include <QObject>

namespace Alert {
class AlertItem;

class ALERT_EXPORT IAlertPlaceHolder : public QObject
{
    Q_OBJECT
public:
    explicit IAlertPlaceHolder(QObject *parent = 0);
    virtual ~IAlertPlaceHolder() {}

    // identification
    virtual QString uuid() const = 0;

    // for UI presentation of the place holder
    virtual QString name(const QString &lang = QString::null) const = 0;
    virtual QString category(const QString &lang = QString::null) const = 0;
    virtual QString description(const QString &lang = QString::null) const = 0;

    // AlertItem management
    virtual void clear() = 0;
    virtual bool addAlert(const AlertItem &alert) = 0;
    virtual bool updateAlert(const AlertItem &alert) = 0;
    virtual bool removeAlert(const AlertItem &alert) = 0;
    virtual bool highlightAlert(const AlertItem &alert) = 0;

    virtual QWidget *createWidget(QWidget *parent = 0) = 0;

Q_SIGNALS:
    void alertItemValidated(const AlertItem &alert);

};

} // namespace Alert

#endif // IALERTPLACEHOLDER_H
