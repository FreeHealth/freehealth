/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
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

    m_ResourcesPath is protected and can be defined, retreive it using resourcesPath().\n
    m_DatabasePath is protected and can be defined, retreive it using databasePath().

    The debugging members are used by Core::DebugDialog :
        - getTreeWidget() returns a treeWidget containing all values of the QSettings\n
        - toString() is idem but returns a QString formatted.

    You can store extra-datas such as webSiteUrl().

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
        |- config.ini                         |- config.ini
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
 Core::ISettings::ResourcesPath.
*/

/*! \var Core::ISettings::Paths Core::ISettings::ResourcesPath
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
     - Core::ISettings::SampleFormsPath
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

/*! \var Core::ISettings::Paths Core::ISettings::FormsPath
 * Defines the users' forms path (FreeMedForms only).
*/

/*! \var Core::ISettings::Paths Core::ISettings::SampleFormsPath
 * Defines the Bundled forms (used only with FreeMedForms).
*/

/*! \var Core::ISettings::Paths Core::ISettings::DocumentationPath
 * Defines the users' manual path.
 * When the application is build with the LINUX_INTEGRATED config flag, the DocumentationPath is setted to <em>/usr/share/doc/ApplicationName-doc/html/</em>.
 * Otherwise it is setted inside the Bundle.
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

// include Qt headers
#include <QApplication>
#include <QMainWindow>
#include <QDockWidget>
#include <QTreeWidget>
#include <QFileInfo>
#include <QDir>
#include <QRegExp>


/**
  \todo Manage user's settings stored into database
*/

namespace SettingsPrivateConstants {

    static const char* const WEBSITE              = "http://www.freemedforms.org/";

    // BUNDLE RESOURCES  --> located inside the bundle. Location calculated from BundleRootPath
#ifdef DEBUG
    static const char* const BUNDLERESOURCE_PATH   = "";                    // resources are located into global_resources paths
#else
    static const char* const BUNDLERESOURCE_PATH  = "/Resources";          // resources are located inside the bundle
#endif
    static const char* const READONLYRESOURCES    = "";
    static const char* const READONLYDATABASE     = "/databases";
    static const char* const TRANSLATIONS_PATH    = "/translations";
    static const char* const DEFAULTFORMS         = "/forms";
    static const char* const DEFAULTTHEME_PATH    = "";
    static const char* const DEFAULTTHEME_PIXMAP  = "/pixmap";
    static const char* const DEFAULTTHEME_SPLASH  = "/pixmap/splashscreens";
    static const char* const USERMANUAL_PATH      = "/doc/%1";
    static const char* const LINUX_USERMANUAL_PATH  = "/usr/share/doc/%1-doc/html";

    // APPLICATIONS RESOURCES --> located next to the application binary
#ifdef DEBUG
    static const char* const MAC_PLUGINSPATH      = "/../../../plugins";
#else
    static const char* const MAC_PLUGINSPATH      = "/../plugins";
#endif
    static const char* const NONMAC_PLUGINSPATH   = "/plugins";
    static const char* const ALL_QTPLUGINSPATH    = "/qt";
    static const char* const MAC_QTFRAMEWORKPATH  = "/../FrameWorks";
    static const char* const WIN_QTFRAMEWORKPATH  = "";
    static const char* const UNIX_QTFRAMEWORKPATH = "/../libs";
    static const char* const MAC_TOBUNDLEROOTPATH = "/../../..";
    static const char* const WIN_TOBUNDLEROOTPATH = "/..";
    static const char* const NUX_TOBUNDLEROOTPATH = "/..";
    static const char* const BSD_TOBUNDLEROOTPATH = "/..";

    // USER WRITABLE RESOURCES  --> located inside/outside the bundle. Location calculated from ResourcesPath (where stands the ini file)
    static const char* const WRITABLEDATABASE     = "/databases";

    // SETTINGS
    static const char* const LICENSE_VERSION      = "License/AcceptedVersion";

}

using namespace Core;
using namespace Core::Internal;
using namespace SettingsPrivateConstants;


/**
  \brief Protected Constructor. Use instance() to create a new instance of this class.
  All path are calculated by the constructor.\n
  Users' writable resources are located in the dir of the config.ini file.
*/
SettingsPrivate::SettingsPrivate(QObject *parent, const QString &appName, const QString &fileName)
        : ISettings(), QSettings(getIniFile(appName, fileName), QSettings::IniFormat,parent)
{
    setObjectName("SettingsPrivate");
    QString resourcesPath;
    QString databasePath;
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
    setPath(ApplicationTempPath, QDir::tempPath());
    setPath(SystemTempPath, QDir::tempPath());
    setPath(WebSiteUrl, WEBSITE);
//    if (Utils::isRunningOnLinux())
//        setPath(FMFPluginsPath, LIBRARY_BASENAME);

    if (Utils::isDebugCompilation()) {
        // DEBUG BUILD
        QString res;
        if (Utils::isRunningOnMac())
            res = qApp->applicationDirPath() + "/../../../../global_resources";
        else
            res = qApp->applicationDirPath() + "/../global_resources";

        res = QDir::cleanPath(res);
        resourcesPath = res + "/";
        setPath(ResourcesPath, QFileInfo(QSettings::fileName()).absolutePath());

        if (Utils::isRunningOnMac()) {
            setPath(BundleResourcesPath, resourcesPath);
        } else {
            setPath(BundleResourcesPath, resourcesPath);
        }
    } else {
        // RELEASE BUILD
#ifdef LINUX_INTEGRATED
        setPath(BundleResourcesPath, QString("/usr/share/%1").arg(qApp->applicationName()));
#else
        if (Utils::isRunningOnMac()) {
            setPath(BundleResourcesPath, qApp->applicationDirPath() + "/../" + QString(BUNDLERESOURCE_PATH));
        } else {
            setPath(BundleResourcesPath, qApp->applicationDirPath() + QDir::separator() + QString(BUNDLERESOURCE_PATH));
        }
#endif
        m_FirstTime = value("FirstTimeRunning", true).toBool();
        setPath(ResourcesPath, QFileInfo(QSettings::fileName()).absolutePath());//QDir::homePath() + "/." + applicationName);//resourcesPath);
    }

    if (parent)
        setParent(parent);
    else
        setParent(qApp);
}

SettingsPrivate::~SettingsPrivate()
{}

/**
  \fn QSettings *Core::ISettings::getQSettings()
  Using this member should be avoid in your code.
*/
QSettings *SettingsPrivate::getQSettings()
{
    return this;
}

/**
  \fn void Core::ISettings::setPath(const int type, const QString & absPath)
  \brief defines a path \e absPath with the index \e type refering to the enumarator \e Settings::Paths.
  When setting ApplicationPath, some paths are automatically recalculated : BundleRootPath, QtFrameWorksPath, FMFPlugInsPath, QtPlugInsPath.\n
  When setting BundleResourcesPath, some paths are automatically recalculated : ReadOnlyDatabasesPath, TranslationsPath, SmallPixmapPath, MediumPixmapPath, BigPixmapPath, SampleFormsPath.\n
  When setting ResourcesPath, some paths are automatically recalculated : ReadWriteDatabasesPath.\n
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
        case ResourcesPath :
        {
            QString resourcesPath = QDir::cleanPath(absPath);
            m_Enum_Path.insert(ResourcesPath, resourcesPath);
            resourcesPath += "/databases";
            m_Enum_Path.insert(ReadWriteDatabasesPath, resourcesPath);
            if (!QDir(resourcesPath).exists())
                if (!QDir().mkpath(resourcesPath))
                    Utils::Log::addError("Settings", Trans::ConstantTranslations::tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(resourcesPath));
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
            m_Enum_Path.insert(SampleFormsPath, bundlePath + DEFAULTFORMS);
            QString appname = qApp->applicationName().toLower();
            if (qApp->applicationName().contains(" ")) {
                appname = appname.left(appname.indexOf(" "));
            }
            if (Utils::isDebugCompilation()) {
                if (appname.contains("_d"))
                    appname = appname.left(appname.indexOf("_d"));
            }
#ifdef LINUX_INTEGRATED
            if (QDir(LINUX_USERMANUAL_PATH).exists()) {
                m_Enum_Path.insert(DocumentationPath, QString(LINUX_USERMANUAL_PATH).arg(appname));
            } else {
                m_Enum_Path.insert(DocumentationPath, bundlePath + QString(USERMANUAL_PATH).arg(appname));
            }
#else
            m_Enum_Path.insert(DocumentationPath, bundlePath + QString(USERMANUAL_PATH).arg(appname));
#endif
            break;
        }
        case ApplicationPath :
        {
            if (m_Enum_Path.value(ApplicationPath)==QDir::cleanPath(absPath))
                break;
            m_Enum_Path.insert(ApplicationPath, absPath);
            if (Utils::isRunningOnMac()) {
                m_Enum_Path.insert(QtFrameWorksPath, QDir::cleanPath(absPath + MAC_QTFRAMEWORKPATH));
                m_Enum_Path.insert(FMFPlugInsPath, QDir::cleanPath(absPath + MAC_PLUGINSPATH));
                m_Enum_Path.insert(QtPlugInsPath, QDir::cleanPath(absPath + MAC_PLUGINSPATH + ALL_QTPLUGINSPATH));
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
  \fn bool Core::ISettings::firstTimeRunning() const
  \brief Return true if the application runs for the first time.
  \sa noMoreFirstTimeRunning()
*/
bool SettingsPrivate::firstTimeRunning() const
{
    return value("FirstTimeRunning", true).toBool();
}

/**
  \fn void Core::ISettings::noMoreFirstTimeRunning()
  \brief Set the first time running of this application to false.
  \sa firstTimeRunning()
*/
void SettingsPrivate::noMoreFirstTimeRunning()
{
    setValue("FirstTimeRunning", false);
    m_FirstTime = false;
}

/**
  \fn QString Core::ISettings::licenseApprovedApplicationNumber() const
*/
QString SettingsPrivate::licenseApprovedApplicationNumber() const
{
    return value(LICENSE_VERSION).toString();
}

/**
  \fn void Core::ISettings::setLicenseApprovedApplicationNumber(const QString &version)
*/
void SettingsPrivate::setLicenseApprovedApplicationNumber(const QString &version)
{
    setValue(LICENSE_VERSION, version);
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

    if (fileName.isEmpty())
        tmpFileName = "config.ini";

    // if QApplication args contains "--config=iniFileName.ini" use it if possible
    // retreive command line arguments
    /** \todo get the command line args through the Core::ICommandLine */
    QStringList list = QCoreApplication::arguments();
    int index = list.indexOf(QRegExp("--config=*", Qt::CaseSensitive, QRegExp::Wildcard));

    if (index != -1) {
        QString iniFileName = list[index];
        iniFileName = iniFileName.mid(iniFileName.indexOf("=") + 1);
        Utils::Log::addMessage("Settings", tr("Passing command line ini file : %1").arg(iniFileName));

        if (QDir::isRelativePath(iniFileName))
            iniFileName.prepend(qApp->applicationDirPath() + QDir::separator());
        iniFileName = QDir::cleanPath(iniFileName);

        QFileInfo info(iniFileName);
        if (info.exists() && info.isReadable()) {
            if (info.isWritable()) {
                Utils::Log::addMessage("Settings", tr("Using ini file %1.").arg(iniFileName));
                return iniFileName;
            }
            else
                Utils::Log::addError("Settings", tr("Ini file %1 is not writable. Can not use it.").arg(iniFileName)) ;
        } else {
            // can we create and access to ini file ?
            QFile file(iniFileName);
            if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
                Utils::Log::addMessage("Settings", tr("Using ini file %1").arg(iniFileName));
                return iniFileName;
            }
            else
                Utils::Log::addMessage("Settings", tr("WARNING : Ini file %1 can not be used.").arg(iniFileName));
        }
    }

    // try to use read the 'pathtoconfig.ini' inside the bundle
    if (QFile(QString("%1/pathtoconfig.ini").arg(qApp->applicationDirPath())).exists()) {
        QString content = Utils::readTextFile(QString("%1/pathtoconfig.ini").arg(qApp->applicationDirPath()), Utils::DontWarnUser);
        content = content.trimmed();
        if (!content.isEmpty()) {
            if (QDir::isRelativePath(content))
                content.prepend(qApp->applicationDirPath() + QDir::separator());
            content = QDir::cleanPath(content);

            QFileInfo info(content);
            if (info.exists() && info.isReadable()) {
                if (info.isWritable()) {
                    Utils::Log::addMessage("Settings", tr("Using ini file %1.").arg(content));
                    return content;
                }
                else
                    Utils::Log::addError("Settings", tr("Ini file %1 is not writable. Can not use it.").arg(content)) ;
            } else {
                // can we create and access to ini file ?
                QFile file(content);
                if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
                    Utils::Log::addMessage("Settings", tr("Using ini file %1").arg(content));
                    return content;
                }
                else
                    Utils::Log::addMessage("Settings", tr("WARNING : Ini file %1 can not be used.").arg(content));
            }
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
    Utils::Log::addMessage("Settings", tr("Trying ini file %1").arg(iniFile));
    QDir dir(QFileInfo(iniFile).absolutePath());

    if (!dir.exists()) {
        dir.cdUp();
        if (!dir.mkdir(tmpAppName)) {
            Utils::Log::addError("Settings" , tr("Unable to create dir : %1, no Ini File can be used.").arg(dir.absolutePath() + QDir::separator() + tmpAppName));
            return QString::null;
        }
    }

    Utils::Log::addMessage("Settings", tr("Using ini file %1").arg(iniFile));
    return iniFile;
}

/**
  \fn void Core::ISettings::restoreState(QMainWindow * window, const QString & prefix)
  \brief Main windows restore state. \e prefix can be used if you store multiple main window in the same settings
*/
void SettingsPrivate::restoreState(QMainWindow * window, const QString & prefix)
{
    if (!window)
        return;
    QString keyGeo = prefix + "MainWindow/Geometry";
    QString keyState = prefix + "MainWindow/State";
    if (value(keyGeo).toByteArray().isEmpty()) {
        window->setGeometry(100, 100, 600, 400);
        Utils::centerWidget(window);
    } else {
        window->restoreGeometry(value(keyGeo).toByteArray());
        window->restoreState(value(keyState).toByteArray());
        // get all settings key starting with prefix+"/Dock"
        QStringList k = this->allKeys().filter(QRegExp(QString(prefix + "Dock/"), Qt::CaseSensitive, QRegExp::Wildcard));
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
  If \e parent is a QTreeWidget, it will be populated with datas and returned. Otherwise a
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
    QFont bold;
    bold.setBold(true);

    // add system informations
    QTreeWidgetItem * sysItem = new QTreeWidgetItem(tree, QStringList() << tr("System informations"));
    sysItem->setFont(0,bold);
    new QTreeWidgetItem(sysItem, QStringList() << tr("Operating System") << Utils::osName());
#ifndef Q_OS_WIN32
    new QTreeWidgetItem(sysItem, QStringList() << tr("uname output") << Utils::uname());
#endif

    // add compilation informations
    QTreeWidgetItem * compilItem = new QTreeWidgetItem(tree, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_INFORMATIONS));
    compilItem->setFont(0,bold);
    new QTreeWidgetItem(compilItem, QStringList() << tr("Compilation Date") << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DATE_TIME_1_2).arg(__DATE__, __TIME__));
    new QTreeWidgetItem(compilItem, QStringList() << tr("Compile Qt version") << QString("%1").arg(QT_VERSION_STR));
    new QTreeWidgetItem(compilItem, QStringList() << tr("Actual Qt version") << QString("%1").arg(qVersion()));
    new QTreeWidgetItem(compilItem, QStringList() << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_VERSION_1).arg("") << qApp->applicationVersion());
    if (Utils::isDebugCompilation())
        new QTreeWidgetItem(compilItem, QStringList() << tr("Compile mode") << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_DEBUG));
    else
        new QTreeWidgetItem(compilItem, QStringList() << tr("Compile mode") << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_RELEASE));
    if (Utils::isFullApplication())
        new QTreeWidgetItem(compilItem, QStringList() << tr("Application source") << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_FULLAPP));
    else
        new QTreeWidgetItem(compilItem, QStringList() << tr("Application source") << Trans::ConstantTranslations::tkTr(Trans::Constants::BUILD_SVNAPP));


    // add paths
    QDir appDir(qApp->applicationDirPath());
    QMap<QString, QString> paths;
    paths.insert(tr("Binary"), path(ApplicationPath));
    paths.insert(tr("Resources"), path(ResourcesPath));
    paths.insert(tr("Read only Databases"), path(ReadOnlyDatabasesPath));
    paths.insert(tr("Writable databases"), path(ReadWriteDatabasesPath));
    paths.insert(tr("Bundle root path"), path(BundleResourcesPath));
    paths.insert(tr("Translations path"), path(TranslationsPath));
    paths.insert(tr("Qt Plugins path"), path(QtPlugInsPath));
    paths.insert(tr("Qt FrameWorks path"), path(QtFrameWorksPath));
    paths.insert(tr("FreeMedForms PlugIns path"), path(FMFPlugInsPath));
    paths.insert(tr("SmallPixmapPath"), path(SmallPixmapPath));
    paths.insert(tr("MediumPixmapPath"), path(MediumPixmapPath));
    paths.insert(tr("BigPixmapPath"), path(BigPixmapPath));
    paths.insert(tr("SystemTempPath"), path(SystemTempPath));
    paths.insert(tr("ApplicationTempPath"), path(ApplicationTempPath));
    paths.insert(tr("FormsPath"), path(FormsPath));
    paths.insert(tr("SampleFormsPath"), path(SampleFormsPath));
    paths.insert(tr("DocumentationPath"), path(DocumentationPath));

    QTreeWidgetItem * absPathsItem = new QTreeWidgetItem(tree, QStringList() << tr("Absolute Paths"));
    absPathsItem->setFont(0,bold);
    new QTreeWidgetItem(absPathsItem, QStringList() << tr("Using Ini File") << fileName());

    QTreeWidgetItem * relPathsItem = new QTreeWidgetItem(tree, QStringList() << tr("Relative Paths"));
    relPathsItem->setFont(0,bold);
    new QTreeWidgetItem(relPathsItem, QStringList() << tr("Using Ini File") << appDir.relativeFilePath(QFileInfo(fileName()).absoluteFilePath()));

    foreach(const QString & p, paths.keys())
    {
        new QTreeWidgetItem(relPathsItem, QStringList() << p << appDir.relativeFilePath(QFileInfo(paths[p]).absoluteFilePath()));
        new QTreeWidgetItem(absPathsItem, QStringList() << p << paths[p]);
    }
    new QTreeWidgetItem(relPathsItem, QStringList() << tr("WebSiteUrl") << path(WebSiteUrl));
    new QTreeWidgetItem(absPathsItem, QStringList() << tr("WebSiteUrl") << path(WebSiteUrl));

    //add library informations
//    new QTreeWidgetItem(tree, QStringList() << tr("Libs") << Utils::getLibraryInformations());

    // add settings
    QTreeWidgetItem * settingsItem = new QTreeWidgetItem(tree, QStringList() << tr("Settings values"));
    settingsItem->setFont(0,bold);
    settingsItem->setExpanded(true);
    QTreeWidgetItem * orphan = new QTreeWidgetItem(settingsItem, QStringList() << tr("Orphan settings"));
    orphan->setFont(0,bold);
    QTreeWidgetItem * group = 0;
    QStringList list = allKeys();
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

/**
 \fn QString Core::ISettings::toString() const
 \brief For debugging purpose.
*/
QString SettingsPrivate::toString() const
{
    QString tmp = "\n\n";
    tmp += "********************\n";
    tmp += "**    SETTINGS    **\n";
    tmp += "********************\n\n";

    // add building informations
    tmp += tr("Running version : %1\n").arg(qApp->applicationName());
    tmp += tr("Build date : %1 %2\n").arg(__DATE__, __TIME__);
    tmp += tr("Qt Build version : %1\n").arg(QT_VERSION_STR);
    tmp += tr("Qt running version : %1\n").arg(qVersion());
    tmp += tr("Application Version : %1\n").arg(qApp->applicationVersion());
    if (Utils::isDebugCompilation())
        tmp += tr("Actual build : Debug\n");
    else
        tmp += tr("Actual build : Release\n");
    if (Utils::isFullApplication())
        tmp += tr("Actual build") + " : " + tr("Full Application Build\n");
    else
        tmp += tr("Actual build") + " : " + tr("Svn Build\n");
    tmp += tr("Application path : %1\n").arg(qApp->applicationDirPath());
    tmp += QString("Ini File Name\t%2").arg(fileName()) + "\n";
    tmp += tr("Using Ini File") + "\t" + fileName() + "\n";
    if (Utils::isRunningOnLinux()) {
        tmp.append(tr("Running on Linux"));
        tmp += tr("   uname returns : %1").arg(Utils::uname());
    }
    else if (Utils::isRunningOnMac()) {
        tmp.append(tr("Running on MacOs"));
        tmp += tr("   uname returns : %1").arg(Utils::uname());
    }
    else if (Utils::isRunningOnWin())
        tmp.append(tr("Running on Windows"));

    tmp += "\n\n";

    // add paths
    QMap<QString, QString> paths;
    paths.insert(tr("Binary"), path(ApplicationPath));
    paths.insert(tr("Resources"), path(ResourcesPath));
    paths.insert(tr("Read only Databases"), path(ReadOnlyDatabasesPath));
    paths.insert(tr("Writable databases"), path(ReadWriteDatabasesPath));
    paths.insert(tr("Bundle root path"), path(BundleResourcesPath));
    paths.insert(tr("Translations path"), path(TranslationsPath));
    paths.insert(tr("Qt Plugins path"), path(QtPlugInsPath));
    paths.insert(tr("Qt FrameWorks path"), path(QtFrameWorksPath));
    paths.insert(tr("FreeMedForms PlugIns path"), path(FMFPlugInsPath));
    paths.insert(tr("SmallPixmapPath"), path(SmallPixmapPath));
    paths.insert(tr("MediumPixmapPath"), path(MediumPixmapPath));
    paths.insert(tr("BigPixmapPath"), path(BigPixmapPath));
    paths.insert(tr("SystemTempPath"), path(SystemTempPath));
    paths.insert(tr("ApplicationTempPath"), path(ApplicationTempPath));
    paths.insert(tr("FormsPath"), path(FormsPath));
    paths.insert(tr("SampleFormsPath"), path(SampleFormsPath));
    paths.insert(tr("DocumentationPath"), path(DocumentationPath));
    paths.insert(tr("WebSiteUrl"), path(WebSiteUrl));
    foreach(const QString & p, paths.keys())
        tmp += p + "\t" + paths[p] + "\n";

    // add all values of the inifile
    foreach(QString k, allKeys())
        tmp += QString("%1\t%2\n").arg(k, value(k).toString());
    tmp += "\n\n";

    return tmp;
}

