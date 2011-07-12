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
#include "defautuserviewerpages.h"
#include "usermodel.h"

#include <translationutils/constanttranslations.h>

#include <QDataWidgetMapper>

#include "ui_userviewer_identity.h"
#include "ui_userviewer_medicalrights.h"
#include "ui_userviewer_papers.h"
#include "ui_userviewer_professional.h"

using namespace UserPlugin;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
}  // End anonymous namespace

DefautUserIdentityWidget::DefautUserIdentityWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserViewer_IdentityUI)
{
    ui->setupUi(this);
    ui->languageCombo->setDisplayMode(Views::LanguageComboBox::AvailableTranslations);

    // prepare the mapper
    m_Mapper = new QDataWidgetMapper(this);
    m_Mapper->setModel(UserModel::instance());
    m_Mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    m_Mapper->addMapping(ui->uuidLineEdit, Core::IUser::Uuid);
    m_Mapper->addMapping(ui->titleCombo, Core::IUser::TitleIndex, "currentIndex");
    m_Mapper->addMapping(ui->genderCombo, Core::IUser::GenderIndex, "currentIndex");
    m_Mapper->addMapping(ui->nameLineEdit, Core::IUser::Name);
    m_Mapper->addMapping(ui->loginLineEdit, Core::IUser::ClearLogin);
    m_Mapper->addMapping(ui->secNameLineEdit, Core::IUser::SecondName);
    m_Mapper->addMapping(ui->firstnameLineEdit, Core::IUser::Firstname);
    m_Mapper->addMapping(ui->lastLoginDateTimeEdit, Core::IUser::LastLogin);
    m_Mapper->addMapping(ui->languageCombo, Core::IUser::LocaleCodedLanguage, "currentLanguage");

//    m_Mapper->setCurrentIndex(UserModel::instance()->currentUserIndex().row());
}

DefautUserIdentityWidget::~DefautUserIdentityWidget()
{
    delete ui;
}

void DefautUserIdentityWidget::clear()
{
//    m_Mapper->model()->revert();
}

bool DefautUserIdentityWidget::submit()
{
    return m_Mapper->submit();
}


DefautUserIdentityPage::DefautUserIdentityPage(QObject *parent) :
    IUserViewerPage(parent)
{
    setObjectName("DefautUserIdentityPage");
}

DefautUserIdentityPage::~DefautUserIdentityPage()
{}

QString DefautUserIdentityPage::uid() const
{
    return objectName();
}

QString DefautUserIdentityPage::name() const
{
    return tkTr(Trans::Constants::IDENTITY_TEXT);
}

QStringList DefautUserIdentityPage::categoryTree() const
{
    return QStringList() << tkTr(Trans::Constants::GENERAL);
}

QWidget *DefautUserIdentityPage::createWidget(QWidget *parent)
{
    if (!m_Widget)
        m_Widget = new DefautUserIdentityWidget(parent);
    return m_Widget;
}

void DefautUserIdentityPage::setUserIndex(const QModelIndex &index)
{
    // checkUserRights();
}

bool DefautUserIdentityPage::clear()
{
    if (m_Widget)
        m_Widget->clear();
    return true;
}

bool DefautUserIdentityPage::submit()
{
    if (m_Widget)
        return m_Widget->submit();
    return false;
}

