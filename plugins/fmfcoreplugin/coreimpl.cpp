#include "coreimpl.h"
#include <coreplugin/settings_p.h>
#include <coreplugin/isettings.h>
#include <coreplugin/theme.h>
#include <coreplugin/translators.h>
#include <coreplugin/actionmanager/actionmanager_p.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/contextmanager/contextmanager.h>
#include <coreplugin/contextmanager/contextmanager_p.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/formmanager.h>

#include <fmfcoreplugin/mainwindow.h>



#include <translationutils/constanttranslations.h>

#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

namespace Core {
namespace Internal {
static CoreImpl *m_instance = 0;
} // namespace Internal
} // namespace Core


using namespace Core;
using namespace Core::Internal;


ICore* ICore::instance()
{
    return m_instance;
}

// instance is created by Core::CorePlugin()
CoreImpl::CoreImpl(QObject *parent) : ICore(parent)
{
    m_UID = new UniqueIDManager();
    m_Settings = new SettingsPrivate(this);
    m_Theme = new ThemePrivate(this);
    m_Theme->setThemeRootPath(m_Settings->path(ISettings::ThemeRootPath));
    m_MainWindow = new MainWindow();
    m_ActionManager = new ActionManagerPrivate(m_MainWindow);
    m_ContextManager = new ContextManagerPrivate(m_MainWindow);
    m_FormManager = new FormManager(this);

    // add translators
    m_Translators = new Translators(this);
    m_Translators->setPathToTranslations(m_Settings->path(ISettings::TranslationsPath));
    // Qt
    m_Translators->addNewTranslator("qt");
    // Core Needed Libs
    m_Translators->addNewTranslator(Trans::Constants::CONSTANTS_TRANSLATOR_NAME);

    m_instance = this;
}

CoreImpl::~CoreImpl()
{
    delete m_MainWindow;
    delete m_UID;
}
//QStringList CoreImpl::showNewItemDialog(const QString &title,
//                                        const QList<IWizard *> &wizards,
//                                        const QString &defaultLocation)
//{
//    return m_mainwindow->showNewItemDialog(title, wizards, defaultLocation);
//}
//
//bool CoreImpl::showOptionsDialog(const QString &group, const QString &page, QWidget *parent)
//{
//    return m_mainwindow->showOptionsDialog(group, page, parent);
//}


ActionManager *CoreImpl::actionManager() const
{
    return m_ActionManager;
}

ContextManager *CoreImpl::contextManager() const
{
    return m_ContextManager;
}

UniqueIDManager *CoreImpl::uniqueIDManager() const
{
    return m_UID;
}

ITheme *CoreImpl::theme() const
{
    return m_Theme;
}

Translators *CoreImpl::translators() const
{
    return m_Translators;
}

ISettings *CoreImpl::settings() const
{
    return m_Settings;
}


IMainWindow *CoreImpl::mainWindow() const
{
    return m_MainWindow;
}

FormManager *CoreImpl::formManager() const
{
    return m_FormManager;
}


bool CoreImpl::initialize(const QStringList &arguments, QString *errorString)
{
    return m_MainWindow->initialize(arguments, errorString);
}

void CoreImpl::extensionsInitialized()
{
    m_MainWindow->extensionsInitialized();
}

