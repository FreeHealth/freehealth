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
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       Christian A. Reiter <christian.a.reiter@gmail.com>>               *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "useridentityandloginpage.h"
#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/database/userbase.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/translators.h>

#include <identityplugin/identityeditorwidget.h>
//#include <listviewplugin/languagecombobox.h>

#include <utils/log.h>
#include <utils/global.h>
//#include <utils/widgets/uppercasevalidator.h>
#include <translationutils/constants.h>

//#include "ui_useridentityandloginpage.h"

#include <QHBoxLayout>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}

UserIdentityAndLoginPage::UserIdentityAndLoginPage(QWidget *parent) :
    QWizardPage(parent),
    _identity(0),
    _showErrorLabels(false)
{
    // TODO: connect(ui->cbLanguage, SIGNAL(currentLanguageChanged(QLocale::Language)), Core::Translators::instance(), SLOT(changeLanguage(QLocale::Language)));

    // Create the layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    setLayout(layout);
    layout->setMargin(0);
    layout->setSpacing(0);

    // Create the identity widget
    _identity = new Identity::IdentityEditorWidget(this);
    _identity->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _identity->setAvailableWidgets(Identity::IdentityEditorWidget::FullIdentity | Identity::IdentityEditorWidget::Photo | Identity::IdentityEditorWidget::FullLogin);
    _identity->setCheckPasswordConfirmation(true);
    _identity->setMinimalLoginLength(6);
    _identity->setMinimalPasswordLength(6);
    layout->addWidget(_identity);

    registerField("Name*", _identity, "birthName");
    registerField("Firstname*", _identity, "firstName");
    registerField("SecondName", _identity, "secondName");
    registerField("Title", _identity, "title");
    registerField("GenderIndex", _identity, "genderIndex");
    registerField("Gender", _identity, "gender");
    registerField("Language*", _identity, "language");

    registerField("Login*", _identity, "clearLogin");
    registerField("Password*", _identity, "clearPassword");

    connect(_identity, SIGNAL(clearLoginEditionFinished()), this, SLOT(checkLoginAfterEdition()));
    connect(_identity, SIGNAL(passwordConfirmed()), this, SLOT(onPasswordConfirmed()));

    Utils::resizeAndCenter(this, parent);
}

UserIdentityAndLoginPage::~UserIdentityAndLoginPage()
{
//    delete ui;
}

bool UserIdentityAndLoginPage::checkLogin() const
{
    // user login must be unique in the FreeMedForms database
    // user login must be unique on the server
    const QString &login = _identity->currentClearLogin();
    if (login.length() < 6)
        return false;

    if (userBase()->isLoginAlreadyExists(_identity->currentClearLogin())) {
        Utils::warningMessageBox(tr("Login error"), tr("Login already in use. Please select another login"));
        return false;
    }

    return true;
}

void UserIdentityAndLoginPage::checkLoginAfterEdition()
{
    if (checkLogin())
        Q_EMIT completeChanged();

    _showErrorLabels = true;
}

/**
 * \internal
 */
void UserIdentityAndLoginPage::onPasswordConfirmed()
{
    if (checkLogin())
        Q_EMIT completeChanged();
}

void UserIdentityAndLoginPage::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
        retranslate();
}

void UserIdentityAndLoginPage::retranslate()
{
    setTitle(tr("Create a new user"));
    setSubTitle(tr("Please enter your identity."));
}

bool UserIdentityAndLoginPage::isComplete() const
{
    return (!_identity->isIdentityValid()
            && !_identity->currentLanguage().isEmpty()
            && checkLogin()
            && _identity->isPasswordCompleted());
}

/**
 * Validate the wizard page. Checks the name and firstname,
 * check the password confirmation then validate
 */
bool UserIdentityAndLoginPage::validatePage()
{
    return true;
}
