/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#include "atcpage.h"
#include "atcmodel.h"

#include "ui_atcpage.h"

using namespace DrugsDbCreator;

AtcPage::AtcPage(QObject *parent) :
        IToolPage(parent)
{}

QWidget *AtcPage::createPage(QWidget *parent)
{
    return new AtcWidget(parent);
}


AtcWidget::AtcWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::AtcPage)
{
    ui->setupUi(this);
    ui->atcView->setModel(AtcModel::instance(this));
}

AtcWidget::~AtcWidget()
{
    delete ui;
}
