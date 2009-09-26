#include "drugsplugin.h"
#include "drugswidgetfactory.h"
#include "mfDrugsConstants.h"
#include "drugspreferences/mfDrugsPreferences.h"
#include <drugsplugin/drugsdatabase/mfDrugsBase.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Drugs::Internal;
//using namespace Drugs;

DrugsPlugin::DrugsPlugin() :
        viewPage(0),
        printPage(0),
        userPage(0),
        extraPage(0)
{
    setObjectName("DrugsPlugin");
    if (Utils::Log::warnPluginsCreation())
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
    if (printPage) {
        removeObject(printPage);
        delete printPage; printPage=0;
    }
}

bool DrugsPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsPlugin::initialize";
#ifdef FREEDIAMS
    Utils::Log::addMessage(this,"Running as FreeDiams");
#endif
    addAutoReleasedObject(new Core::PluginAboutPage(this->pluginSpec(), this));
    addAutoReleasedObject(new DrugsWidgetsFactory(this));

    // Initialize drugs database
    DrugsBase::instance();

    return true;
}

void DrugsPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsPlugin::extensionsInitialized";
    viewPage = new DrugsViewOptionsPage(this);
    printPage = new DrugsPrintOptionsPage(this);
    userPage = new DrugsUserOptionsPage(this);
    extraPage = new DrugsExtraOptionsPage(this);
    // check settings
    if (!Core::ICore::instance()->settings()->value(mfDrugsConstants::MFDRUGS_SETTING_CONFIGURED, false).toBool()) {
        viewPage->writeDefaultSettings(Core::ICore::instance()->settings());
        printPage->writeDefaultSettings(Core::ICore::instance()->settings());
        userPage->writeDefaultSettings(Core::ICore::instance()->settings());
        extraPage->writeDefaultSettings(Core::ICore::instance()->settings());
    } else {
        viewPage->checkSettingsValidity();
        printPage->checkSettingsValidity();
        userPage->checkSettingsValidity();
        extraPage->checkSettingsValidity();
    }

    addObject(viewPage);
    addObject(printPage);
    addObject(userPage);
    addObject(extraPage);

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("mfDrugsWidget");
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
