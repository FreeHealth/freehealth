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
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/database/userbase.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/translators.h>

#include <listviewplugin/languagecombobox.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/uppercasevalidator.h>
#include <translationutils/constanttranslations.h>

#include "ui_useridentityandloginpage.h"

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline UserPlugin::UserModel *userModel() { return UserModel::instance(); }

UserIdentityAndLoginPage::UserIdentityAndLoginPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::UserIdentityAndLoginPage),
    _showErrorLabels(false)
{
    ui->setupUi(this);
    toggleErrorLabels();

    ui->cbLanguage->setDisplayMode(Views::LanguageComboBox::AvailableTranslations);
    ui->cbLanguage->setCurrentLanguage(QLocale().language());

    Utils::UpperCaseValidator *val = new Utils::UpperCaseValidator(this);
    ui->leName->setValidator(val);
    ui->leSecondName->setValidator(val);
    Utils::CapitalizationValidator *firstval = new Utils::CapitalizationValidator(this);
    ui->leFirstName->setValidator(firstval);

    connect(ui->cbLanguage, SIGNAL(currentLanguageChanged(QLocale::Language)), Core::Translators::instance(), SLOT(changeLanguage(QLocale::Language)));

    registerField("Language", ui->cbLanguage , "currentLanguage");
    registerField("Name*", ui->leName);
    registerField("Firstname*", ui->leFirstName);
    registerField("SecondName", ui->leSecondName);
    registerField("Title", ui->cbTitle);
    registerField("Gender", ui->cbGender);

    // TODO: centralize login/password length with a constant!
    ui->leLogin->setValidator(new QRegExpValidator(QRegExp("^[a-zA-Z0-9\\.\\-_]{6,}"), this));
    ui->leLogin->setToolTip(tr("minimum: 6 characters\nonly characters and digits allowed"));
    ui->leLogin->setIcon(theme()->icon(Core::Constants::ICONEYES));

    // TODO: centralize login/password length with a constant!
    ui->lePassword->setValidator(new QRegExpValidator(QRegExp("^[a-zA-Z0-9\\.\\-_]{6,}"),this));
    ui->lePassword->setToolTip(tr("minimum: 6 characters"));
    ui->lePassword->toogleEchoMode();
    ui->lePassword->setIcon(theme()->icon(Core::Constants::ICONEYES));

    ui->lePasswordConfirm->toogleEchoMode();
    ui->lePasswordConfirm->setIcon(theme()->icon(Core::Constants::ICONEYES));

    registerField("Login*", ui->leLogin);
    registerField("Password*", ui->lePassword);
    registerField("ConfirmPassword*", ui->lePasswordConfirm);

    retranslate();

    connect(ui->leLogin, SIGNAL(textChanged(QString)), this, SLOT(checkLogin()));
//    connect(ui->leLogin, SIGNAL(editingFinished()), this, SLOT(checkLoginAfterEdition()));
    connect(ui->lePasswordConfirm, SIGNAL(textChanged(QString)), this, SLOT(checkControlPassword(QString)));
    connect(ui->lePassword, SIGNAL(textChanged(QString)), this, SLOT(checkControlPassword(QString)));

    // set right stylesheets to the labels
    checkControlPassword("");
    on_leName_textChanged("");
    on_leFirstName_textChanged("");
    on_leLogin_textChanged("");
}

UserIdentityAndLoginPage::~UserIdentityAndLoginPage()
{
    delete ui;
}

void UserIdentityAndLoginPage::checkLogin()
{
    // user login must be unique in the FreeMedForms database
    // user login must be unique on the server
    if (ui->leLogin->text().length() < 6) {
        ui->lblLogin->setStyleSheet("color:red;");
        ui->lblLogin->setToolTip(tr("You must specify a valid login. Login must be more than 6 characters."));
        ui->lblLoginError->setText(tr("You must specify a valid login. Login must be more than 6 characters."));
    } else if (UserBase::instance()->isLoginAlreadyExists(ui->leLogin->text())) {
        ui->lblLogin->setStyleSheet("color:red;");
        ui->lblLogin->setStyleSheet(tr("Login in use. Please select another login"));
        ui->lblLoginError->setText(tr("Login in use. Please select another login"));
    } else {
        ui->lblLogin->setStyleSheet(QString::null);
        ui->lblLogin->setStyleSheet(QString::null);
        ui->lblLoginError->clear();
    }
//    toggleErrorLabels();
}

void UserIdentityAndLoginPage::checkLoginAfterEdition()
{
    checkLogin();
    _showErrorLabels = true;
    toggleErrorLabels();
}

void UserIdentityAndLoginPage::checkControlPassword(const QString &text)
{
    Q_UNUSED(text);

    QString stylesheet = "color:red;";
    // TODO: centralize login/password length with a constant!

    // check wether both passwords meet the specifications
    if (ui->lePassword->text().length() >= 6 && ui->lePasswordConfirm->text().length() >= 6) {
        if (ui->lePasswordConfirm->text() == ui->lePassword->text()) { // congruent passwords
            stylesheet = "";
        }
    }
    ui->lblConfirmPassword->setStyleSheet(stylesheet);
    ui->lblPassword->setStyleSheet(stylesheet);
}

void UserIdentityAndLoginPage::on_leName_textChanged(const QString &text)
{
        ui->lblName->setStyleSheet(!text.isEmpty()? 0 : "color:red;");
}

void UserIdentityAndLoginPage::on_leFirstName_textChanged(const QString &text)
{
        ui->lblFirstName->setStyleSheet(!text.isEmpty()? 0 : "color:red;");
}

void UserIdentityAndLoginPage::on_leLogin_textChanged(const QString &text)
{
        ui->lblLogin->setStyleSheet(text.length() >= 6? 0 : "color:red;");
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

    if (ui->langLbl) {
        ui->langLbl->setText(tr("Language"));
        ui->lblTitle->setText(tkTr(Trans::Constants::TITLE));
        ui->lblName->setText(tkTr(Trans::Constants::NAME));
        ui->lblFirstName->setText(tr("First name"));
        ui->lblSecondName->setText(tr("Second name"));
        ui->lblGender->setText(tkTr(Trans::Constants::GENDER));
        ui->cbTitle->addItems(titles());
        ui->cbGender->addItems(genders());
        ui->lblLogin->setText(tkTr(Trans::Constants::LOGIN));
        ui->lblPassword->setText(tkTr(Trans::Constants::PASSWORD));
        ui->lblConfirmPassword->setText(tr("Confirm password"));
        ui->identGroup->setTitle(tr("Identity"));
        ui->logGroup->setTitle(tr("Database connection"));
    }
}

void UserIdentityAndLoginPage::toggleErrorLabels()
{
    WARN_FUNC << ui->lblLoginError->text();
    if (_showErrorLabels) {
        ui->lblLoginError->setVisible(!ui->lblLoginError->text().isEmpty());
        ui->lblPasswordError->setVisible(!ui->lblPasswordError->text().isEmpty());
    } else {
        ui->lblLoginError->setVisible(false);
        ui->lblPasswordError->setVisible(false);
    }
}

bool UserIdentityAndLoginPage::validatePage()
{
    if (field("Name").toString().isEmpty() || field("Firstname").toString().isEmpty()) {
        Utils::warningMessageBox(tr("Forbidden anonymous user."),
                                 tr("All users must have at least a name and a first name.\n"
                                    "You can not proceed with an anonymous user."), "",
                                 tr("Forbidden anonymous user."));
        return false;
    }
    if (field("Password").toString() != field("ConfirmPassword")) {
        Utils::warningMessageBox(tr("Password confirmation error."),
                                 tr("You must correctly confirm your password to go through this page."),
                                 "", tr("Wrong password"));
        return false;
    }
    if (field("Login").toString().isEmpty()) {
        Utils::warningMessageBox(tr("Login error."),
                                 tr("You must specify a valid login. An empty login is forbidden."),
                                 "", tr("Wrong login"));
        return false;
    }
    if (field("Login").toString().size() < 6) {
        Utils::warningMessageBox(tr("Login error."),
                                 tr("You must specify a valid login. Login must be more than 6 characters."),
                                 "", tr("Wrong login"));
        return false;
    }

    // log/pass already used ?
    if (userModel()->isCorrectLogin(field("Login").toString(),
                                    field("Password").toString())) {
        Utils::warningMessageBox(tr("Login and password already used"),
                                 tr("The users' database already contains the same login/password couple.\n"
                                    "You must specify a different login/password."),
                                 "", tr("Login/Password already used"));
        return false;
    }
    return true;
}
