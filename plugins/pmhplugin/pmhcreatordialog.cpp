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
#include "pmhcreatordialog.h"
#include "pmhcore.h"
#include "pmhbase.h"
#include "pmhdata.h"
#include "pmhcategorymodel.h"
#include "constants.h"
//#include "pmhmode.h"

#include <coreplugin/dialogs/helpdialog.h>
#include <coreplugin/icore.h>
//#include <coreplugin/modemanager/modemanager.h>
//#include <coreplugin/modemanager/imode.h>
//#include <coreplugin/constants_menus.h>

#include "ui_pmhcreatordialog.h"

#include <QDebug>

using namespace PMH;

static inline PmhCore *pmhCore() {return PmhCore::instance();}
//static inline Core::ModeManager *modeManager() {return Core::ICore::instance()->modeManager();}

PmhCreatorDialog::PmhCreatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PmhCreatorDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Past Medical History Creator"));
    ui->pmhViewer->setEditMode(PmhViewer::ReadWriteMode);
    ui->pmhViewer->createNewPmh();
    ui->pmhViewer->setShowPatientInformation(true);
    // Get selected category id
//    if (modeManager()->currentMode()->uniqueModeName()==Core::Constants::MODE_PATIENT_HISTORY) {
//        Internal::PmhMode *mode = qobject_cast<Internal::PmhMode*>(modeManager()->currentMode());
//        if (mode) {
//            if (mode->widget()->isVisible()) {

//            }
//        }
//    }
}

PmhCreatorDialog::~PmhCreatorDialog()
{
    delete ui;
}

void PmhCreatorDialog::setCategory(Category::CategoryItem *category)
{
    ui->pmhViewer->setCategoryForPmh(category);
}

void PmhCreatorDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    switch (ui->buttonBox->standardButton(button)) {
    case QDialogButtonBox::Save:
        {
            Internal::PmhData *pmh = ui->pmhViewer->modifiedPmhData();
            // Feed category model with this new PmhData
            pmhCore()->pmhCategoryModel()->addPmhData(pmh);
            accept();
            break;
        }
    case QDialogButtonBox::Cancel: reject(); break;
    case QDialogButtonBox::Help: helpRequested(); break;
    default: break;
    }
}

void PmhCreatorDialog::helpRequested()
{
    Core::HelpDialog::showPage(Constants::H_PMH_CREATOR_PAGE);
}

void PmhCreatorDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
