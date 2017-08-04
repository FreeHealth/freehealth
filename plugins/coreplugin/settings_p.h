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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CORESETTINGSPRIVATE_H
#define CORESETTINGSPRIVATE_H

#include <coreplugin/core_exporter.h>
#include <coreplugin/isettings.h>

#include <utils/databaseconnector.h>

#include <QSettings>
#include <QStringList>
#include <QDir>
#include <QHash>

QT_BEGIN_NAMESPACE
class QMainWindow;
class QTreeWidget;
QT_END_NAMESPACE

/**
 * \file settings_p.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 01 Jun 2011
*/

namespace Core {
namespace Internal {

class CORE_EXPORT SettingsPrivate : public ISettings
{
    Q_OBJECT
public:
    SettingsPrivate(QObject *parent = 0, const QString &appName = QString::null, const QString &fileName = QString::null);
    ~SettingsPrivate();

    void setUserSettings(const QString &file);
    QString userSettings() const;

    QSettings *getQSettings();

    // QSettings wrappers
    void beginGroup(const QString &prefix);
    QStringList childGroups() const;
    QStringList childKeys() const;
    bool contains(const QString &key) const;
    void endGroup();
    QString fileName() const;
    QString group() const;

    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

    void sync();

    // first time running ?
    bool firstTimeRunning(const QString &subProcess = QString::null) const;
    void setFirstTimeRunning(const bool state, const QString &subProcess = QString::null);
    void noMoreFirstTimeRunning(const QString &subProcess = QString::null);

    QString licenseApprovedApplicationNumber() const;
    void setLicenseApprovedApplicationNumber(const QString &version);
    void setDefaultForm(const QString &formUid);
    QString defaultForm() const;

    // manage mainwindow
    void restoreState(QMainWindow *window, const QString &prefix = QString::null);
    void saveState(QMainWindow *window, const QString &prefix = QString::null);

    // paths
    void setPath(const int type, const QString &absPath);
    QString path(const int type) const;

    // for debugging functions : to treewidget and to string
    QTreeWidget *getTreeWidget(QWidget *parent) const;
    QString toString() const;

    // Network data
    Utils::DatabaseConnector databaseConnector() const;
    void setDatabaseConnector(Utils::DatabaseConnector &dbConnector);
    void readDatabaseConnector();
    void writeDatabaseConnector();
    bool updateDatabaseSettings();

    // values management
    void appendToValue(const QString &key, const QString &value);

protected:
    QString getIniFile(const QString &name = QString::null, const QString &version = QString::null);

private:
    QSettings *m_NetworkSettings, *m_UserSettings;
    QHash< int, QString > m_Enum_Path;
    bool m_FirstTime;
    Utils::DatabaseConnector m_DbConnector;
    bool m_NeedsSync;
};

} // End Internal
} // End Core

#endif // CORESETTINGSPRIVATE_H
