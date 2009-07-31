/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
  \class tkSettings
  \brief This class is a multiOS settings manager.
    When instanciating this class, the ini file is determined using member getIniFile().\n
    If command line contains '--config=path/to/ini/file.ini' this file is tested and used if possible.\n
    Else the better ini file is retreived (testing first into app bundle, then user home dir).

    m_ResourcesPath is protected and can be defined, retreive it using resourcesPath().\n
    m_DatabasePath is protected and can be defined, retreive it using databasePath().

    The debugging members are used by tkDebugDialog :\n
    getTreeWidget() returns a treeWidget containing all values of the QSettings\n
    toString() is idem but returns a QString formatted.

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
         `- drugs (dosages)                    `- drugs (dosages)


       ApplicationName.app                   Application-Version
       |- Contents                           |
          |- MacOs                           |
          |   `- applicationbinary           |- applicationbinary
          |                                  |
          |- Resources                       |- Resources                <-- ReadOnly Resources at least
          |   |                              |  |
          |   |- CONFIG.INI                  |  |- CONFIG.INI            <-- if user can write into bundle
          |   |                              |  |
          |   |- databases                   |  |- databases
          |   |  |- datas                    |  |  |- datas              <-- if user can write into bundle
          |   |  |- drugs                    |  |  |- drugs
          |   |  `- users                    |  |  `- users              <-- if user can write into bundle
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

  \ingroup toolkit
  \ingroup object_toolkit
*/

/**
  \enum Paths
        ResourcesPath = 0,
        ApplicationPath,
        BundleResourcesPath,
        ReadOnlyDatabasesPath,
        ReadWriteDatabasesPath,
        TranslationsPath,
        QtPlugInsPath,
        QtFrameWorksPath,
        FMFPlugInsPath,
        SmallPixmapPath,
        MediumPixmapPath,
        BigPixmapPath,
        SystemTempPath,
        ApplicationTempPath,
        FormsPath,
        SampleFormsPath,
        WebSiteUrl
*/

/*!
 \enum tkSettings::Paths
 Defines the availables paths to use with setPath() and path().\n
 Some paths are calculated when setting the ApplicationPath, BundleRootPath and the ResourcesPath.
*/

/*! \var tkSettings::Paths tkSettings::ResourcesPath
 * Defines the users' resources path. This path is readable and writable for the user.
*/

/*! \var tkSettings::Paths tkSettings::ApplicationPath
 * Defines the users' resources path. This path is readable and writable for the user.
*/

/*! \var tkSettings::Paths tkSettings::BundleRootPath
 * Defines the root path of the bundle. On MacOs, the path is the one where lies the \e Application.app. On other OS,
 * the path is the path where stands the \e Application path.
*/

/*! \var tkSettings::Paths tkSettings::BundleResourcesPath
 * Defines the users' resources path. This path is readable and writable for the user.
*/

/*! \var tkSettings::Paths tkSettings::ReadOnlyDatabasesPath
 * Defines the users' resources path. This path is readable and writable for the user.
*/

/*! \var tkSettings::Paths tkSettings::DocumentationPath
 * Defines the users' manual path.
*/


#include "tkSettings.h"

// include toolkit headers
#include <tkLog.h>
#include <tkGlobal.h>
#include <tkTheme.h>

// include Qt headers
#include <QApplication>
#include <QMainWindow>
#include <QTreeWidget>
#include <QFileInfo>
#include <QDir>

namespace tkSettingsPrivateConstants {

    static const char* const WEBSITE              = "http://www.ericmaeker.fr/FreeMedForms";

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
    static const char* const USERMANUAL_PATH      = "/doc/%1/html";

    // APPLICATIONS RESOURCES --> located next to the application binary
    static const char* const MAC_PLUGINSPATH      = "/../plugins";
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

using namespace tkSettingsPrivateConstants;

class tkSettingsPrivate
{
public:
    QHash< int, QString >   m_Enum_Path;
    bool                    m_FirstTime;
};

tkSettings* tkSettings::m_Instance = 0;

/** \brief Returns the single instance of tkSettings */
tkSettings* tkSettings::instance( QObject * parent, const QString & appName, const QString & fileName )
{
    if (!m_Instance) {
        m_Instance = new tkSettings(parent,appName,fileName);
    }
    return m_Instance;
}

/**
  \brief Protected Constructor. Use instance() to create a new instance of this class.
  All path are calculated by the constructor.\n
  Users' writable resources are located in the dir of the config.ini file.
*/
tkSettings::tkSettings( QObject * parent, const QString & appName, const QString & fileName )
        : QSettings( getIniFile( appName, fileName ) , QSettings::IniFormat, parent ), d( new tkSettingsPrivate() )
{
    setObjectName( "tkSettings" );
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

    setPath( ApplicationPath, qApp->applicationDirPath() );
    setPath( ApplicationTempPath, QDir::tempPath() );
    setPath( SystemTempPath, QDir::tempPath() );
    setPath( WebSiteUrl, WEBSITE );

    if (tkGlobal::isDebugCompilation()) {
        // DEBUG BUILD
        // transform defined path during compilation to relative path during execution...
        QDir fmfBin( QDir::cleanPath( FMF_BIN_PATH ) );
        QString res = fmfBin.relativeFilePath( QDir::cleanPath( FMF_GLOBAL_RESOURCES ) );
        res = QDir::cleanPath( qApp->applicationDirPath() + QDir::separator() + res );
        resourcesPath = res + "/";
        setPath( ResourcesPath, QFileInfo(QSettings::fileName()).absolutePath() );//QDir::homePath() + "/." + applicationName );//resourcesPath );

        if (tkGlobal::isRunningOnMac()) {
            setPath( BundleResourcesPath, resourcesPath );
        } else {
            setPath( BundleResourcesPath, resourcesPath );
        }
    } else {
        // RELEASE BUILD
        if (tkGlobal::isRunningOnMac()) {
//            resourcesPath = QFileInfo( QSettings::fileName() ).absolutePath();
            setPath( BundleResourcesPath, qApp->applicationDirPath() + "/.." );
        } else {
//            resourcesPath = QFileInfo( QSettings::fileName() ).absolutePath() + QDir::separator() + "Resources";
            setPath( BundleResourcesPath, qApp->applicationDirPath() );
        }
        d->m_FirstTime = value( "FirstTimeRunning", true ).toBool();
        setPath( ResourcesPath, QFileInfo(QSettings::fileName()).absolutePath() );//QDir::homePath() + "/." + applicationName );//resourcesPath );
    }

    if (parent)
        setParent(parent);
    else
        setParent(qApp);
}

/** \brief Destructor */
tkSettings::~tkSettings()
{
    if (d) delete d;
    d=0;
}

/**
  \brief defines a path \e absPath with the index \e type refering to the enumarator \e tkSettings::Paths.
  When setting ApplicationPath, some paths are automatically recalculated : BundleRootPath, QtFrameWorksPath, FMFPlugInsPath, QtPlugInsPath.\n
  When setting BundleResourcesPath, some paths are automatically recalculated : ReadOnlyDatabasesPath, TranslationsPath, SmallPixmapPath, MediumPixmapPath, BigPixmapPath, SampleFormsPath.\n
  When setting ResourcesPath, some paths are automatically recalculated : ReadWriteDatabasesPath.\n
  It also inform tkTheme (that is instanciate by tkGlobal::initLib()) of the default theme to use.
*/
void tkSettings::setPath( const int type, const QString & absPath )
{
    switch (type)
    {
        case WebSiteUrl :
        {
            d->m_Enum_Path.insert( type, absPath );
            break;
        }
        case ResourcesPath :
        {
            QString resourcesPath = QDir::cleanPath(absPath);
            d->m_Enum_Path.insert( ResourcesPath, resourcesPath );
            resourcesPath += "/databases";
            d->m_Enum_Path.insert( ReadWriteDatabasesPath, resourcesPath );
            if (!QDir(resourcesPath).exists())
                if (!QDir().mkpath(resourcesPath))
                    tkLog::addError("tkSettings", tkTr(_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(resourcesPath));
            break;
        }
        case BundleResourcesPath :
        {
            if (d->m_Enum_Path.value(BundleResourcesPath)==QDir::cleanPath(absPath))
                break;
            QString bundlePath = QDir::cleanPath(absPath) + BUNDLERESOURCE_PATH;
            d->m_Enum_Path.insert( BundleResourcesPath, bundlePath );
            d->m_Enum_Path.insert( ReadOnlyDatabasesPath, bundlePath + READONLYDATABASE );
            d->m_Enum_Path.insert( TranslationsPath, bundlePath + TRANSLATIONS_PATH );
            d->m_Enum_Path.insert( SmallPixmapPath, bundlePath + DEFAULTTHEME_PIXMAP + "/16x16/" );
            d->m_Enum_Path.insert( MediumPixmapPath, bundlePath + DEFAULTTHEME_PIXMAP + "/32x32/" );
            d->m_Enum_Path.insert( BigPixmapPath, bundlePath + DEFAULTTHEME_PIXMAP + "/64x64/" );
            d->m_Enum_Path.insert( SampleFormsPath, bundlePath + DEFAULTFORMS );
            if (qApp->applicationName().contains(" "))
                d->m_Enum_Path.insert( DocumentationPath, bundlePath + QString(USERMANUAL_PATH).arg(qApp->applicationName().left(qApp->applicationName().indexOf(" "))) );
            else
                d->m_Enum_Path.insert( DocumentationPath, bundlePath + QString(USERMANUAL_PATH).arg(qApp->applicationName()) );
            tkTheme::instance()->setThemeRootPath( bundlePath + DEFAULTTHEME_PATH );
            break;
        }
        case ApplicationPath :
        {
            if (d->m_Enum_Path.value(ApplicationPath)==QDir::cleanPath(absPath))
                break;
            d->m_Enum_Path.insert( ApplicationPath, absPath );
            if (tkGlobal::isRunningOnMac()) {
                d->m_Enum_Path.insert( QtFrameWorksPath, QDir::cleanPath(absPath + MAC_QTFRAMEWORKPATH) );
                d->m_Enum_Path.insert( FMFPlugInsPath, QDir::cleanPath(absPath + MAC_PLUGINSPATH) );
                d->m_Enum_Path.insert( QtPlugInsPath, QDir::cleanPath(absPath + MAC_PLUGINSPATH + ALL_QTPLUGINSPATH ) );
                d->m_Enum_Path.insert( BundleRootPath, QDir::cleanPath(absPath + MAC_TOBUNDLEROOTPATH ) );
            } else if (tkGlobal::isRunningOnLinux()) {
                d->m_Enum_Path.insert( QtFrameWorksPath, QDir::cleanPath(absPath + UNIX_QTFRAMEWORKPATH) );
                d->m_Enum_Path.insert( FMFPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH) );
                d->m_Enum_Path.insert( QtPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH + ALL_QTPLUGINSPATH ) );
                d->m_Enum_Path.insert( BundleRootPath, QDir::cleanPath(absPath + NUX_TOBUNDLEROOTPATH ) );
            } else if (tkGlobal::isRunningOnWin()) {
                d->m_Enum_Path.insert( QtFrameWorksPath, QDir::cleanPath(absPath + WIN_QTFRAMEWORKPATH) );
                d->m_Enum_Path.insert( FMFPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH) );
                d->m_Enum_Path.insert( QtPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH + ALL_QTPLUGINSPATH ) );
                d->m_Enum_Path.insert( BundleRootPath, QDir::cleanPath(absPath + WIN_TOBUNDLEROOTPATH ) );
            } else if (tkGlobal::isRunningOnFreebsd()) {
                d->m_Enum_Path.insert( QtFrameWorksPath, QDir::cleanPath(absPath + UNIX_QTFRAMEWORKPATH) );
                d->m_Enum_Path.insert( FMFPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH) );
                d->m_Enum_Path.insert( QtPlugInsPath, QDir::cleanPath(absPath + NONMAC_PLUGINSPATH + ALL_QTPLUGINSPATH ) );
                d->m_Enum_Path.insert( BundleRootPath, QDir::cleanPath(absPath + BSD_TOBUNDLEROOTPATH ) );
            }
            break;
        }
        default: d->m_Enum_Path.insert( type, QDir::cleanPath( absPath ) ); break;
    }
}

/** \brief Returns the path according to the enumerator tkSettings::Paths */
QString tkSettings::path( const int type ) const
{
    return d->m_Enum_Path.value( type );
}

/**
  \brief Return true if the application runs for the first time.
  \sa noMoreFirstTimeRunning()
*/
bool tkSettings::firstTimeRunning() const
{
    return value( "FirstTimeRunning", true ).toBool();
}

/**
  \brief Set the first time running of this application to false.
  \sa firstTimeRunning()
*/
void tkSettings::noMoreFirstTimeRunning()
{
    setValue( "FirstTimeRunning", false );
    d->m_FirstTime = false;
}

QString tkSettings::licenseApprovedApplicationNumber() const
{
    return value(LICENSE_VERSION).toString();
}

void tkSettings::setLicenseApprovedApplicationNumber(const QString &version)
{
    setValue(LICENSE_VERSION, version);
}

/**
  \brief Returns the ini file to use the the initialization of QSettings. See constructor.
  Test in this order :
  \li command line --config="/abs/path/to/config.ini" or --config="../relative/path/to/config.ini". If the ini file can be used it is returned.
  \li home path
*/
QString tkSettings::getIniFile( const QString & appName, const QString & fileName )
{
    // manage defaults --> ".AppName/config.ini"
    QString tmpAppName = appName;
    QString tmpFileName = fileName;
    if ( appName.isEmpty() ) {
        tmpAppName = qApp->applicationName();
        if (tmpAppName.contains(" "))
            tmpAppName = tmpAppName.left(tmpAppName.indexOf(" "));
    }
    tmpAppName.prepend( "." );

    if (fileName.isEmpty())
        tmpFileName = "config.ini";

    // if QApplication args contains "--config=iniFileName.ini" use it if possible
    // retreive command line arguments
    QStringList list = QCoreApplication::arguments();
    int index = list.indexOf( QRegExp( "--config=*", Qt::CaseSensitive, QRegExp::Wildcard ) );

    if ( index != -1 ) {
        QString iniFileName = list[ index ];
        iniFileName = iniFileName.mid( iniFileName.indexOf( "=" ) + 1 );
        tkLog::addMessage( "tkSettings", tr( "Passing command line ini file : %1" ).arg( iniFileName ) );

        if ( QDir::isRelativePath( iniFileName ) )
            iniFileName.prepend( qApp->applicationDirPath() + QDir::separator() );
        iniFileName = QDir::cleanPath( iniFileName );

        QFileInfo info( iniFileName );
        if ( info.exists() && info.isReadable() ) {
            if ( info.isWritable() ) {
                tkLog::addMessage( "tkSettings", tr( "Using ini file %1." ).arg( iniFileName) );
                return iniFileName;
            }
            else
                tkLog::addError( "tkSettings", tr( "Ini file %1 is not writable. Can not use it." ).arg( iniFileName ) ) ;
        } else {
            // can we create and access to ini file ?
            QFile file( iniFileName );
            if ( file.open( QIODevice::ReadWrite | QIODevice::Text ) ) {
                tkLog::addMessage( "tkSettings", tr( "Using ini file %1" ).arg( iniFileName ) );
                return iniFileName;
            }
            else
                tkLog::addMessage( "tkSettings", tr( "WARNING : Ini file %1 can not be used." ).arg( iniFileName) );
        }
    }

    // try to use 'config.ini' inside the bundle (ex : /usr/share/application/config.ini , appli.app/Resources/config.ini)
    QString iniFile;
//    if (tkGlobal::isRunningOnMac())
//        iniFile = QDir::cleanPath( qApp->applicationDirPath() + "/../Resources" ) + QDir::separator() + tmpFileName;
//    else
//        iniFile = qApp->applicationDirPath() + QDir::separator() + tmpFileName;
//
//    // test iniFile
//    tkLog::addMessage( "tkSettings", tr( "Trying ini file %1" ).arg( iniFile ) );
//    QFile file( iniFile );
//    QFileInfo info( iniFile );
//    if ( info.exists() && info.isReadable() && info.isWritable() ) {
//        tkLog::addMessage( "tkSettings", tr( "Using ini file %1" ).arg( iniFile ) );
//        return iniFile;
//    } else {
//        if ( ( ! info.exists() ) &&  file.open( QIODevice::ReadWrite | QIODevice::Text ) ) {
//            tkLog::addMessage( "tkSettings", tr( "Using ini file %1" ).arg( iniFile ) );
//            return iniFile;
//        }
//        else tkLog::addError( "tkSettings", tr( "Ini file %1 can not be used." ).arg( iniFile) );
//    }

    // Now use the $HOME path
    iniFile = QString( "%1/%2/%3" ).arg( QDir::homePath(), tmpAppName, tmpFileName);
    tkLog::addMessage( "tkSettings", tr( "Trying ini file %1" ).arg( iniFile ) );
    QDir dir( QFileInfo( iniFile ).absolutePath() );

    if (!dir.exists()) {
        dir.cdUp();
        if (!dir.mkdir(tmpAppName)) {
            tkLog::addError( "tkSettings" , tr( "Unable to create dir : %1, no Ini File can be used.").arg( dir.absolutePath() + QDir::separator() + tmpAppName ) );
            return QString::null;
        }
    }

    tkLog::addMessage( "tkSettings", tr( "Using ini file %1" ).arg( iniFile ) );
    return iniFile;
}

/** \brief Main windows restore state. \e prefix can be used if you store multiple main window in the same tkSettings */
void tkSettings::restoreState( QMainWindow * window, const QString & prefix )
{
    if ( !window )
        return;
    QString keyGeo = prefix + "MainWindow/Geometry";
    QString keyState = prefix + "MainWindow/State";
    if ( value( keyGeo ).toByteArray().isEmpty() ) {
        window->setGeometry( 100, 100, 600, 400 );
        tkGlobal::centerWidget( window );
    } else {
        window->restoreGeometry( value( keyGeo ).toByteArray() );
        window->restoreState( value( keyState ).toByteArray() );
    }
}

/** \brief Main windows save state. \e prefix can be used if you store multiple main window in the same tkSettings */
void tkSettings::saveState( QMainWindow * window, const QString & prefix )
{
    if ( !window )
        return;
    setValue( prefix + "MainWindow/Geometry", window->saveGeometry() );
    setValue( prefix + "MainWindow/State", window->saveState() );
}

/**
  \brief Append a string \e value to the stringlist represented by the \e key in settings assuming no doublon.
*/
void tkSettings::appendToValue( const QString &key, const QString &value )
{
    QVariant q = this->value(key);
    if (q.isNull()) {
        this->setValue( key, value );
        return;
    }
    if (q.toStringList().indexOf(value) == -1) {
        QStringList list = q.toStringList();
        list.append(value);
        this->setValue( key, list );
    }
}

/** \brief For debugging purpose. */
QTreeWidget* tkSettings::getTreeWidget( QWidget * parent ) const
{
    // manage parent as tree widget
    QTreeWidget * tree = qobject_cast<QTreeWidget*>(parent);
    if ( !tree )
        tree = new QTreeWidget( parent );

    // add columns
    tree->setColumnCount( 2 );

    // add system informations
    QTreeWidgetItem * sysItem = new QTreeWidgetItem( tree, QStringList() << tr( "System informations" ) );
    new QTreeWidgetItem( sysItem, QStringList() << tr("Operating System") << tkGlobal::osName() );
    new QTreeWidgetItem( sysItem, QStringList() << tr("uname output") << tkGlobal::uname() );

    // add compilation informations
    QTreeWidgetItem * compilItem = new QTreeWidgetItem( tree, QStringList() << tr( "Compilation informations" ) );
    new QTreeWidgetItem( compilItem, QStringList() << tr("Compilation Date") << QString("%1 - %2").arg( __DATE__, __TIME__));
    new QTreeWidgetItem( compilItem, QStringList() << tr("Compile Qt version") << QString("%1").arg( QT_VERSION_STR ));
    new QTreeWidgetItem( compilItem, QStringList() << tr("Actual Qt version") << QString("%1").arg( qVersion() ));
    new QTreeWidgetItem( compilItem, QStringList() << tr("Actual Application Version") << QString("%1").arg( qApp->applicationVersion() ));
    if (tkGlobal::isDebugCompilation())
        new QTreeWidgetItem( compilItem, QStringList() << tr("Actual build") << tr("Debug") );
    else
        new QTreeWidgetItem( compilItem, QStringList() << tr("Actual build") << tr("Release") );
    if (tkGlobal::isFullApplication())
        new QTreeWidgetItem( compilItem, QStringList() << tr("Actual build") << tr("Full Application Build") );
    else
        new QTreeWidgetItem( compilItem, QStringList() << tr("Actual build") << tr("Svn Build") );


    // add paths
    QDir appDir( qApp->applicationDirPath() );
    QMap<QString, QString> paths;
    paths.insert( tr("Binary"), path( ApplicationPath ) );
    paths.insert( tr("Resources"), path( ResourcesPath ) );
    paths.insert( tr("Read only Databases"), path( ReadOnlyDatabasesPath ) );
    paths.insert( tr("Writable databases"), path( ReadWriteDatabasesPath ) );
    paths.insert( tr("Bundle root path"), path( BundleResourcesPath ) );
    paths.insert( tr("Translations path"), path( TranslationsPath ) );
    paths.insert( tr("Qt Plugins path"), path( QtPlugInsPath ) );
    paths.insert( tr("Qt FrameWorks path"), path( QtFrameWorksPath ) );
    paths.insert( tr("FreeMedForms PlugIns path"), path( FMFPlugInsPath ) );
    paths.insert( tr("SmallPixmapPath"), path( SmallPixmapPath ) );
    paths.insert( tr("MediumPixmapPath"), path( MediumPixmapPath ) );
    paths.insert( tr("BigPixmapPath"), path( BigPixmapPath ) );
    paths.insert( tr("SystemTempPath"), path( SystemTempPath ) );
    paths.insert( tr("ApplicationTempPath"), path( ApplicationTempPath ) );
    paths.insert( tr("FormsPath"), path( FormsPath ) );
    paths.insert( tr("SampleFormsPath"), path( SampleFormsPath ) );
    paths.insert( tr("DocumentationPath"), path( DocumentationPath ) );

    QTreeWidgetItem * absPathsItem = new QTreeWidgetItem( tree, QStringList() << tr( "Absolute Paths" ) );
    new QTreeWidgetItem( absPathsItem, QStringList() << tr( "Using Ini File" ) << fileName() );

    QTreeWidgetItem * relPathsItem = new QTreeWidgetItem( tree, QStringList() << tr( "Relative Paths" ) );
    new QTreeWidgetItem( relPathsItem, QStringList() << tr( "Using Ini File" ) << appDir.relativeFilePath( QFileInfo( fileName() ).absoluteFilePath() ) );

    foreach( const QString & p, paths.keys() )
    {
        new QTreeWidgetItem( relPathsItem, QStringList() << p << appDir.relativeFilePath( QFileInfo( paths[p] ).absoluteFilePath() ) );
        new QTreeWidgetItem( absPathsItem, QStringList() << p << paths[p] );
    }
    new QTreeWidgetItem( relPathsItem, QStringList() << tr("WebSiteUrl") << path( WebSiteUrl ) );
    new QTreeWidgetItem( absPathsItem, QStringList() << tr("WebSiteUrl") << path( WebSiteUrl ) );

    //add library informations
    new QTreeWidgetItem( tree, QStringList() << tr( "Libs" ) << tkGlobal::getLibraryInformations() );

    // add settings
    QTreeWidgetItem * orphan = new QTreeWidgetItem( tree, QStringList() << tr( "Orphan settings" ) );
    QTreeWidgetItem * group = 0;
    QStringList list = allKeys();
    qSort( list );
    foreach( const QString & k, list ) {
        if ( k.contains( "/" ) ) {
            QString tmp = k.left( k.indexOf( "/" ) );
            if ( ! group )
                group = new QTreeWidgetItem( tree, QStringList() << tmp );
            if ( ! k.startsWith( group->text( 0 ) ) )
                group = new QTreeWidgetItem( tree, QStringList() << tmp );
            new QTreeWidgetItem( group, QStringList() << k.mid( k.indexOf( "/" ) + 1 )  << value( k ).toString() );
        }
        else
            new QTreeWidgetItem( orphan, QStringList() << k << value( k ).toString() );
    }

    // resize columns
    tree->resizeColumnToContents( 0 );
    tree->resizeColumnToContents( 1 );

    return tree;
}

/** \brief For debugging purpose. */
QString tkSettings::toString() const
{
    /** \todo add uname output if running on linux */
    QString tmp = "\n\n";
    tmp += "********************\n";
    tmp += "**    SETTINGS    **\n";
    tmp += "********************\n\n";

    // add building informations
    tmp += tr( "Running version : %1\n" ).arg( qApp->applicationName() );
    tmp += tr( "Build date : %1 %2\n").arg( __DATE__, __TIME__);
    tmp += tr( "Qt Build version : %1\n").arg( QT_VERSION_STR );
    tmp += tr( "Qt running version : %1\n").arg( qVersion() );
    tmp += tr( "Application Version : %1\n").arg( qApp->applicationVersion() );
    if (tkGlobal::isDebugCompilation())
        tmp += tr("Actual build : Debug\n" );
    else
        tmp += tr("Actual build : Release\n" );
    if (tkGlobal::isFullApplication())
        tmp += tr("Actual build") + " : " + tr("Full Application Build\n");
    else
        tmp += tr("Actual build") + " : " + tr("Svn Build\n");
    tmp += tr( "Application path : %1\n").arg( qApp->applicationDirPath() );
    tmp += QString( "Ini File Name\t%2" ).arg( fileName() ) + "\n";
    tmp += tr( "Using Ini File" ) + "\t" + fileName() + "\n";
    if (tkGlobal::isRunningOnLinux()) {
        tmp.append( tr("Running on Linux" ) );
        tmp += tr("   uname returns : %1").arg(tkGlobal::uname());
    }
    else if (tkGlobal::isRunningOnMac())
        tmp.append( tr("Running on MacOs" ) );
    else if (tkGlobal::isRunningOnWin())
        tmp.append( tr("Running on Windows" ) );

    tmp += "\n\n";

    // add paths
    QMap<QString, QString> paths;
    paths.insert( tr("Binary"), path( ApplicationPath ) );
    paths.insert( tr("Resources"), path( ResourcesPath ) );
    paths.insert( tr("Read only Databases"), path( ReadOnlyDatabasesPath ) );
    paths.insert( tr("Writable databases"), path( ReadWriteDatabasesPath ) );
    paths.insert( tr("Bundle root path"), path( BundleResourcesPath ) );
    paths.insert( tr("Translations path"), path( TranslationsPath ) );
    paths.insert( tr("Qt Plugins path"), path( QtPlugInsPath ) );
    paths.insert( tr("Qt FrameWorks path"), path( QtFrameWorksPath ) );
    paths.insert( tr("FreeMedForms PlugIns path"), path( FMFPlugInsPath ) );
    paths.insert( tr("SmallPixmapPath"), path( SmallPixmapPath ) );
    paths.insert( tr("MediumPixmapPath"), path( MediumPixmapPath ) );
    paths.insert( tr("BigPixmapPath"), path( BigPixmapPath ) );
    paths.insert( tr("SystemTempPath"), path( SystemTempPath ) );
    paths.insert( tr("ApplicationTempPath"), path( ApplicationTempPath ) );
    paths.insert( tr("FormsPath"), path( FormsPath ) );
    paths.insert( tr("SampleFormsPath"), path( SampleFormsPath ) );
    paths.insert( tr("DocumentationPath"), path( DocumentationPath ) );
    paths.insert( tr("WebSiteUrl"), path( WebSiteUrl ) );
    foreach( const QString & p, paths.keys() )
        tmp += p + "\t" + paths[p] + "\n";

    // add all values of the inifile
    foreach( QString k, allKeys() )
        tmp += QString( "%1\t%2\n" ).arg( k, value( k ).toString() );
    tmp += "\n\n";

    return tmp;
}

