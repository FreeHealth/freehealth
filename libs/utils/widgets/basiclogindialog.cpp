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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developers:                                                     *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "basiclogindialog.h"
#include "ui_basiclogindialog.h"

#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QPushButton>
#include <QTextBrowser>

using namespace Utils;
using namespace Trans::ConstantTranslations;

BasicLoginDialog::BasicLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BasicLoginDialog),
    _more(0),
    _moreBrowser(0)
{
    ui->setupUi(this);
    ui->loginWidget->togglePasswordEcho(false);
    adjustSize();
}

BasicLoginDialog::~BasicLoginDialog()
{
    delete ui;
}

void BasicLoginDialog::setTitle(const QString &title)
{
    ui->title->setText(title);
    this->setWindowTitle(title);
}

void BasicLoginDialog::setToggleViewIcon(const QString &fullAbsPath)
{
    ui->loginWidget->setToggleViewIcon(fullAbsPath);
}

void BasicLoginDialog::setHtmlExtraInformation(const QString &html)
{
    // Add a 'more information' button
    _more = ui->buttonBox->addButton(tkTr(Trans::Constants::MORE_INFORMATION), QDialogButtonBox::ActionRole);
    _moreBrowser = new QTextBrowser(this);
    _moreBrowser->setHtml(html);
    _moreBrowser->setHidden(true);
    layout()->addWidget(_moreBrowser);
    connect(_more, SIGNAL(clicked()), this, SLOT(onMoreClicked()));
}

void BasicLoginDialog::focusLogin()
{
    ui->loginWidget->focusLogin();
}

QString BasicLoginDialog::login() const
{
    return ui->loginWidget->login();
}

QString BasicLoginDialog::password() const
{
    return ui->loginWidget->password();
}

void BasicLoginDialog::onMoreClicked()
{
    if (!_moreBrowser)
        return;
    _moreBrowser->setVisible(!_moreBrowser->isVisible());
    adjustSize();
}
