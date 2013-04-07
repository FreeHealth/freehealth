/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
  \namespace Print
  Printing engine of the FreeMedForms project
*/

#include "printerplugin.h"
#include "printerpreferences.h"
#include "printcorrectionpreferences.h"
#include "documentprinter.h"

#include <utils/log.h>
#include <utils/global.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>

#include <QtPlugin>
#if QT_VERSION < 0x050000
#include <QPrinterInfo>
#else
#include <QtPrintSupport/QPrinterInfo>
#endif

#include <QDebug>

using namespace Print;
using namespace Internal;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

PrinterPlugin::PrinterPlugin() :
    prefPage(0),
    printCorrectionPage(0),
    docPrinter(0)
{
    setObjectName("PrinterPlugin");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating PrinterPlugin";

    // Add preferences pages, printer manager
    prefPage = new Print::Internal::PrinterPreferencesPage(this);
    addObject(prefPage);
    printCorrectionPage = new Print::Internal::PrintCorrectionPreferencesPage(this);
    addObject(printCorrectionPage);
    docPrinter = new Internal::DocumentPrinter(this);
    addObject(docPrinter);
}

PrinterPlugin::~PrinterPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
}

bool PrinterPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PrinterPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Add translator
    Core::ICore::instance()->translators()->addNewTranslator("plugin_printer");

    return true;
}

void PrinterPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PrinterPlugin::extensionsInitialized";

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Check system for existing printers
//    if (QPrinterInfo::availablePrinters().isEmpty()) {
//        LOG_ERROR("No printer installed in this system.");
//        Utils::warningMessageBox(tr("No printer"),
//                                 tr("No printer is configured in your system. The printing preview and printing will not work."),
//                                 tr("You must configure at least one printer. Please refer to your system documentation\n""),
//                                 qApp->applicationName());
//    }
}

ExtensionSystem::IPlugin::ShutdownFlag PrinterPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    if (prefPage) {
        removeObject(prefPage);
        delete prefPage; prefPage=0;
    }
    if (printCorrectionPage) {
        removeObject(printCorrectionPage);
        delete printCorrectionPage; printCorrectionPage = 0;
    }
    if (docPrinter) {
        removeObject(docPrinter);
        delete docPrinter; docPrinter=0;
    }
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(PrinterPlugin)
