/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre, GP               *
 *   pm.desombre@medsyn.fr                                                 *
 *   original from Eric Maeker, MD.                                        *
 *   eric.maeker@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
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
static const char * USERMANAGERPLUGINSNAME = "UserManager";

static const QString HELP_MESSAGE =
        QString("");

static const QString VERSION_MESSAGE =
        QString("%1 - %2 ; build on %3 %4 \n  %5 \n  Compiled with Qt: %6 - Running with Qt: %7")
        .arg(BINARY_NAME " - " PACKAGE_VERSION)
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
#endif
        .arg(__DATE__, __TIME__)
#ifdef FULLAPPLICATION_BUILD
        .arg("Full application")
#else
        .arg("SVN application")
#endif
        .arg(QT_VERSION_STR)
        .arg(qVersion());


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

    int r = app.exec();
//    Utils::Log::saveLog();
    return r;
}
