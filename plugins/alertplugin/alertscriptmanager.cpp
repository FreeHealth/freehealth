/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class Alert::Internal::AlertScriptManager
 * Internal script management for alerts. Connected to the Core::IScriptManager to allow interactions
 * with patient/user data and all registered object in the central script manager.
 * Allow interaction with alerts inside a script: alert can be accessed in read/write mode.
 * \sa Alert::Internal::AlertItemScriptWrapper, Alert::AlertScript::ScriptType
 */

#include "alertscriptmanager.h"

#include <coreplugin/icore.h>
#include <coreplugin/iscriptmanager.h>

#include <QScriptEngine>

#include <QDebug>
enum { WarnExecutedScripts = false };

using namespace Alert;
using namespace Internal;

static inline Core::IScriptManager *scriptManager() {return Core::ICore::instance()->scriptManager();}

AlertScriptManager::AlertScriptManager(QObject *parent) :
    QObject(parent),
    _wrapper(0),
    _test(0)
{
    setObjectName("AlertScriptManager");
    if (!scriptManager()) {
        // for test purpose only
        _test = new QScriptEngine(this);
    }
}

/**
 * Executes the script \e scriptType from the AlertItem \e item and return the value of the script.
 * The script can modify the AlertItem.
 * \sa Alert::AlertScript::ScriptType
 */
QVariant AlertScriptManager::execute(AlertItem &item, const int scriptType)
{
    // Remove all alert wrapper from the script engine
    if (_wrapper) {
        delete _wrapper;
        _wrapper = 0;
    }

    const QString &script = item.scriptType(AlertScript::ScriptType(scriptType)).script();
    if (WarnExecutedScripts) {
        qWarning() << "Alert::Execute script"
                   << AlertScript::typeToXml(AlertScript::ScriptType(scriptType))
                   << "\n    "
                   << item.uuid()
                   << "\n    "
                   << item.label()
                   << "\n    "
                   << script;
    }

    if (script.isEmpty())
        return QVariant();

    // Create the AlertItem wrapper
    if (scriptManager()) {
        _wrapper = new AlertItemScriptWrapper(item);
        QScriptValue wrapperValue = scriptManager()->addScriptObject(_wrapper);
        scriptManager()->evaluate("namespace.com.freemedforms").setProperty("alert", wrapperValue);
    } else {
        _wrapper = new AlertItemScriptWrapper(item);
        QScriptValue wrapperValue = _test->newQObject(_wrapper, QScriptEngine::QtOwnership);
        _test->globalObject().setProperty("alert", wrapperValue);
    }

    // Evaluate
    QScriptValue toReturn;
    if (scriptManager())
        toReturn = scriptManager()->evaluate(script);
    else
        toReturn = _test->evaluate(script);

    // Remove the alert wrapper from the script engine
    if (_wrapper) {
        delete _wrapper;
        _wrapper = 0;
    }

    if (WarnExecutedScripts)
        qWarning() << "Alert::returned value" << toReturn.toVariant();

    return toReturn.toVariant();
}

