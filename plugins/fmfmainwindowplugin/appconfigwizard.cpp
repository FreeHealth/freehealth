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
#include "appconfigwizard.h"
#include "virtualdatabasepreferences.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants.h>
#include <coreplugin/ioptionspage.h>
#include <coreplugin/iuser.h>

#include <formmanagerplugin/formfilesselectorwidget.h>
#include <formmanagerplugin/episodebase.h>
#include <formmanagerplugin/iformio.h>

#include <usermanagerplugin/widgets/userwizard.h>
#include <usermanagerplugin/widgets/userpassworddialog.h>
#include <usermanagerplugin/constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/updatechecker.h>

#include <extensionsystem/pluginmanager.h>

#include <QComboBox>
#include <QLabel>
#include <QGridLayout>

using namespace MainWin;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() {return Core::ICore::instance()->user();}
static inline Form::Internal::EpisodeBase *episodeBase() {return Form::Internal::EpisodeBase::instance();}

AppConfigWizard::AppConfigWizard(QWidget *parent)
    : QWizard(parent)
{
    addPage(new BeginConfigPage(this));
    addPage(new CreateNewUserPage(this));
    addPage(new DatabaseConfigurationPage(this));
    addPage(new PatientFilePage(this));
    addPage(new VirtualDatabasePage(this));
    addPage(new EndConfigPage(this));
    this->setWindowTitle(tr("Application Configurator Wizard"));
    QList<QWizard::WizardButton> layout;
    layout << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);

    // Delete the login information in settings
    settings()->setValue(Core::Constants::S_LASTLOGIN, QVariant());
    settings()->setValue(Core::Constants::S_LASTPASSWORD, QVariant());
}

void AppConfigWizard::done(int r)
{
    if (r == QDialog::Rejected) {
//        bool yes = Utils::yesNoMessageBox(tr("WARNING ! You don't save this user."),
//                               tr("If you continue changes will be lost.\n"
//                                  "Do you really want to close this dialog ?"),
//                               "", tr("Data losing"));
//        if (yes)
            QDialog::done(r);
    } else {

            QDialog::done(r);
    }
}

BeginConfigPage::BeginConfigPage(QWidget *parent)
    : QWizardPage(parent)
{
    langLabel = new QLabel(this);
    retranslate();

    QComboBox *cbLanguage = new QComboBox(this);
    cbLanguage->addItems(Core::Translators::availableLocales());
    int actual = Core::Translators::availableLocales().indexOf(QLocale().name().left(2));
    if (actual == -1) {
        cbLanguage->setCurrentIndex(0);
    } else {
        cbLanguage->setCurrentIndex(actual);
    }
    /** \todo connection here to retranslate (changeEvent())... */
    connect(cbLanguage, SIGNAL(activated(QString)), Core::Translators::instance(), SLOT(changeLanguage(const QString &)));

    registerField("Language", cbLanguage , "currentIndex");

    QGridLayout *layout = new QGridLayout(this);
    layout->setVerticalSpacing(30);
    layout->addWidget(langLabel, 2, 0);
    layout->addWidget(cbLanguage, 2, 1);
    setLayout(layout);
}

void BeginConfigPage::retranslate()
{
    setTitle(tr("Welcome to FreeMedForms"));
    setSubTitle(tr("<b>Welcome to FreeMedForms</b><br /><br />"
                   "This wizard will help you to configure the base parameters of the application.<br />"
                   "At any time, you can cancel this wizard, the default values will be activated "
                   "for the undefined parameters."));
    langLabel->setText(tr("Select your language"));
}

bool BeginConfigPage::validatePage()
{
    // if user is admin && password == "admin"
    if ((user()->value(Core::IUser::Login).toString()==UserPlugin::Constants::DEFAULT_USER_LOGIN) &&
        (user()->value(Core::IUser::Password).toString()==UserPlugin::Constants::DEFAULT_USER_PASSWORD)) {
        // ask for a new password
        UserPlugin::UserPasswordDialog dlg(user()->value(Core::IUser::Password).toString(), this);
        dlg.changeTitle(tr("You must redefine the default administrator's password"));
        dlg.exec();
        if (dlg.canGetNewPassword())
            user()->setValue(Core::IUser::Password, dlg.cryptedPassword());
    }
    return true;
}


CreateNewUserPage::CreateNewUserPage(QWidget *parent) :
        QWizardPage(parent), passredefined(false)
{
    setTitle(tr("Create a new user and redefine admin's password."));
    setSubTitle(tr("It is recommended to create a new user instead of using the default one.\n"
                   "The default administrator's password must be redefined here."));

    QPushButton *but = new QPushButton(tr("Click here to create a new user"), this);
    newUserName = new QLabel(" ", this);
    connect(but, SIGNAL(clicked()), this, SLOT(createNewUser()));

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(but, 0, 0);
    layout->addWidget(newUserName, 1, 0);
    setLayout(layout);
}

void CreateNewUserPage::createNewUser()
{
    UserPlugin::UserWizard wiz(this);
    wiz.createUser(true);
    if (wiz.exec()==QDialog::Accepted) {
        if (!wiz.setCreatedUserAsCurrent()) {
            Utils::Log::addError(this, "Can not define the current user to the newly created");
            newUserName->setText(tr("Can not define the current user to the newly created"));
        } else {
            newUserName->setText(tr("New user created."));
        }
    }
}

bool CreateNewUserPage::validatePage()
{
    return true;
}

DatabaseConfigurationPage::DatabaseConfigurationPage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Update checking and database configuration"));
    setSubTitle(tr("By default, FreeMedForms is configured to use a local SQLite database. You can choose an external server."));

    // get all optionpages in "general" category
    QWidget *w = 0;
    pages = ExtensionSystem::PluginManager::instance()->getObjects<Core::IOptionsPage>();
    foreach(Core::IOptionsPage *p, pages) {
        if (p->name()==tkTr(Trans::Constants::GENERAL))
            w = p->createPage(this);
    }
    QHBoxLayout *l = new QHBoxLayout(this);
    l->addWidget(w);
}

DatabaseConfigurationPage::~DatabaseConfigurationPage()
{
}

bool DatabaseConfigurationPage::validatePage()
{
    foreach(Core::IOptionsPage *p, pages) {
        if (p->name()==tkTr(Trans::Constants::GENERAL)) {
            p->applyChanges();
            p->finish();
        }
    }
    return true;
}


PatientFilePage::PatientFilePage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Patients Forms File"));
    setSubTitle(tr("FreeMedForms allows you to define your own patient forms file. You can select it from here. All patients will have the same forms."));

    selector = new Form::FormFilesSelectorWidget(this);
    selector->setFormType(Form::FormFilesSelectorWidget::CompleteForms);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(selector, 0, 0);
    setLayout(layout);
}

bool PatientFilePage::validatePage()
{
    if (!selector->selectedForms().count())
        return false;
    Form::FormIODescription *descr = selector->selectedForms().at(0);
    return episodeBase()->setGenericPatientFormFile(descr->data(Form::FormIODescription::UuidOrAbsPath).toString());
}


VirtualDatabasePage::VirtualDatabasePage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Create Virtual Patients"));
    setSubTitle(tr("You can create some virtual patients and users in order to test FreeMedForms.\n"
                   "Just select the number of patients/users you want to create and click on the "
                   "'populate' button."));

    vd = new Internal::VirtualDatabasePreferences(this);
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setSpacing(0);
    l->setMargin(0);
    l->addWidget(vd);
}

void VirtualDatabasePage::initializePage()
{
    vd->writeDefaultSettings(0);
}


EndConfigPage::EndConfigPage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("FreeMedForms is now configured"));
    setSubTitle(tr("Please read the user's manual. If you have any question, you can ask them to the mailing list."));
    QLabel *lbl = new QLabel(tr("French/english mailing list : <a href=\"mailto:freemedforms@googlegroups.com\">freemedforms@googlegroups.com</a>"), this);
    lbl->setOpenExternalLinks(true);
}
