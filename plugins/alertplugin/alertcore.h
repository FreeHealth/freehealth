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
#ifndef ALERTPLUGIN_ALERTCORE_H
#define ALERTPLUGIN_ALERTCORE_H

#include <QObject>
#include <alertplugin/alertplugin_exporter.h>
#include <alertplugin/alertitem.h>
#include <alertplugin/alertpackdescription.h>

/**
 * \file alertcore.h
 * \author Eric MAEKER <eric.maeker@gmail.com>, Pierre-Marie Desombre <pm.desombre@gmail.com>
 * \version 0.8.0
 * \date 29 July 2012
*/

namespace DataPack {
class Pack;
}

namespace Alert {
class BlockingAlertDialog;
class NonBlockingAlertToolButton;
namespace Internal {
class AlertCorePrivate;
class AlertPlugin;
}

class ALERT_EXPORT AlertCore : public QObject
{
    Q_OBJECT
    friend class Alert::Internal::AlertPlugin;
    friend class Alert::BlockingAlertDialog;
    friend class Alert::NonBlockingAlertToolButton;

protected:
    AlertCore(QObject *parent = 0);
    bool initialize();

public:
    enum AlertToCheck {
        CurrentPatientAlerts =      0x00000001,
        CurrentUserAlerts =         0x00000002,
        CurrentApplicationAlerts =  0x00000004,
        AllAlerts = CurrentPatientAlerts | CurrentUserAlerts | CurrentApplicationAlerts
    };
    Q_DECLARE_FLAGS(AlertsToCheck, AlertToCheck)

    static AlertCore *instance();
    ~AlertCore();

    // Getters/Setters
    QVector<AlertItem> getAlertItemForCurrentUser() const;
    QVector<AlertItem> getAlertItemForCurrentPatient() const;
    QVector<AlertItem> getAlertItemForCurrentApplication() const;
    bool saveAlert(AlertItem &item);
    bool saveAlerts(QList<AlertItem> &items);

public Q_SLOTS:
    // Checks
    bool checkAllAlerts() {return checkAlerts(AllAlerts);}
    bool checkPatientAlerts() {return checkAlerts(CurrentPatientAlerts);}
    bool checkUserAlerts() {return checkAlerts(CurrentUserAlerts);}
    bool checkApplicationAlerts() {return checkAlerts(CurrentApplicationAlerts);}
    bool checkAlerts(AlertsToCheck check);

    // Alert management
    bool registerAlert(const AlertItem &item);
    bool updateAlert(const AlertItem &item);
    bool removeAlert(const AlertItem &item);

    // Alert packs
    bool registerAlertPack(const QString &absPath);
    bool removeAlertPack(const QString &uid);
    AlertPackDescription getAlertPackDescription(const QString &uuid);

    //    bool executeAlert(const AlertItem &alert);  // add a delay ?

    // Editors
    //    AlertItem &editAlert(AlertItem &alert);
    //    AlertItem &createAlert();
    //  OR
    //    bool editAlert(AlertItem &alert);
    //    bool createAlert(AlertItem &alert);


Q_SIGNALS:
//    void alertItemUpdated(const AlertItem &alert);
//    void alertItemRemoved(const AlertItem &alert);
//    void alertItemValidated(const AlertItem &alert);

private:
    void processAlerts(QVector<AlertItem> &alerts);

protected:
    void postCoreInitialization();
    // Script manager wrapper
    QVariant execute(AlertItem &item, const int scriptType);

private Q_SLOTS:
    void packInstalled(const DataPack::Pack &pack);
    void packRemoved(const DataPack::Pack &pack);

private:
    static AlertCore *_instance;
    Internal::AlertCorePrivate *d;
};

}  // Alert

Q_DECLARE_OPERATORS_FOR_FLAGS(Alert::AlertCore::AlertsToCheck)

#endif  // ALERTPLUGIN_ALERTCORE_H

