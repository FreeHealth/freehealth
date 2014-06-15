/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
/***************************************************************************
 *   Main Developer: Eric Maeker <eric.maeker@gmail.com>                  *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_IPLUGIN_H
#define TOOLS_IPLUGIN_H

#include "tools_exporter.h"
#include "toolspreferences.h"

#include <extensionsystem/iplugin.h>

namespace Tools {
namespace Internal {
class PdfTkWrapper;
class FspPrinterPreferencesPage;
class ChequePrinterPreferencesPage;
class HprimPreferencesPage;

class ToolsPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeMedForms.ToolsPlugin" FILE "Tools.json")

public:
    ToolsPlugin();
    ~ToolsPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private Q_SLOTS:
    void postCoreInitialization();
    void printCheque();
    void printFsp();

private:
    ToolsPreferencesPage *m_prefPage;
    PdfTkWrapper *pdf;
    FspPrinterPreferencesPage *m_FspPage;
    ChequePrinterPreferencesPage *m_ChequePage;
    HprimPreferencesPage *m_HprimPage;
};

} // namespace Internal
} // namespace Tools

#endif // TOOLS_IPLUGIN_H

