/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "sfmgaboutdialog.h"
#include "ui_sfmgaboutdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>

using namespace eDRC;
using namespace Internal;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}

SfmgAboutDialog::SfmgAboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SfmgAboutDialog)
{
    ui->setupUi(this);
    ui->logo->setPixmap(theme()->icon("sfmg_logo.png", Core::ITheme::MediumIcon).pixmap(32,32));
    ui->content->setText("<center><b>Société Française de Médecine Générale<br>Société Savante</b><br><br>"
                         "141, avenue de Verdun — 92130 Issy Les Moulineaux<br>"
                         "Tél.: 01 41 90 98 20 — Fax: 01 41 90 07 82<br>"
                         "e-mail : sfmg@sfmg.org • http://www.sfmg.org<br>"
                         "O.N.G. agréée auprès de l’O.M.S.</center>");
    setWindowIcon(theme()->icon("sfmg_logo.png"));
    setWindowTitle("À propos de la Société Française de Médecine Générale");
    //resize(250, 250);
    //Utils::resizeAndCenter(this, parent);
    Utils::centerWidget(this, parent);
}

SfmgAboutDialog::~SfmgAboutDialog()
{
    delete ui;
}
