/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "userfistrunpage.h"
#include "widgets/useridentifier.h"
#include "widgets/usermanager.h"
#include "widgets/userwizard.h"
#include "usermanagerplugin/database/userbase.h"
#include "usermanagerplugin/usermodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include "ui_firstrunusercreationwidget.h"

using namespace UserPlugin;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline UserPlugin::UserModel *userModel() {return UserPlugin::UserModel::instance();}
static inline UserPlugin::Internal::UserBase *userBase() {return UserPlugin::Internal::UserBase::instance();}


UserConnectionPage::UserConnectionPage(QWidget *parent) :
    QWizardPage(parent)
//    ui(new Ui::UserConnectionPage)
{
//    ui->setupUi(this);
}

UserConnectionPage::~UserConnectionPage()
{
//    delete ui;
}

void UserConnectionPage::initializePage()
{
    Internal::UserIdentifier id(this);
    if (id.exec()!=QDialog::Accepted) {
        /** \todo improve this */
        qApp->exit(123);
    }
}

bool UserConnectionPage::validatePage()
{
    return true;
}

void UserConnectionPage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
//        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


UserCreationPage::UserCreationPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::FirstRunUserCreationWidget)
{
    ui->setupUi(this);
    ui->userManagerButton->setIcon(theme()->icon(Core::Constants::ICONUSERMANAGER, Core::ITheme::MediumIcon));
    ui->completeWizButton->setIcon(theme()->icon(Core::Constants::ICONNEWUSER, Core::ITheme::MediumIcon));
    ui->quickWizButton->setIcon(theme()->icon(Core::Constants::ICONNEWUSER_QUICK, Core::ITheme::MediumIcon));
    ui->quickDocWizButton->setIcon(theme()->icon(Core::Constants::ICONDOCTOR, Core::ITheme::MediumIcon));
    ui->quickSecWizButton->setIcon(theme()->icon(Core::Constants::ICONSECRETARY, Core::ITheme::MediumIcon));
    ui->quickAdminWizButton->setIcon(theme()->icon(Core::Constants::ICONSERVERADMIN, Core::ITheme::MediumIcon));

    QPixmap pix = theme()->splashScreenPixmap("freemedforms-wizard-users.png");
    setPixmap(QWizard::BackgroundPixmap, pix);
    setPixmap(QWizard::WatermarkPixmap, pix);

    connect(ui->userManagerButton, SIGNAL(clicked()), this, SLOT(userManager()));
    connect(ui->completeWizButton, SIGNAL(clicked()), this, SLOT(userWizard()));
//    connect(ui->quickWizButton, SIGNAL(clicked()), this, SLOT());
//    connect(ui->quickDocWizButton, SIGNAL(clicked()), this, SLOT());
//    connect(ui->quickSecWizButton, SIGNAL(clicked()), this, SLOT());
//    connect(ui->quickAdminWizButton, SIGNAL(clicked()), this, SLOT());
}

UserCreationPage::~UserCreationPage()
{
    delete ui;
}

void UserCreationPage::userManager()
{
//    UserManager *m_UserManager = new UserManager(this);
//    m_UserManager->initialize();
//    m_UserManager->show();
    UserManagerDialog dlg(this);
    dlg.initialize();
    dlg.exec();
}

void UserCreationPage::userWizard()
{
    UserWizard wiz;
    wiz.createUser(true);
    wiz.exec();
}

void UserCreationPage::initializePage()
{
    // Create the user database
    userBase()->initialize();
    // Set current user into user model
    userModel()->setCurrentUserIsServerManager();

    adjustSize();
}

bool UserCreationPage::validatePage()
{
    // Are there user created ? no -> can not validate
    // disconnected user database ?
    // remove currentUserIsServerManager() from userModel
    // delete usermanager pointer
    // remove login/pass from settings()
    return true;
}

void UserCreationPage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
