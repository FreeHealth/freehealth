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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include <QtPlugin>
#include <QApplication>
#include <QTextCodec>
#include <QDir>

#include <QDebug>

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <extensionsystem/iplugin.h>

#include <utils/log.h>

#include <iostream>

enum { WarnAllPluginSpecs=false };

typedef QList<ExtensionSystem::PluginSpec *> PluginSpecSet;
static const char* COREPLUGINSNAME = "Core";
static const char* USERPLUGINSNAME = "UserManager";

static const QString HELP_MESSAGE =
        QString("FreeMedForms %1 (%2%3 compilation)\n"
                "Usage: freemedforms [--clear-user-databases] [--create-virtuals] [--config=...]\n"
                "                    [--version,-version,-v] [--help,-help,-h]\n\n"
                "FreeMedForms is free and open source an Electronic Medical Record manager.\n\n"
                "Options:\n"
                "  -h, -help, --help:         show this message\n"
                "  -v, -version, --version:   show the version\n"
                "  --config=/path/to/config.ini, --config=../../relative/path/config.ini\n"
                "                             Define the configuration file to use.\n"
                "                             In the case the specified file is found, in debug\n"
                "                             mode, the application resources will be located\n"
                "                             next to it (pixmaps, forms, user databases...)\n"
                "  --clear-user-databases:    only available with debug compilations,\n"
                "                             removes all user databases and recreate them. All\n"
                "                             data will be lost.\n"
                "  --create-virtuals:         create virtual data to ease tests only available\n"
                "                             with debug compilation.\n"
                "  --wine                     obsolete, ensure compatibility with wine\n"
                )
        .arg(PACKAGE_VERSION)
#ifdef LINUX_INTEGRATED
#  ifdef DEBUG
        .arg("Debug (Linux Integrated)")
#  else
        .arg("Release (Linux Integrated)")
#  endif
#else  // NOT LINUX_INTEGRATED
#  ifdef DEBUG
        .arg("Debug")
#  else
        .arg("Release")
#  endif
#endif  // LINUX_INTEGRATED

#ifdef DEBUG_WITHOUT_INSTALL
        .arg("-debug_without_install")
#else
        .arg("")
#endif

    ;
static inline QString getPluginPaths()
{
    QString app = qApp->applicationDirPath();

#ifdef DEBUG_WITHOUT_INSTALL
#    ifdef Q_OS_MAC
        app = QDir::cleanPath(app+"/../../../");
#    endif
    app += "/plugins/";
    return app;
#endif

#ifdef LINUX_INTEGRATED
    app = QString(BINARY_NAME).remove("_debug").toLower();
    return QString("/usr/%1/%2").arg(LIBRARY_BASENAME).arg(app);
#endif

#  ifdef Q_OS_MAC
    app = QDir::cleanPath(app+"/../plugins/");
    return app;
#  endif

// TODO: Add FreeBSD pluginPath

#  ifdef Q_OS_WIN
    app = QDir::cleanPath(app + "/plugins/");
    return app;
#  endif

    return QDir::cleanPath(app + "/plugins/");
}

static inline void defineLibraryPaths()
{
#ifdef LINUX_INTEGRATED
    qApp->addLibraryPath(getPluginPaths());
#else
#  ifndef DEBUG_WITHOUT_INSTALL
    qApp->setLibraryPaths(QStringList() << getPluginPaths() << QDir::cleanPath(getPluginPaths() + "/qt"));
#  endif
#endif
}

#include <QDebug>
#include <QLocale>
int main(int argc, char *argv[])
{
     QApplication app(argc, argv);

#if QT_VERSION < 0x050000
     // Removed in Qt5
     QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
     QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

     app.setApplicationName(BINARY_NAME);
     app.setOrganizationName(BINARY_NAME);
     app.setApplicationVersion(PACKAGE_VERSION);

     QStringList args = qApp->arguments();
     if (args.contains("--version") ||
         args.contains("-version") ||
         args.contains("-v")) {
         std::cout << qPrintable(VERSION_MESSAGE);
         return 0;
     }

     if (args.contains("--help") ||
         args.contains("-help") ||
         args.contains("-h")) {
         std::cout << qPrintable(HELP_MESSAGE);
         return 0;
     }

    ExtensionSystem::PluginManager pluginManager;
    pluginManager.setFileExtension(QString("pluginspec"));

    QString pluginPaths = getPluginPaths();
    pluginManager.setPluginPaths(QStringList() << pluginPaths);

    // Add some debugging information
#ifdef DEBUG
#   ifdef DEBUG_WITHOUT_INSTALL
        LOG_FOR("Main", "Running debug non-installed version (debug_without_install)");
#   else
        LOG_FOR("Main", "Running debug installed version");
#   endif
#else
    LOG_FOR("Main", "Running release version");
#endif
#ifdef LINUX_INTEGRATED
    LOG_FOR("Main", "Linux Integrated");
#endif
    defineLibraryPaths();
    LOG_FOR("Main","looking for libraries in path: " + qApp->libraryPaths().join(";"));

//    const QStringList arguments = app.arguments();
//    QMap<QString, QString> foundAppOptions;
//    if (arguments.size() > 1) {
//        QMap<QString, bool> appOptions;
//        appOptions.insert(QLatin1String(HELP_OPTION1), false);
//        appOptions.insert(QLatin1String(HELP_OPTION2), false);
//        appOptions.insert(QLatin1String(HELP_OPTION3), false);
//        appOptions.insert(QLatin1String(HELP_OPTION4), false);
//        appOptions.insert(QLatin1String(VERSION_OPTION), false);
//        appOptions.insert(QLatin1String(CLIENT_OPTION), false);
//        QString errorMessage;
//        if (!pluginManager.parseOptions(arguments,
//                                        appOptions,
//                                        &foundAppOptions,
//                                        &errorMessage)) {
//            displayError(errorMessage);
//            printHelp(QFileInfo(app.applicationFilePath()).baseName(), pluginManager);
//            return -1;
//        }
//    }

    const PluginSpecSet plugins = pluginManager.plugins();
    ExtensionSystem::PluginSpec *coreplugin = 0;

    if (WarnAllPluginSpecs) {
        foreach (ExtensionSystem::PluginSpec *spec, plugins) {
            qWarning() << "PluginSpecs :::"<< spec->filePath() << spec->name() << spec->version();
        }
    }

    foreach (ExtensionSystem::PluginSpec *spec, plugins) {
        if (spec->name() == QString(COREPLUGINSNAME)) {
            coreplugin = spec;
            break;
        }
    }

    ExtensionSystem::PluginSpec *userplugin = 0;
    foreach (ExtensionSystem::PluginSpec *spec, plugins) {
        if (spec->name() == QString(USERPLUGINSNAME)) {
            userplugin = spec;
            break;
        }
    }

    if (!coreplugin) {
        const QString reason = QCoreApplication::translate("Application", "Couldn't find 'Core.pluginspec' in %1").arg(pluginPaths);
        qWarning() << reason;
//        displayError(msgCoreLoadFailure(reason));
        return 1;
    }
    if (!userplugin) {
        const QString reason = QCoreApplication::translate("Application", "Couldn't find 'UserManager.pluginspec' in %1").arg(pluginPaths);
        qWarning() << reason;
//        displayError(msgCoreLoadFailure(reason));
        return 1;
    }


    if (coreplugin->hasError()) {
        qWarning() << coreplugin->errorString();
//        displayError(msgCoreLoadFailure(coreplugin->errorString()));
        return 1;
    }

    if (userplugin->hasError()) {
        qWarning() << userplugin->errorString();
//        displayError(msgCoreLoadFailure(coreplugin->errorString()));
        return 1;
    }

//    if (foundAppOptions.contains(QLatin1String(VERSION_OPTION))) {
//        printVersion(coreplugin, pluginManager);
//        return 0;
//    }
//    if (foundAppOptions.contains(QLatin1String(HELP_OPTION1))
//            || foundAppOptions.contains(QLatin1String(HELP_OPTION2))
//            || foundAppOptions.contains(QLatin1String(HELP_OPTION3))
//            || foundAppOptions.contains(QLatin1String(HELP_OPTION4))) {
//        printHelp(QFileInfo(app.applicationFilePath()).baseName(), pluginManager);
//        return 0;
//    }

//    const bool isFirstInstance = !app.isRunning();
//    if (!isFirstInstance && foundAppOptions.contains(QLatin1String(CLIENT_OPTION)))
//        return sendArguments(app, pluginManager.arguments()) ? 0 : -1;

//    foreach (ExtensionSystem::PluginSpec *spec, plugins) {
//        qWarning() << "PlugInSpec" << spec->name() << spec->errorString() << spec->state();
//    }

    pluginManager.loadPlugins();
    if (WarnAllPluginSpecs) {
        foreach (ExtensionSystem::PluginSpec *spec, plugins) {
            qWarning() << "PluginSpecs :::"<< spec->name() << "hasError:" << spec->hasError() << spec->errorString();
        }
    }

    if (coreplugin->hasError()) {
        qWarning() << coreplugin->errorString();
        return 1;
    }
    if (userplugin->hasError()) {
        qWarning() << userplugin->errorString();
//        displayError(msgCoreLoadFailure(coreplugin->errorString()));
        return 1;
    }


//    if (isFirstInstance) {
//        // Set up lock and remote arguments for the first instance only.
//        // Silently fallback to unconnected instances for any subsequent
//        // instances.
//        app.initialize();
//        QObject::connect(&app, SIGNAL(messageReceived(QString)), coreplugin->plugin(), SLOT(remoteArgument(QString)));
//    }
//    QObject::connect(&app, SIGNAL(fileOpenRequest(QString)), coreplugin->plugin(), SLOT(remoteArgument(QString)));

    // shutdown plugin manager on the exit
    QObject::connect(&app, SIGNAL(aboutToQuit()), &pluginManager, SLOT(shutdown()));

    int r = app.exec();
//    Utils::Log::saveLog();
    return r;
}

