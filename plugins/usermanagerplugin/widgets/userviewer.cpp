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
  \class UserPlugin::UserViewer
  \brief This class is a data wrapper for users.
  When you instanciate this widget, it retreive and show the UserPlugin::UserModel::currentUserIndex().\n
  Rights are managed via the UserPlugin::UserModel.\n
  Changes are automaticaly saved into the UserPlugin::UserModel.
  \todo limit memory usage.
  \ingroup usertoolkit widget_usertoolkit usermanager
*/

#include "userviewer.h"
#include "userviewer_p.h"

#include <utils/global.h>
#include <utils/serializer.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/translators.h>
#include <coreplugin/iuser.h>

#include <listviewplugin/stringlistview.h>

#include <printerplugin/printer.h>

#include <usermanagerplugin/usermodel.h>
#include <usermanagerplugin/widgets/userpassworddialog.h>

#include <QByteArray>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QStringListModel>
#include <QTextEdit>

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- Constructors / Destructors --------------------------------------
//--------------------------------------------------------------------------------------------------------
UserViewer::UserViewer(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("UserViewer");
    d = new UserViewerPrivate(this);
    d->initialize();
    if (!parent)
        Utils::centerWidget(this);
}

UserViewerPrivate::UserViewerPrivate(QObject *parent)
    : QObject(parent), m_Parent(0), m_Mapper(0),
      genericPreview(0), adminPreview(0), prescriptionPreview(0)
{
    Q_ASSERT_X(static_cast<QDialog *>(parent), "UserViewerPrivate", "*parent is not a QDialog");
    setObjectName("UserViewerPrivate");
    m_Parent = static_cast<QWidget *>(parent);
    m_Row = UserModel::instance()->currentUserIndex().row();
}

/** \brief Change current viewing user to \e modelRow from UserModel */
void UserViewer::changeUserTo(const int modelRow)
{
    d->changeUserIndex(modelRow);
}

/** \brief Change current viewing user to \e modelRow from UserModel */
void UserViewerPrivate::changeUserIndex(const int modelRow)
{
    // clear ui
    genericPreview->headerEditor()->clear();
    genericPreview->footerEditor()->clear();
    genericPreview->watermarkEditor()->clear();

    adminPreview->headerEditor()->clear();
    adminPreview->footerEditor()->clear();
    adminPreview->watermarkEditor()->clear();

    prescriptionPreview->headerEditor()->clear();
    prescriptionPreview->footerEditor()->clear();
    prescriptionPreview->watermarkEditor()->clear();

    // manage row changing
    int oldRow = m_Row;
    m_Row = modelRow;
    checkUserRights();
    if (m_CanRead)
        m_Mapper->setCurrentModelIndex(UserModel::instance()->index(modelRow, 0));
    else {
        m_Row = oldRow;
        Utils::informativeMessageBox(tr("You can not access to these datas."), tr("You don't have these rights."), "");
    }
}

//--------------------------------------------------------------------------------------------------------
//------------------------------------------- PRIVATE PART -----------------------------------------------
//--------------------------------------------------------------------------------------------------------
/** \brief Ui initializer. */
void UserViewerPrivate::initialize()
{
    setupUi(m_Parent);
    // add previewer
    genericPreview = Print::Printer::previewer(m_Parent);
    adminPreview = Print::Printer::previewer(m_Parent);
    prescriptionPreview = Print::Printer::previewer(m_Parent);
    editorsGenericLayout->addWidget(genericPreview, 0,0);
    editorsAdminLayout->addWidget(adminPreview, 0,0);
    editorsPrescriptionLayout->addWidget(prescriptionPreview, 0,0);

    // populate combos
    titleCombo->addItems(titles());
    genderCombo->addItems(genders());
    languageCombo->addItems(Core::Translators::availableLocales());
    // QListView need to be managed by hand
    QStringListModel *modelspe = new QStringListModel(this);
    specialtyListView->setModel(modelspe);
    specialtyListView->setActions(Views::Constants::AllActions);
    QStringListModel *modelqual = new QStringListModel(this);
    qualificationsListView->setModel(modelqual);
    QStringListModel *modelids = new QStringListModel(this);
    practIdsListView->setModel(modelids);

    checkUserRights();
    prepareMapper();

    // make connections
    connect(languageCombo, SIGNAL(activated(int)), this, SLOT(on_languageCombo_activated(int)));
    connect(but_changePassword, SIGNAL(clicked()), this, SLOT(on_but_changePassword_clicked()));
    connect(but_viewHistory, SIGNAL(clicked()), this, SLOT(on_but_viewHistory_clicked()));

    tabWidget->setCurrentWidget(tabIdentity);
    tabHeadersFooters->setCurrentWidget(genericTab);
}

/**
  \brief Mapper preparer
*/
void UserViewerPrivate::prepareMapper()
{
    m_Mapper = new QDataWidgetMapper(m_Parent);
    m_Mapper->setModel(UserModel::instance());
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->addMapping(uuidLineEdit, Core::IUser::Uuid);
    m_Mapper->addMapping(titleCombo, Core::IUser::TitleIndex, "currentIndex");
    m_Mapper->addMapping(genderCombo, Core::IUser::GenderIndex, "currentIndex");
    m_Mapper->addMapping(nameLineEdit, Core::IUser::Name);
    m_Mapper->addMapping(loginLineEdit, Core::IUser::DecryptedLogin);
    m_Mapper->addMapping(secNameLineEdit, Core::IUser::SecondName);
    m_Mapper->addMapping(firstnameLineEdit, Core::IUser::Firstname);
    m_Mapper->addMapping(lastLoginDateTimeEdit, Core::IUser::LastLogin);
    m_Mapper->addMapping(languageCombo, Core::IUser::LanguageIndex, "currentIndex");
    m_Mapper->addMapping(adressTextEdit, Core::IUser::Adress, "plainText");
    m_Mapper->addMapping(countryLineEdit, Core::IUser::Country);
    m_Mapper->addMapping(zipcodeLineEdit, Core::IUser::Zipcode);
    m_Mapper->addMapping(cityLineEdit, Core::IUser::City);
    m_Mapper->addMapping(tel1LineEdit, Core::IUser::Tel1);
    m_Mapper->addMapping(tel2LineEdit, Core::IUser::Tel2);
    m_Mapper->addMapping(tel3LineEdit, Core::IUser::Tel3);
    m_Mapper->addMapping(faxLineEdit, Core::IUser::Fax);
    m_Mapper->addMapping(mailLineEdit, Core::IUser::Mail);
    m_Mapper->addMapping(specialtyListView, Core::IUser::Specialities, "stringList");
    m_Mapper->addMapping(qualificationsListView, Core::IUser::Qualifications, "stringList");
    m_Mapper->addMapping(practIdsListView, Core::IUser::PractitionerId, "stringList");

    m_Mapper->addMapping(genericPreview->headerEditor() , Core::IUser::GenericHeader, "html");
    m_Mapper->addMapping(genericPreview->headerPresenceCombo(), Core::IUser::GenericHeaderPresence, "currentIndex");
    m_Mapper->addMapping(genericPreview->footerEditor() , Core::IUser::GenericFooter, "html");
    m_Mapper->addMapping(genericPreview->headerPresenceCombo(), Core::IUser::GenericFooterPresence, "currentIndex");
    m_Mapper->addMapping(genericPreview->watermarkEditor(), Core::IUser::GenericWatermark, "html");
    m_Mapper->addMapping(genericPreview->watermarkPresenceCombo(), Core::IUser::GenericWatermarkPresence, "currentIndex");

    m_Mapper->addMapping(adminPreview->headerEditor(), Core::IUser::AdministrativeHeader, "html");
    m_Mapper->addMapping(adminPreview->headerPresenceCombo(), Core::IUser::AdministrativeHeaderPresence, "currentIndex");
    m_Mapper->addMapping(adminPreview->footerEditor(), Core::IUser::AdministrativeFooter, "html");
    m_Mapper->addMapping(adminPreview->footerPresenceCombo(), Core::IUser::AdministrativeFooterPresence, "currentIndex");
    m_Mapper->addMapping(adminPreview->watermarkEditor(), Core::IUser::AdministrativeWatermark, "html");
    m_Mapper->addMapping(adminPreview->watermarkPresenceCombo(), Core::IUser::AdministrativeWatermarkPresence, "currentIndex");

    m_Mapper->addMapping(prescriptionPreview->headerEditor(), Core::IUser::PrescriptionHeader, "html");
    m_Mapper->addMapping(prescriptionPreview->headerPresenceCombo(), Core::IUser::PrescriptionHeaderPresence, "currentIndex");
    m_Mapper->addMapping(prescriptionPreview->footerEditor(), Core::IUser::PrescriptionFooter, "html");
    m_Mapper->addMapping(prescriptionPreview->footerPresenceCombo(), Core::IUser::PrescriptionFooterPresence, "currentIndex");
    m_Mapper->addMapping(prescriptionPreview->watermarkEditor(), Core::IUser::PrescriptionWatermark, "html");
    m_Mapper->addMapping(prescriptionPreview->watermarkPresenceCombo(), Core::IUser::PrescriptionWatermarkPresence, "currentIndex");

    m_Mapper->addMapping(userManagerRightsListWidget, Core::IUser::ManagerRights, "rights");
    m_Mapper->addMapping(drugsRightsListWidget, Core::IUser::DrugsRights, "rights");
    m_Mapper->addMapping(medicalRightsListWidget, Core::IUser::MedicalRights, "rights");
    m_Mapper->addMapping(paramedicalRightsWidget, Core::IUser::ParamedicalRights, "rights");
    m_Mapper->addMapping(administrativeRightsWidget, Core::IUser::AdministrativeRights, "rights");

    m_Mapper->setCurrentModelIndex(UserModel::instance()->currentUserIndex());
    // make connections
    connect (m_Mapper->model(), SIGNAL(modelReset()), this, SLOT(on_modelReseted()));
}

/** \brief Change current user view. No save are done into the database from the model. */
void UserViewerPrivate::on_modelReseted()
{
    changeUserIndex(UserModel::instance()->currentUserIndex().row());
}

/** \brief Verify rights of user */
void UserViewerPrivate::checkUserRights()
{
    UserModel *m = UserModel::instance();
    int currentUserRow = m->currentUserIndex().row();
    if (currentUserRow == m_Row){
        // showing currentuser
        Core::IUser::UserRights r = Core::IUser::UserRights(m->index(currentUserRow, Core::IUser::ManagerRights).data().toInt());
        m_CanModify = (r ^ Core::IUser::WriteOwn);
        m_CanRead = (r ^ Core::IUser::ReadOwn);
    } else {
        // not showing currentuser
        Core::IUser::UserRights r = Core::IUser::UserRights(m->index(currentUserRow, Core::IUser::ManagerRights).data().toInt());
        m_CanModify = (r & Core::IUser::WriteAll);
        m_CanRead = (r & Core::IUser::ReadAll);
    }
    // enable editors
    foreach(QLineEdit *l, findChildren<QLineEdit *>())
        l->setEnabled(m_CanModify);
    foreach(QComboBox *c, findChildren<QComboBox *>())
        c->setEnabled(m_CanModify);
    foreach(QPushButton *c, findChildren<QPushButton *>())
        c->setEnabled(m_CanModify);
}

/** \brief Lang change requested by user from the ui */
void UserViewerPrivate::on_languageCombo_activated(int)
{
    Core::Translators::instance()->changeLanguage(languageCombo->currentText());
    retranslateUi(m_Parent);
}

void UserViewerPrivate::on_but_changePassword_clicked()
{
    UserModel *m = UserModel::instance();
    UserPasswordDialog d(m->index(m_Row, Core::IUser::Password).data().toString(), m_Parent);
    if (d.exec() == QDialog::Accepted){
        if (! d.canGetNewPassword())
            return;
        QModelIndex idx = m->index(m_Row, Core::IUser::Password);
        m->setData(idx, d.cryptedPassword());
    }
}

void UserViewerPrivate::on_but_viewHistory_clicked()
{
    UserModel *m = UserModel::instance();
    Utils::informativeMessageBox(tr("Login history."),
                                     tr("User %1\nLast connection : %2")
                           .arg(m->index(m_Row,Core::IUser::Name).data().toString())
                           .arg(m->index(m_Row,Core::IUser::LastLogin).data().toDateTime().toString()),
                            m->index(m_Row,Core::IUser::LoginHistory).data().toString(),
                            qApp->applicationName());
}
