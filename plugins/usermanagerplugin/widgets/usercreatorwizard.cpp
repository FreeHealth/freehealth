/***************************************************************************
 * The FreeMedForms project is a set of free, open source medical         *
 * applications.                                                          *
 * (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 * All rights reserved.                                                   *
 *                                                                        *
 * This program is free software: you can redistribute it and/or modify   *
 * it under the terms of the GNU General Public License as published by   *
 * the Free Software Foundation, either version 3 of the License, or      *
 * (at your option) any later version.                                    *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program (COPYING file).                   *
 * If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *      Jerome Pinguet <jerome@jerome.cc>                                                *
 *      NAME <MAIL@Address>                                                *
 ***************************************************************************/
/**
 * \class UserPlugin::UserCreatorWizard
 * \brief Wizard for user creation.
 * Creating new user:\n
 * You use the UserPlugin::UserCreatorWizard to create a new user.
 * The wizard is directly connected to the UserPlugin::Internal::UserBase and
 * request a reset of the UserPlugin::UserCore::userModel() when the user
 * is saved in the database.\n
 *
 * Checking user rights: \n
 * Before creating this wizard you can use the checkUserRights() to
 * check if the current has the rights to create a new user.
 *
 * Extending the wizard: \n
 * You can extend the wizard with the UserPlugin::IUserWizardPage interface.
 * The wizard will get all objects from the plugin manager object pool
 * and will include theses pages.\n
 *
 * Usage :
 * \code
 *   UserCreatorWizard wiz;
 *   wiz.createUser(true);   // will create the user (optional)
 *   wiz.show();
 * \endcode
 */

#include "usercreatorwizard.h"
#include <usermanagerplugin/userdata.h>
#include <usermanagerplugin/database/userbase.h>

#include <texteditorplugin/texteditor.h>

#include <listviewplugin/stringlistview.h>
#include <listviewplugin/stringlistmodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>

#include <usermanagerplugin/usercore.h>
#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/widgets/userrightswidget.h>
#include <usermanagerplugin/iuserwizardpage.h>

#include <usermanagerplugin/widgets/useridentityandloginpage.h>
#include <listviewplugin/languagecombobox.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/passwordandlogin.h>
#include <translationutils/constanttranslations.h>
#include <extensionsystem/pluginmanager.h>

#include <QGridLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QCoreApplication>
#include <QEvent>
#include <QPushButton>
#include <QList>
#include <QCheckBox>
#include <QGroupBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringListModel>

#include "ui_userwizardcontactwidget.h"

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline UserPlugin::UserCore &userCore() {return UserPlugin::UserCore::instance();}
static inline UserPlugin::UserModel *userModel() {return userCore().userModel();}
static inline UserPlugin::Internal::UserBase *userBase() {return userCore().userBase();}

namespace {
class UserContactPage: public QWizardPage
{
    Q_DECLARE_TR_FUNCTIONS(UserContactPage)
public:
    UserContactPage(QWidget *parent = 0);
    ~UserContactPage();

private:
    Ui::UserWizardContactWidget *ui;
};

class UserProfilePage : public QWizardPage
{
    Q_DECLARE_TR_FUNCTIONS(UserProfilePage)
public:
    UserProfilePage(QWidget *parent = 0);
    bool validatePage();

    int nextId() const {return next;}

private:
    Views::StringListView *view;
    QCheckBox *box;
    int next;
};

class UserSpecialiesQualificationsPage: public QWizardPage
{
    Q_DECLARE_TR_FUNCTIONS(UserSpecialiesQualificationsPage)
public:
    UserSpecialiesQualificationsPage(QWidget *parent = 0);
};

class UserRightsPage: public QWizardPage
{
    Q_DECLARE_TR_FUNCTIONS(UserRightsPage)
public:
    UserRightsPage(QWidget *parent = 0);
    void initializePage();
    bool validatePage();

private:
    Internal::UserRightsWidget *um, *drugs, *med, *paramed, *secretary;
};

class UserLastPage: public QWizardPage
{
    Q_DECLARE_TR_FUNCTIONS(UserLastPage)
public:
    UserLastPage(QWidget *parent = 0);
    void initializePage();
private:
    QTreeWidget *tree;
};

} // namespace anonymous

namespace UserPlugin {
namespace Internal {
class UserCreatorWizardPrivate
{
public:
    UserCreatorWizardPrivate(UserCreatorWizard */*parent*/) :
        m_User(new UserData),
        m_Row(-1),
        m_Saved(false),
        m_CreateUser(true) //, q(parent)
    {}

    ~UserCreatorWizardPrivate()
    {
        if (m_User)
            delete m_User;
        m_User = 0;
    }

public:
    Internal::UserData *m_User;
    int m_Row;
    bool m_Saved, m_CreateUser;
    QString m_Uuid;
    QList<IUserWizardPage*> m_ExtraPages;
    static QHash<int, QString> m_Papers;
    static QHash<int, int> m_Rights;
    Utils::PasswordCrypter crypter;

private:
//    UserCreatorWizard *q;
};
} // namespace Internal
} // namespace UserPlugin

static inline QString bundlePath()  { return settings()->path(Core::ISettings::BundleResourcesPath); }

// Returns the path to the default papers (headers, footers, watermarks)
// according to the \e profession, \e paper (header, footer, watermark),
// \e paperType ("prescription" "administrative" "generic")
// All files are located in the application resources path
static inline QString defaultPaper(const QString &profession, const QString &paper, const QString &paperType = QString::null)
{
    QString lang = QLocale().name().left(2).toLower();
    QString pad;
#ifdef WITH_PAD
    pad = "pad_";
#endif
    QString fileName;
    if (paperType.isEmpty()) {
        fileName = QString("%1/profiles/%2/default/%3user_%4_%5.xml")
                .arg(bundlePath())
                .arg(profession)
                .arg(pad)
                .arg(paper)
                .arg(lang);
    } else {
        fileName = QString("%1/profiles/%2/default/%3user_%4_%5_%6.xml")
                .arg(bundlePath())
                .arg(profession)
                .arg(pad)
                .arg(paper)
                .arg(paperType)
                .arg(lang);
    }
    if (QFileInfo(fileName).exists())
        return Utils::readTextFile(fileName);

    // Try to find all languages files
    lang = Trans::Constants::ALL_LANGUAGE;
    if (paperType.isEmpty()) {
        fileName = QString("%1/profiles/%2/default/%3user_%4_%5.xml")
                .arg(bundlePath())
                .arg(profession)
                .arg(pad)
                .arg(paper)
                .arg(lang);
    } else {
        fileName = QString("%1/profiles/%2/default/%3user_%4_%5_%6.xml")
                .arg(bundlePath())
                .arg(profession)
                .arg(pad)
                .arg(paper)
                .arg(paperType)
                .arg(lang);
    }
    if (QFileInfo(fileName).exists())
        return Utils::readTextFile(fileName);

    // No file found neither for the current language nor for all language, try with the paperType
    if (!paperType.isEmpty()) {
        fileName = QString("%1/profiles/%2/default/%3user_%4_%5.xml")
                .arg(bundlePath())
                .arg(profession)
                .arg(pad)
                .arg(paper)
                .arg(lang);
        if (QFileInfo(fileName).exists())
            return Utils::readTextFile(fileName);
    }
    return QString();
}

//static inline QString defaultHeader(const QString &profession)
//{
//    return defaultPaper(profession, "header");
//}

//static inline QString defaultFooter(const QString &profession)
//{
//    return defaultPaper(profession, "footer");
//}

//// paperType == "prescription" "administrative" "generic"
//static inline QString defaultWatermark(const QString &profession, const QString &paperType = QString::null)
//{
//    return defaultPaper(profession, "watermark", paperType);
//}

QHash<int, QString> UserCreatorWizardPrivate::m_Papers;
QHash<int, int> UserCreatorWizardPrivate::m_Rights;

/**
 * Create a full UserCreatorWizard creator with the extending
 * UserPlugin::IUserWizardPage from the plugin manager object pool.
 */
UserCreatorWizard::UserCreatorWizard(QWidget *parent) :
    QWizard(parent),
    d(new UserCreatorWizardPrivate(this))
{
    setObjectName("UserCreatorWizard");
//    setModal(true);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);

    setPage(IdentityAndLoginPage, new UserIdentityAndLoginPage(this));
    setPage(ContactPage, new UserContactPage(this));
    setPage(ProfilPage, new UserProfilePage(this));
    setPage(RightsPage, new UserRightsPage(this));
    setPage(SpecialiesQualificationsPage, new UserSpecialiesQualificationsPage(this));

    // hook in all plugged in extrapages using the PluginManager
    d->m_ExtraPages = pluginManager()->getObjects<IUserWizardPage>();
    for(int i = 0; i < d->m_ExtraPages.count(); ++i) {
        setPage(ExtraPages + i, d->m_ExtraPages.at(i)->createWizardPage(this));
    }

    setPage(LastPage, new UserLastPage(this));

    QList<QWizard::WizardButton> layout;
    layout << QWizard::CancelButton << QWizard::Stretch << QWizard::BackButton
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);

    QPixmap pix = QPixmap(theme()->iconFullPath(Core::Constants::ICONUSERMANAGER, Core::ITheme::BigIcon));
    setPixmap(QWizard::BackgroundPixmap, pix);
    setPixmap(QWizard::WatermarkPixmap, pix);

    setWindowTitle(tr("User Creator Wizard"));
    setOptions(options() | QWizard::HaveHelpButton);
}

UserCreatorWizard::~UserCreatorWizard()
{
    if (d)
        delete d;
    d = 0;
}

void UserCreatorWizard::initializePage(int id)
{
    QWizard::initializePage(id);
    //Utils::resizeAndCenter(this, parentWidget());
}

/**
 * Check the current user rights for the creation of new users.
 * Return \e true if user can create a new user, otherwise returns false.
 * \sa Core::IUser::ManagerRights, Core::IUser::Create
 */
bool UserCreatorWizard::checkUserRights()
{
    Core::IUser::UserRights r(userModel()->currentUserData(Core::IUser::ManagerRights).toInt());
    return r & Core::IUser::Create;
}

void UserCreatorWizard::done(int r)
{
    validateCurrentPage();

    // No user -> rejected & close
    if (!d->m_User) {
        QDialog::done(QDialog::Rejected);
        return;
    }

    // dialog result == Rejected -> ask for a confirmation
    if (r == QDialog::Rejected) {
        d->m_Saved = false;
        bool yes = Utils::yesNoMessageBox(tr("WARNING! You did not save this user. "
                                             "If you continue without saving, all changes will be lost."),
                               tr("Do you really want to close this dialog?"),
                               "", tr("User not saved"));
        if (yes) {
            QDialog::done(r);
        }
        return;
    }

    // Dialog is accepted here and user not saved -> save it
    // Feed userData with the wizard values
    d->m_User->setValidity(1);
    d->m_User->setLogin64(Utils::loginForSQL(field("Login").toString()));
    d->m_User->setClearPassword(field("Password").toString());
    d->m_User->setCryptedPassword(d->crypter.cryptPassword(field("Password").toString()));
    d->m_User->setUsualName(field("UsualName"));
    d->m_User->setOtherNames(field("OtherNames"));
    d->m_User->setFirstname(field("Firstname"));
    d->m_User->setTitleIndex(field("Title").toInt());
    d->m_User->setGenderIndex(field("Gender").toInt());
    d->m_User->setStreet(field("Address"));
    d->m_User->setZipcode(field("Zipcode"));
    d->m_User->setCity(field("City"));
    d->m_User->setCountry(field("Country"));
    d->m_User->setLocaleLanguage(QLocale::Language(field("Language").toInt()));
    d->m_User->setTel1(field("Tel1"));
    d->m_User->setTel2(field("Tel2"));
    d->m_User->setTel3(field("Tel3"));
    d->m_User->setFax(field("Fax"));
    d->m_User->setMail(field("Mail"));
    d->m_User->setSpecialty(field("Specialities").toStringList());
    d->m_User->setQualification(field("Qualifications").toStringList());
    d->m_User->setPractitionerIdentifiers(field("Identifiers").toStringList());

    d->m_User->setRights(Constants::USER_ROLE_USERMANAGER, Core::IUser::UserRights(d->m_Rights.value(Core::IUser::ManagerRights)));
    d->m_User->setRights(Constants::USER_ROLE_MEDICAL, Core::IUser::UserRights(d->m_Rights.value(Core::IUser::MedicalRights)));
    d->m_User->setRights(Constants::USER_ROLE_DOSAGES, Core::IUser::UserRights(d->m_Rights.value(Core::IUser::DrugsRights)));
    d->m_User->setRights(Constants::USER_ROLE_PARAMEDICAL, Core::IUser::UserRights(d->m_Rights.value(Core::IUser::ParamedicalRights)));
    d->m_User->setRights(Constants::USER_ROLE_SECRETARY, Core::IUser::UserRights(d->m_Rights.value(Core::IUser::SecretaryRights)));

    d->m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(d->m_Papers.value(Core::IUser::GenericHeader)), Core::IUser::GenericHeader);
    d->m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(d->m_Papers.value(Core::IUser::GenericFooter)), Core::IUser::GenericFooter);
    d->m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(d->m_Papers.value(Core::IUser::GenericWatermark)), Core::IUser::GenericWatermark);

    d->m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(d->m_Papers.value(Core::IUser::AdministrativeHeader)), Core::IUser::AdministrativeHeader);
    d->m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(d->m_Papers.value(Core::IUser::AdministrativeFooter)), Core::IUser::AdministrativeFooter);
    d->m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(d->m_Papers.value(Core::IUser::AdministrativeWatermark)), Core::IUser::AdministrativeWatermark);

    d->m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(d->m_Papers.value(Core::IUser::PrescriptionHeader)), Core::IUser::PrescriptionHeader);
    d->m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(d->m_Papers.value(Core::IUser::PrescriptionFooter)), Core::IUser::PrescriptionFooter);
    d->m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(d->m_Papers.value(Core::IUser::PrescriptionWatermark)), Core::IUser::PrescriptionWatermark);

    if (d->m_CreateUser) {
        // Check if user already exists in database
        if (userBase()->userExists(d->m_User->clearLogin())) {
            Utils::warningMessageBox(tr("Username %1 already exists in MySQL database.").arg(d->m_User->clearLogin())
                                        ,tr("Please choose a different login or delete user %1 in MySQL database and try again.").arg(d->m_User->clearLogin())
                                        ,tr("To change login, click on button 'Back' 2 times and modify the login.\n To delete user %1 in MySQL, ask your database administrator.").arg(d->m_User->clearLogin())                                 
                                        ,tr("User creation issue"));
            return;
        }

        // Create user in database
        if (!userBase()->createUser(d->m_User)) {
            Utils::warningMessageBox(tr("An error occured during database access."),
                                     tr("Logged errors saved. Please refer to the %1 to manage this error.")
                                     .arg(Utils::Log::saveLog()),
                                     "", tr("Error during database access"));
            QDialog::done(QDialog::Rejected);
        } else {
            // Reset the usermodel
            userModel()->forceReset();

            // Submit extra-pages ?
            for(int i = 0; i < d->m_ExtraPages.count(); ++i) {
                d->m_ExtraPages.at(i)->submit(d->m_User->uuid());
            }

            Utils::informativeMessageBox(tr("User successfully saved into database."),
                                         tr("The user was successfully created and saved into database."),
                                         "", tr("User successfully saved into database."));
            d->m_Saved = true;
            QDialog::done(r);
        }
    }
}

void UserCreatorWizard::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    Utils::centerWidget(this);
}

void UserCreatorWizard::setUserPaper(const int ref, const QString &xml)
{
    UserCreatorWizardPrivate::m_Papers.insert(ref, xml);
}

void UserCreatorWizard::setUserRights(const int role, const int value)
{
    UserCreatorWizardPrivate::m_Rights.insert(role, value);
}

int UserCreatorWizard::userRights(const int role)
{
    return UserCreatorWizardPrivate::m_Rights.value(role,0);
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////    UserContactPage    ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserContactPage::UserContactPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::UserWizardContactWidget)
{
    setTitle(tr("Provide your address."));
    setSubTitle(tr("Please enter your complete, professional address."));

    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->zipCodesWidget->initialize(ZipCodes::ZipCodesWidget::GridLayout);

    const QRegExp emailRegExp = QRegExp("[A-Z0-9._%-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}", Qt::CaseInsensitive);
    ui->mail->setValidator(new QRegExpValidator(emailRegExp,this));
    registerField("Address", ui->zipCodesWidget, "street");
    registerField("City", ui->zipCodesWidget, "city");
    registerField("Zipcode", ui->zipCodesWidget, "zipCode");
    registerField("Country", ui->zipCodesWidget, "countryIso");
    registerField("State", ui->zipCodesWidget, "stateProvince");

    registerField("Tel1", ui->tel1);
    registerField("Tel2", ui->tel2);
    registerField("Tel3", ui->tel3);
    registerField("Fax", ui->fax);
    registerField("Mail", ui->mail);
}

UserContactPage::~UserContactPage()
{
    delete ui;
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////    UserProfilePage    ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserProfilePage::UserProfilePage(QWidget *parent) :
    QWizardPage(parent),
    view(0),
    box(0),
    next(-1)
{
    setTitle(tr("Select a profile"));
    setSubTitle(tr("FreeMedForms allows you to create users using predefined profiles. Select your profile and options."));

    // TODO: code here = add new profiles
    Views::StringListModel *model = new Views::StringListModel(this);
    model->setStringList(QStringList()
                         << tkTr(Trans::Constants::DOCTOR)
                         << tkTr(Trans::Constants::MEDICAL_STUDENT)
                         << tkTr(Trans::Constants::NURSE)
                         << tkTr(Trans::Constants::CAREGIVER)
                         << tkTr(Trans::Constants::SECRETARY)
                         << tkTr(Trans::Constants::SOFT_ADMIN)
                         );
    model->setCheckable(true);
    model->setReadOnly(true);
    view = new Views::StringListView(this);
    view->setModel(model);
    view->setActions(0);

    box = new QCheckBox(tr("Select advanced rights in the next step"), this);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(view, 0, 0);
    layout->addWidget(box, 1, 0);
}

bool UserProfilePage::validatePage()
{
    UserCreatorWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::NoRights);
    UserCreatorWizard::setUserRights(Core::IUser::DrugsRights, Core::IUser::NoRights);
    UserCreatorWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::NoRights);
    UserCreatorWizard::setUserRights(Core::IUser::AgendaRights, Core::IUser::NoRights);
    UserCreatorWizard::setUserRights(Core::IUser::ParamedicalRights, Core::IUser::NoRights);
    UserCreatorWizard::setUserRights(Core::IUser::SecretaryRights, Core::IUser::NoRights);
    next = UserCreatorWizard::SpecialiesQualificationsPage;
    QStringList result = view->getCheckedStringList().toStringList();

    if (result.contains(tkTr(Trans::Constants::DOCTOR)) || result.contains(tkTr(Trans::Constants::MEDICAL_STUDENT))) {
        UserCreatorWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::ReadOwn | Core::IUser::ReadDelegates | Core::IUser::WriteOwn | Core::IUser::WriteDelegates | Core::IUser::Print);
        UserCreatorWizard::setUserRights(Core::IUser::DrugsRights, Core::IUser::AllRights);
        UserCreatorWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::AllRights);
        UserCreatorWizard::setUserRights(Core::IUser::AgendaRights, Core::IUser::AllRights);
        UserCreatorWizard::setUserRights(Core::IUser::ParamedicalRights, Core::IUser::ReadAll | Core::IUser::Print);
        UserCreatorWizard::setUserRights(Core::IUser::SecretaryRights, Core::IUser::NoRights);
        next = UserCreatorWizard::SpecialiesQualificationsPage;

        // create default papers
        UserCreatorWizard::setUserPaper(Core::IUser::GenericHeader, defaultPaper("medicals", "header"));
        UserCreatorWizard::setUserPaper(Core::IUser::GenericFooter, defaultPaper("medicals", "footer"));
        UserCreatorWizard::setUserPaper(Core::IUser::GenericWatermark, defaultPaper("medicals", "watermark"));

        UserCreatorWizard::setUserPaper(Core::IUser::PrescriptionHeader, defaultPaper("medicals", "header", "prescriptions"));
        UserCreatorWizard::setUserPaper(Core::IUser::PrescriptionFooter, defaultPaper("medicals", "footer", "prescriptions"));
        UserCreatorWizard::setUserPaper(Core::IUser::PrescriptionWatermark, defaultPaper("medicals", "watermark", "prescriptions"));

        UserCreatorWizard::setUserPaper(Core::IUser::AdministrativeHeader, defaultPaper("medicals", "header"));
        UserCreatorWizard::setUserPaper(Core::IUser::AdministrativeFooter, defaultPaper("medicals", "footer"));
        UserCreatorWizard::setUserPaper(Core::IUser::AdministrativeWatermark, defaultPaper("medicals", "watermark"));
    } else if (result.contains(tkTr(Trans::Constants::NURSE))) {
        UserCreatorWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::ReadOwn | Core::IUser::ReadDelegates | Core::IUser::WriteOwn | Core::IUser::WriteDelegates | Core::IUser::Print);
        UserCreatorWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::ReadAll | Core::IUser::Print);
        UserCreatorWizard::setUserRights(Core::IUser::DrugsRights, Core::IUser::ReadAll | Core::IUser::Print);
        UserCreatorWizard::setUserRights(Core::IUser::ParamedicalRights, Core::IUser::AllRights);
        UserCreatorWizard::setUserRights(Core::IUser::SecretaryRights, Core::IUser::AllRights);

    } else if (result.contains(tkTr(Trans::Constants::CAREGIVER))) {
        UserCreatorWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::ReadOwn | Core::IUser::ReadDelegates | Core::IUser::WriteOwn | Core::IUser::WriteDelegates | Core::IUser::Print);
        UserCreatorWizard::setUserRights(Core::IUser::AgendaRights, Core::IUser::AllRights);
        UserCreatorWizard::setUserRights(Core::IUser::ParamedicalRights, Core::IUser::ReadOwn | Core::IUser::ReadDelegates | Core::IUser::WriteOwn | Core::IUser::WriteDelegates | Core::IUser::Print);

    } else if (result.contains(tkTr(Trans::Constants::SECRETARY))) {
         UserCreatorWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::ReadOwn | Core::IUser::ReadDelegates | Core::IUser::WriteOwn | Core::IUser::WriteDelegates | Core::IUser::Print);
        UserCreatorWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::ReadAll);
        UserCreatorWizard::setUserRights(Core::IUser::AgendaRights, Core::IUser::ReadAll | Core::IUser::WriteAll | Core::IUser::Print);
        UserCreatorWizard::setUserRights(Core::IUser::SecretaryRights, Core::IUser::AllRights);
    }
    if (result.contains(tkTr(Trans::Constants::SOFT_ADMIN))) {
        UserCreatorWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::AllRights);
        UserCreatorWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::AllRights);
        UserCreatorWizard::setUserRights(Core::IUser::DrugsRights, Core::IUser::AllRights);
        UserCreatorWizard::setUserRights(Core::IUser::ParamedicalRights, Core::IUser::AllRights);
        UserCreatorWizard::setUserRights(Core::IUser::AgendaRights, Core::IUser::AllRights);
        UserCreatorWizard::setUserRights(Core::IUser::SecretaryRights, Core::IUser::AllRights);
    }

    if (box->isChecked()) {
        next = UserCreatorWizard::RightsPage;
        return true;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////    UserSpecialiesQualificationsPage    ///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserSpecialiesQualificationsPage::UserSpecialiesQualificationsPage(QWidget *parent)
        : QWizardPage(parent)
{
    setTitle(tr("Define user's specialties and qualifications."));
    setSubTitle(tr("Use the context menu to add, remove, move up or down items."));
    QTabWidget *tab = new QTabWidget(this);

    QStringListModel *modelspe = new QStringListModel(this);
    Views::StringListView *speView = new Views::StringListView(this);
    speView->setModel(modelspe);
    QStringListModel *modelqual = new QStringListModel(this);
    Views::StringListView *quaView = new Views::StringListView(this);
    quaView->setModel(modelqual);
    QStringListModel *modelids = new QStringListModel(this);
    Views::StringListView *idsView = new Views::StringListView(this);
    idsView->setModel(modelids);

    tab->addTab(speView, tr("Specialities"));
    tab->addTab(quaView, tr("Qualifications"));
    tab->addTab(idsView, tr("Identifiers"));

    registerField("Specialities", speView, "stringList");
    registerField("Qualifications", quaView, "stringList");
    registerField("Identifiers", idsView, "stringList");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(tab, 0, 1);
    setLayout(layout);
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////    UserRightsPage    ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
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
    secretary = new Internal::UserRightsWidget(this);

    tab->addTab(um, tr("Users"));
    tab->addTab(drugs, tr("Drugs"));
    tab->addTab(med, tr("Medicals"));
    tab->addTab(paramed, tr("Paramedicals"));
    tab->addTab(secretary, tr("Secretary"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(tab, 0, 0);
    setLayout(layout);
}

void UserRightsPage::initializePage()
{
    um->setRights(UserCreatorWizard::userRights(Core::IUser::ManagerRights));
    drugs->setRights(UserCreatorWizard::userRights(Core::IUser::DrugsRights));
    med->setRights(UserCreatorWizard::userRights(Core::IUser::MedicalRights));
    paramed->setRights(UserCreatorWizard::userRights(Core::IUser::ParamedicalRights));
    secretary->setRights(UserCreatorWizard::userRights(Core::IUser::SecretaryRights));

}

bool UserRightsPage::validatePage()
{
    UserCreatorWizard::setUserRights(Core::IUser::ManagerRights, um->getRights());
    UserCreatorWizard::setUserRights(Core::IUser::DrugsRights, drugs->getRights());
    UserCreatorWizard::setUserRights(Core::IUser::MedicalRights, med->getRights());
    UserCreatorWizard::setUserRights(Core::IUser::ParamedicalRights, paramed->getRights());
    UserCreatorWizard::setUserRights(Core::IUser::SecretaryRights, secretary->getRights());
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////    UserLastPage    /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
UserLastPage::UserLastPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle(tr("User creation"));
    setSubTitle(tr("The user will be created."));
    tree = new QTreeWidget(this);
    tree->header()->hide();
    QVBoxLayout *lay = new QVBoxLayout(this);
    this->setLayout(lay);
    lay->addWidget(tree);
}

void UserLastPage::initializePage()
{
    tree->clear();
    tree->setColumnCount(2);
    QFont bold;
    bold.setBold(true);
    QTreeWidgetItem *general = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::GENERAL).remove("&"));
    general->setFont(0, bold);
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::LOGIN) << field("Login").toString());
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::PASSWORD) << QString(field("Password").toString().count(), QLatin1Char('*')));
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::TITLE) << titles().at(field("Title").toInt()));
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::NAME) << field("UsualName").toString() + " " + field("OtherNames").toString() + " " + field("Firstname").toString());
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::GENDER) << genders().at(field("Gender").toInt()));
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::M_LANGUAGES_TEXT).remove("&") << QLocale::languageToString(QLocale::Language(field("Language").toInt())) );

    QTreeWidgetItem *contact = new QTreeWidgetItem(tree, QStringList() << tr("Contact"));
    contact->setFont(0, bold);
    new QTreeWidgetItem(contact, QStringList() << tkTr(Trans::Constants::FULLADDRESS) << field("Address").toString());
    new QTreeWidgetItem(contact, QStringList() << tkTr(Trans::Constants::ZIP_CODES) << field("Zipcode").toString());
    new QTreeWidgetItem(contact, QStringList() << tr("City") << field("City").toString());
    new QTreeWidgetItem(contact, QStringList() << tr("Country") << field("Country").toString());
    new QTreeWidgetItem(contact, QStringList() << tr("Tel1") << field("Tel1").toString());
    new QTreeWidgetItem(contact, QStringList() << tr("Tel2") << field("Tel2").toString());
    new QTreeWidgetItem(contact, QStringList() << tr("Tel3") << field("Tel3").toString());
    new QTreeWidgetItem(contact, QStringList() << tr("Fax") << field("Fax").toString());
    new QTreeWidgetItem(contact, QStringList() << tr("Mail") << field("Mail").toString());

    QTreeWidgetItem *spe = new QTreeWidgetItem(tree, QStringList() << tr("Specialties"));
    spe->setFont(0, bold);
    const QStringList &spes =  field("Specialities").toStringList();
    foreach(const QString &s, spes) {
        new QTreeWidgetItem(spe, QStringList() << s);
    }

    QTreeWidgetItem *qual = new QTreeWidgetItem(tree, QStringList() << tr("Qualifications"));
    qual->setFont(0, bold);
    const QStringList &quals =  field("Qualifications").toStringList();
    foreach(const QString &s, quals) {
        new QTreeWidgetItem(qual, QStringList() << s);
    }

    QTreeWidgetItem *idents = new QTreeWidgetItem(tree, QStringList() << tr("Professionnal identifiers"));
    idents->setFont(0, bold);
    const QStringList &ids =  field("Identifiers").toStringList();
    foreach(const QString &s, ids) {
        new QTreeWidgetItem(idents, QStringList() << s);
    }

    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
    tree->expandAll();
}
