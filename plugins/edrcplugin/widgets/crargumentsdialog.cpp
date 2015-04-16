/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "crargumentsdialog.h"
#include <edrcplugin/edrccore.h>
#include <edrcplugin/database/edrcbase.h>

#include "ui_crargumentsdialog.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>

#include <utils/global.h>

using namespace eDRC;
using namespace Internal;

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline eDRC::EdrcCore &edrcCore() {return eDRC::EdrcCore::instance();}
static inline eDRC::Internal::DrcDatabase &edrcBase() {return eDRC::EdrcCore::instance().edrcBase();}

CrArgumentsDialog::CrArgumentsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrArgumentsDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Result of consultation arguments"));
    setWindowIcon(theme()->icon("sfmg_logo.png"));
    resize(500, 350);
    Utils::centerWidget(this, parent);
}

CrArgumentsDialog::~CrArgumentsDialog()
{
    delete ui;
}

void CrArgumentsDialog::setRcId(const int rcId)
{
    ui->rcLabel->setText(edrcBase().getCrLabel(rcId));
    ui->arguments->setHtml(edrcBase().getCrArguments(rcId, true));
}
