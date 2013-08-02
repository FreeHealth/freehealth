/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SCRIPT_PLUGIN_H
#define SCRIPT_PLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QObject>
QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

/**
 * \file scriptplugin.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.2
 * \date 30 Dec 2012
*/

namespace Script {
namespace Internal {
class ScriptManager;

class ScriptPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.freemedforms.FreeMedForms.ScriptPlugin" FILE "Script.json")

public:
    ScriptPlugin();
    ~ScriptPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private Q_SLOTS:
    void postCoreInitialization();
    void patientSelected();
    void onScriptDialogTriggered();

private:
    ScriptManager *m_Manager;
    QAction *aScriptDialog;
};

}  // namespace Internal
}  // namespace Script

#endif  // End SCRIPT_PLUGIN_H
