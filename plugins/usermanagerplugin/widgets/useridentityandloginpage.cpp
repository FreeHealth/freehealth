/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *       Christian A. Reiter <christian.a.reiter@gmail.com>>               *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/**
 * \class UserPlugin::Internal::UserIdentityAndLoginPage
 * Wizard page for the UserPlugin::UserCreatorWizard that manages
 * user identity and login/password.
 *
 * Wizard fields:
 * Some fields are mandatory:
 * - (usual)name
 * - firstname
 * - language
 * - login
 * - password
 * Login and password are checked:
 * - no duplicate login possible
 * - password needs a confirmation
 *
 * Workflow:
 * The page becomes completed when:
 * - all mandatory fields are populated
 * - login and password are correctly checked.
 *
 * Fields name:
 * - "UsualName*"
 * - "Firstname*"
 * - "OtherName"
 * - "Title"
 * - "GenderIndex"
 * - "Gender"
 * - "Language*"
 * - "Login*"
 * - "Password*"
 */
// TODO: remove fields name "magic number"

#include "useridentityandloginpage.h"
#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/database/userbase.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/translators.h>

#include <identityplugin/identityeditorwidget.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QHBoxLayout>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}

UserIdentityAndLoginPage::UserIdentityAndLoginPage(QWidget *parent) :
    QWizardPage(parent),
    _identity(0),
    _showErrorLabels(false)
{
    // TODO: connect(ui->cbLanguage, SIGNAL(currentLanguageChanged(QLocale::Language)), Core::Translators::instance(), SLOT(changeLanguage(QLocale::Language)));

    // Create the layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSizeConstraint(QLayout::SetDefaultConstraint);
    layout->setMargin(0);
    layout->setSpacing(0);

    // Create the identity widget
    _identity = new Identity::IdentityEditorWidget(this);
//    _identity->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _identity->setAvailableWidgets(Identity::IdentityEditorWidget::FullIdentity | Identity::IdentityEditorWidget::Photo | Identity::IdentityEditorWidget::FullLogin);
    layout->addWidget(_identity);
    setLayout(layout);

    registerField("UsualName*", _identity, "usualName");
    registerField("Firstname*", _identity, "firstName");
    registerField("OtherNames", _identity, "otherNames");
    registerField("Title", _identity, "title");
    registerField("GenderIndex", _identity, "genderIndex");
    registerField("Gender", _identity, "gender");
    registerField("Language*", _identity, "language");

    registerField("Login*", _identity, "clearLogin");
    registerField("Password*", _identity, "clearPassword");

    connect(_identity, SIGNAL(clearLoginEditionFinished()), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(passwordConfirmed()), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(titleChanged(QString)), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(usualNameChanged(QString)), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(otherNamesChanged(QString)), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(firstNameChanged(QString)), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(dateOfBirthChanged(QDate)), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(genderIndexChanged(int)), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(genderChanged(QString)), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(languageChanged(QString)), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(clearLoginChanged(QString)), this, SLOT(checkCompleteState()));
    connect(_identity, SIGNAL(clearPasswordChanged(QString)), this, SLOT(checkCompleteState()));
}

UserIdentityAndLoginPage::~UserIdentityAndLoginPage()
{
}


/** Check the current login. Return \e true if it can be used otherwise return false*/
bool UserIdentityAndLoginPage::checkLogin() const
{
    // user login must be unique in the FreeMedForms database
    // user login must be unique on the server
    // MySQL user names can be up to 16 characters long.
    const QString &login = _identity->currentClearLogin();
    if (login.length() < 6 || login.length() > 16 ) // FIXME : avoid magic number \sa void PasswordWidget::onLoginChanged(const QString &login)
        return false;

    if (userBase()->isLoginAlreadyExists(_identity->currentClearLogin())) {
        Utils::warningMessageBox(tr("Login error"), tr("Login already in use. Please select another login"));
        return false;
    }

    return true;
}

void UserIdentityAndLoginPage::checkCompleteState()
{
    if (checkLogin() && _identity->isIdentityValid(false)) {
        Q_EMIT completeChanged();
    }
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

QSize UserIdentityAndLoginPage::sizeHint() const
{
    return _identity->sizeHint();
}

QSize UserIdentityAndLoginPage::minimumSizeHint() const
{
    return _identity->minimumSizeHint();
}

void UserIdentityAndLoginPage::retranslate()
{
    setTitle(tr("Create a new user"));
    setSubTitle(tr("Please enter your identity."));
}

void UserIdentityAndLoginPage::initializePage()
{
    //FIXME: layout problematic in UserCreatorWizard: identity page does not correctly resize
    wizard()->resize(_identity->sizeHint() + QSize(100, 50));
}

bool UserIdentityAndLoginPage::isComplete() const
{
//    qWarning() << "isComplete" <<
//                  !_identity->currentUsualName().isEmpty()
//               << !_identity->currentFirstName().isEmpty()
//               << !_identity->currentGender().isEmpty()
//               << !_identity->currentLanguage().isEmpty()
//               << checkLogin()
//               << _identity->isPasswordCompleted();
    return (!_identity->currentUsualName().isEmpty()
            && !_identity->currentFirstName().isEmpty()
            && !_identity->currentGender().isEmpty()
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
