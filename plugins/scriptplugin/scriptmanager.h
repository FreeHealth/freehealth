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
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef SCRIPT_SCRIPTMANAGER_H
#define SCRIPT_SCRIPTMANAGER_H

#include <scriptplugin/script_exporter.h>
#include <coreplugin/iscriptmanager.h>

#include <QScriptEngine>

/**
 * \file scriptmanager.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 30 Dec 2012
*/

namespace Script {
namespace Internal {
class UiTools;
class ScriptPatientWrapper;
class ScriptUserWrapper;
class FormManagerScriptWrapper;
class Tools;

class ScriptManager : public Core::IScriptManager
{
    Q_OBJECT

public:
    ScriptManager(QObject *parent);

    QScriptValue evaluate(const QString &script);
    QScriptValue addScriptObject(QObject *object);
//    QScriptValue addScriptObject(QObject *object, const QString &objectNamespace, const QString &objectScriptName);

    QScriptEngine *engine() {return m_Engine;}

private Q_SLOTS:
    void onAllFormsLoaded();
    void onSubFormLoaded(const QString &subFormUuid);

private:
    static ScriptManager *m_Instance;
    QScriptEngine *m_Engine;
    ScriptPatientWrapper *patient;
    ScriptUserWrapper *user;
    FormManagerScriptWrapper *forms;
    UiTools *uitools;
    Tools *tools;
};

}  // namespace Internal
}  // namespace Script

#endif // SCRIPT_SCRIPTMANAGER_H
