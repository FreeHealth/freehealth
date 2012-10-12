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
 *  Main Developers : Eric Maeker <eric.maeker@gmail.com>,                *
 *                    Guillaume Denry <guillaume.denry@gmail.com>          *
 *  Contributors :                                                         *
 *      NAME <MAIL@ADDRESS.COM>                                            *
 ***************************************************************************/
/**
  \namespace PadTools
  \brief Namespace reserved for the PadTools plugin.
  The PadTools namespace includes:
  - Token engine
  - PadDocument creator
  - PadWriter
*/

#include "padtoolsplugin.h"
#include "padtoolsimpl.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/ipatient.h>
#include <coreplugin/iuser.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/translators.h>

#include <QtPlugin>
#include <QDebug>

using namespace PadTools;

namespace {
class TestingToken : public Core::IToken
{
public:
    TestingToken(const QString &name, const QVariant &value) :
        IToken(name),
        _value(value)
    {
    }

    QVariant testValue() const {return "TEST";}
    QVariant value() const {return _value;}

private:
     QVariant _value;
};

}

PadToolsPlugin::PadToolsPlugin() :
    ExtensionSystem::IPlugin(),
    _impl(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating PadToolsPlugin";
    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("padtoolsplugin");

    // Create the Core::IPadTools implementation and register it to the Core::ICore::instance()
    _impl = new PadToolsImpl(this);
    Core::ICore::instance()->setPadTools(_impl);
}

PadToolsPlugin::~PadToolsPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    Core::ICore::instance()->setPadTools(0);
}

bool PadToolsPlugin::initialize(const QStringList &arguments, QString *errorString)
{
	qDebug("PadToolsPlugin::initialize");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PadToolsPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

#ifdef FREEPAD
    Core::ICore::instance()->patient()->registerPatientTokens();
    Core::ICore::instance()->user()->registerUserTokens();
#endif

    // Register testing tokens (A, B, C, D)
    Core::IToken *t;
    QVector<Core::IToken *> _tokens;

    t = new TestingToken("test.A", "Token A");
    t->setUntranslatedHumanReadableName("TokenA");
    _tokens << t;

    t = new TestingToken("test.B", "Token B");
    t->setUntranslatedHumanReadableName("TokenB");
    _tokens << t;

    t = new TestingToken("test.C", "Token C");
    t->setUntranslatedHumanReadableName("TokenC");
    _tokens << t;

    t = new TestingToken("test.D", "Token D");
    t->setUntranslatedHumanReadableName("TokenD");
    _tokens << t;

//    t = new TestingToken("Prescription.Drug.brandname", "Drug");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Quantity.From", "1");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Quantity.to", "2");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Quantity.Scheme", "Day");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.RepeatedDailyScheme", "repeat");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Meal", "meal");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Period.Value", "3");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Period.Scheme", "month");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Duration.From", "4");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Duration.To", "5");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Duration.Scheme", "weeks");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Route", "route");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Distribution.DailyScheme", "distrib");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Quantity.MinInterval", "6");
//    _tokens << t;
//    t = new TestingToken("Prescription.Protocol.Note", "Note");
//    _tokens << t;

    if (_impl->tokenPool()) {
        LOG("Registering  testing tokens");
        _impl->tokenPool()->addTokens(_tokens);
    } else {
        LOG_ERROR("PadTools object is not available, can not register the testing tokens");
    }
    return true;
}

void PadToolsPlugin::extensionsInitialized()
{
	qDebug("PadToolsPlugin::extensionsInitialized");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "PadToolsPlugin::extensionsInitialized";

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));
}

ExtensionSystem::IPlugin::ShutdownFlag PadToolsPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(PadToolsPlugin)
