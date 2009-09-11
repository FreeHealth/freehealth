#include "drugsplugin.h"
#include "drugswidgetfactory.h"
#include "mfDrugsConstants.h"
#include "drugspreferences/mfDrugsPreferences.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Drugs::Internal;
using namespace Drugs;

DrugsPlugin::DrugsPlugin() :
        viewPage(0),
        userPage(0),
        extraPage(0)
{
    setObjectName("DrugsPlugin");
    qWarning() << "creating DrugsPlugin";
}

DrugsPlugin::~DrugsPlugin()
{
    if (viewPage) {
        removeObject(viewPage);
        delete viewPage; viewPage=0;
    }
    if (userPage) {
        removeObject(userPage);
        delete userPage; userPage=0;
    }
    if (extraPage) {
        removeObject(extraPage);
        delete extraPage; extraPage=0;
    }
}

bool DrugsPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    qWarning() << "DrugsPlugin::initialize";
#ifdef FREEDIAMS
    Utils::Log::addMessage(this,"Running as FreeDiams");
#endif
    addAutoReleasedObject(new Core::PluginAboutPage(this->pluginSpec(), this));
    addAutoReleasedObject(new Drugs::DrugsWidgetsFactory(this));
    return true;
}

void DrugsPlugin::extensionsInitialized()
{
    qWarning() << "DrugsPlugin::extensionsInitialized";
    viewPage = new DrugsViewOptionsPage(this);
    userPage = new DrugsUserOptionsPage(this);
    extraPage = new DrugsExtraOptionsPage(this);
    // check settings
    if (!Core::ICore::instance()->settings()->value(mfDrugsConstants::MFDRUGS_SETTING_CONFIGURED, false).toBool()) {
        viewPage->writeDefaultSettings(Core::ICore::instance()->settings());
        userPage->writeDefaultSettings(Core::ICore::instance()->settings());
        extraPage->writeDefaultSettings(Core::ICore::instance()->settings());
    }
    addObject(viewPage);
    addObject(userPage);
    addObject(extraPage);
}

void DrugsPlugin::remoteArgument(const QString& arg)
{
    qWarning() << "drugplugin" << arg;
    // An empty argument is sent to trigger activation
    // of the window via QtSingleApplication. It should be
    // the last of a sequence.
//    if (arg.isEmpty()) {
//        m_mainWindow->activateWindow();
//    } else {
//        m_mainWindow->openFiles(QStringList(arg));
//    }
}

Q_EXPORT_PLUGIN(DrugsPlugin)
