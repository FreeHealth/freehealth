/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
/**
  \class Core::ISettings
  \brief This class is a multiOS settings manager.
    When instanciating this class, the ini file is determined using member getIniFile().\n
     - If command line contains '--config=path/to/ini/file.ini' this file is tested and used if possible.\n
     - Else :
         - try ini file in home path
         - try to find next to the binary a file called \e pathtoconfig.ini which contains the path to the
         ini file to use (this feature is used for the USB-Key multi-OS configuration)

    m_ResourcesPath is protected and can be defined, retreive it using userResourcesPath().\n
    m_DatabasePath is protected and can be defined, retreive it using databasePath().

    The debugging members are used by Core::DebugDialog :
        - getTreeWidget() returns a treeWidget containing all values of the QSettings\n
        - toString() is idem but returns a QString formatted.

    You can store extra-data such as webSiteUrl().

    getTreeWidget() mechanism :\n
    - if *parent is a QTreeWidget, just populate it with infos\n
    - if passing QMap each element is supposed to be : "Name of path to show", "/absolute/path/to/show"\n

    DEFAULT BUNDLE PATHS

\verbatim
       MacOSX                                Linux/Win32

       homeDir (/Users/name)                homeDir (/home/name)
       |                                    |
       `- .ApplicationName                   `- .ApplicationName
        |                                     |
        |- config.ini                         |- config.ini             == user settings for non-networked apps
        |- config-network.ini                 |- config-network.ini
        |                                     |
        `- databases                          `- databases
         |                                     |
         |- drugs                              |- drugs
         |- templates                          |- templates
         `- ...                                `- ...


       ApplicationName.app                   Application-Version
       |- Contents                           |
          |- MacOs                           |
          |   `- applicationbinary           |- applicationbinary
          |                                  |
          |- Resources                       |- Resources                <-- ReadOnly Resources at least
          |   |                              |  |
          |   |- databases                   |  |- databases
          |   |  |- drugs                    |  |  |- drugs
          |   |                              |  |
          |   |- doc/application/html        |  |- doc/application/html  <-- user's manual
          |   |                              |  |
          |   |- forms                       |  |- forms
          |   |                              |  |
          |   |- pixmap                      |  |- pixmap                <-- Default Theme
          |   |  |- 16x16                    |  |  |- 16x16
          |   |  `- 32x32                    |  |  `- 32x32
          |   `- translations                |  `- translations
          |                                  |
          |- plugins                         |- plugins
          |  |- qt                           |  |- qt
          |                                  |
          `- FrameWorks (Qt FrameWorks)      `- libs (Qt if needed)
\endverbatim
*/

/*!
 \enum Core::ISettings::Paths
 Defines the availables paths to use with setPath() and path().\n
 Some paths are calculated when setting the
 Core::ISettings::ApplicationPath,
 Core::ISettings::BundleRootPath and the
 Core::ISettings::UserResourcesPath.
*/

/*! \var Core::ISettings::Paths Core::ISettings::UserResourcesPath
 * Defines the users' resources path. This path is readable and writable for the user.
 * When setting this path, the Core::ISettings::ReadWriteDatabasesPath is automatically calculated.
*/

/*! \var Core::ISettings::Paths Core::ISettings::ApplicationPath
 * Defines the application path. This path is to be concidered as read-only for the application.
 * When this path is defined, some paths are automatically calculated:
     - Core::ISettings::QtFrameWorksPath
     - Core::ISettings::FMFPlugInsPath
     - Core::ISettings::QtPlugInsPath
     - Core::ISettings::BundleRootPath
*/

/*! \var Core::ISettings::Paths Core::ISettings::BundleRootPath
 * Defines the root path of the bundle. On MacOs, the path is the one where lies the \e Application.app. On other OS,
 * the path is the path where stands the \e Application path.
*/

/*! \var Core::ISettings::Paths Core::ISettings::BundleResourcesPath
 * Defines the application bundle resources path. This path is to be concidered as read-only for the application.
 * When setting this path, some paths are calculated:
     - Core::ISettings::BundleResourcesPath
     - Core::ISettings::ReadOnlyDatabasesPath
     - Core::ISettings::TranslationsPath
     - Core::ISettings::ThemeRootPath
     - Core::ISettings::SmallPixmapPath
     - Core::ISettings::MediumPixmapPath
     - Core::ISettings::BigPixmapPath
     - Core::ISettings::CompleteFormsPath
     - Core::ISettings::SubFormsPath
     - Core::ISettings::DocumentationPath
*/

/*! \var Core::ISettings::Paths Core::ISettings::ReadOnlyDatabasesPath
 * Defines the read only databases path (mainly stored in application's bundle resources path).
*/

/*! \var Core::ISettings::Paths Core::ISettings::ReadWriteDatabasesPath
 * Defines the read-write databases path (mainly stored in user's resources path).
*/

/*! \var Core::ISettings::Paths Core::ISettings::TranslationsPath
 * Defines the translations dictionnaries path.
*/

/*! \var Core::ISettings::Paths Core::ISettings::QtPlugInsPath
 * Linked to the application path. Represents the application linked Qt plugins path.
*/

/*! \var Core::ISettings::Paths Core::ISettings::QtFrameWorksPath
 * Linked to the application path. Represents the application linked Qt framework path.
*/

/*! \var Core::ISettings::Paths Core::ISettings::FMFPlugInsPath
 * Linked to the application path. Represents the application plugin path.
*/

/*! \var Core::ISettings::Paths Core::ISettings::ThemeRootPath
 * Defines the theme root path.
*/

/*! \var Core::ISettings::Paths Core::ISettings::SmallPixmapPath
 * Defines the theme 16x16 pixmap.
*/

/*! \var Core::ISettings::Paths Core::ISettings::MediumPixmapPath
 * Defines the theme 32x32 pixmap.
*/

/*! \var Core::ISettings::Paths Core::ISettings::BigPixmapPath
 * Defines the theme 64x64 pixmap.
*/

/*! \var Core::ISettings::Paths Core::ISettings::SystemTempPath
 * Defines the system temporary path (accessible in read-write mode).
*/

/*! \var Core::ISettings::Paths Core::ISettings::ApplicationTempPath
 * Defines the temporary application path.
*/

/*! \var Core::ISettings::Paths Core::ISettings::CompleteFormsPath
 * Defines the Bundled full patient forms (used only with FreeMedForms).
*/

/*! \var Core::ISettings::Paths Core::ISettings::SubFormsPath
 * Defines the Bundled sub-forms (used only with FreeMedForms).
*/

/*! \var Core::ISettings::Paths Core::ISettings::DocumentationPath
 * Defines the users' manual path.
 * When the application is build with the LINUX_INTEGRATED config flag, the DocumentationPath is set to <em>/usr/share/doc/ApplicationName-doc/html/</em>.
 * Otherwise it is set inside the Bundle.
*/

/*! \var Core::ISettings::Paths Core::ISettings::DataPackPersistentTempPath
 * The datapack persistent cache path is the place where pack and config files are downloaded by the DataPack lib.
*/

/*! \var Core::ISettings::Paths Core::ISettings::DataPackInstallPath
 * Datapack user specific install path.
*/

/*! \var Core::ISettings::Paths Core::ISettings::DataPackApplicationPath
 * Datapack provided by default with the application are stored in this path.
*/

/*! \var Core::ISettings::Paths Core::ISettings::WebSiteUrl
 * Defines the application main web site.
*/

/*! \var Core::ISettings::Paths Core::ISettings::UpdateUrl
 * Defines the URL to use for the update changelog retriever.
*/

/**
 \fn virtual void Core::ISettings::beginGroup( const QString & prefix ) = 0;
 Wrapper member to QSettings. Read QSettings documentation.
*/
/**
 \fn virtual QStringList Core::ISettings::childGroups() const = 0;
 Wrapper member to QSettings. Read QSettings documentation.
*/
/**
 \fn virtual QStringList Core::ISettings::childKeys() const = 0;
 Wrapper member to QSettings. Read QSettings documentation.
*/
/**
 \fn virtual bool Core::ISettings::contains( const QString & key ) const = 0;
 Wrapper member to QSettings. Read QSettings documentation.
*/
/**
 \fn virtual void Core::ISettings::endGroup() = 0;
 Wrapper member to QSettings. Read QSettings documentation.
*/
/**
 \fn virtual QString Core::ISettings::fileName() const = 0;
 Wrapper member to QSettings. Read QSettings documentation.
*/
/**
 \fn virtual QString Core::ISettings::group() const = 0;
 Wrapper member to QSettings. Read QSettings documentation.
*/
/**
 \fn virtual void Core::ISettings::setValue( const QString & key, const QVariant & value ) = 0;
 Wrapper member to QSettings. Read QSettings documentation.
*/
/**
 \fn virtual QVariant Core::ISettings::value( const QString & key, const QVariant & defaultValue = QVariant() ) const = 0;
 Wrapper member to QSettings. Read QSettings documentation.
*/
/**
 \fn virtual void Core::ISettings::sync () = 0;
 Wrapper member to QSettings. Read QSettings documentation.
*/

#include "isettings.h"
#include "settings_p.h"

#include <translationutils/constanttranslations.h>
#include <utils/log.h>

#include <utils/global.h>

#include <QApplication>
#include <QMainWindow>
#include <QDockWidget>
#include <QTreeWidget>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QDesktopWidget>

/**
  \todo Manage user's settings stored into database
*/

namespace {

    const char* const WEBSITE              = "http://www.freemedforms.org/";

    // BUNDLE RESOURCES  --> located inside the bundle. Location calculated from BundleRootPath
    const char* const READONLYRESOURCES    = "";
    const char* const READONLYDATABASE     = "/databases";
    const char* const TRANSLATIONS_PATH    = "/translations";
    const char* const DEFAULTFORMS         = "/forms";
    const char* const DEFAULTDATAPACK      = "/datapacks";
    const char* const DEFAULTDATAPACK_APPINSTALLED               = "/datapacks/appinstalled";
    const char* const DEFAULTDATAPACK_APPINSTALLED_SUBFORMS      = "/datapacks/appinstalled/forms/subforms";
    const char* const DEFAULTDATAPACK_APPINSTALLED_COMPLETEFORMS = "/datapacks/appinstalled/forms/completeforms";
    const char* const DEFAULT_BUNDLED_ALERTPACKS = "/alertpacks";
    const char* const DEFAULTTHEME_PATH    = "";
    const char* const DEFAULTTHEME_PIXMAP  = "/pixmap";
    const char* const DEFAULTTHEME_SPLASH  = "/pixmap/splashscreens";
    const char* const USERMANUAL_PATH      = "/doc/%1";
    const char* const LINUX_USERMANUAL_PATH  = "/usr/share/doc/freemedforms-project/%1/html";
    // User documents sub-paths
    const char* const USER_SUBFORMSPATH       = "/forms/subforms";
    const char* const USER_COMPLETEFORMSPATH  = "/forms/completeforms";

    // APPLICATIONS RESOURCES --> located next to the application binary
    const char* const NONMAC_PLUGINSPATH   = "/plugins";
    const char* const ALL_QTPLUGINSPATH    = "/qt";
    const char* const MAC_QTFRAMEWORKPATH  = "/../FrameWorks";
    const char* const WIN_QTFRAMEWORKPATH  = "";
    const char* const UNIX_QTFRAMEWORKPATH = "/../libs";
    const char* const MAC_TOBUNDLEROOTPATH = "/../../..";
    const char* const WIN_TOBUNDLEROOTPATH = "/..";
    const char* const NUX_TOBUNDLEROOTPATH = "/..";
    const char* const BSD_TOBUNDLEROOTPATH = "/..";

    // USER WRITABLE RESOURCES  --> located inside/outside the bundle. Location calculated from UserResourcesPath (where stands the ini file)
    const char* const WRITABLEDATABASE     = "/databases";

    // SETTINGS
    const char* const S_LICENSE_VERSION    = "License/AcceptedVersion";
    const char* const S_DATABASECONNECTOR  = "Network/Db";
    const char* const S_DEFAULTFORM        = "Form/Default";

}

using namespace Core;
using namespace Core::Internal;


/**
  \brief Protected Constructor. Use instance() to create a new instance of this class.
  All path are calculated by the constructor.\n
  Users' writable resources are located in the dir of the config.ini file.
*/
SettingsPrivate::SettingsPrivate(QObject *parent, const QString &appName, const QString &fileName) :
    ISettings(parent),
    m_NetworkSettings(0),
    m_UserSettings(0),
    m_NeedsSync(false)
{
    setObjectName("SettingsPrivate");

    QString file = getIniFile(appName, fileName);
    QFileInfo fi(file);
    QString f = fi.absolutePath() + QDir::separator() + fi.baseName() + "-net." + fi.completeSuffix();
    m_NetworkSettings = new QSettings(f, QSettings::IniFormat, this);
    LOG_FOR("Settings", "Using network ini file " + f);

    m_UserSettings = new QSettings(file, QSettings::IniFormat, this);
    QString resourcesPath;
    QString applicationName;

    // if appName like "AppName - debug"  --> use "AppName" only
    if (appName.isEmpty())
        applicationName = qApp->applicationName();
    else
        applicationName = appName;
    if (applicationName.contains(" "))
        applicationName = applicationName.left(applicationName.indexOf(" "));
    Q_ASSERT(!applicationName.isEmpty());

    setPath(ApplicationPath, qApp->applicationDirPath());
    setPath(ApplicationTempPath, QString("%1/%2-%3").arg(QDir::tempPath()).arg(qApp->applicationName()).arg(Utils::createUid()));
    setPath(SystemTempPath, path(ApplicationTempPath));
    setPath(WebSiteUrl, WEBSITE);
    setPath(UserDocumentsPath, QDir::homePath() + QDir::separator() + applicationName.remove("-alpha").remove("_alpha").remove("_debug").remove("-debug") + QDir::separator() + "Documents");

    // Create paths
    if (!QDir(path(ApplicationTempPath)).exists()) {
        QDir().mkpath(path(ApplicationTempPath));
    }
    //    // Create UserDocumentsPath
//    if (!QDir(path(UserDocumentsPath)).exists()) {
//        QDir().mkpath(path(UserDocumentsPath));
//    }

//    if (Utils::isRunningOnLinux())
//        setPath(FMFPluginsPath, LIBRARY_BASENAME);

    if (Utils::isDebugWithoutInstallCompilation()) {
        LOG("Compiled with the debug_and_release configuration");
        // DEBUG WITHOUT INSTALL BUILD
        QString res;
        if (Utils::isRunningOnMac())
            res = qApp->applicationDirPath() + "/../../../../../global_resources";
        else
            res = qApp->applicationDirPath() + "/../../global_resources";

        res = QDir::cleanPath(res);
        resourcesPath = res + "/";
        setPath(UserResourcesPath, QFileInfo(file).absolutePath());

        if (Utils::isRunningOnMac()) {
            setPath(BundleResourcesPath, resourcesPath);
        } else {
            setPath(BundleResourcesPath, resourcesPath);
        }
    } else {
        // RELEASE OR DEBUG INSTALLED BUILD
        if (Utils::isLinuxIntegratedCompilation()) {
            setPath(BundleResourcesPath, QString("/usr/share/freemedforms"));
        } else {
            QString bundleResourcesPath;
            if (!Utils::isDebugWithoutInstallCompilation())
                bundleResourcesPath = "Resources";  // resources are located into global_resources paths

            if (Utils::isRunningOnMac())
                setPath(BundleResourcesPath, qApp->applicationDirPath() + "/../" + bundleResourcesPath);
            else
                setPath(BundleResourcesPath, qApp->applicationDirPath() + QDir::separator() + bundleResourcesPath);
        }
        m_FirstTime = value("FirstTimeRunning", true).toBool();
        setPath(UserResourcesPath, QFileInfo(file).absolutePath());//QDir::homePath() + "/." + applicationName);//resourcesPath);
    }

    readDatabaseConnector();

    if (parent)
        setParent(parent);
    else
        setParent(qApp);
}

SettingsPrivate::~SettingsPrivate()
{
    if (m_NetworkSettings) {
        m_NetworkSettings->sync();
        delete m_NetworkSettings;
        m_NetworkSettings = 0;
    }
    if (m_UserSettings) {
//        m_UserSettings->sync();
        delete m_UserSettings;
        m_UserSettings = 0;
    }
    QString error;
    Utils::removeDirRecursively(path(ApplicationTempPath), &error);
    if (!error.isEmpty())
        LOG_ERROR(QString("Unable to remove application temporary path: %1; %2").arg(path(ApplicationTempPath)).arg(error));
}

void SettingsPrivate::setUserSettings(const QString &content)
{
    // create a temp file with the content
    QString fileName = path(ApplicationTempPath) + QDir::separator() + Utils::Database::createUid() + ".ini";
    QFile f(fileName);
    while (f.exists()) {
        fileName = path(ApplicationTempPath) + QDir::separator() + Utils::Database::createUid() + ".ini";
        f.setFileName(fileName);
    }

    // populate the file with the content
    if (!Utils::saveStringToFile(content, fileName, Utils::Overwrite, Utils::DontWarnUser))
        LOG_ERROR("Unable to save user preferences content");

    // change the fileName of the current QSettings
    if (m_UserSettings) {
        delete m_UserSettings;
        m_UserSettings = 0;
    }
    m_UserSettings = new QSettings(fileName, QSettings::IniFormat, this);
    m_NeedsSync = false;
}

QString SettingsPrivate::userSettings() const
{
    return Utils::readTextFile(m_UserSettings->fileName(), Utils::DontWarnUser);
}

/**
  \fn QSettings *Core::ISettings::getQSettings()
  Using this member should be avoid in your code.
*/
QSettings *SettingsPrivate::getQSettings()
{
    return m_UserSettings;
}

void SettingsPrivate::beginGroup(const QString &prefix) { m_UserSettings->beginGroup(prefix); m_NeedsSync=true; }
QStringList SettingsPrivate::childGroups() const { return m_UserSettings->childGroups(); }
QStringList SettingsPrivate::childKeys() const { return m_UserSettings->childKeys(); }
bool SettingsPrivate::contains(const QString &key) const { return m_UserSettings->contains(key); }
void SettingsPrivate::endGroup() { m_UserSettings->endGroup(); m_NeedsSync=true; }
QString SettingsPrivate::fileName() const { return m_UserSettings->fileName(); }
QString SettingsPrivate::group() const { return m_UserSettings->group();}

void SettingsPrivate::setValue(const QString &key, const QVariant &value)
{
    if (m_UserSettings->value(key, QVariant()) == value)
        return;
    m_UserSettings->setValue(key, value);
    m_NeedsSync=true;
}

QVariant SettingsPrivate::value(const QString &key, const QVariant &defaultValue) const
{
    return m_UserSettings->value(key, defaultValue);
}

void SettingsPrivate::sync()
{
    if (!m_NeedsSync)
        return;
    if (m_UserSettings)
        m_UserSettings->sync();
    if (m_NetworkSettings)
        m_NetworkSettings->sync();
    m_NeedsSync = false;
    Q_EMIT userSettingsSynchronized();
}

/**
  \fn void Core::ISettings::setPath(const int type, const QString & absPath)
  \brief defines a path \e absPath with the index \e type refering to the enumarator \e Settings::Paths.
  When setting ApplicationPath, some paths are automatically recalculated : BundleRootPath, QtFrameWorksPath, FMFPlugInsPath, QtPlugInsPath.\n
  When setting BundleResourcesPath, some paths are automatically recalculated : ReadOnlyDatabasesPath, TranslationsPath, SmallPixmapPath, MediumPixmapPath, BigPixmapPath, CompleteFormsPath, SubFormsPath.\n
  When setting UserResourcesPath, some paths are automatically recalculated : ReadWriteDatabasesPath.\n
*/
void SettingsPrivate::setPath(const int type, const QString & absPath)
{
    switch (type)
    {
        case WebSiteUrl :
        {
            m_Enum_Path.insert(type, absPath);
            qApp->setOrganizationDomain(absPath);
            break;
        }
        case UpdateUrl :
        {
            m_Enum_Path.insert(type, absPath);
            break;
        }
        case UserResourcesPath :
        {
            QString resourcesPath = QDir::cleanPath(absPath);
            m_Enum_Path.insert(UserResourcesPath, resourcesPath);
            // Read Write user databases
            QString dbPath = resourcesPath + "/databases";
            Utils::checkDir(dbPath, true, "Settings::ReadWriteDatabasesPath");
            m_Enum_Path.insert(ReadWriteDatabasesPath, dbPath);
            // User datapack config
            QString packtmp = resourcesPath + "/datapacks/tmp";
            Utils::checkDir(packtmp, true, "Settings::DataPackPersistentTempPath");
            m_Enum_Path.insert(DataPackPersistentTempPath, packtmp);
            // Datapack install path
            QString packinst = resourcesPath + "/datapacks/install";
            Utils::checkDir(packinst, true, "Settings::DataPackPersistentTempPath");
            m_Enum_Path.insert(DataPackInstallPath, packinst);
            // Datapack user installed forms path
            packinst = resourcesPath + "/datapacks/install/forms/completeforms";
            Utils::checkDir(packinst, true, "Settings::DataPackCompleteFormsInstallPath");
            m_Enum_Path.insert(DataPackCompleteFormsInstallPath, packinst);
            packinst = resourcesPath + "/datapacks/install/forms/subforms";
            Utils::checkDir(packinst, true, "Settings::DataPackSubFormsInstallPath");
            m_Enum_Path.insert(DataPackSubFormsInstallPath, packinst);
            break;
        }
        case BundleResourcesPath :
        {
            if (m_Enum_Path.value(BundleResourcesPath)==QDir::cleanPath(absPath))
                break;
            QString bundlePath = QDir::cleanPath(absPath);
            m_Enum_Path.insert(BundleResourcesPath, bundlePath);
            m_Enum_Path.insert(ReadOnlyDatabasesPath, bundlePath + READONLYDATABASE);
            m_Enum_Path.insert(TranslationsPath, bundlePath + TRANSLATIONS_PATH);
            m_Enum_Path.insert(ThemeRootPath, bundlePath + DEFAULTTHEME_PATH);
            m_Enum_Path.insert(SmallPixmapPath, bundlePath + DEFAULTTHEME_PIXMAP + "/16x16/");
            m_Enum_Path.insert(MediumPixmapPath, bundlePath + DEFAULTTHEME_PIXMAP + "/32x32/");
            m_Enum_Path.insert(BigPixmapPath, bundlePath + DEFAULTTHEME_PIXMAP + "/64x64/");
            m_Enum_Path.insert(SvgPixmapPath, bundlePath + DEFAULTTHEME_PIXMAP + "/svg/");
            m_Enum_Path.insert(SplashScreenPixmapPath, bundlePath + DEFAULTTHEME_PIXMAP + "/splashscreens/");
            m_Enum_Path.insert(SubFormsPath, bundlePath + DEFAULTFORMS + "/subforms");
            m_Enum_Path.insert(CompleteFormsPath, bundlePath + DEFAULTFORMS + "/completeforms");
            m_Enum_Path.insert(DataPackApplicationPath, bundlePath + DEFAULTDATAPACK_APPINSTALLED);
            m_Enum_Path.insert(DataPackCompleteFormsPath, bundlePath + DEFAULTDATAPACK_APPINSTALLED_COMPLETEFORMS);
            m_Enum_Path.insert(DataPackSubFormsPath, bundlePath + DEFAULTDATAPACK_APPINSTALLED_SUBFORMS);
            m_Enum_Path.insert(BundledAlertPacks, bundlePath + DEFAULT_BUNDLED_ALERTPACKS);
            QString appname = qApp->applicationName().toLower();
            if (qApp->applicationName().contains(" ")) {
                appname = appname.left(appname.indexOf(" "));
            }
            if (!Utils::isReleaseCompilation()) {
                if (appname.contains("_d"))
                    appname = appname.left(appname.indexOf("_d"));
            }
            if (Utils::isLinuxIntegratedCompilation()) {
                if (QDir(QString(LINUX_USERMANUAL_PATH).arg(appname)).exists()) {
                    m_Enum_Path.insert(DocumentationPath, QString(LINUX_USERMANUAL_PATH).arg(appname));
                } else {
                    m_Enum_Path.insert(DocumentationPath, bundlePath + QString(USERMANUAL_PATH).arg(appname));
                }
            } else {
                m_Enum_Path.insert(DocumentationPath, bundlePath + QString(USERMANUAL_PATH).arg(appname));
            }
            break;
        }
        case ApplicationPath :
        {
            if (m_Enum_Path.value(ApplicationPath)==QDir::cleanPath(absPath))
                break;
            m_Enum_Path.insert(ApplicationPath, absPath);
            if (Utils::isRunningOnMac()) {
                QString pluginPath;
                if (Utils::isDebugWithoutInstallCompilation())
                    pluginPath = "/../../../plugins";
                else
                    pluginPath = "/../plugins";
                m_Enum_Path.insert(QtFrameWorksPath, QDir::cleanPath(absPath + MAC_QTFRAMEWORKPATH));
                m_Enum_Path.insert(FMFPlugInsPath, QDir::cleanPath(absPath + pluginPath));
                m_Enum_Path.insert(QtPlugInsPath, QDir::cleanPath(absPath + pluginPath + ALL_QTPLUGINSPATH));
                m_Enum_Path.insert(BundleRootPath, QDir::cleanPath(absPath + MAC_TOBUNDLEROOTPATH));
            } else if (Utils::isRunningOnLinux()) {
#ifndef LINUX_INTEGRATED
                m_Enum_Path.insert(QtFrameWorksPath, QDir::cleanPath(absPath + UNIX_QTFRAMEWORKPATH));
                m_Enum_Path.insert(QtPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH + ALL_QTPLUGINSPATH));
#else
                m_Enum_Path.insert(QtFrameWorksPath, QDir::cleanPath(LINUX_QT_PATH));
                m_Enum_Path.insert(QtPlugInsPath, QDir::cleanPath(LINUX_QT_PLUGINS_PATH));
#endif
                m_Enum_Path.insert(FMFPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH));
                m_Enum_Path.insert(BundleRootPath, QDir::cleanPath(absPath + NUX_TOBUNDLEROOTPATH));
            } else if (Utils::isRunningOnWin()) {
                m_Enum_Path.insert(QtFrameWorksPath, QDir::cleanPath(absPath + WIN_QTFRAMEWORKPATH));
                m_Enum_Path.insert(FMFPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH));
                m_Enum_Path.insert(QtPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH + ALL_QTPLUGINSPATH));
                m_Enum_Path.insert(BundleRootPath, QDir::cleanPath(absPath + WIN_TOBUNDLEROOTPATH));
            } else if (Utils::isRunningOnFreebsd()) {
                m_Enum_Path.insert(QtFrameWorksPath, QDir::cleanPath(absPath + UNIX_QTFRAMEWORKPATH));
                m_Enum_Path.insert(FMFPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH));
                m_Enum_Path.insert(QtPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH + ALL_QTPLUGINSPATH));
                m_Enum_Path.insert(BundleRootPath, QDir::cleanPath(absPath + BSD_TOBUNDLEROOTPATH));
            }
            break;
        }
    case UserDocumentsPath:
    {
        const QString &cleanPath = QDir::cleanPath(absPath);
        Utils::checkDir(cleanPath, true, "Settings::UserDocumentsPath");
        Utils::checkDir(cleanPath + USER_SUBFORMSPATH, true, "Settings::UserSubFormsPath");
        Utils::checkDir(cleanPath + USER_COMPLETEFORMSPATH, true, "Settings::UserCompleteFormsPath");
        m_Enum_Path.insert(UserDocumentsPath, cleanPath);
        m_Enum_Path.insert(UserSubFormsPath, cleanPath + USER_SUBFORMSPATH);
        m_Enum_Path.insert(UserCompleteFormsPath, cleanPath + USER_COMPLETEFORMSPATH);
        break;
    }
    default: m_Enum_Path.insert(type, QDir::cleanPath(absPath)); break;
    }
}

/**
  \fn QString Core::ISettings::path(const int type) const
  \brief Returns the path according to the enumerator Settings::Paths
*/
QString SettingsPrivate::path(const int type) const
{
    if (type == ISettings::DocumentationPath) {
        QString tmp = m_Enum_Path.value(type);
        QString translatedPath = tmp + QDir::separator() + QLocale().name().left(2) + "/html";
//        qWarning() << tmp << translatedPath;
        if (QDir(translatedPath).exists())
            return translatedPath;
        else
            return tmp + "/en/html";

    }
    return m_Enum_Path.value(type);
}

/**
  \fn bool Core::ISettings::firstTimeRunning(const QString &subProcess) const
  \brief Return true if the application runs for the first time.
  \sa noMoreFirstTimeRunning(const QString &subProcess)
*/
bool SettingsPrivate::firstTimeRunning(const QString &subProcess) const
{
    if (subProcess.isEmpty())
        return m_NetworkSettings->value("FirstTimeRunning", true).toBool();
    return m_UserSettings->value("FirstTimeRunning/" + subProcess, true).toBool();
}

/**
  \fn void Core::ISettings::noMoreFirstTimeRunning(const QString &subProcess)
  \brief Set the first time running of this application to false.
  \sa firstTimeRunning(const QString &subProcess)
*/
void SettingsPrivate::noMoreFirstTimeRunning(const QString &subProcess)
{
    if (subProcess.isEmpty()) {
        m_NetworkSettings->setValue("FirstTimeRunning", false);
        m_NetworkSettings->sync();
        m_FirstTime = false;
    } else {
        m_UserSettings->setValue("FirstTimeRunning/" + subProcess, false);
        m_NeedsSync = true;
        Q_EMIT userSettingsSynchronized();
    }
}

/**
  \fn void Core::ISettings::setFirstTimeRunning(const bool state, const QString &subProcess)
  \brief Set the first time running of this application to false.
  \sa firstTimeRunning(const QString &subProcess)
*/
void SettingsPrivate::setFirstTimeRunning(const bool state, const QString &subProcess)
{
    if (subProcess.isEmpty()) {
        m_NetworkSettings->setValue("FirstTimeRunning", state);
        m_NetworkSettings->sync();
        m_FirstTime = false;
    } else {
        m_UserSettings->setValue("FirstTimeRunning/" + subProcess, state);
        m_NeedsSync = true;
        Q_EMIT userSettingsSynchronized();
    }
}

/**
  \fn QString Core::ISettings::licenseApprovedApplicationNumber() const
*/
QString SettingsPrivate::licenseApprovedApplicationNumber() const
{
    return m_NetworkSettings->value(S_LICENSE_VERSION).toString();
}

/**
  \fn void Core::ISettings::setLicenseApprovedApplicationNumber(const QString &version)
*/
void SettingsPrivate::setLicenseApprovedApplicationNumber(const QString &version)
{
    m_NetworkSettings->setValue(S_LICENSE_VERSION, version);
}

/**
  \fn void Core::ISettings::setDefaultForm(const QString &formUid)
  Store the default form to use for patient files. Only used by the Application General Configurator.
*/
void SettingsPrivate::setDefaultForm(const QString &formUid)
{
    m_NetworkSettings->setValue(S_DEFAULTFORM, formUid);
}

/**
  \fn void Core::ISettings::defaultForm() const
  Return the default form to use for patient files. Only used by the Application General Configurator.
*/
QString SettingsPrivate::defaultForm() const
{
    return m_NetworkSettings->value(S_DEFAULTFORM).toString();
}

/**
  \fn QString Core::ISettings::getIniFile(const QString & appName, const QString & fileName)
  \brief Returns the ini file to use the the initialization of QSettings. See constructor.
  Test in this order :
  \li command line --config="/abs/path/to/config.ini" or --config="../relative/path/to/config.ini". If the ini file can be used it is returned.
  \li read the applicationBinaryPath/pathtoconfig.ini and search for the config.ini in the specified file
  \li /homePath/.(qApp->applicationName)
*/
QString SettingsPrivate::getIniFile(const QString & appName, const QString & fileName)
{
    // manage defaults --> ".AppName/config.ini"
    QString tmpAppName = appName;
    QString tmpFileName = fileName;
    if (appName.isEmpty()) {
        tmpAppName = qApp->applicationName();
        if (tmpAppName.contains(" "))
            tmpAppName = tmpAppName.left(tmpAppName.indexOf(" "));
    }
    tmpAppName.prepend(".");
    if (Utils::isAlpha())
        tmpAppName.append("_alpha");

    if (fileName.isEmpty())
        tmpFileName = "config.ini";

    // if QApplication args contains "--config=iniFileName.ini" use it if possible
    // retreive command line arguments
    // TODO: get the command line args through the Core::ICommandLine?
    QStringList list = QCoreApplication::arguments();
    int index = list.indexOf(QRegExp("--config=*", Qt::CaseSensitive, QRegExp::Wildcard));
    if (index != -1) {
        // Config file passed
        QString iniFileName = list[index];
        iniFileName = iniFileName.mid(iniFileName.indexOf("=") + 1);
        LOG_FOR("Settings", tr("Passing command line ini file: %1").arg(iniFileName));

        if (QDir::isRelativePath(iniFileName))
            iniFileName.prepend(qApp->applicationDirPath() + QDir::separator());
        iniFileName = QDir::cleanPath(iniFileName);

        QFileInfo info(iniFileName);
        if (info.exists() && info.isReadable()) {
            if (info.isWritable()) {
                LOG_FOR("Settings", tr("Using ini file %1.").arg(iniFileName));
                return iniFileName;
            } else {
                LOG_ERROR_FOR("Settings", tr("Ini file %1 is not writable. Can not use it.").arg(iniFileName));
            }
        } else {
            // can we create and access to ini file ?
            QFile file(iniFileName);
            if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
                LOG_FOR("Settings", tr("Using ini file %1").arg(iniFileName));
                return iniFileName;
            } else {
                LOG_FOR("Settings", tr("WARNING: Ini file %1 can not be used.").arg(iniFileName));
            }
        }
    }

    // No config file was passed in the command line
    // Or the param config file does not exists
    // -> try to use read the 'pathtoconfig.ini' inside the bundle
    if (QFile(QString("%1/pathtoconfig.ini").arg(qApp->applicationDirPath())).exists()) {
        QString content = Utils::readTextFile(QString("%1/pathtoconfig.ini").arg(qApp->applicationDirPath()), Utils::DontWarnUser);
        content = content.trimmed();
        if (!content.isEmpty()) {
            LOG_FOR("Settings", tr("Found a configuration file next to the binary. Reading file."));
            if (content.startsWith("~/"))
                content.replace("~/", QDir::homePath() + "/");
            if (QDir::isRelativePath(content))
                content.prepend(qApp->applicationDirPath() + QDir::separator());
            content = QDir::cleanPath(content);
            QFileInfo info(content);
            if (info.exists() && info.isReadable()) {
                if (info.isWritable()) {
                    LOG_FOR("Settings", tr("Using ini file %1.").arg(content));
                    return content;
                } else {
                    LOG_ERROR_FOR("Settings", tr("Ini file %1 is not writable. Can not use it.").arg(content));
                }
            } else {
                // can we create and access to ini file ?
                if (!info.absoluteDir().exists()) {
                    if (!QDir().mkpath(info.absolutePath()))
                        LOG_ERROR_FOR("Settings", tr("File does not exists and can not be created: %1").arg(content));
                }
                QFile file(content);
                if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
                    LOG_FOR("Settings", tr("Using ini file %1").arg(content));
                    return content;
                } else {
                    LOG_FOR("Settings", tr("WARNING: Ini file %1 can not be used.").arg(content));
                }
            }
            LOG_ERROR_FOR("Settings", tr("File does not exists and can not be created: %1").arg(content));
        }
    }

    QString iniFile;
    //    if (Utils::isRunningOnMac())
    //        iniFile = QDir::cleanPath(qApp->applicationDirPath() + "/../Resources") + QDir::separator() + tmpFileName;
    //    else
    //        iniFile = qApp->applicationDirPath() + QDir::separator() + tmpFileName;
    //
    //    // test iniFile
    //    Log::addMessage("Settings", tr("Trying ini file %1").arg(iniFile));
    //    QFile file(iniFile);
    //    QFileInfo info(iniFile);
    //    if (info.exists() && info.isReadable() && info.isWritable()) {
    //        Log::addMessage("Settings", tr("Using ini file %1").arg(iniFile));
    //        return iniFile;
    //    } else {
    //        if ((! info.exists()) &&  file.open(QIODevice::ReadWrite | QIODevice::Text)) {
    //            Log::addMessage("Settings", tr("Using ini file %1").arg(iniFile));
    //            return iniFile;
    //        }
    //        else Log::addError("Settings", tr("Ini file %1 can not be used.").arg(iniFile));
    //    }

    // Now use the $HOME path
    iniFile = QString("%1/%2/%3").arg(QDir::homePath(), tmpAppName, tmpFileName);
    LOG_FOR("Settings", tr("Trying ini file %1").arg(iniFile));
    QDir dir(QFileInfo(iniFile).absolutePath());

    if (!dir.exists()) {
        dir.cdUp();
        if (!dir.mkdir(tmpAppName)) {
            LOG_ERROR_FOR("Settings" , tr("Unable to create dir: %1, no Ini File can be used.").arg(dir.absolutePath() + QDir::separator() + tmpAppName));
            return QString::null;
        }
    }

    LOG_FOR("Settings", tr("Using ini file %1").arg(iniFile));
    return iniFile;
}

/**
  \fn void Core::ISettings::restoreState(QMainWindow * window, const QString & prefix)
  \brief Main windows restore state. \e prefix can be used if you store multiple main window in the same settings
*/
void SettingsPrivate::restoreState(QMainWindow *window, const QString & prefix)
{
    if (!window)
        return;
    QString keyGeo = prefix + "MainWindow/Geometry";
    QString keyState = prefix + "MainWindow/State";
    if (value(keyGeo).toByteArray().isEmpty()) {
        int height = qApp->desktop()->height() * 0.75;
        int width = qApp->desktop()->width() * 0.75;
        // screen is 4/3 ?
        double screenRatio = (double)width/(double)height;
        QSize ratio;
        if (screenRatio < 1.5) {
            ratio = QSize(4,3);
        } else {
            ratio = QSize(16, 9);
        }
        QSize result(width, height);
        ratio.scale(result, Qt::KeepAspectRatio);
        QRect appScreen(QPoint(0,0), ratio);
        QRect rect = qApp->desktop()->rect();
        appScreen.moveCenter(rect.center());
        window->setGeometry(appScreen);
    } else {
        window->restoreGeometry(value(keyGeo).toByteArray());
        window->restoreState(value(keyState).toByteArray());
        // get all settings key starting with prefix+"/Dock"
        QStringList k = m_UserSettings->allKeys().filter(QRegExp(QString(prefix + "Dock/"), Qt::CaseSensitive, QRegExp::Wildcard));
        QWidget *w = 0;
        foreach(const QString &s, k) {
            w = window->findChild<QDockWidget*>(s.mid(s.indexOf("Dock/")+5));
            if (w) {
                w->restoreGeometry(value(prefix + "/Dock/" + w->objectName()).toByteArray());
            }
            w = 0;
        }
    }
}

/**
  \fn void Core::ISettings::saveState(QMainWindow * window, const QString & prefix)
  \brief Main windows save state. \e prefix can be used if you store multiple main window in the same Settings
*/
void SettingsPrivate::saveState(QMainWindow * window, const QString & prefix)
{
    if (!window)
        return;
    setValue(prefix + "MainWindow/Geometry", window->saveGeometry());
    setValue(prefix + "MainWindow/State", window->saveState());
    // save all dockwidget
    foreach(QDockWidget * w, window->findChildren<QDockWidget*>()) {
        setValue(prefix + "Dock/" + w->objectName(), w->saveGeometry());
    }
}

/**
  \fn void Core::ISettings::appendToValue(const QString &key, const QString &value)
  \brief Append a string \e value to the stringlist represented by the \e key in settings assuming no doublon.
*/
void SettingsPrivate::appendToValue(const QString &key, const QString &value)
{
    QVariant q = this->value(key);
    if (q.isNull()) {
        this->setValue(key, value);
        return;
    }
    if (q.toStringList().indexOf(value) == -1) {
        QStringList list = q.toStringList();
        list.append(value);
        this->setValue(key, list);
    }
}

/**
  \fn QTreeWidget* Core::ISettings::getTreeWidget(QWidget *parent) const
  \brief For debugging purpose.
  If \e parent is a QTreeWidget, it will be populated with data and returned. Otherwise a
  QTreeWidget is created with \e parent as parent and returned.
*/
QTreeWidget* SettingsPrivate::getTreeWidget(QWidget *parent) const
{
    // manage parent as tree widget
    QTreeWidget * tree = qobject_cast<QTreeWidget*>(parent);
    if (!tree)
        tree = new QTreeWidget(parent);

    // add columns
    tree->setColumnCount(2);
    tree->setHeaderLabels(QStringList() << tr("Name") << tr("Value"));
    QFont bold;
    bold.setBold(true);

    // add system information
    QTreeWidgetItem * sysItem = new QTreeWidgetItem(tree, QStringList() << tr("System information"));
    sysItem->setFont(0,bold);
    new QTreeWidgetItem(sysItem, QStringList() << tr("Operating System") << Utils::osName());
#ifndef Q_OS_WIN32
    new QTreeWidgetItem(sysItem, QStringList() << tr("uname output") << Utils::uname());
#endif

    // add compilation information
    QTreeWidgetItem * compileItem = new QTreeWidgetItem(tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_INFORMATION));
    compileItem->setFont(0,bold);
    new QTreeWidgetItem(compileItem, QStringList() << tr("Compilation Date") << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DATE_TIME_1_2).arg(__DATE__, __TIME__));
    new QTreeWidgetItem(compileItem, QStringList() << tr("Compile Qt version") << QString("%1").arg(QT_VERSION_STR));
    new QTreeWidgetItem(compileItem, QStringList() << tr("Actual Qt version") << QString("%1").arg(qVersion()));
    new QTreeWidgetItem(compileItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_VERSION_1).arg("") << qApp->applicationVersion());
    if (Utils::isReleaseCompilation()) {
        new QTreeWidgetItem(compileItem, QStringList() << tr("Compile mode") << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_RELEASE));
    } else {
        if (Utils::isDebugWithoutInstallCompilation()) {
            new QTreeWidgetItem(compileItem, QStringList() << tr("Compile mode") << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DEBUG) + " - no install");
        } else {
            new QTreeWidgetItem(compileItem, QStringList() << tr("Compile mode") << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DEBUG));
        }
    }
    new QTreeWidgetItem(compileItem, QStringList() << tr("GIT revision") << QString(GIT_REVISION_HASH));


    // add paths
    QDir appDir(qApp->applicationDirPath());
    QMap<QString, QString> paths;
    paths.insert(tr("Binary"), path(ApplicationPath));
    paths.insert(tr("UserResourcesPath"), path(UserResourcesPath));
    paths.insert(tr("Read only Databases"), path(ReadOnlyDatabasesPath));
    paths.insert(tr("Writable databases"), path(ReadWriteDatabasesPath));
    paths.insert(tr("Bundle root path"), path(BundleRootPath));
    paths.insert(tr("Bundle resources path"), path(BundleResourcesPath));
    paths.insert(tr("Translations path"), path(TranslationsPath));
    paths.insert(tr("Qt Plugins path"), path(QtPlugInsPath));
    paths.insert(tr("Qt FrameWorks path"), path(QtFrameWorksPath));
    paths.insert(tr("FreeMedForms PlugIns path"), path(FMFPlugInsPath));
    paths.insert(tr("SmallPixmapPath"), path(SmallPixmapPath));
    paths.insert(tr("MediumPixmapPath"), path(MediumPixmapPath));
    paths.insert(tr("BigPixmapPath"), path(BigPixmapPath));
    paths.insert(tr("SystemTempPath"), path(SystemTempPath));
    paths.insert(tr("ApplicationTempPath"), path(ApplicationTempPath));
    paths.insert(tr("CompleteFormsPath"), path(CompleteFormsPath));
    paths.insert(tr("SubFormsPath"), path(SubFormsPath));
    paths.insert(tr("UserCompleteFormsPath"), path(UserCompleteFormsPath));
    paths.insert(tr("UserSubFormsPath"), path(UserSubFormsPath));
    paths.insert(tr("Default installed datapack path"), path(DataPackApplicationPath));
    paths.insert(tr("Datapack persistent temporary path"), path(DataPackPersistentTempPath));
    paths.insert(tr("Datapack installation path"), path(DataPackInstallPath));
    paths.insert(tr("Datapack Complete Forms installation path"), path(DataPackCompleteFormsInstallPath));
    paths.insert(tr("Datapack SubForms installation path"), path(DataPackSubFormsInstallPath));

    QTreeWidgetItem * absPathsItem = new QTreeWidgetItem(tree, QStringList() << tr("Absolute Paths"));
    absPathsItem->setFont(0,bold);
    new QTreeWidgetItem(absPathsItem, QStringList() << tr("Using Ini File") << fileName());

    QTreeWidgetItem * relPathsItem = new QTreeWidgetItem(tree, QStringList() << tr("Relative Paths"));
    relPathsItem->setFont(0,bold);
    new QTreeWidgetItem(relPathsItem, QStringList() << tr("Using Ini File") << appDir.relativeFilePath(QFileInfo(fileName()).absoluteFilePath()));

    foreach(const QString & p, paths.keys()) {
        new QTreeWidgetItem(relPathsItem, QStringList() << p << appDir.relativeFilePath(QFileInfo(paths[p]).absoluteFilePath()));
        new QTreeWidgetItem(absPathsItem, QStringList() << p << paths[p]);
    }
    new QTreeWidgetItem(relPathsItem, QStringList() << tr("WebSiteUrl") << path(WebSiteUrl));
    new QTreeWidgetItem(absPathsItem, QStringList() << tr("WebSiteUrl") << path(WebSiteUrl));

    //add library information
//    new QTreeWidgetItem(tree, QStringList() << tr("Libs") << Utils::getLibraryInformation());

    // add settings
    QTreeWidgetItem * settingsItem = new QTreeWidgetItem(tree, QStringList() << tr("Settings values"));
    settingsItem->setFont(0,bold);
    settingsItem->setExpanded(true);
    QTreeWidgetItem * orphan = new QTreeWidgetItem(settingsItem, QStringList() << tr("Orphan settings"));
    orphan->setFont(0,bold);
    QTreeWidgetItem * group = 0;
    QStringList list = m_UserSettings->allKeys();
    qSort(list);
    foreach(const QString & k, list) {
        if (k.contains("/")) {
            QString tmp = k.left(k.indexOf("/"));
            if (!group) {
                group = new QTreeWidgetItem(settingsItem, QStringList() << tmp);
                group->setFont(0,bold);
            }
            if (!k.startsWith(group->text(0))) {
                group = new QTreeWidgetItem(settingsItem, QStringList() << tmp);
                group->setFont(0,bold);
            }
            new QTreeWidgetItem(group, QStringList() << k.mid(k.indexOf("/") + 1)  << value(k).toString());
        }
        else
            new QTreeWidgetItem(orphan, QStringList() << k << value(k).toString());
    }

    // resize columns
    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);

    return tree;
}

//static QString cupsInfo()
//{
//    QString tmp;
//    QProcess cups;
//    cups.start("cups-config", QStringList() << "--version");
//    if (!cups.waitForStarted())
//        LOG_ERROR_FOR("Utils", QApplication::translate("Utils", "Error while retrieve information of uname under %1").arg(system));
//    if (!cups.waitForFinished())
//        LOG_ERROR_FOR("Utils", QApplication::translate("Utils", "Error while retrieve information of uname under %1").arg(system));
//    tmp += "| Version | " + cups.readAll() + " |";

//    cups.start("cups-config", QStringList() << "--datadir");
//    if (!cups.waitForStarted())
//        LOG_ERROR_FOR("Utils", QApplication::translate("Utils", "Error while retrieve information of uname under %1").arg(system));
//    if (!cups.waitForFinished())
//        LOG_ERROR_FOR("Utils", QApplication::translate("Utils", "Error while retrieve information of uname under %1").arg(system));
//    tmp += "| DataDir | " + cups.readAll() + " |";
//    return tmp;
//}

/** \brief For debugging purpose. */
QString SettingsPrivate::toString() const
{
    QString tmp;
    tmp += "===== SETTINGS =====\n";

    // add building information
    tmp += "\n^ Object ^ Value ^";
    tmp += "\n| " + tr("Running version: %1").arg(qApp->applicationName()).replace(":", "|") + " |";
    tmp += "\n| " + tr("Build date | %1 %2").arg(__DATE__, __TIME__) + " |";
    tmp += "\n| " + tr("Qt Build version: %1").arg(QT_VERSION_STR).replace(":", "|") + " |";
    tmp += "\n| " + tr("Qt running version: %1").arg(qVersion()).replace(":", "|") + " |";
    tmp += "\n| " + tr("Application Version: %1").arg(qApp->applicationVersion()).replace(":", "|") + " |";
    if (!Utils::isReleaseCompilation())
        tmp += "\n| " + tr("Actual build: Debug").replace(":", "|") + " |";
    else
        tmp += "\n| " + tr("Actual build: Release").replace(":", "|") + " |";
    tmp += "\n| " + tr("GIT revision: %1").arg(GIT_REVISION_HASH).replace(":", "|") + " |";
    tmp += "\n| " + tr("Application path: %1").arg(qApp->applicationDirPath()).replace(":", "|") + " |";
    tmp += "\n| " + QString("Ini File Name: %2").arg(fileName()).replace(":", "|") + " |";
    tmp += "\n| " + tr("Using Ini File") + " | " + fileName() + " |";
    if (Utils::isRunningOnLinux()) {
        tmp.append("\n| " + tr("Running on Linux"));
        tmp += "| " + tr("uname returns: %1").arg(Utils::uname()).replace("\n"," ") + " |";
    }
    else if (Utils::isRunningOnMac()) {
        tmp.append("\n| " + tr("Running on MacOs"));
        tmp += "| " + tr("uname returns: %1").arg(Utils::uname()).replace("\n"," ") + " |";
    }
    else if (Utils::isRunningOnWin())
        tmp.append("| " + tr("Running on Windows"))+ " | |";

    tmp += "\n\n";

    // add paths
    tmp += "===== PATH =====\n\n";
    QMap<QString, QString> paths;
    paths.insert(tr("Binary"), path(ApplicationPath));
    paths.insert(tr("UserResourcesPath"), path(UserResourcesPath));
    paths.insert(tr("Read only Databases"), path(ReadOnlyDatabasesPath));
    paths.insert(tr("Writable databases"), path(ReadWriteDatabasesPath));
    paths.insert(tr("Bundle root path"), path(BundleRootPath));
    paths.insert(tr("Bundle resources path"), path(BundleResourcesPath));
    paths.insert(tr("Translations path"), path(TranslationsPath));
    paths.insert(tr("Qt Plugins path"), path(QtPlugInsPath));
    paths.insert(tr("Qt FrameWorks path"), path(QtFrameWorksPath));
    paths.insert(tr("FreeMedForms PlugIns path"), path(FMFPlugInsPath));
    paths.insert(tr("SmallPixmapPath"), path(SmallPixmapPath));
    paths.insert(tr("MediumPixmapPath"), path(MediumPixmapPath));
    paths.insert(tr("BigPixmapPath"), path(BigPixmapPath));
    paths.insert(tr("SystemTempPath"), path(SystemTempPath));
    paths.insert(tr("ApplicationTempPath"), path(ApplicationTempPath));
    paths.insert(tr("FormsPath"), path(CompleteFormsPath));
    paths.insert(tr("UserCompleteFormsPath"), path(UserCompleteFormsPath));
    paths.insert(tr("UserSubFormsPath"), path(UserSubFormsPath));
    paths.insert(tr("Default installed datapack path"), path(DataPackApplicationPath));
    paths.insert(tr("Datapack persistent temporary path"), path(DataPackPersistentTempPath));
    paths.insert(tr("Datapack installation path"), path(DataPackInstallPath));
    paths.insert(tr("Datapack Complete Forms installation path"), path(DataPackCompleteFormsInstallPath));
    paths.insert(tr("Datapack SubForms installation path"), path(DataPackSubFormsInstallPath));
    paths.insert(tr("DocumentationPath"), path(DocumentationPath));
    paths.insert(tr("WebSiteUrl"), path(WebSiteUrl));
    tmp += "^ Resource ^ Path ^\n";
    foreach(const QString &p, paths.keys())
        tmp += "| " + p + " | " + paths[p] + " |\n";

    // add all values of the inifile

    tmp += "===== USER INI VALUES =====\n\n";
    tmp += "^ Group ^ Name ^ Value ^\n";
    QStringList keys = m_UserSettings->childKeys();
    keys.sort();
    foreach(const QString &k, keys)
        tmp += QString("| RootKeys | %1 | <nowiki>%2</nowiki> |\n").arg(k, m_UserSettings->value(k).toString());

    QStringList groups = m_UserSettings->childGroups();
    groups.sort();
    foreach(const QString &g, groups) {
        keys.clear();
        m_UserSettings->beginGroup(g);
        keys = m_UserSettings->allKeys();
        keys.sort();
        QString groupName;
        foreach(const QString &k, keys) {
            if (groupName.isEmpty())
                groupName = g;
            tmp += QString("| %1 | %2 | <nowiki>%3</nowiki> |\n")
                    .arg(groupName)
                    .arg(k).arg(m_UserSettings->value(k).toString());
            groupName = ":::";
        }
        m_UserSettings->endGroup();
    }

    tmp += "\n\n";

    tmp += "===== NETWORK INI VALUES =====\n\n";
    tmp += "^ Name ^ Value ^\n";
    foreach(const QString &k, m_NetworkSettings->allKeys())
        tmp += QString("| %1 | <nowiki>%2</nowiki> |\n").arg(k, m_NetworkSettings->value(k).toString());

    return tmp;
}

/** Returns all server connection's params */
Utils::DatabaseConnector SettingsPrivate::databaseConnector() const
{
    return m_DbConnector;
}

/** Define all server connection's params. Params are automatically saved into the settings file. */
void SettingsPrivate::setDatabaseConnector(Utils::DatabaseConnector &dbConnector)
{
    m_DbConnector = dbConnector;
    m_DbConnector.setAbsPathToReadOnlySqliteDatabase(path(Core::ISettings::ReadOnlyDatabasesPath));
    m_DbConnector.setAbsPathToReadWriteSqliteDatabase(path(Core::ISettings::ReadWriteDatabasesPath));
    writeDatabaseConnector();
}

void SettingsPrivate::readDatabaseConnector()
{
    m_DbConnector.fromSettings(m_NetworkSettings->value(S_DATABASECONNECTOR).toString());
    m_DbConnector.setAbsPathToReadOnlySqliteDatabase(path(Core::ISettings::ReadOnlyDatabasesPath));
    m_DbConnector.setAbsPathToReadWriteSqliteDatabase(path(Core::ISettings::ReadWriteDatabasesPath));
}

void SettingsPrivate::writeDatabaseConnector()
{
    m_NetworkSettings->setValue(S_DATABASECONNECTOR, m_DbConnector.forSettings());
    m_NetworkSettings->sync();
}
