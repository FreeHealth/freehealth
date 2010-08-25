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
