/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "druginteractionsplugin.h"
#include "allergyengine.h"
#include "drugdruginteractionengine.h"
#include "pimengine.h"

#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>

#include <QtPlugin>
#include <QDebug>

using namespace DrugInteractions;
using namespace Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline void messageSplash(const QString &s) {theme()->messageSplashScreen(s); }

DrugInteractionsPlugin::DrugInteractionsPlugin() :
    IPlugin(),
    m_DDIEngine(0),
    m_PimEngine(0),
    m_AllergyEngine(0)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating DrugInteractionsPlugin";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("plugin_druginteractions");

    // Create the drugs engines
    m_AllergyEngine = new DrugAllergyEngine(this);
    addObject(m_AllergyEngine);
    m_DDIEngine = new DrugDrugInteractionEngine(this);
    addObject(m_DDIEngine);
    m_PimEngine = new PimEngine(this);
    addObject(m_PimEngine);
}

DrugInteractionsPlugin::~DrugInteractionsPlugin()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
}

bool DrugInteractionsPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugInteractionsPlugin::initialize";
    Q_UNUSED(arguments);
    Q_UNUSED(errorString);

    return true;
}

void DrugInteractionsPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugInteractionsPlugin::extensionsInitialized";

    messageSplash(tr("Initializing drugs interactions plugin..."));

    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Initialize the drugs engines
    m_DDIEngine->init();
    m_PimEngine->init();
    m_AllergyEngine->init();

//    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitialization()));
}

void DrugInteractionsPlugin::postCoreInitialization()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << Q_FUNC_INFO;
    // Core is fully intialized as well as all plugins
}

ExtensionSystem::IPlugin::ShutdownFlag DrugInteractionsPlugin::aboutToShutdown()
{
    if (Utils::Log::warnPluginsCreation())
        WARN_FUNC;
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    // Remove preferences pages to plugins manager object pool
    removeObject(m_DDIEngine);
    if (m_DDIEngine)
        delete m_DDIEngine;
    m_DDIEngine = 0;

    removeObject(m_PimEngine);
    if (m_PimEngine)
        delete m_PimEngine;
    m_PimEngine = 0;

    removeObject(m_AllergyEngine);
    if (m_AllergyEngine)
        delete m_AllergyEngine;
    m_AllergyEngine = 0;
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(DrugInteractionsPlugin)
