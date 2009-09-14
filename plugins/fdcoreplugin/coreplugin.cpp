#include "coreplugin.h"
#include "coreimpl.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <fdcoreplugin/appaboutpage.h>
#include <coreplugin/dialogs/commonaboutpages.h>
#include <coreplugin/dialogs/commondebugpages.h>
#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/translators.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace Core::Internal;

CorePlugin::CorePlugin() : m_CoreImpl(new CoreImpl(this))
{
}

CorePlugin::~CorePlugin()
{
//    if (m_welcomeMode) {
//        removeObject(m_welcomeMode);
//        delete m_welcomeMode;
//    }
//    if (m_editMode) {
//        removeObject(m_editMode);
//        delete m_editMode;
//    }
//
//    // delete FileIconProvider singleton
//    delete FileIconProvider::instance();
//
//    delete m_mainWindow;
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "CorePlugin::initialize";
    return m_CoreImpl->initialize(arguments,errorMessage);
//    Q_UNUSED(arguments);
//    const bool success = m_mainWindow->init(errorMessage);
//    if (success) {
//        m_welcomeMode = new WelcomeMode;
//        addObject(m_welcomeMode);
//
//        EditorManager *editorManager = m_mainWindow->editorManager();
//        m_editMode = new EditMode(editorManager);
//        addObject(m_editMode);
//    }
//    return success;
}

void CorePlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "CorePlugin::extensionsInitialized";
    m_CoreImpl->extensionsInitialized();
    // add about pages
    this->addAutoReleasedObject(new AppAboutPage(this));
    this->addAutoReleasedObject(new TeamAboutPage(this));
    this->addAutoReleasedObject(new LicenseAboutPage(this));
    this->addAutoReleasedObject(new BuildAboutPage(this));
    this->addAutoReleasedObject(new CommandLineAboutPage(this));
    // add debugging pages
    this->addAutoReleasedObject(new LogErrorDebugPage(this));
    this->addAutoReleasedObject(new LogMessageDebugPage(this));
    this->addAutoReleasedObject(new SettingDebugPage(this));
    // add plugin info page
    addAutoReleasedObject(new Core::PluginAboutPage(pluginSpec(), this));

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("utils");
    Core::ICore::instance()->translators()->addNewTranslator("translationutils");
}

void CorePlugin::remoteArgument(const QString& arg)
{
    qWarning() << arg;
    Q_UNUSED(arg);
    // An empty argument is sent to trigger activation
    // of the window via QtSingleApplication. It should be
    // the last of a sequence.
//    if (arg.isEmpty()) {
//        m_mainWindow->activateWindow();
//    } else {
//        m_mainWindow->openFiles(QStringList(arg));
//    }
}

Q_EXPORT_PLUGIN(CorePlugin)
