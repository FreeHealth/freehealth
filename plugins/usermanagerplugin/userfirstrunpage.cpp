/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "userfirstrunpage.h"
#include <usermanagerplugin/widgets/useridentifier.h>
#include <usermanagerplugin/widgets/usermanager.h>
#include <usermanagerplugin/widgets/usercreatorwizard.h>
#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/database/userbase.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>

#include <utils/global.h>
#include <utils/log.h>
#include <utils/databaseconnector.h>
#include <translationutils/constanttranslations.h>

#include <QProgressDialog>
#include <QDesktopWidget>

#include "ui_firstrunusercreationwidget.h"

using namespace UserPlugin;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}

UserCreationPage::UserCreationPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::FirstRunUserCreationWidget),
    _userManagerDialog(0),
    _userWizard(0)
{
    ui->setupUi(this);
    ui->userManagerButton->setIcon(theme()->icon(Core::Constants::ICONUSERMANAGER, Core::ITheme::MediumIcon));
    ui->completeWizButton->setIcon(theme()->icon(Core::Constants::ICONNEWUSER, Core::ITheme::MediumIcon));

    QPixmap pix = theme()->splashScreenPixmap("freemedforms-wizard-users.png");
    setPixmap(QWizard::BackgroundPixmap, pix);
    setPixmap(QWizard::WatermarkPixmap, pix);

    connect(ui->userManagerButton, SIGNAL(clicked()), this, SLOT(userManager()));
    connect(ui->completeWizButton, SIGNAL(clicked()), this, SLOT(userWizard()));

    retranslate();
}

UserCreationPage::~UserCreationPage()
{
    delete ui;
}

void UserCreationPage::userManager()
{
    if (!_userManagerDialog) {
        _userManagerDialog = new UserManagerDialog(this);
        _userManagerDialog->setModal(true);
        _userManagerDialog->initialize();
    }
    QSize size = QDesktopWidget().availableGeometry(_userManagerDialog).size();
    _userManagerDialog->resize(size*0.75);
    _userManagerDialog->show();
    _userManagerDialog->initializeAfterShowing();
    Utils::centerWidget(_userManagerDialog, this->wizard());
}

void UserCreationPage::userWizard()
{
    if (!_userWizard) {
        _userWizard = new UserCreatorWizard(this);
        _userWizard->setModal(true);
//        _userWizard->adjustSize();
//        Utils::centerWidget(_userWizard, this->wizard());
    }
    _userWizard->show();
}

void UserCreationPage::initializePage()
{
    // Create the user database
    if (!userCore().initialize()
            || !userBase()->isInitialized())
        LOG_ERROR("Unable to initialize user core or database");

    const Utils::DatabaseConnector &db = settings()->databaseConnector();
    if (db.driver()==Utils::Database::SQLite) {
        // keep language
        QLocale::Language l = QLocale().language();
        if (!userModel()->setCurrentUser(Constants::DEFAULT_USER_CLEARLOGIN, Constants::DEFAULT_USER_CLEARPASSWORD)) {
            LOG_ERROR("Unable to connect has default admin user");
            ui->userManagerButton->setEnabled(false);
        }
        // return to the selected language
        Core::ICore::instance()->translators()->changeLanguage(l);
    }
//    else if (db.driver()==Utils::Database::MySQL) {
//        if (!userModel()->setCurrentUser(Constants::DEFAULT_USER_CLEARLOGIN, Constants::DEFAULT_USER_CLEARPASSWORD)) {
//            LOG_ERROR("Unable to connect has default admin user");
//            ui->userManagerButton->setEnabled(false);
//        }
//    }

    // Set current user into user model
    userModel()->setCurrentUserIsServerManager();

    adjustSize();
    Utils::centerWidget(this);
}

bool UserCreationPage::validatePage()
{
    if (_userManagerDialog && _userManagerDialog->isVisible()) {
        _userManagerDialog->setVisible(false);
        _userManagerDialog->close();
        delete _userManagerDialog;
        _userManagerDialog = 0;
    }
    if (_userWizard) {
        _userWizard->close();
        delete _userWizard;
        _userWizard = 0;
    }
    // TODO: code here
    // Are there user created ? no -> can not validate
    // disconnected user database ?
    // disconnect currentUser from userModel
    userModel()->clear();

    // remove login/pass from settings()
    Utils::DatabaseConnector db = settings()->databaseConnector();
    db.setClearLog(Constants::DEFAULT_USER_CLEARLOGIN);
    db.setClearPass(Constants::DEFAULT_USER_CLEARPASSWORD);
    settings()->setDatabaseConnector(db);
    settings()->sync();
    Core::ICore::instance()->databaseServerLoginChanged();
    return true;
}

void UserCreationPage::retranslate()
{
    setTitle(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::CREATE_USER));
    setSubTitle(tr("You can use the full user manager dialog to create user or create simple users using the user wizard."));
    ui->userManagerButton->setText(tkTr(Trans::Constants::USERMANAGER_TEXT));
    ui->completeWizButton->setText(QCoreApplication::translate(Constants::TR_CONTEXT_USERS, Constants::USER_WIZARD));
}

void UserCreationPage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslate();
        break;
    default:
        break;
    }
}
