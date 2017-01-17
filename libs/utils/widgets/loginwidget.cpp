/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "loginwidget.h"

#include <translationutils/constants.h>
#include <translationutils/trans_database.h>

#include "ui_loginwidget.h"

using namespace Utils;
using namespace Trans::ConstantTranslations;

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    layout()->setMargin(2);
    ui->loginLabel->setText(tkTr(Trans::Constants::LOGIN));
    ui->passwordLabel->setText(tkTr(Trans::Constants::PASSWORD));
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::setToggleViewIcon(const QString &fullAbsPath)
{
    ui->login->setIcon(QIcon(fullAbsPath));
    ui->password->setIcon(QIcon(fullAbsPath));
}

void LoginWidget::focusLogin()
{
    this->setFocus();
    ui->login->setFocus();
}

void LoginWidget::toggleLoginEcho(bool visible)
{
    if (visible)
        ui->login->setEchoMode(QLineEdit::Normal);
    else
        ui->login->setEchoMode(QLineEdit::Password);
}

void LoginWidget::togglePasswordEcho(bool visible)
{
    if (visible)
        ui->password->setEchoMode(QLineEdit::Normal);
    else
        ui->password->setEchoMode(QLineEdit::Password);
}

QString LoginWidget::login() const
{
    return ui->login->text();
}

QString LoginWidget::password() const
{
    return ui->password->text();
}

void LoginWidget::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::LanguageChange) {
        // Retranslate
        ui->loginLabel->setText(tkTr(Trans::Constants::LOGIN));
        ui->passwordLabel->setText(tkTr(Trans::Constants::PASSWORD));
    }
}
