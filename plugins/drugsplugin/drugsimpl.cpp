#include "coreimpl.h"
#include "settings_p.h"
#include "isettings.h"
#include "theme.h"
#include "translators.h"
#include "actionmanager/actionmanager_p.h"
#include "actionmanager/actionmanager.h"
#include "contextmanager/contextmanager.h"
#include "contextmanager/contextmanager_p.h"
#include <coreplugin/ipadtools.h>


//#include ".h"
//#include ".h"

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
CoreImpl::CoreImpl(QObject *parent) :
    ICore(parent),
    m_PadTools(0)
{
    m_Settings = new SettingsPrivate(this);
    m_Theme = new ThemePrivate(this);
    m_Theme->setThemeRootPath(m_Settings->path(ISettings::ThemeRootPath));
    m_Translators = new Translators(this);
    m_MainWindow = new MainWindow();
    m_ActionManager = new ActionManagerPrivate(m_MainWindow);
    m_ContextManager = new ContextManagerPrivate(m_MainWindow);
    m_instance = this;

//    void initLib()
//{
//    // load translator
//    if ( tkTranslators::instance() )
//        tkTranslators::instance()->addNewTranslator( "toolkit" );
//    // initialize tkTheme
//    tkTheme::instance();
//    // intialize tkActionManager
////    ActionManager::instance();
//    // initialize tkSerialNumber
//    tkSerialNumber::initialize();
//}

}

CoreImpl::~CoreImpl()
{
    delete m_MainWindow;
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


QMainWindow *CoreImpl::mainWindow() const
{
    return m_MainWindow;
}

bool CoreImpl::initialize()
{
    m_MainWindow->initialize();
    return true;
}

bool CoreImpl::extensionsInitialized()
{
    m_MainWindow->extensionInitialized();
    return true;
}

