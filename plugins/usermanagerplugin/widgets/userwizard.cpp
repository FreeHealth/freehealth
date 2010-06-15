/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
/**
  \class UserWizard
  \brief Wizard for user creation.
  You can tell tkWiz to create the user itself or use a defined user. createUser() define the
  creation mode. If you set it to false, inform the row of the model to use with setModelRow(). By default,
  tkUserWizard create itself a new user.\n
  Usage :
  \code
    UserWizard wiz;
    wiz.createUser(true);   // will create the user (optionnal)
    wiz.show();
  \endcode

  \todo If row is defined --> populate all wizard pages with users values.
  \ingroup usertoolkit widget_usertoolkit usermanager
*/

#include "userwizard.h"

#include <texteditorplugin/texteditor.h>

#include <listviewplugin/stringlistview.h>

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/widgets/lineeditechoswitcher.h>

#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/widgets/userrightswidget.h>

#include <translationutils/constanttranslations.h>

#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QCoreApplication>
#include <QEvent>
#include <QPushButton>
#include <QList>
#include <QStringListModel>

using namespace UserPlugin;
using namespace Trans::ConstantTranslations;

static inline UserPlugin::UserModel *userModel() { return UserModel::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

UserWizard::UserWizard(QWidget *parent)
    : QWizard(parent),
      m_Row(-1),
      m_Saved(false),
      m_CreateUser(false)
{
    addPage(new UserLanguageSelectorPage);
    addPage(new UserLoginPasswordPage);
    addPage(new UserIdentityPage);
    addPage(new UserAdressPage);
    addPage(new UserTelsAndMailPage);
    addPage(new UserSpecialiesQualificationsPage);
    addPage(new UserRightsPage);
    addPage(new UserGenericPage);
    addPage(new UserAdministrativePage);
    addPage(new UserPrescriptionsPage);
    this->setWindowTitle(tr("User Creator Wizard"));
    QList<QWizard::WizardButton> layout;
    layout << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);
    //    setAttribute(Qt::WA_DeleteOnClose);
}

void UserWizard::done(int r)
{
    if (r == QDialog::Rejected) {
        m_Saved = false;
        bool yes = Utils::yesNoMessageBox(tr("WARNING ! You don't save this user."),
                               tr("If you continue changes will be lost.\n"
                                  "Do you really want to close this dialog ?"),
                               "", tr("Data losing"));
        if (yes)
            QDialog::done(r);
    } else if (m_Saved) {
        QDialog::done(r);
    } else {
        if ((m_CreateUser) || (m_Row == -1)) {
            m_Row = userModel()->rowCount();
            if (!userModel()->insertRows(m_Row, 1)) {
                Utils::warningMessageBox(tr("An error occured during database access."),
                                             tr("Logged errors saved. Please refer to the %1 to manage this error.")
                                       .arg(Utils::Log::saveLog()),
                                       "", tr("Error during database access"));
                QDialog::done(QDialog::Rejected);
            }
        }
        QModelIndex idx;
        idx = userModel()->index(m_Row, Core::IUser::DecryptedLogin);
        userModel()->setData(idx, field("Login"));
        idx = userModel()->index(m_Row, Core::IUser::Password);
        userModel()->setData(idx, UserPlugin::crypt(field("Password").toString()));

        idx = userModel()->index(m_Row, Core::IUser::Name);
        userModel()->setData(idx, field("Name"));
        idx = userModel()->index(m_Row, Core::IUser::SecondName);
        userModel()->setData(idx, field("SecondName"));
        idx = userModel()->index(m_Row, Core::IUser::Surname);
        userModel()->setData(idx, field("Surname"));
        idx = userModel()->index(m_Row, Core::IUser::Title);
        userModel()->setData(idx, field("Title"));
        idx = userModel()->index(m_Row, Core::IUser::Gender);
        userModel()->setData(idx, field("Gender"));

        idx = userModel()->index(m_Row, Core::IUser::Adress);
        userModel()->setData(idx, field("Adress"));
        idx = userModel()->index(m_Row, Core::IUser::Zipcode);
        userModel()->setData(idx, field("Zipcode"));
        idx = userModel()->index(m_Row, Core::IUser::City);
        userModel()->setData(idx, field("City"));
        idx = userModel()->index(m_Row, Core::IUser::Country);
        userModel()->setData(idx, field("Country"));
        idx = userModel()->index(m_Row, Core::IUser::LanguageIndex);
        userModel()->setData(idx, field("Language"));

        idx = userModel()->index(m_Row, Core::IUser::Tel1);
        userModel()->setData(idx, field("Tel1"));
        idx = userModel()->index(m_Row, Core::IUser::Tel2);
        userModel()->setData(idx, field("Tel2"));
        idx = userModel()->index(m_Row, Core::IUser::Tel3);
        userModel()->setData(idx, field("Tel3"));
        idx = userModel()->index(m_Row, Core::IUser::Fax);
        userModel()->setData(idx, field("Fax"));
        idx = userModel()->index(m_Row, Core::IUser::Mail);
        userModel()->setData(idx, field("Mail"));

        idx = userModel()->index(m_Row, Core::IUser::Specialities);
        userModel()->setData(idx, field("Specialities"));
        idx = userModel()->index(m_Row, Core::IUser::Qualifications);
        userModel()->setData(idx, field("Qualifications"));
        idx = userModel()->index(m_Row, Core::IUser::PractitionerId);
        userModel()->setData(idx, field("Identifiants"));

        idx = userModel()->index(m_Row, Core::IUser::ManagerRights);
        userModel()->setData(idx, field("UserManager"));
        idx = userModel()->index(m_Row, Core::IUser::DrugsRights);
        userModel()->setData(idx, field("DrugsManager"));
        idx = userModel()->index(m_Row, Core::IUser::MedicalRights);
        userModel()->setData(idx, field("Medical"));
        idx = userModel()->index(m_Row, Core::IUser::ParamedicalRights);
        userModel()->setData(idx, field("Paramedical"));
        idx = userModel()->index(m_Row, Core::IUser::AdministrativeRights);
        userModel()->setData(idx, field("Administrative"));

        idx = userModel()->index(m_Row, Core::IUser::GenericHeader);
        userModel()->setData(idx, field("GenericHeader"));
        idx = userModel()->index(m_Row, Core::IUser::GenericFooter);
        userModel()->setData(idx, field("GenericFooter"));

        idx = userModel()->index(m_Row, Core::IUser::PrescriptionHeader);
        userModel()->setData(idx, field("PrescrHeader"));
        idx = userModel()->index(m_Row, Core::IUser::PrescriptionFooter);
        userModel()->setData(idx, field("PrescrFooter"));

        idx = userModel()->index(m_Row, Core::IUser::AdministrativeHeader);
        userModel()->setData(idx, field("AdminHeader"));
        idx = userModel()->index(m_Row, Core::IUser::AdministrativeFooter);
        userModel()->setData(idx, field("AdminFooter"));

#ifdef DEBUG
        // warn user
        idx = userModel()->index(m_Row, Core::IUser::Warn);
        idx.data();
#endif

        if (userModel()->submitRow(m_Row)) {
            Utils::informativeMessageBox(tr("User correctly saved into database."),
                                             tr("You can poursue with the current user %1 or set this new user to current user.")
                                             .arg(userModel()->currentUserData(Core::IUser::Name).toString()),
                                             "", tr("User correctly saved into database."));
            m_Saved = true;
            QDialog::done(r);
        } else {
            userModel()->removeRows(m_Row, 1);
            Utils::warningMessageBox(tr("An error occured during database access."),
                                         tr("Logged errors saved. Please refer to the %1 to manage this error.")
                                         .arg(Utils::Log::saveLog()),
                                         "", tr("Error during database access"));
            QDialog::done(r);
            m_Saved = false;
        }
    }
}

UserLanguageSelectorPage::UserLanguageSelectorPage(QWidget *parent)
    : QWizardPage(parent), lbl(0)
{
    lbl = new QLabel(tr("Language"), this);
    retranslate();
    QComboBox * cbLanguage = new QComboBox(this);

    cbLanguage->addItems(Core::Translators::availableLocales());
    cbLanguage->setCurrentIndex(Core::Translators::availableLocales().indexOf("en"));
    connect(cbLanguage, SIGNAL(activated(QString)), Core::Translators::instance(), SLOT(changeLanguage(const QString &)));

    registerField("Language", cbLanguage , "currentIndex");

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(lbl, 0, 0);
    layout->addWidget(cbLanguage, 0, 1);
    setLayout(layout);
}

void UserLanguageSelectorPage::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
        retranslate();
}

void UserLanguageSelectorPage::retranslate()
{
    setTitle(tr("Please set your language."));
    setSubTitle(tr("Whole application will be translated to this language, forms too (when possible).\n"
                     "For instance, FreeMedForms is available in few languages.\n"
                     "New translations are planned."));
    if (lbl)
        lbl->setText(tr("Language"));
}

UserIdentityPage::UserIdentityPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Please enter your identity."));
    QLabel * lblTitle = new QLabel(tr("Title"), this);
    QLabel * lblName = new QLabel(tr("Name"), this);
    QLabel * lblSurName = new QLabel(tr("Surname"), this);
    QLabel * lblSecondName = new QLabel(tr("Second Name"), this);
    QLabel * lblGender = new QLabel(tr("Gender"), this);

    QLineEdit * leName = new QLineEdit(this);
    QLineEdit * leSurName = new QLineEdit(this);
    QLineEdit * leSecondName = new QLineEdit(this);
    QComboBox * cbTitle = new QComboBox(this);
    QComboBox * cbGender = new QComboBox(this);

    registerField("Name", leName, "text");
    registerField("Surname", leSurName, "text");
    registerField("SecondName", leSecondName, "text");
    registerField("Title", cbTitle, "currentIndex");
    registerField("Gender", cbGender, "currentIndex");

    cbTitle->addItems(titles());
    cbGender->addItems(genders());

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lblTitle, 0, 0);
    layout->addWidget(cbTitle, 0, 1);
    layout->addWidget(lblName, 1, 0);
    layout->addWidget(leName, 1, 1);
    layout->addWidget(lblSurName, 2, 0);
    layout->addWidget(leSurName, 2, 1);
    layout->addWidget(lblSecondName, 3, 0);
    layout->addWidget(leSecondName, 3, 1);
    layout->addWidget(lblGender, 4, 0);
    layout->addWidget(cbGender, 4, 1);
    setLayout(layout);
}

bool UserIdentityPage::validatePage()
{
    if (field("Name").toString().isEmpty() || field("Surname").toString().isEmpty()) {
        Utils::warningMessageBox(tr("Forbidden anonymous user."),
                                 tr("All users must have at least a name and a surname.\n"
                                    "You can not poursue with an anonymous user."), "",
                                 tr("Forbidden anonymous user."));
        return false;
    }
    return true;
}

UserLoginPasswordPage::UserLoginPasswordPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Please enter your login and password."));
    QLabel * lblL = new QLabel(tr("Login"), this);
    QLabel * lblP = new QLabel(tr("Password"), this);
    QLabel * lblCP = new QLabel(tr("Confirm Password"), this);

    leLogin = new Utils::LineEditEchoSwitcher(this);
    lePassword = new Utils::LineEditEchoSwitcher(this);
    lePasswordConfirm = new Utils::LineEditEchoSwitcher(this);
    leLogin->toogleEchoMode();
    lePassword->toogleEchoMode();
    lePasswordConfirm->toogleEchoMode();
    leLogin->setIcon(theme()->icon(Core::Constants::ICONEYES));
    lePassword->setIcon(theme()->icon(Core::Constants::ICONEYES));
    lePasswordConfirm->setIcon(theme()->icon(Core::Constants::ICONEYES));

    registerField("Login", leLogin, "text");
    registerField("Password", lePassword, "text");
    registerField("ConfirmPassword", lePasswordConfirm, "text");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lblL, 0, 0);
    layout->addWidget(leLogin, 0, 1);
    layout->addWidget(lblP, 1, 0);
    layout->addWidget(lePassword, 1, 1);
    layout->addWidget(lblCP, 2, 0);
    layout->addWidget(lePasswordConfirm, 2, 1);
    setLayout(layout);
}

bool UserLoginPasswordPage::validatePage()
{
    if (field("Password").toString() != field("ConfirmPassword")) {
        Utils::warningMessageBox(tr("Password confirmation error."),
                                 tr("You must correctly confirm your password to go throw this page."),
                                 "", tr("Wrong password"));
        return false;
    }
    if (field("Login").toString().isEmpty()) {
        Utils::warningMessageBox(tr("Login error."),
                                 tr("You must specify a valid login. Empty login is forbidden."),
                                 "", tr("Wrong login"));
        return false;
    }
    // log/pass already used ?
    if (userModel()->isCorrectLogin(UserPlugin::loginForSQL(field("Login").toString()),
                            UserPlugin::crypt(field("Password").toString()))) {
        Utils::warningMessageBox(tr("Login and password already used"),
                                 tr("The users' database already contains the same login/password couple.\n"
                                    "You must specify a different login/password."),
                                 "", tr("Login/Password already used"));
        return false;
    }
    return true;
}

UserAdressPage::UserAdressPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Please enter your complete adress."));
    setSubTitle(tr("This represents your professionnal adress."));
    QLabel * lblAdress = new QLabel(tr("Adress"), this);
    QLabel * lblCity = new QLabel(tr("City"), this);
    QLabel * lblZipcode = new QLabel(tr("Zipcode"), this);
    QLabel * lblCountry = new QLabel(tr("Country"), this);

    QTextEdit * teAdress = new QTextEdit(this);
    QLineEdit * leCity = new QLineEdit(this);
    QLineEdit * leZipcode = new QLineEdit(this);
    QLineEdit * leCountry = new QLineEdit(this);

    registerField("Adress", teAdress , "plainText");
    registerField("City", leCity , "text");
    registerField("Zipcode", leZipcode , "text");
    registerField("Country", leCountry , "text");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lblAdress, 0, 0);
    layout->addWidget(teAdress, 0, 1);
    layout->addWidget(lblCity, 1, 0);
    layout->addWidget(leCity, 1, 1);
    layout->addWidget(lblZipcode, 2, 0);
    layout->addWidget(leZipcode, 2, 1);
    layout->addWidget(lblCountry, 3, 0);
    layout->addWidget(leCountry, 3, 1);
    setLayout(layout);
}

UserTelsAndMailPage::UserTelsAndMailPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Please, enter tels, fax and mail adress."));
    QLabel * lblT1 = new QLabel(tr("Tel1"), this);
    QLabel * lblT2 = new QLabel(tr("Tel2"), this);
    QLabel * lblT3 = new QLabel(tr("Tel3"), this);
    QLabel * lblFax = new QLabel(tr("Fax"), this);
    QLabel * lblMail = new QLabel(tr("Mail"), this);

    QLineEdit * leT1 = new QLineEdit(this);
    QLineEdit * leT2 = new QLineEdit(this);
    QLineEdit * leT3 = new QLineEdit(this);
    QLineEdit * leFax = new QLineEdit(this);
    QLineEdit * leMail = new QLineEdit(this);

    registerField("Tel1", leT1, "text");
    registerField("Tel2", leT2, "text");
    registerField("Tel3", leT3, "text");
    registerField("Fax", leFax, "text");
    registerField("Mail", leMail, "text");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lblT1, 0, 0);
    layout->addWidget(leT1, 0, 1);
    layout->addWidget(lblT2, 1, 0);
    layout->addWidget(leT2, 1, 1);
    layout->addWidget(lblT3, 2, 0);
    layout->addWidget(leT3, 2, 1);
    layout->addWidget(lblFax, 3, 0);
    layout->addWidget(leFax, 3, 1);
    layout->addWidget(lblMail, 4, 0);
    layout->addWidget(leMail, 4, 1);
    setLayout(layout);
}

bool UserTelsAndMailPage::validatePage()
{
    // TODO check mail and tel/fax formatting
    return true;
}

UserSpecialiesQualificationsPage::UserSpecialiesQualificationsPage(QWidget *parent)
        : QWizardPage(parent)
{
    setTitle(tr("Define user's specialties and qualifications."));
    setSubTitle(tr("Use the context menu to add, remove, move up or down items."));
    QLabel * lblSpe = new QLabel(tr("Specialities"), this);
    QLabel * lblQua = new QLabel(tr("Qualifications"), this);
    QLabel * lblId = new QLabel(tr("Identifiants"), this);

    QStringListModel * modelspe = new QStringListModel(this);
    Views::StringListView * speView = new Views::StringListView(this);
    speView->setModel(modelspe);
    QStringListModel * modelqual = new QStringListModel(this);
    Views::StringListView * quaView = new Views::StringListView(this);
    quaView->setModel(modelqual);
    QStringListModel * modelids = new QStringListModel(this);
    Views::StringListView * idsView = new Views::StringListView(this);
    idsView->setModel(modelids);

    registerField("Specialities", speView, "stringList");
    registerField("Qualifications", quaView, "stringList");
    registerField("Identifiants", idsView, "stringList");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lblSpe, 0, 1);
    layout->addWidget(speView, 1, 1);
    layout->addWidget(lblQua, 2, 1);
    layout->addWidget(quaView, 3, 1);
    layout->addWidget(lblId, 4, 1);
    layout->addWidget(idsView, 5, 1);
    setLayout(layout);
}

UserRightsPage::UserRightsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Define user's rights."));
    setSubTitle(tr("Role by role, define the user's rights."));
    QLabel *lblUM = new QLabel(tr("User Management"), this);
    QLabel *lblDrugs = new QLabel(tr("Drugs dosages Management"), this);
    QLabel *lblMed = new QLabel(tr("Medicals"), this);
    QLabel *lblUParaMed = new QLabel(tr("Paramedicals"), this);
    QLabel *lblUAdminist = new QLabel(tr("Administrative"), this);
    QFont bold;
    bold.setBold(true);
    lblUM->setFont(bold);
    lblDrugs->setFont(bold);
    lblMed->setFont(bold);
    lblUParaMed->setFont(bold);
    lblUAdminist->setFont(bold);

    Internal::UserRightsWidget * um = new Internal::UserRightsWidget(this);
    Internal::UserRightsWidget * drugs = new Internal::UserRightsWidget(this);
    Internal::UserRightsWidget * med = new Internal::UserRightsWidget(this);
    Internal::UserRightsWidget * paramed = new Internal::UserRightsWidget(this);
    Internal::UserRightsWidget * administ = new Internal::UserRightsWidget(this);

    registerField("UserManager", um, "rights");
    registerField("DrugsManager", drugs, "rights");
    registerField("Medical", med, "rights");
    registerField("Paramedical", paramed, "rights");
    registerField("Administrative", administ, "rights");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lblUM, 0, 0);
    layout->addWidget(um, 1, 0);
    layout->addWidget(lblMed, 2, 0);
    layout->addWidget(med, 3, 0);
    layout->addWidget(lblDrugs, 4, 0);
    layout->addWidget(drugs, 5, 0);
    layout->addWidget(lblUParaMed, 6, 0);
    layout->addWidget(paramed, 7, 0);
    layout->addWidget(lblUAdminist, 8, 0);
    layout->addWidget(administ, 9, 0);
    setLayout(layout);
}

UserGenericPage::UserGenericPage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Setup generic headers and footers"));
    setSubTitle(tr("You can define three types of headers and footers : one to use by default, "
                    "one for the prescriptions, one for administrative correspondance.\n"
                    "These header and footer will be used by default while printing.\n"));
    QLabel * lblH = new QLabel(tr("Header"), this);
    QLabel * lblF = new QLabel(tr("Footer"), this);

    /** \todo use tkPrinterPreviewer instead of Editor::TextEditors */
    Editor::TextEditor * head = new Editor::TextEditor(this);
    Editor::TextEditor * foot = new Editor::TextEditor(this);

    registerField("GenericHeader", head, "html");
    registerField("GenericFooter", foot, "html");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lblH, 0, 0);
    layout->addWidget(head, 1, 0);
    layout->addWidget(lblF, 2, 0);
    layout->addWidget(foot, 3, 0);
    setLayout(layout);
}

UserPrescriptionsPage::UserPrescriptionsPage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Setup prescriptions headers and footers"));
    setSubTitle(tr("You can define three types of headers and footers : one to use by default, "
                    "one for the prescriptions, one for administrative correspondance.\n"
                    "These header and footer will be used by default while printing prescriptions.\n"));
    QLabel * lblH = new QLabel(tr("Header"), this);
    QLabel * lblF = new QLabel(tr("Footer"), this);

    /** \todo use Print::PrinterPreviewer instead of Editor::TextEditors */
    Editor::TextEditor * head = new Editor::TextEditor(this);
    Editor::TextEditor * foot = new Editor::TextEditor(this);

    registerField("PrescrHeader", head, "html");
    registerField("PrescrFooter", foot, "html");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lblH, 0, 0);
    layout->addWidget(head, 1, 0);
    layout->addWidget(lblF, 2, 0);
    layout->addWidget(foot, 3, 0);
    setLayout(layout);
}

UserAdministrativePage::UserAdministrativePage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Setup administrative headers and footers"));
    setSubTitle(tr("You can define three types of headers and footers : one to use by default, "
                    "one for the prescriptions, one for administrative correspondance.\n"
                    "These header and footer will be used by default while printing administrative correspondance.\n"));
    QLabel * lblH = new QLabel(tr("Header"), this);
    QLabel * lblF = new QLabel(tr("Footer"), this);

    /** \todo use tkPrinterPreviewer instead of Editor::TextEditors */
    Editor::TextEditor * head = new Editor::TextEditor(this);
    Editor::TextEditor * foot = new Editor::TextEditor(this);

    registerField("AdminHeader", head, "html");
    registerField("AdminFooter", foot, "html");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lblH, 0, 0);
    layout->addWidget(head, 1, 0);
    layout->addWidget(lblF, 2, 0);
    layout->addWidget(foot, 3, 0);
    setLayout(layout);
}
