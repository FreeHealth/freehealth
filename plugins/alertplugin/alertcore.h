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
#ifndef ALERTCORE_H
#define ALERTCORE_H

#include <QObject>

/**
 * \file alertcore.h
 * \author Eric MAEKER <eric.maeker@gmail.com>, Pierre-Marie Desombre <pm.desombre@gmail.com>
 * \version 0.8.0
 * \date 31 May 2012
*/

namespace Alert {
namespace Internal {
class AlertCorePrivate;
class AlertPlugin;
}
class AlertManager;

class AlertCore : public QObject
{
    Q_OBJECT
    friend class Alert::Internal::AlertPlugin;

protected:
    AlertCore(QObject *parent = 0);
    bool initialize();

public:
    static AlertCore *instance(QObject *parent = 0);
    ~AlertCore();

    // Getters/Setters
    //    QVector<AlertItem> getAlertItemForCurrentUser() const;
    //    QVector<AlertItem> getAlertItemForCurrentPatient() const;
    //    QVector<AlertItem> getAlertItemForCurrentApplication() const;
    //    bool saveAlertItem(const AlertItem &item);

    // Executers
    //    bool executeAlert(const AlertItem &alert);  // add a delay ?

    // Editors
    //    AlertItem &editAlert(AlertItem &alert);
    //    AlertItem &createAlert();
    //  OR
    //    bool editAlert(AlertItem &alert);
    //    bool createAlert(AlertItem &alert);


    void showIHMaccordingToType(int type = 0);

Q_SIGNALS:
//    void alertItemUpdated(const AlertItem &alert);
//    void alertItemRemoved(const AlertItem &alert);
//    void alertItemValidated(const AlertItem &alert);

private:
    static AlertCore *_instance;
    Internal::AlertCorePrivate *d;
};

}  // Alert

#endif

