/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "scriptmanager.h"
#include "scriptwrappers.h"
#include "scriptpatientwrapper.h"

#include <coreplugin/icore.h>

#include <formmanagerplugin/formmanager.h>
#include <formmanagerplugin/iformitem.h>

#include <utils/log.h>

#include <QDebug>

using namespace Script;

static inline Form::FormManager *formManager() {return Form::FormManager::instance();}

QScriptValue FormItemScriptWrapperToScriptValue(QScriptEngine *engine, FormItemScriptWrapper* const &in)
{ return engine->newQObject(in); }

void FormItemScriptWrapperFromScriptValue(const QScriptValue &object, FormItemScriptWrapper* &out)
{ out = qobject_cast<FormItemScriptWrapper*>(object.toQObject()); }

ScriptManager::ScriptManager(QObject *parent) :
    Core::IScriptManager(parent),
    m_Engine(new QScriptEngine(this))
{
    // Add the patient
    ScriptPatientWrapper *patient = new ScriptPatientWrapper(this);
    QScriptValue patientValue = m_Engine->newQObject(patient, QScriptEngine::QtOwnership);
    m_Engine->globalObject().setProperty("patient", patientValue);

    // Add the form manager
    FormManagerScriptWrapper *forms = new FormManagerScriptWrapper(this);
    QScriptValue formsValue = m_Engine->newQObject(forms, QScriptEngine::QtOwnership);
    m_Engine->globalObject().setProperty("forms", formsValue);

    // Add meta types
    qScriptRegisterMetaType<Script::FormItemScriptWrapper*>(m_Engine, ::FormItemScriptWrapperToScriptValue, ::FormItemScriptWrapperFromScriptValue);

    // Register to Core::ICore
    Core::ICore::instance()->setScriptManager(this);

    // Connect to formmanager
    connect(formManager(), SIGNAL(patientFormsLoaded()), this, SLOT(onAllFormsLoaded()));
}

bool ScriptManager::evaluate(const QString &script)
{
    if (script.isEmpty())
        return false;
//    qWarning() << script;
    QScriptSyntaxCheckResult check = m_Engine->checkSyntax(script);
    if (check.state()!=QScriptSyntaxCheckResult::Valid) {
        LOG_ERROR(QString("Script error (%1;%2): ").arg(check.errorLineNumber()).arg(check.errorColumnNumber()) + check.errorMessage());
        return false;
    }
    m_Engine->evaluate(script).toString();
    return true;
}

QScriptValue ScriptManager::addScriptObject(const QObject *object)
{
    return m_Engine->newQObject((QObject*)object);
}

void ScriptManager::onAllFormsLoaded()
{
    // Execute RootForm all OnLoad scripts
    foreach(Form::FormMain *main, formManager()->forms()) {
        evaluate(main->scripts()->onLoadScript());
        QList<Form::FormMain *> children = main->flattenFormMainChildren();
        foreach(Form::FormMain *mainChild, children) {
            evaluate(mainChild->scripts()->onLoadScript());
            foreach(Form::FormItem *item, mainChild->flattenFormItemChildren()) {
                evaluate(item->scripts()->onLoadScript());
            }
        }
    }
    // Execute empty root SubForms OnLoad scripts
    foreach(Form::FormMain *main, formManager()->subFormsEmptyRoot()) {
        evaluate(main->scripts()->onLoadScript());
    }
}
