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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "defaultuserviewerpages.h"
#include "userpassworddialog.h"
#include <usermanagerplugin/usermodel.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <printerplugin/printer.h>
#include <printerplugin/textdocumentextra.h>

#include <identityplugin/identityeditorwidget.h>

#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QDataWidgetMapper>
#include <QStringListModel>

#include "ui_userviewer_contact.h"
#include "ui_userviewer_medicalrights.h"
#include "ui_userviewer_papers.h"
#include "ui_userviewer_professional.h"

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////   DefaultUserContactWidget   ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
DefaultUserContactWidget::DefaultUserContactWidget(QWidget *parent) :
    UserPlugin::IUserViewerWidget(parent),
    m_Model(0)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    setLayout(lay);
    lay->setMargin(0);
    lay->setSpacing(0);
    m_identity = new Identity::IdentityEditorWidget(this);
//    m_identity->setAvailableWidgets(Identity::IdentityEditorWidget::FullIdentity | Identity::IdentityEditorWidget::FullAddress | Identity::IdentityEditorWidget::Photo | Identity::IdentityEditorWidget::FullLogin);
    lay->addWidget(m_identity);
}

DefaultUserContactWidget::~DefaultUserContactWidget()
{
}

void DefaultUserContactWidget::setUserModel(UserModel *model)
{
    qWarning() << "DefaultUserContactWidget::setUserModel" << model;
    m_Model = model;
    m_identity->setModel(model);
    m_identity->addMapping(Identity::IdentityEditorWidget::Street, Core::IUser::Street);
    m_identity->addMapping(Identity::IdentityEditorWidget::Province, Core::IUser::StateProvince);
    m_identity->addMapping(Identity::IdentityEditorWidget::City, Core::IUser::City);
    m_identity->addMapping(Identity::IdentityEditorWidget::Country_TwoCharIso, Core::IUser::Country);
    m_identity->addMapping(Identity::IdentityEditorWidget::Zipcode, Core::IUser::Zipcode);

    m_identity->addMapping(Identity::IdentityEditorWidget::FirstName, Core::IUser::Firstname);
    m_identity->addMapping(Identity::IdentityEditorWidget::BirthName, Core::IUser::Name);
    m_identity->addMapping(Identity::IdentityEditorWidget::SecondName, Core::IUser::SecondName);
    m_identity->addMapping(Identity::IdentityEditorWidget::TitleIndex, Core::IUser::TitleIndex);
    m_identity->addMapping(Identity::IdentityEditorWidget::GenderIndex, Core::IUser::GenderIndex);
    m_identity->addMapping(Identity::IdentityEditorWidget::LanguageIso, Core::IUser::LanguageISO);
    m_identity->addMapping(Identity::IdentityEditorWidget::DateOfBirth, Core::IUser::DateOfBirth);
    m_identity->addMapping(Identity::IdentityEditorWidget::Photo, Core::IUser::PhotoPixmap);

    m_identity->addMapping(Identity::IdentityEditorWidget::Extra_Login, Core::IUser::ClearLogin);
    m_identity->addMapping(Identity::IdentityEditorWidget::Extra_Password, Core::IUser::Password);
}

void DefaultUserContactWidget::setUserIndex(const int row)
{
    if (m_identity)
        m_identity->setCurrentIndex(m_Model->index(row, 0));
}

void DefaultUserContactWidget::clear()
{
    m_identity->clear();
}

bool DefaultUserContactWidget::submit()
{
    return m_identity->submit();
}

//void DefaultUserContactWidget::changeEvent(QEvent *e)
//{
//    if (e->type()==QEvent::LanguageChange) {
//        ui->retranslateUi(this);
//    }
//}

DefaultUserContactPage::DefaultUserContactPage(QObject *parent) :
    IUserViewerPage(parent)
{
    setObjectName("DefaultUserContactPage");
}

DefaultUserContactPage::~DefaultUserContactPage()
{}

QString DefaultUserContactPage::id() const
{
    return objectName();
}

QString DefaultUserContactPage::displayName() const
{
    return tr("Contact");
}

QString DefaultUserContactPage::category() const
{
    return tkTr(Trans::Constants::IDENTITY_TEXT);
}

QString DefaultUserContactPage::title() const
{
    return tr("Contact");
}

int DefaultUserContactPage::sortIndex() const
{
    return 20;
}

QWidget *DefaultUserContactPage::createPage(QWidget *parent)
{
    DefaultUserContactWidget *w = new DefaultUserContactWidget(parent);
    w->setParentPageId(id());
    return w;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////   DefaultUserProfessionalWidget   /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
DefaultUserProfessionalWidget::DefaultUserProfessionalWidget(QWidget *parent) :
    UserPlugin::IUserViewerWidget(parent),
    ui(new Ui::UserViewer_ProfessionalUI),
    m_Mapper(0),
    m_Model(0)
{
    ui->setupUi(this);
    QStringListModel *m1 = new QStringListModel(ui->specialtyListView);
    ui->specialtyListView->setModel(m1);
    QStringListModel *m2 = new QStringListModel(ui->qualificationsListView);
    ui->qualificationsListView->setModel(m2);
    QStringListModel *m3 = new QStringListModel(ui->practIdsListView);
    ui->practIdsListView->setModel(m3);
}

DefaultUserProfessionalWidget::~DefaultUserProfessionalWidget()
{
    delete ui;
}

void DefaultUserProfessionalWidget::setUserModel(UserModel *model)
{
    // prepare the mapper
    if (!m_Mapper) {
        m_Mapper = new QDataWidgetMapper(this);
    }
    m_Mapper->setModel(model);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_Mapper->addMapping(ui->specialtyListView, Core::IUser::Specialities, "stringList");
    m_Mapper->addMapping(ui->qualificationsListView, Core::IUser::Qualifications, "stringList");
    m_Mapper->addMapping(ui->practIdsListView, Core::IUser::ProfessionalIdentifiants, "stringList");
}

void DefaultUserProfessionalWidget::setUserIndex(const int index)
{
    if (m_Mapper)
        m_Mapper->setCurrentIndex(index);
}

void DefaultUserProfessionalWidget::clear()
{
//    m_Mapper->model()->revert();
}

bool DefaultUserProfessionalWidget::submit()
{
    if (m_Mapper)
        return m_Mapper->submit();
    return false;
}

void DefaultUserProfessionalWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}


DefaultUserProfessionalPage::DefaultUserProfessionalPage(QObject *parent) :
    IUserViewerPage(parent)
{
    setObjectName("DefaultUserProfessionalPage");
}

DefaultUserProfessionalPage::~DefaultUserProfessionalPage()
{}

QString DefaultUserProfessionalPage::id() const
{
    return objectName();
}

QString DefaultUserProfessionalPage::displayName() const
{
    return tr("Professional identifiers");
}

QString DefaultUserProfessionalPage::category() const
{
    return tkTr(Trans::Constants::IDENTITY_TEXT);
}

QString DefaultUserProfessionalPage::title() const
{
    return tr("Professional identifiers");
}

int DefaultUserProfessionalPage::sortIndex() const
{
    return 30;
}

QWidget *DefaultUserProfessionalPage::createPage(QWidget *parent)
{
    DefaultUserProfessionalWidget *w = new DefaultUserProfessionalWidget(parent);
    w->setParentPageId(id());
    return w;
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////   DefaultUserRightsWidget   //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
DefaultUserRightsWidget::DefaultUserRightsWidget(QWidget *parent) :
    UserPlugin::IUserViewerWidget(parent),
    ui(new Ui::UserViewer_RightsUI),
    m_Mapper(0),
    m_Model(0)
{
    ui->setupUi(this);
}

DefaultUserRightsWidget::~DefaultUserRightsWidget()
{
    delete ui;
}

void DefaultUserRightsWidget::setUserModel(UserModel *model)
{
    // prepare the mapper
    if (!m_Mapper) {
        m_Mapper = new QDataWidgetMapper(this);
    }
    m_Mapper->setModel(model);
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->addMapping(ui->userManagerRightsListWidget, Core::IUser::ManagerRights, "rights");
    m_Mapper->addMapping(ui->drugsRightsListWidget, Core::IUser::DrugsRights, "rights");
    m_Mapper->addMapping(ui->medicalRightsListWidget, Core::IUser::MedicalRights, "rights");
    m_Mapper->addMapping(ui->paramedicalRightsWidget, Core::IUser::ParamedicalRights, "rights");
    m_Mapper->addMapping(ui->agendaRightsWidget, Core::IUser::AgendaRights, "rights");
    m_Mapper->addMapping(ui->administrativeRightsWidget, Core::IUser::AdministrativeRights, "rights");
}

void DefaultUserRightsWidget::setUserIndex(const int index)
{
    if (m_Mapper)
        m_Mapper->setCurrentIndex(index);
}

void DefaultUserRightsWidget::clear()
{
//    m_Mapper->model()->revert();
}

bool DefaultUserRightsWidget::submit()
{
    if (m_Mapper)
        return m_Mapper->submit();
    return false;
}

void DefaultUserRightsWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}


DefaultUserRightsPage::DefaultUserRightsPage(QObject *parent) :
    IUserViewerPage(parent)
{
    setObjectName("DefaultUserRightsPage");
}

DefaultUserRightsPage::~DefaultUserRightsPage()
{}

QString DefaultUserRightsPage::id() const
{
    return objectName();
}

QString DefaultUserRightsPage::displayName() const
{
    return tkTr(Trans::Constants::RIGHTS);
}

QString DefaultUserRightsPage::category() const
{
    return tkTr(Trans::Constants::RIGHTS);
}

QString DefaultUserRightsPage::title() const
{
    return tkTr(Trans::Constants::USER_RIGHTS);
}

int DefaultUserRightsPage::sortIndex() const
{
    return 40;
}

QWidget *DefaultUserRightsPage::createPage(QWidget *parent)
{
    DefaultUserRightsWidget *w = new DefaultUserRightsWidget(parent);
    w->setParentPageId(id());
    return w;
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////   DefaultUserPapersWidget   //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
DefaultUserPapersWidget::DefaultUserPapersWidget(const int type, QWidget *parent) :
    UserPlugin::IUserViewerWidget(parent),
    m_Mapper(0),
    m_Model(0),
    m_type(type),
    m_row(-1)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);
    preview = Print::Printer::previewer(this);
    layout->addWidget(preview);
}

DefaultUserPapersWidget::~DefaultUserPapersWidget()
{
}

void DefaultUserPapersWidget::setUserModel(UserModel *model)
{
    m_Model = model;
}

void DefaultUserPapersWidget::setUserIndex(const int index)
{
    if (m_Model) {
        m_row = index;
        switch (m_type) {
        case DefaultUserPapersPage::GenericPaper:
            preview->setHeader(m_Model->paper(m_row, Core::IUser::GenericHeader));
            preview->setFooter(m_Model->paper(m_row, Core::IUser::GenericFooter));
            preview->setWatermark(m_Model->paper(m_row, Core::IUser::GenericWatermark));
            break;

        case DefaultUserPapersPage::AdministrativePaper:
            preview->setHeader(m_Model->paper(m_row, Core::IUser::AdministrativeHeader));
            preview->setFooter(m_Model->paper(m_row, Core::IUser::AdministrativeFooter));
            preview->setWatermark(m_Model->paper(m_row, Core::IUser::AdministrativeWatermark));
            break;

        case DefaultUserPapersPage::PrescriptionPaper:
            preview->setHeader(m_Model->paper(m_row, Core::IUser::PrescriptionHeader));
            preview->setFooter(m_Model->paper(m_row, Core::IUser::PrescriptionFooter));
            preview->setWatermark(m_Model->paper(m_row, Core::IUser::PrescriptionWatermark));
            break;
        }
    }
}

void DefaultUserPapersWidget::clear()
{
//    m_Mapper->model()->revert();
}

bool DefaultUserPapersWidget::submit()
{
    if (m_Model) {
        Print::TextDocumentExtra *text = 0;
        QString uid = m_Model->index(m_row, Core::IUser::Uuid).data().toString();
        switch (m_type) {
        case DefaultUserPapersPage::GenericPaper:
            text = new Print::TextDocumentExtra();
            preview->headerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::GenericHeader, text);
            text = new Print::TextDocumentExtra();
            preview->footerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::GenericFooter, text);
            text = new Print::TextDocumentExtra();
            preview->watermarkToPointer(text);
            m_Model->setPaper(uid, Core::IUser::GenericWatermark, text);
            break;

        case DefaultUserPapersPage::AdministrativePaper:
            text = new Print::TextDocumentExtra();
            preview->headerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::AdministrativeHeader, text);
            text = new Print::TextDocumentExtra();
            preview->footerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::AdministrativeFooter, text);
            text = new Print::TextDocumentExtra();
            preview->watermarkToPointer(text);
            m_Model->setPaper(uid, Core::IUser::AdministrativeWatermark, text);
            break;

        case DefaultUserPapersPage::PrescriptionPaper:
            text = new Print::TextDocumentExtra();
            preview->headerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::PrescriptionHeader, text);
            text = new Print::TextDocumentExtra();
            preview->footerToPointer(text);
            m_Model->setPaper(uid, Core::IUser::PrescriptionFooter, text);
            text = new Print::TextDocumentExtra();
            preview->watermarkToPointer(text);
            m_Model->setPaper(uid, Core::IUser::PrescriptionWatermark, text);
            break;
        }
        // sync settings to save user papers
        settings()->sync();
    }
    return true;
}


DefaultUserPapersPage::DefaultUserPapersPage(const PaperType type, QObject *parent) :
    IUserViewerPage(parent),
    m_type(type)
{
    setObjectName("DefaultUserPapersPage_" + QString::number((int)type));
}

DefaultUserPapersPage::~DefaultUserPapersPage()
{}

QString DefaultUserPapersPage::id() const
{
    return objectName();
}

QString DefaultUserPapersPage::displayName() const
{
    switch (m_type) {
    case GenericPaper: return tr("Generic papers");
    case AdministrativePaper: return tr("Administrative papers");
    case PrescriptionPaper:return tr("Prescription papers");
    }
    return QString();
}

QString DefaultUserPapersPage::category() const
{
    return tr("Documents");
}

QString DefaultUserPapersPage::title() const
{
    return displayName();
}

int DefaultUserPapersPage::sortIndex() const
{
    return 50;
}

QWidget *DefaultUserPapersPage::createPage(QWidget *parent)
{
    DefaultUserPapersWidget *w = new DefaultUserPapersWidget(m_type, parent);
    w->setParentPageId(id());
    return w;
}



