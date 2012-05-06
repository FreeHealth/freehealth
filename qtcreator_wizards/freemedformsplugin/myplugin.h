/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by %Author%, <%AuthorEmail%>                             *
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
 *   Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                  *
 *   Contributors:                                                         *
 *       %Author% <%AuthorEmail%>                                            *
 ***************************************************************************/
#ifndef %PluginName:u%_%CppHeaderSuffix:u%
#define %PluginName:u%_%CppHeaderSuffix:u%

#include "%PluginName:l%_exporter.%CppHeaderSuffix%"

#include <extensionsystem/iplugin.h>

namespace %PluginName% {
namespace Internal {

class %PluginName%Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    %PluginName%Plugin();
    ~%PluginName%Plugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
//    ShutdownFlag aboutToShutdown();

private slots:
    void postCoreInitialization();
    void coreAboutToClose();
//    void triggerAction();
};

} // namespace Internal
} // namespace %PluginName%

#endif // %PluginName:u%_%CppHeaderSuffix:u%
