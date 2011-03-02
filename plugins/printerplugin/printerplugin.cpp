/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#include "documentprinter.h"

#include <utils/log.h>
#include <utils/global.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>
#include <QPrinterInfo>

#include <QDebug>

using namespace Print;

PrinterPlugin::PrinterPlugin() :
        prefPage(0), docPrinter(0)
{
    setObjectName("PrinterPlugin");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating PrinterPlugin";
}

PrinterPlugin::~PrinterPlugin()
{
    qWarning() << "PrinterPlugin::~PrinterPlugin()";
    if (prefPage) {
        removeObject(prefPage);
        delete prefPage; prefPage=0;
    }
    if (docPrinter) {
        removeObject(docPrinter);
        delete docPrinter; docPrinter=0;
    }
}

bool PrinterPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PrinterPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    // Add translator
    Core::ICore::instance()->translators()->addNewTranslator("printerplugin");

    return true;
}

void PrinterPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PrinterPlugin::extensionsInitialized";

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Add preferences pages, printer manager
    prefPage = new Print::Internal::PrinterPreferencesPage(this);
    addObject(prefPage);
    docPrinter = new Internal::DocumentPrinter(this);
    addObject(docPrinter);

    // Check system for existing printers
//    if (QPrinterInfo::availablePrinters().count()) {
    if (QPrinterInfo::availablePrinters().isEmpty()) {
        Utils::Log::addError(this, "No printer installed in this system.", __FILE__, __LINE__);
        Utils::warningMessageBox(tr("No printer"),
                                 tr("No printer is configured in your system. The print preview and printing will not work."),
                                 tr("You must configure at least on printer. Please refer to your system documentation. \n"),
                                 qApp->applicationName());
    }
}


Q_EXPORT_PLUGIN(PrinterPlugin)
