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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Tools::Internal::FspPrinterDialog
 * \brief short description of class
 *
 * Long description of class
 * \sa Tools::
 */

#include "fspprinterdialog.h"
#include "ui_fspprinterdialog.h"
#include "ui_fspprinterdialog_patient.h"
#include "ui_fspprinterdialog_conds.h"
#include "ui_fspprinterdialog_amounts.h"

#include <utils/widgets/detailswidget.h>
#include <translationutils/constants.h>

#include <QPushButton>

#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Tools {
namespace Internal {
class FspPrinterDialogPrivate
{
public:
    FspPrinterDialogPrivate(FspPrinterDialog *parent) :
        ui(0),
        _patientUi(0),
        _condsUi(0),
        _patientDetailsWidget(0),
        _condDetailsWidget(0),
        _actDetailsWidget(0),
        q(parent)
    {
        ui = new Ui::FspPrinterDialog;
        _patientUi = new Ui::FspPrinterDialogPatient;
        _condsUi = new Ui::FspPrinterDialogConds;
        _amountsUi = new Ui::FspPrinterDialogAmounts;
    }
    
    ~FspPrinterDialogPrivate()
    {
        delete ui;
    }
    
public:
    Ui::FspPrinterDialog *ui;
    Ui::FspPrinterDialogPatient *_patientUi;
    Ui::FspPrinterDialogConds *_condsUi;
    Ui::FspPrinterDialogAmounts *_amountsUi;
    Utils::DetailsWidget *_patientDetailsWidget, *_condDetailsWidget, *_actDetailsWidget;

private:
    FspPrinterDialog *q;
};
} // namespace Internal
} // namespace Tools


/*! Constructor of the Tools::Internal::FspPrinterDialog class */
FspPrinterDialog::FspPrinterDialog(QWidget *parent) :
    QDialog(parent),
    d(new FspPrinterDialogPrivate(this))
{
    d->ui->setupUi(this);

    // Create the detailsWidget
    d->_patientDetailsWidget = new Utils::DetailsWidget(this);
    d->_patientDetailsWidget->setSummaryText("Informations patient");
    d->_patientDetailsWidget->setSummaryFontBold(true);
    d->_patientDetailsWidget->setState(Utils::DetailsWidget::Expanded);
    QWidget *pW = new QWidget(this);
    d->_patientUi->setupUi(pW);
    d->_patientDetailsWidget->setWidget(pW);

    d->_condDetailsWidget = new Utils::DetailsWidget(this);
    d->_condDetailsWidget->setSummaryText("Conditions");
    d->_condDetailsWidget->setSummaryFontBold(true);
    d->_condDetailsWidget->setState(Utils::DetailsWidget::Expanded);
    QWidget *cW = new QWidget(this);
    d->_condsUi->setupUi(cW);
    d->_condDetailsWidget->setWidget(cW);

    d->_actDetailsWidget = new Utils::DetailsWidget(this);
    d->_actDetailsWidget->setSummaryText("Actes");
    d->_actDetailsWidget->setSummaryFontBold(true);
    d->_actDetailsWidget->setState(Utils::DetailsWidget::Expanded);
    QWidget *aW = new QWidget(this);
    d->_amountsUi->setupUi(aW);
    d->_actDetailsWidget->setWidget(aW);

    // Change the button box
    QPushButton *button = d->ui->buttonBox->addButton("Imprimer la FSP", QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()), this, SLOT(printFsp()));
    button = d->ui->buttonBox->addButton("Imprimer un chèque", QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()), this, SLOT(printCheque()));

    d->ui->contentLayout->addWidget(d->_patientDetailsWidget);
    d->ui->contentLayout->addWidget(d->_condDetailsWidget);
    d->ui->contentLayout->addWidget(d->_actDetailsWidget);
    d->ui->contentLayout->addStretch(100);

    resize(800, 400);
}

/*! Destructor of the Tools::Internal::FspPrinterDialog class */
FspPrinterDialog::~FspPrinterDialog()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FspPrinterDialog::initialize()
{
    return true;
}

void FspPrinterDialog::printFsp()
{}

void FspPrinterDialog::printCheque()
{}

