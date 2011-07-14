/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "agendauserviewerpages.h"

#include <utils/global.h>
#include <translationutils/constanttranslations.h>

#include <QDataWidgetMapper>

#include "ui_agendauserviewerwidget.h"

using namespace Agenda;
using namespace Internal;
using namespace Trans::ConstantTranslations;

AgendaUserIdentityWidget::AgendaUserIdentityWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgendaUserViewer),
    m_Mapper(0),
    m_Model(0)
{
    ui->setupUi(this);
}

AgendaUserIdentityWidget::~AgendaUserIdentityWidget()
{
    delete ui;
}

void AgendaUserIdentityWidget::setUserModel(UserPlugin::UserModel *model)
{
//    // prepare the mapper
//    if (!m_Mapper) {
//        m_Mapper = new QDataWidgetMapper(this);
//    }
//    m_Mapper->setModel(model);
//    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
//    m_Mapper->addMapping(ui->uuidLineEdit, Core::IUser::Uuid);
//    m_Mapper->addMapping(ui->titleCombo, Core::IUser::TitleIndex, "currentIndex");
//    m_Mapper->addMapping(ui->genderCombo, Core::IUser::GenderIndex, "currentIndex");
//    m_Mapper->addMapping(ui->nameLineEdit, Core::IUser::Name);
//    m_Mapper->addMapping(ui->loginLineEdit, Core::IUser::ClearLogin);
//    m_Mapper->addMapping(ui->secNameLineEdit, Core::IUser::SecondName);
//    m_Mapper->addMapping(ui->firstnameLineEdit, Core::IUser::Firstname);
//    m_Mapper->addMapping(ui->lastLoginDateTimeEdit, Core::IUser::LastLogin);
//    m_Mapper->addMapping(ui->languageCombo, Core::IUser::LocaleCodedLanguage, "currentLanguage");
}

void AgendaUserIdentityWidget::setUserIndex(const int index)
{
//    if (m_Mapper)
//        m_Mapper->setCurrentIndex(index);
}

void AgendaUserIdentityWidget::clear()
{
//    m_Mapper->model()->revert();
}

bool AgendaUserIdentityWidget::submit()
{
    return m_Mapper->submit();
}

//void AgendaUserIdentityWidget::on_but_changePassword_clicked()
//{
//    int row = m_Mapper->currentIndex();
//    UserPasswordDialog d(m_Model->index(row, Core::IUser::Password).data().toString(), this);
//    if (d.exec() == QDialog::Accepted){
//        if (!d.canGetNewPassword())
//            return;
//        QModelIndex idx = m_Model->index(row, Core::IUser::Password);
//        m_Model->setData(idx, d.cryptedPassword());
//    }
//}

//void AgendaUserIdentityWidget::on_but_viewHistory_clicked()
//{
//    int row = m_Mapper->currentIndex();
//    Utils::informativeMessageBox(tr("Login history."),
//                                     tr("User %1\nLast connection : %2")
//                           .arg(m_Model->index(row,Core::IUser::Name).data().toString())
//                           .arg(m_Model->index(row,Core::IUser::LastLogin).data().toDateTime().toString()),
//                            m_Model->index(row,Core::IUser::LoginHistory).data().toString(),
//                            qApp->applicationName());
//}


AgendaUserViewerPage::AgendaUserViewerPage(QObject *parent) :
    UserPlugin::IUserViewerPage(parent),
    m_Model(0)
{
    setObjectName("AgendaUserViewerPage");
}

AgendaUserViewerPage::~AgendaUserViewerPage()
{}

QString AgendaUserViewerPage::id() const
{
    return objectName();
}

QString AgendaUserViewerPage::name() const
{
    return tkTr(Trans::Constants::AGENDA);
}

QString AgendaUserViewerPage::category() const
{
    return tkTr(Trans::Constants::AGENDA);
}

QString AgendaUserViewerPage::title() const
{
    return tr("User agendas");
}

QWidget *AgendaUserViewerPage::createPage(QWidget *parent)
{
    if (!m_Widget)
        m_Widget = new AgendaUserIdentityWidget(parent);
    return m_Widget;
}

void AgendaUserViewerPage::setUserModel(UserPlugin::UserModel *model)
{
    m_Model = model;
    if (m_Widget)
        m_Widget->setUserModel(model);
}

void AgendaUserViewerPage::setUserIndex(const int index)
{
    if (m_Model && m_Widget)
        m_Widget->setUserIndex(index);
}

bool AgendaUserViewerPage::clear()
{
    if (m_Widget)
        m_Widget->clear();
    return true;
}

bool AgendaUserViewerPage::submit()
{
    if (m_Widget)
        return m_Widget->submit();
    return false;
}
