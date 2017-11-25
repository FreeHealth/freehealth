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
#include <QTimer>

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <extensionsystem/iplugin.h>

#include <utils/log.h>
#include <utils/global.h>

#include <QDebug>

#include <iostream>

enum { WarnAllPluginSpecs = false };

typedef QList<ExtensionSystem::PluginSpec *> PluginSpecSet;


static const char * COREPLUGINSNAME = "Core";
//static const char * USERMANAGERPLUGINSNAME = "UserManager";

static const QString HELP_MESSAGE =
        QString("");

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

     // Create plugins manager
     ExtensionSystem::PluginManager pluginManager;
     pluginManager.setFileExtension(QString("pluginspec"));

     // Getting & feeding libraries/plugins path
     QStringList libpath;
#ifdef LIBRARY_BASENAME
     libpath << Utils::applicationPluginsPath(QString(BINARY_NAME), QString(LIBRARY_BASENAME));
#else
     libpath << Utils::applicationPluginsPath(QString(BINARY_NAME), "");
#endif
     if (libpath.isEmpty())
         return 123;
     pluginManager.setPluginPaths(libpath);

     // Add some debugging information
     Utils::Log::logCompilationConfiguration();

     // Manage plugin manager options
     QStringList arguments = app.arguments(); // adapted arguments list is passed to plugin manager later
     QMap<QString, QString> foundAppOptions;
     if (arguments.size() > 1) {
         QMap<QString, bool> appOptions;
         QString errorMessage;
         if (!pluginManager.parseOptions(arguments, appOptions, &foundAppOptions, &errorMessage)) {
             qWarning() << errorMessage;
             qWarning() << HELP_MESSAGE;
             return -1;
         }
     }

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

//    ExtensionSystem::PluginSpec *userplugin = 0;
//    foreach (ExtensionSystem::PluginSpec *spec, plugins) {
//        if (spec->name() == QString(USERPLUGINSNAME)) {
//            userplugin = spec;
//            break;
//        }
//    }

    if (!coreplugin) {
        const QString reason = QCoreApplication::translate("Application", "Couldn't find 'Core.pluginspec' in %1").arg(qApp->libraryPaths().join("; "));
        qWarning() << reason;
        return 1;
    }
//    if (!userplugin) {
//        const QString reason = QCoreApplication::translate("Application", "Couldn't find 'UserManager.pluginspec' in %1").arg(qApp->libraryPaths().join("; "));
//        qWarning() << reason;
//        return 1;
//    }


    if (coreplugin->hasError()) {
        qWarning() << coreplugin->errorString();
        return 1;
    }

//    if (userplugin->hasError()) {
//        qWarning() << userplugin->errorString();
//        return 1;
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
//    if (userplugin->hasError()) {
//        qWarning() << userplugin->errorString();
//        return 1;
//    }

    // shutdown plugin manager on the exit
    QObject::connect(&app, SIGNAL(aboutToQuit()), &pluginManager, SLOT(shutdown()));

#ifdef WITH_TESTS
    // Do this after the event loop has started
    if (pluginManager.runningTests())
        QTimer::singleShot(100, &pluginManager, SLOT(startTests()));
#endif

    int r = app.exec();
//    Utils::Log::saveLog();
    return r;
}
