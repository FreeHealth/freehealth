/***************************************************************************
 * The FreeMedForms project is a set of free, open source medical         *
 * applications.                                                          *
 * (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 * along with this program (COPYING.FREEMEDFORMS file).                   *
 * If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *  Contributors :                                                        *
 *      NAME <MAIL@Address>                                                *
 *      NAME <MAIL@Address>                                                *
 ***************************************************************************/
/**
  \class UserPlugin::UserWizard
  \brief Wizard for user creation.
  You can tell UserWizrd to create the user itself or use a defined user. createUser() define the
  creation mode. If you set it to false, inform the row of the model to use with setModelRow(). By default,
  UserWizard create itself a new user.\n
  You can extend the wizard with the UserPlugin::IUserWizardPage interface. The wizard will catch all
  objects (in its contructor) from the plugin manager object pool and present the pages to the user.\n
  Usage :
  \code
    UserWizard wiz;
    wiz.createUser(true);   // will create the user (optional)
    wiz.show();
  \endcode

  \todo If row is defined --> populate all wizard pages with users values.
*/

#include "userwizard.h"

#include <usermanagerplugin/userdata.h>
#include <usermanagerplugin/database/userbase.h>

#include <texteditorplugin/texteditor.h>

#include <listviewplugin/stringlistview.h>
#include <listviewplugin/stringlistmodel.h>

#include <zipcodesplugin/zipcodescompleters.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/itheme.h>
#include <coreplugin/translators.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>

#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/widgets/userrightswidget.h>
#include <usermanagerplugin/iuserwizardpage.h>

#include <usermanagerplugin/widgets/useridentityandloginpage.h>
#include <listviewplugin/languagecombobox.h>

#include <utils/log.h>
#include <utils/global.h>
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
static inline UserPlugin::UserModel *userModel() { return UserModel::instance(); }
static inline UserPlugin::Internal::UserBase *userBase() { return UserPlugin::Internal::UserBase::instance(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline QString bundlePath()  { return settings()->path(Core::ISettings::BundleResourcesPath); }

static inline QString defaultPaper(const QString &profession, const QString &paper, const QString &paperType = QString::null)
{
    QString lang = QLocale().name().left(2);
    QString fileName;
    if (paperType.isEmpty()) {
        fileName = QString(bundlePath() + "/profiles/%1/default/user_%2_%3.xml").arg(profession).arg(paper).arg(lang);
    } else {
        fileName = QString(bundlePath() + "/profiles/%1/default/user_%2_%3_%4.xml").arg(profession).arg(paper).arg(paperType).arg(lang);
    }
    if (QFileInfo(fileName).exists()) {
        return Utils::readTextFile(fileName);
    }
    lang = Trans::Constants::ALL_LANGUAGE;
    if (paperType.isEmpty()) {
        fileName = QString(bundlePath() + "/profiles/%1/default/user_%2_%3.xml").arg(profession).arg(paper).arg(lang);
    } else {
        fileName = QString(bundlePath() + "/profiles/%1/default/user_%2_%3_%4.xml").arg(profession).arg(paper).arg(paperType).arg(lang);
    }
    if (QFileInfo(fileName).exists()) {
        return Utils::readTextFile(fileName);
    }
    if (!paperType.isEmpty()) {
        fileName = QString(bundlePath() + "/profiles/%1/default/user_%2_%3.xml").arg(profession).arg(paper).arg(lang);
        if (QFileInfo(fileName).exists()) {
            return Utils::readTextFile(fileName);
        }
    }
    return QString();
}

static inline QString defaultHeader(const QString &profession)
{
    return defaultPaper(profession, "header");
}

static inline QString defaultFooter(const QString &profession)
{
    return defaultPaper(profession, "footer");
}

// paperType == "prescription" "administrative" "generic"
static inline QString defaultWatermark(const QString &profession, const QString &paperType = QString::null)
{
    return defaultPaper(profession, "watermark", paperType);
}

QHash<int, QString> UserWizard::m_Papers;
QHash<int, int> UserWizard::m_Rights;

/** Create a full UserWizard creator with the extending IUserWizardPage from the plugin manager object pool. */
UserWizard::UserWizard(QWidget *parent) :
        QWizard(parent),
        m_User(new UserData),
        m_Row(-1),
        m_Saved(false),
        m_CreateUser(true)
{
    setPage(IdentityAndLoginPage, new UserIdentityAndLoginPage(this));
    setPage(ContactPage, new UserContactPage(this));
    setPage(ProfilPage, new UserProfilePage(this));
    setPage(RightsPage, new UserRightsPage(this));
    setPage(SpecialiesQualificationsPage, new UserSpecialiesQualificationsPage(this));

    // hook in all plugged in extrapages using the PluginManager
    m_ExtraPages = pluginManager()->getObjects<IUserWizardPage>();
    for(int i = 0; i < m_ExtraPages.count(); ++i) {
        setPage(ExtraPages + i, m_ExtraPages.at(i)->createWizardPage(this));
    }

    setPage(LastPage, new UserLastPage(this));

    setWindowTitle(tr("User Creator Wizard"));
    setOptions(options() | QWizard::HaveHelpButton);
}

UserWizard::~UserWizard()
{
    if (m_User) {
        delete m_User;
        m_User = 0;
    }
}

void UserWizard::done(int r)
{
    validateCurrentPage();

    // No user -> rejected & close
    if (!m_User) {
        QDialog::done(QDialog::Rejected);
        return;
    }

    // dialog result == Rejected -> ask for a confirmation
    if (r == QDialog::Rejected) {
        m_Saved = false;
        bool yes = Utils::yesNoMessageBox(tr("WARNING! You did not save this user. "
                                             "If you continue without saving, all changes will be lost."),
                               tr("Do you really want to close this dialog?"),
                               "", tr("User not saved"));
        if (yes) {
            QDialog::done(r);
        }
        return;
    }

    // Dialog is accepted here, but user not saved -> save it
    if (true) {

        // Feed userData with the wizard values
        m_User->setValidity(1);
        m_User->setLogin64(Utils::loginForSQL(field("Login").toString()));
        m_User->setClearPassword(field("Password").toString());
        m_User->setCryptedPassword(Utils::cryptPassword(field("Password").toString()));
        m_User->setName(field("Name"));
        m_User->setSecondName(field("SecondName"));
        m_User->setFirstname(field("Firstname"));
        m_User->setTitleIndex(field("Title"));
        m_User->setGenderIndex(field("Gender"));
        m_User->setAddress(field("Address"));
        m_User->setZipcode(field("Zipcode"));
        m_User->setCity(field("City"));
        m_User->setCountry(field("Country"));
        m_User->setLocaleLanguage(QLocale::Language(field("Language").toInt()));
        m_User->setTel1(field("Tel1"));
        m_User->setTel2(field("Tel2"));
        m_User->setTel3(field("Tel3"));
        m_User->setFax(field("Fax"));
        m_User->setMail(field("Mail"));
        m_User->setSpecialty(field("Specialities").toStringList());
        m_User->setQualification(field("Qualifications").toStringList());
        m_User->setPractitionerIdentifiant(field("Identifiants").toStringList());

        m_User->setRights(Constants::USER_ROLE_USERMANAGER, Core::IUser::UserRights(m_Rights.value(Core::IUser::ManagerRights)));
        m_User->setRights(Constants::USER_ROLE_MEDICAL, Core::IUser::UserRights(m_Rights.value(Core::IUser::MedicalRights)));
        m_User->setRights(Constants::USER_ROLE_DOSAGES, Core::IUser::UserRights(m_Rights.value(Core::IUser::DrugsRights)));
        m_User->setRights(Constants::USER_ROLE_PARAMEDICAL, Core::IUser::UserRights(m_Rights.value(Core::IUser::ParamedicalRights)));
        m_User->setRights(Constants::USER_ROLE_ADMINISTRATIVE, Core::IUser::UserRights(m_Rights.value(Core::IUser::AdministrativeRights)));

        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::GenericHeader)), Core::IUser::GenericHeader);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::GenericFooter)), Core::IUser::GenericFooter);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::GenericWatermark)), Core::IUser::GenericWatermark);

        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::AdministrativeHeader)), Core::IUser::AdministrativeHeader);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::AdministrativeFooter)), Core::IUser::AdministrativeFooter);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::AdministrativeWatermark)), Core::IUser::AdministrativeWatermark);

        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::PrescriptionHeader)), Core::IUser::PrescriptionHeader);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::PrescriptionFooter)), Core::IUser::PrescriptionFooter);
        m_User->setExtraDocument(Print::TextDocumentExtra::fromXml(m_Papers.value(Core::IUser::PrescriptionWatermark)), Core::IUser::PrescriptionWatermark);

        if (m_CreateUser) {
            // Create user in database
            if (!userBase()->createUser(m_User)) {
                Utils::warningMessageBox(tr("An error occured during database access."),
                                         tr("Logged errors saved. Please refer to the %1 to manage this error.")
                                         .arg(Utils::Log::saveLog()),
                                         "", tr("Error during database access"));
                QDialog::done(QDialog::Rejected);
            } else {
                // Reset the usermodel
                userModel()->forceReset();

                // Submit extra-pages ?
                for(int i = 0; i < m_ExtraPages.count(); ++i) {
                    m_ExtraPages.at(i)->submit(m_User->uuid());
                }

                Utils::informativeMessageBox(tr("User successfully saved into database."),
                                             tr("The user was successfully created and saved into database."),
                                             "", tr("User successfully saved into database."));
                m_Saved = true;
                QDialog::done(r);
            }
        }
    }
}

void UserWizard::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    Utils::centerWidget(this);
}

QString UserWizard::createdUuid() const
{
    if (m_User) {
        return m_User->uuid();
    }
    return QString();
}


UserContactPage::UserContactPage(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::UserWizardContactWidget)
{
    setTitle(tr("Provide your address."));
    setSubTitle(tr("Please enter your complete, professional address."));

    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    ZipCodes::ZipCountryCompleters *c = new ZipCodes::ZipCountryCompleters(this);
    c->setCountryComboBox(ui->comboBox);
    c->setZipLineEdit(ui->zipcode);
    c->setCityLineEdit(ui->city);
    const QRegExp emailRegExp = QRegExp("[A-Z0-9._%-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}", Qt::CaseInsensitive);
    ui->mail->setValidator(new QRegExpValidator(
                               emailRegExp,this));
    registerField("Address", ui->address, "plainText");
    registerField("City", ui->city, "text");
    registerField("Zipcode", ui->zipcode, "text");
    registerField("Country", ui->comboBox, "currentIsoCountry");

    registerField("Tel1", ui->tel1, "text");
    registerField("Tel2", ui->tel2, "text");
    registerField("Tel3", ui->tel3, "text");
    registerField("Fax", ui->fax, "text");
    registerField("Mail", ui->mail, "text");
}

UserContactPage::~UserContactPage()
{
    delete ui;
}


UserProfilePage::UserProfilePage(QWidget *parent) :
        QWizardPage(parent)
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
    UserWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::DrugsRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::AgendaRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::ParamedicalRights, Core::IUser::NoRights);
    UserWizard::setUserRights(Core::IUser::AdministrativeRights, Core::IUser::NoRights);
    next = UserWizard::SpecialiesQualificationsPage;
    QStringList result = view->getCheckedStringList().toStringList();

    if (result.contains(tkTr(Trans::Constants::DOCTOR)) || result.contains(tkTr(Trans::Constants::MEDICAL_STUDENT))) {
        UserWizard::setUserRights(Core::IUser::ManagerRights, Core::IUser::ReadOwn | Core::IUser::ReadDelegates | Core::IUser::WriteOwn | Core::IUser::WriteDelegates | Core::IUser::Print);
        UserWizard::setUserRights(Core::IUser::DrugsRights, Core::IUser::AllRights);
        UserWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::AllRights);
        UserWizard::setUserRights(Core::IUser::AgendaRights, Core::IUser::AllRights);
        UserWizard::setUserRights(Core::IUser::ParamedicalRights, int(Core::IUser::ReadAll | Core::IUser::Print));
        UserWizard::setUserRights(Core::IUser::AdministrativeRights, Core::IUser::NoRights);
        next = UserWizard::SpecialiesQualificationsPage;

        // create default papers
        UserWizard::setUserPaper(Core::IUser::GenericHeader, defaultPaper("medicals", "header"));
        UserWizard::setUserPaper(Core::IUser::GenericFooter, defaultPaper("medicals", "footer"));
        UserWizard::setUserPaper(Core::IUser::GenericWatermark, defaultPaper("medicals", "watermark"));

        UserWizard::setUserPaper(Core::IUser::PrescriptionHeader, defaultPaper("medicals", "header", "prescriptions"));
        UserWizard::setUserPaper(Core::IUser::PrescriptionFooter, defaultPaper("medicals", "footer", "prescriptions"));
        UserWizard::setUserPaper(Core::IUser::PrescriptionWatermark, defaultPaper("medicals", "watermark", "prescriptions"));

        UserWizard::setUserPaper(Core::IUser::AdministrativeHeader, defaultPaper("medicals", "header"));
        UserWizard::setUserPaper(Core::IUser::AdministrativeFooter, defaultPaper("medicals", "footer"));
        UserWizard::setUserPaper(Core::IUser::AdministrativeWatermark, defaultPaper("medicals", "watermark"));
    } else if (result.contains(tkTr(Trans::Constants::NURSE))) {

    } else if (result.contains(tkTr(Trans::Constants::CAREGIVER))) {

    } else if (result.contains(tkTr(Trans::Constants::SECRETARY))) {
        UserWizard::setUserRights(Core::IUser::MedicalRights, Core::IUser::ReadAll);
        UserWizard::setUserRights(Core::IUser::AgendaRights, Core::IUser::ReadAll | Core::IUser::WriteAll | Core::IUser::Print);
    }
    if (result.contains(tkTr(Trans::Constants::SOFT_ADMIN))) {
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

UserLastPage::UserLastPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle(tr("User creation"));
    setSubTitle(tr("The user will be created."));
    tree = new QTreeWidget(this);
    tree->header()->hide();
    // DONE: hide password!
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
    QTreeWidgetItem *general = new QTreeWidgetItem(tree, QStringList() << tkTr(Trans::Constants::GENERAL));
    general->setFont(0, bold);
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::LOGIN) << field("Login").toString());
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::PASSWORD) << QString(field("Password").toString().count(), QLatin1Char('*')));
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::TITLE) << titles().at(field("Title").toInt()));
    new QTreeWidgetItem(general, QStringList() << tkTr(Trans::Constants::NAME) << field("Name").toString() + " " + field("SecondName").toString() + " " + field("Firstname").toString());
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
    const QStringList &ids =  field("Identifiants").toStringList();
    foreach(const QString &s, ids) {
        new QTreeWidgetItem(idents, QStringList() << s);
    }

    tree->resizeColumnToContents(0);
    tree->resizeColumnToContents(1);
    tree->expandAll();
}
