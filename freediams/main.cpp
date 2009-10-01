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
#include <QtPlugin>
#include <QApplication>
#include <QTextCodec>
#include <QDir>

#include <QDebug>

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <extensionsystem/iplugin.h>

#include <utils/log.h>

typedef QList<ExtensionSystem::PluginSpec *> PluginSpecSet;
static const char* COREPLUGINSNAME = "Core";


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

#ifdef LINUX_INTEGRATED
    return QString("/usr/%1/%2").arg(LIBRARY_BASENAME).arg(BINARY_NAME);
#endif

#ifdef RELEASE
#  ifdef Q_OS_MAC
    app = QDir::cleanPath(app+"/../");
    app += "/plugins/";
    return app;
#  endif
#endif
    return QString();
}

inline static void defineLibraryPaths()
{
#ifdef LINUX_INTEGRATED
    qApp->addLibraryPath(getPluginPaths());
#else
#  ifndef DEBUG
    qApp->setLibraryPaths(QStringList() << getPluginPaths() << QDir::cleanPath(getPluginPaths() + "/qt"));
#  endif
#endif
#endif
}


int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);

    QTextCodec::setCodecForTr( QTextCodec::codecForName( "UTF-8" ) );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );

    app.setApplicationName( QString( "%1" ).arg( BINARY_NAME ) );

    app.setOrganizationName( BINARY_NAME );
    app.setApplicationVersion( PACKAGE_VERSION );

    ExtensionSystem::PluginManager pluginManager;
    pluginManager.setFileExtension(QString("pluginspec"));

    QString pluginPaths = getPluginPaths();
    pluginManager.setPluginPaths(QStringList() << pluginPaths);

    Utils::Log::addMessage("Main","Command line : " + qApp->arguments().join(" "));
    Utils::Log::addMessage("Main","looking for plugins in path : " + pluginPaths);

    defineLibraryPaths();

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

    foreach (ExtensionSystem::PluginSpec *spec, plugins) {
        if (spec->name() == QString(COREPLUGINSNAME)) {
            coreplugin = spec;
            break;
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
    if (coreplugin->hasError()) {
        qWarning() << "main" << coreplugin->errorString();
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

    // Do this after the event loop has started
//    QTimer::singleShot(100, &pluginManager, SLOT(startTests()));
    int r = app.exec();
//    Utils::Log::saveLog();
    return r;
}

