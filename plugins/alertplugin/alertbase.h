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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ALERTBASE_H
#define ALERTBASE_H

#include <utils/database.h>
#include <alertplugin/alertitem.h>

#include <QObject>
#include <QDate>
#include <QString>
#include <QStringList>
#include <QVector>

/**
 * \file alertbase.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 15 July 2012
*/

namespace Alert {
class AlertCore;
class AlertItem;
class AlertPackDescription;
namespace Internal {
class AlertPlugin;
class AlertCorePrivate;

class AlertBaseQuery
{
public:
    enum AlertValidity {
        ValidAlerts = 0,
        InvalidAlerts,
        ValidAndInvalidAlerts
    };

    AlertBaseQuery();
    ~AlertBaseQuery();
    void clear();

    void getAlertItemFromUuid(const QString &uuid);
    QString alertItemFromUuid() const;

    void setAlertValidity(AlertValidity validity);
    AlertValidity alertValidity() const;

    void addCurrentUserAlerts();
    void addUserAlerts(const QString &uuid);

    void addCurrentPatientAlerts();
    void addPatientAlerts(const QString &uuid);

    void addApplicationAlerts(const QString &appName);

    QStringList userUids() const;
    QStringList patientUids() const;
    QStringList applicationNames() const;

    void setDateRange(const QDate &start, const QDate &end);
    bool dateRangeDefined() const;
    QDate dateRangeStart() const;
    QDate dateRangeEnd() const;

    void setAlertViewType(AlertItem::ViewType viewType);
    AlertItem::ViewType alertViewType() const;

//    void addCategory(const QString &category, const QString &lang = QString::null);
//    QMultiHash<QString, QString> categories() const;

private:
    QString _itemUid;
    QStringList _userUids, _patientUids, _appNames;
    QDate _start, _end;
    AlertValidity _validity;
    int _viewType;
    QMultiHash<QString, QString> _categories;
};

class AlertBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class Alert::AlertCore;
    friend class Alert::Internal::AlertCorePrivate;

protected:
    AlertBase(QObject * parent = 0);
    bool initialize();

public:
    ~AlertBase();
    bool isInitialized() const {return m_initialized;}

    // AlertItem
    bool saveAlertItem(AlertItem &item);
    QVector<AlertItem> getAlertItems(const AlertBaseQuery &query);
    bool purgeAlertItem(const QString &uuid);

    // AlertPacks
    bool saveAlertPackDescription(AlertPackDescription &descr);
    bool removeAlertPack(const QString &uid);
    AlertPackDescription getAlertPackDescription(const QString &uuid);

    // For debugging purpose
    void toTreeWidget(QTreeWidget *tree) const;

private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                          const QString &pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString &login, const QString &pass,
                          const int port,
                          CreationOption createOption
                         );
    bool updateDatabase();
    quint32 getSchemaVersionNumber() const;
    QString getOldVersionField() const;

    // Alerts
    bool updateAlertItem(AlertItem &item);
    bool saveItemRelations(AlertItem &item);
    bool saveItemScripts(AlertItem &item);
    bool saveItemTimings(AlertItem &item);
    bool saveItemValidations(AlertItem &item);
    bool saveItemLabels(AlertItem &item);

    AlertItem &getAlertItemFromUuid(const QString &uuid);
    bool getItemRelations(AlertItem &item);
    bool getItemScripts(AlertItem &item);
    bool getItemTimings(AlertItem &item);
    bool getItemValidations(AlertItem &item);
    bool getItemLabels(AlertItem &item);

    // AlertPacks
    bool updateAlertPackDescription(AlertPackDescription &descr, const int id);
    bool saveAlertPackLabels(AlertPackDescription &descr);
    bool getAlertPackLabels(AlertPackDescription &descr);

private Q_SLOTS:
    void onCoreDatabaseServerChanged();
    void onCoreFirstRunCreationRequested();

private:
    bool m_initialized;
};

}  // namespace Internal
}  // namespace Alert

#endif  // ALERTBASE_H

