#include "UserManagerMainWindow.h"

#include <QApplication>
#include <tkActionManager.h>
#include <tkUniqueIdentifier.h>
#include <tkContextManager.h>
#include <tkConstants.h>
#include <tkDebugDialog.h>
#include <tkSettings.h>

UserManagerMainWindow::UserManagerMainWindow(QWidget *parent) : tkUserManager(parent)
{
    setObjectName("UserManagerMainWindow");
    setWindowTitle(qApp->applicationName());
}

bool UserManagerMainWindow::initialize()
{
    createFileMenu();
    createEditMenu();
    createFormatMenu();
    createConfigurationMenu();
    createHelpMenu();
    createEditActions();
    createConfigurationActions( A_AppPreferences );
    createHelpActions( A_AppAbout | A_AppHelp | A_DebugDialog);
    connectHelpActions();
    tkContextManager::instance()->addAdditionalContext(tkUID->uniqueIdentifier(tkConstants::C_USERMANAGER));
    tkUserManager::initialize();
    tkActionManager::instance()->retranslateMenusAndActions();
}

bool UserManagerMainWindow::aboutApplication()
{
    return true;
}

bool UserManagerMainWindow::applicationHelp()
{
    return true;
}

bool UserManagerMainWindow::debugDialog()
{
    tkDebugDialog dlg(this , tkSettings::instance());
    dlg.exec();
    return true;
}

