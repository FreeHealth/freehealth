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
/**
  \class UserPlugin::UserWizard
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

#include <printerplugin/printer.h>
#include <printerplugin/textdocumentextra.h>

#include <listviewplugin/stringlistview.h>
#include <listviewplugin/stringlistmodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
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
#include <QCheckBox>

using namespace UserPlugin;
using namespace Trans::ConstantTranslations;

static inline UserPlugin::UserModel *userModel() { return UserModel::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }


QHash<int, QString> UserWizard::m_Papers;
QHash<int, int> UserWizard::m_Rights;


UserWizard::UserWizard(QWidget *parent)
    : QWizard(parent),
      m_Row(-1),
      m_Saved(false),
      m_CreateUser(false)
{
    setPage(LanguageSelectorPage, new UserLanguageSelectorPage(this));
    setPage(LoginPasswordPage, new UserLoginPasswordPage(this));
    setPage(IdentityPage, new UserIdentityPage(this));
    setPage(AdressPage, new UserAdressPage(this));
    setPage(TelsAndMailPage, new UserTelsAndMailPage(this));
    setPage(ProfilPage, new UserProfilPage(this));
    setPage(SpecialiesQualificationsPage, new UserSpecialiesQualificationsPage(this));
    setPage(RightsPage, new UserRightsPage(this));
    setPage(PaperGenericPage, new UserPaperPage("Generic", PaperAdministrativePage, this));
    setPage(PaperAdministrativePage, new UserPaperPage("Administrative", PaperPrescriptionsPage, this));
    setPage(PaperPrescriptionsPage, new UserPaperPage("Prescription", -1, this));

    setWindowTitle(tr("User Creator Wizard"));
    QList<QWizard::WizardButton> layout;
    layout << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);
    //    setAttribute(Qt::WA_DeleteOnClose);
}

void UserWizard::done(int r)
{
    validateCurrentPage();
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
        idx = userModel()->index(m_Row, Core::IUser::Firstname);
        userModel()->setData(idx, field("Firstname"));
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

        foreach(int role, m_Rights.keys()) {
            idx = userModel()->index(m_Row, role);
            userModel()->setData(idx, m_Rights.value(role));
        }

        const QString &uuid = userModel()->index(m_Row, Core::IUser::Uuid).data().toString();
        QHashIterator<int, QString> it(m_Papers);
        while (it.hasNext()) {
             it.next();
             userModel()->setPaper(uuid, it.key(), Print::TextDocumentExtra::fromXml(it.value()));
         }


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

bool UserWizard::setCreatedUserAsCurrent() const
{
    if (!m_CreateUser)
        return false;
    return userModel()->setCurrentUser(loginForSQL(field("Login").toString()),
                                       crypt(field("Password").toString()));
}



UserLanguageSelectorPage::UserLanguageSelectorPage(QWidget *parent)
    : QWizardPage(parent), lbl(0)
{
    lbl = new QLabel(tr("Language"), this);
    retranslate();
    QComboBox * cbLanguage = new QComboBox(this);

    cbLanguage->addItems(Core::Translators::availableLocales());
    cbLanguage->setCurrentIndex(Core::Translators::availableLocales().indexOf(QLocale().name().left(2)));
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
    QLabel * lblFirstName = new QLabel(tr("Firstname"), this);
    QLabel * lblSecondName = new QLabel(tr("Second Name"), this);
    QLabel * lblGender = new QLabel(tr("Gender"), this);

    QLineEdit * leName = new QLineEdit(this);
    QLineEdit * leFirstName = new QLineEdit(this);
    QLineEdit * leSecondName = new QLineEdit(this);
    QComboBox * cbTitle = new QComboBox(this);
    QComboBox * cbGender = new QComboBox(this);

    registerField("Name", leName, "text");
    registerField("Firstname", leFirstName, "text");
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
    layout->addWidget(lblFirstName, 2, 0);
    layout->addWidget(leFirstName, 2, 1);
    layout->addWidget(lblSecondName, 3, 0);
    layout->addWidget(leSecondName, 3, 1);
    layout->addWidget(lblGender, 4, 0);
    layout->addWidget(cbGender, 4, 1);
    setLayout(layout);
}

bool UserIdentityPage::validatePage()
{
    if (field("Name").toString().isEmpty() || field("Firstname").toString().isEmpty()) {
        Utils::warningMessageBox(tr("Forbidden anonymous user."),
                                 tr("All users must have at least a name and a firstname.\n"
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
    setSubTitle(tr("This represents your professional address."));
    QLabel * lblAdress = new QLabel(tr("Address"), this);
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


UserProfilPage::UserProfilPage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Select a profil"));
    setSubTitle(tr("FreeMedForms allows you to create users using predefined profils. Select your profil and options."));

//    registerField("isMedical", new QWidget(this));
//    registerField("UserManager", new QWidget(this));
//    registerField("DrugsManager", new QWidget(this));
//    registerField("Medical", new QWidget(this));
//    registerField("Paramedical", new QWidget(this));
//    registerField("Administrative", new QWidget(this));

    Views::StringListModel *model = new Views::StringListModel(this);
    model->setStringList(QStringList() << tkTr(Trans::Constants::DOCTOR) << tr("Software administrator"));
    model->setCheckable(true);
    model->setReadOnly(true);
    view = new Views::StringListView(this);
    view->setModel(model);
    view->setActions(0);

    box = new QCheckBox(tr("Define all rights"), this);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(view, 0, 0);
    layout->addWidget(box, 1, 0);
}

bool UserProfilPage::validatePage()
{
    UserWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::DrugsRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::ParamedicalRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::AdministrativeRights, Core::IUser::NoRights);
    next = UserWizard::PaperGenericPage;
    QStringList result = view->getCheckedStringList().toStringList();
    if (result.contains(tkTr(Trans::Constants::DOCTOR))) {
        UserWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::NoRights);
        UserWizard::setUserRights(Core::IUser::DrugsRights, Core::IUser::AllRights);
        UserWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::AllRights);
        UserWizard::setUserRights(Core::IUser::ParamedicalRights, int(Core::IUser::ReadAll | Core::IUser::Print));
        UserWizard::setUserRights(Core::IUser::AdministrativeRights, Core::IUser::NoRights);
        next = UserWizard::SpecialiesQualificationsPage;
    }
    if (result.contains(tr("Software administrator"))) {
        UserWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::AllRights);
    }

    if (box->isChecked()) {
        next = UserWizard::RightsPage;
        return true;
    }
    return true;
}

UserSpecialiesQualificationsPage::UserSpecialiesQualificationsPage(QWidget *parent)
        : QWizardPage(parent)
{
    setTitle(tr("Define user's specialties and qualifications."));
    setSubTitle(tr("Use the context menu to add, remove, move up or down items."));
    QTabWidget *tab = new QTabWidget(this);

    QStringListModel * modelspe = new QStringListModel(this);
    Views::StringListView * speView = new Views::StringListView(this);
    speView->setModel(modelspe);
    QStringListModel * modelqual = new QStringListModel(this);
    Views::StringListView * quaView = new Views::StringListView(this);
    quaView->setModel(modelqual);
    QStringListModel * modelids = new QStringListModel(this);
    Views::StringListView * idsView = new Views::StringListView(this);
    idsView->setModel(modelids);

    tab->addTab(speView, tr("Specialities"));
    tab->addTab(quaView, tr("Qualifications"));
    tab->addTab(idsView, tr("Identifiants"));

    registerField("Specialities", speView, "stringList");
    registerField("Qualifications", quaView, "stringList");
    registerField("Identifiants", idsView, "stringList");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(tab, 0, 1);
    setLayout(layout);
}

UserRightsPage::UserRightsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Define user's rights."));
    setSubTitle(tr("Role by role, define the user's rights."));

    QTabWidget *tab = new QTabWidget(this);

    um = new Internal::UserRightsWidget(this);
    drugs = new Internal::UserRightsWidget(this);
    med = new Internal::UserRightsWidget(this);
    paramed = new Internal::UserRightsWidget(this);
    administ = new Internal::UserRightsWidget(this);

    tab->addTab(um, tr("Users"));
    tab->addTab(drugs, tr("Drugs"));
    tab->addTab(med, tr("Medicals"));
    tab->addTab(paramed, tr("Paramedicals"));
    tab->addTab(administ, tr("Administrative"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(tab, 0, 0);
    setLayout(layout);
}

void UserRightsPage::initializePage()
{
    um->setRights(UserWizard::userRights(Core::IUser::ManagerRights));
    drugs->setRights(UserWizard::userRights(Core::IUser::DrugsRights));
    med->setRights(UserWizard::userRights(Core::IUser::MedicalRights));
    paramed->setRights(UserWizard::userRights(Core::IUser::ParamedicalRights));
    administ->setRights(UserWizard::userRights(Core::IUser::AdministrativeRights));

}

bool UserRightsPage::validatePage()
{
    UserWizard::setUserRights(Core::IUser::ManagerRights, um->getRights());
    UserWizard::setUserRights(Core::IUser::DrugsRights, drugs->getRights());
    UserWizard::setUserRights(Core::IUser::MedicalRights, med->getRights());
    UserWizard::setUserRights(Core::IUser::ParamedicalRights, paramed->getRights());
    UserWizard::setUserRights(Core::IUser::AdministrativeRights, administ->getRights());
    return true;
}

static inline QString defaultHeader()
{
    return Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/default_user_header.htm");
}

static inline QString defaultFooter()
{
    return Utils::readTextFile(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/default_user_footer.htm");
}

UserPaperPage::UserPaperPage(const QString &paperName, int nextPage, QWidget *parent) :
        QWizardPage(parent), type(paperName), m_Next(nextPage)
{
    QString title;
    if (type=="Generic")
        title = tr("Generic");
    else if (type == "Prescription")
        title = tr("Prescription");
    else if (type == "Administrative")
            title = tr("Administrative");
    setTitle(tr("%1 headers and footers").arg(title));

    header = new Print::TextDocumentExtra;
    footer = new Print::TextDocumentExtra;
    wm = new Print::TextDocumentExtra;
    previewer = Print::Printer::previewer(this);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(previewer, 0, 0);
    setLayout(layout);

    header->setHtml(defaultHeader());
    footer->setHtml(defaultFooter());

    if (type=="Prescription") {
        wm->setPresence(Print::Printer::DuplicataOnly);
    }

    previewer->setHeader(header);
    previewer->setFooter(footer);
    previewer->setWatermark(wm);
}

bool UserPaperPage::validatePage()
{
    Print::TextDocumentExtra *tmp = new Print::TextDocumentExtra;
    int header, footer, wmk;
    if (type=="Generic") {
        header = Core::IUser::GenericHeader;
        footer = Core::IUser::GenericFooter;
        wmk = Core::IUser::GenericWatermark;
    } else if (type == "Prescription") {
        header = Core::IUser::PrescriptionHeader;
        footer = Core::IUser::PrescriptionFooter;
        wmk = Core::IUser::PrescriptionWatermark;
    } else if (type == "Administrative") {
        header = Core::IUser::AdministrativeHeader;
        footer = Core::IUser::AdministrativeFooter;
        wmk = Core::IUser::AdministrativeWatermark;
    }

    previewer->headerToPointer(tmp);
    UserWizard::setUserPaper(header, tmp->toXml());

    previewer->footerToPointer(tmp);
    UserWizard::setUserPaper(footer, tmp->toXml());

    previewer->watermarkToPointer(tmp);
    UserWizard::setUserPaper(wmk, tmp->toXml());

    return true;
}
