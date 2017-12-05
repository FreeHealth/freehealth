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
#ifndef CORESETTINGS_H
#define CORESETTINGS_H

#include <coreplugin/core_exporter.h>

#include <QObject>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QStringList;
class QTreeWidget;
class QMainWindow;
class QSettings;
class QFile;
QT_END_NAMESPACE

/**
 * \file isettings.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 01 Jun 2011
*/


namespace Utils {
class DatabaseConnector;
}


namespace Core {

class CORE_EXPORT ISettings : public QObject
{
    Q_OBJECT
public:

    enum Paths {
        UserResourcesPath = 0,
        LocalResourcesPath,
        ApplicationPath,
        BundleRootPath,
        BundleResourcesPath,
        ReadOnlyDatabasesPath,
        ReadWriteDatabasesPath,
        TranslationsPath,
        QtPluginsPath,
        QtFrameworkPath,
        AppPluginsPath,
        DocumentationPath,
        // Theme path
        ThemeRootPath,
        SmallPixmapPath,
        MediumPixmapPath,
        BigPixmapPath,
        SvgPixmapPath,
        SplashScreenPixmapPath,
        SystemTempPath,
        ApplicationTempPath,
        // User && Forms path
        CompleteFormsPath,
        SubFormsPath,
        UserDocumentsPath,
        UserCompleteFormsPath,
        LocalCompleteFormsPath,
        UserSubFormsPath,
        LocalSubFormsPath,
        DataPackCompleteFormsPath,
        DataPackSubFormsPath,
        // DataPack path
        DataPackPersistentTempPath,
        DataPackInstallPath,
        DataPackCompleteFormsInstallPath,
        DataPackSubFormsInstallPath,
        DataPackApplicationPath,
        // Default alert packs
        BundledAlertPacks,
        // WWW
        WebSiteUrl,
        UpdateUrl,
        SplashScreen
    };

    ISettings(QObject *parent = 0) : QObject(parent) {}
    virtual ~ISettings() {}

    virtual void setUserSettings(const QString &file) = 0;
    virtual QString userSettings() const = 0;

    virtual QSettings *getQSettings() = 0;

    // QSettings wrappers
    virtual void beginGroup(const QString &prefix) = 0;
    virtual QStringList childGroups() const = 0;
    virtual QStringList childKeys() const = 0;
    virtual bool contains(const QString &key) const = 0;
    virtual void endGroup() = 0;
    virtual QString fileName() const = 0;
    virtual QString group() const = 0;

    virtual void setValue(const QString &key, const QVariant &value) = 0;
    virtual QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const = 0;

    virtual bool firstTimeRunning(const QString &subProcess = QString::null) const = 0;
    virtual void setFirstTimeRunning(const bool state, const QString &subProcess = QString::null) = 0;
    virtual void noMoreFirstTimeRunning(const QString &subProcess = QString::null) = 0;

    virtual void sync() = 0;

    // Window's states
    virtual void restoreState(QMainWindow *window, const QString &prefix = QString::null) = 0;
    virtual void saveState(QMainWindow *window, const QString &prefix = QString::null) = 0;

    // paths
    virtual void setPath(const int type, const QString &absPath) = 0;
    virtual QString path(const int type) const = 0;
    virtual QString userResourcesPath() const { return path(UserResourcesPath); }
    virtual QString databasePath() const  { return path(ReadOnlyDatabasesPath); }
    virtual QString dataPackApplicationInstalledPath() const  { return path(DataPackApplicationPath); }
    virtual QString dataPackPersitentTempPath() const  { return path(DataPackPersistentTempPath); }
    virtual QString dataPackInstallPath() const  { return path(DataPackInstallPath); }
    virtual QString dataPackCompleteFormsInstallPath() const  { return path(DataPackCompleteFormsPath); }
    virtual QString dataPackSubFormsInstallPath() const  { return path(DataPackSubFormsInstallPath); }

    // Network data
    virtual Utils::DatabaseConnector databaseConnector() const = 0;
    virtual void setDatabaseConnector(Utils::DatabaseConnector &dbConnector) = 0;
    virtual void setDefaultForm(const QString &formUid) = 0;
    virtual QString defaultForm() const = 0;

    // values management
    virtual void appendToValue(const QString &key, const QString &value) = 0;

    // for debugging functions : to treewidget and to string
    virtual QTreeWidget *getTreeWidget(QWidget *parent) const = 0;
    virtual QString toString() const = 0;

Q_SIGNALS:
    void userSettingsSynchronized();
};

} // End Core

#endif // CORESETTINGS_H
