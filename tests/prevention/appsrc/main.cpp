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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
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
#include <utils/database.h>

#include <iostream>

enum { WarnAllPluginSpecs = false };

typedef QList<ExtensionSystem::PluginSpec *> PluginSpecSet;


static const char * COREPLUGINSNAME = "Core";
static const char * USERMANAGERPLUGINSNAME = "UserManager";

static const QString HELP_MESSAGE =
        QString("");

static inline QString getPluginPaths()
{
    QString app = qApp->applicationDirPath();

#ifdef DEBUG
#    ifdef Q_OS_MAC
        app = QDir::cleanPath(app+"/../../../");
#    endif
    app += "/plugins/";
    return app;
#endif

#ifdef RELEASE
#ifdef LINUX_INTEGRATED
    return QString("/usr/%1/%2").arg(LIBRARY_BASENAME).arg(QString(BINARY_NAME).toLower());
#endif


#  ifdef Q_OS_MAC
    app = QDir::cleanPath(app+"/../"+"/plugins/");
    return app;
#  endif

// TODO: Add FreeBSD pluginPath */

#  ifdef Q_OS_WIN
    app = QDir::cleanPath(app + "/plugins/");
    return app;
#  endif

#endif
    return QDir::cleanPath(app + "/plugins/");
}

static inline void defineLibraryPaths()
{
#ifdef LINUX_INTEGRATED
    qApp->addLibraryPath(getPluginPaths());
#else
#  ifndef DEBUG
    qApp->setLibraryPaths(QStringList() << getPluginPaths() << QDir::cleanPath(getPluginPaths() + "/qt"));
#  endif
#endif
}


int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);

#if QT_VERSION < 0x050000
     // Removed in Qt5
     QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
     QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

    app.setApplicationName( QString("%1").arg(BINARY_NAME));
    app.setOrganizationName(BINARY_NAME);
    app.setApplicationVersion(PACKAGE_VERSION);

    QStringList args = qApp->arguments();
    if (args.contains("--version") ||
        args.contains("-version") ||
        args.contains("-v")) {
        std::cout << qPrintable(VERSION_MESSAGE);
        return 0;
    }

    ExtensionSystem::PluginManager pluginManager;
    pluginManager.setFileExtension(QString("pluginspec"));

    QString pluginPaths = getPluginPaths();
    pluginManager.setPluginPaths(QStringList() << pluginPaths);

    // Add some debugging informations
    Utils::Database::logAvailableDrivers();

#ifdef DEBUG
    Utils::Log::addMessage("Main", "Running debug version");
#else
    Utils::Log::addMessage("Main", "Running release version");
#endif
#ifdef LINUX_INTEGRATED
    Utils::Log::addMessage("Main", "Linux Integrated");
#endif

    defineLibraryPaths();
    Utils::Log::addMessage("Main","looking for libraries in path : " + qApp->libraryPaths().join(";"));

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
    ExtensionSystem::PluginSpec *usermanagerplugin = 0;

    if (WarnAllPluginSpecs) {
        foreach (ExtensionSystem::PluginSpec *spec, plugins) {
            qWarning() << "PluginSpecs :::"<< spec->filePath() << spec->name() << spec->version();
        }
    }

    foreach (ExtensionSystem::PluginSpec *spec, plugins) {
        if (spec->name() == QString(COREPLUGINSNAME)) {
            coreplugin = spec;
        } else if (spec->name() == QString(USERMANAGERPLUGINSNAME)) {
            usermanagerplugin = spec;
        }
    }
    if (!coreplugin) {
    const QString reason = QCoreApplication::translate("Application", "Couldn't find 'Core.pluginspec' in %1").arg(pluginPaths);
        qWarning() << reason;
//        displayError(msgCoreLoadFailure(reason));
        return 1;
    }
    if (coreplugin->hasError()) {
        qWarning() << coreplugin->errorString();
//        displayError(msgCoreLoadFailure(coreplugin->errorString()));
        return 1;
    }

    pluginManager.loadPlugins();
    if (WarnAllPluginSpecs) {
        foreach (ExtensionSystem::PluginSpec *spec, plugins) {
            qWarning() << "PluginSpecs :::"<< spec->name() << "hasError:" << spec->hasError() << spec->errorString();
        }
    }
    if (coreplugin->hasError()) {
        qWarning() << "main" << coreplugin->errorString();
        return 1;
    }


    // shutdown plugin manager on the exit
    QObject::connect(&app, SIGNAL(aboutToQuit()), &pluginManager, SLOT(shutdown()));

    int r = app.exec();
//    Utils::Log::saveLog();
    return r;
}

