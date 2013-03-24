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
#include "fsp.h"
#include "fspprinter.h"

#include "ui_fspprinterdialog.h"
#include "ui_fspprinterdialog_patient.h"
#include "ui_fspprinterdialog_conds.h"
#include "ui_fspprinterdialog_amounts.h"
#include "ui_fspprinterdialog_prerecorded.h"

#include <utils/global.h>
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
        _preRecUi(0),
        _patientDetailsWidget(0),
        _condDetailsWidget(0),
        _actDetailsWidget(0),
        _previewDetailsWidget(0),
        _prerecordedFspDetailsWidget(0),
        _preview(0),
        q(parent)
    {
        ui = new Ui::FspPrinterDialog;
        _patientUi = new Ui::FspPrinterDialogPatient;
        _condsUi = new Ui::FspPrinterDialogConds;
        _amountsUi = new Ui::FspPrinterDialogAmounts;
        _preRecUi = new Ui::FspPrinterDialogPrecorded;
    }
    
    ~FspPrinterDialogPrivate()
    {
        delete ui;
        delete _patientUi;
        delete _condsUi;
        delete _amountsUi;
        delete _preRecUi;
    }
    
    void fspToUi(const Fsp &fsp)
    {
        _fsp = fsp;
        //        fsp.data(Fsp::Bill_Number, "123456789012345");
        //        fsp.data(Fsp::Bill_Date, QDate::currentDate());

        _patientUi->name->setText(fsp.data(Fsp::Patient_FullName).toString());
        _patientUi->dob->setDate(fsp.data(Fsp::Patient_DateOfBirth).toDate());
        _patientUi->nns->setText(fsp.data(Fsp::Patient_Personal_NSS).toString().leftJustified(13, '_') + fsp.data(Fsp::Patient_Personal_NSSKey).toString().leftJustified(2, '_'));
        _patientUi->orga->setText(fsp.data(Fsp::Patient_Assurance_Number).toString());
        _patientUi->assureName->setText(fsp.data(Fsp::Patient_Assure_FullName).toString());
        _patientUi->assureNss->setText(fsp.data(Fsp::Patient_Assure_NSS).toString().leftJustified(13, '_') + fsp.data(Fsp::Patient_Assure_NSSKey).toString().leftJustified(2, '_'));
        _patientUi->address->setText(fsp.data(Fsp::Patient_FullAddress).toString());

        if (!fsp.data(Fsp::Condition_Maladie).isNull()
                && fsp.data(Fsp::Condition_Maladie).toBool())
            _condsUi->maladie->setChecked(true);
        fsp.data(Fsp::Condition_Maladie_ETM);

        if (!fsp.data(Fsp::Condition_Maladie_ETM_Ald).isNull()
                && fsp.data(Fsp::Condition_Maladie_ETM_Ald).toBool())
            _condsUi->ald->setChecked(true);

        if (!fsp.data(Fsp::Condition_Maladie_ETM_Autre).isNull()
                && fsp.data(Fsp::Condition_Maladie_ETM_Autre).toBool())
            _condsUi->autre->setChecked(true);

        if (!fsp.data(Fsp::Condition_Maladie_ETM_L115).isNull()
                && fsp.data(Fsp::Condition_Maladie_ETM_L115).toBool())
            _condsUi->l115->setChecked(true);

        if (!fsp.data(Fsp::Condition_Maladie_ETM_Prevention).isNull()
                && fsp.data(Fsp::Condition_Maladie_ETM_Prevention).toBool())
            _condsUi->prev->setChecked(true);

        if (fsp.data(Fsp::Condition_Maladie_ETM_AccidentParTiers_Oui).isNull()) {
            _condsUi->accident->setCurrentIndex(0);
        } else {
            if (fsp.data(Fsp::Condition_Maladie_ETM_AccidentParTiers_Oui).toBool())
                _condsUi->accident->setCurrentIndex(1);
            else
                _condsUi->accident->setCurrentIndex(2);
        }
        _condsUi->accidentDate->setDate(fsp.data(Fsp::Condition_Maladie_ETM_AccidentParTiers_Date).toDate());

        if (!fsp.data(Fsp::Condition_Maternite).isNull()
                && fsp.data(Fsp::Condition_Maternite).toBool())
            _condsUi->mater->setChecked(true);
        _condsUi->materDate->setDate(fsp.data(Fsp::Condition_Maternite).toDate());

        if (!fsp.data(Fsp::Condition_ATMP).isNull()
                && fsp.data(Fsp::Condition_ATMP).toBool())
            _condsUi->at->setChecked(true);
        _condsUi->atNb->setText(fsp.data(Fsp::Condition_ATMP_Number).toString());
        _condsUi->atDate->setDate(fsp.data(Fsp::Condition_ATMP_Date).toDate());

        if (!fsp.data(Fsp::Condition_NouveauMedTraitant).isNull()
                && fsp.data(Fsp::Condition_NouveauMedTraitant).toBool())
            _condsUi->nouveau->setChecked(true);
        _condsUi->envoyeur->setText(fsp.data(Fsp::Condition_MedecinEnvoyeur).toString());

        _condsUi->parcours->setCurrentIndex(-1);
        if (fsp.data(Fsp::Condition_AccesSpecifique).toBool()) {
            _condsUi->parcours->setCurrentIndex(1);
        } else if (fsp.data(Fsp::Condition_Urgence).toBool()) {
            _condsUi->parcours->setCurrentIndex(2);
        } else if (fsp.data(Fsp::Condition_HorsResidence).toBool()) {
            _condsUi->parcours->setCurrentIndex(3);
        } else if (fsp.data(Fsp::Condition_Remplace).toBool()) {
            _condsUi->parcours->setCurrentIndex(4);
        } else if (fsp.data(Fsp::Condition_HorsCoordination).toBool()) {
            _condsUi->parcours->setCurrentIndex(5);
        }

        // TODO: _condsUi->prealable->setDate(fsp.data(Fsp::Condition_AccordPrealableDate).toDate());

        //        for(int i=0; i < 4; ++i) {
        //            test.addAmountData(i, Fsp::Amount_Date, QDate::currentDate());
        //            test.addAmountData(i, Fsp::Amount_ActCode, "CODE123456");
        //            test.addAmountData(i, Fsp::Amount_Activity, i);
        //            test.addAmountData(i, Fsp::Amount_CV, "CV");
        //            test.addAmountData(i, Fsp::Amount_OtherAct1, "ACT1");
        //            test.addAmountData(i, Fsp::Amount_OtherAct2, "ACT2");
        //            test.addAmountData(i, Fsp::Amount_Amount, 234.00);
        //            test.addAmountData(i, Fsp::Amount_Depassement, 1);
        //            test.addAmountData(i, Fsp::Amount_Deplacement_IKMD, "IK");
        //            test.addAmountData(i, Fsp::Amount_Deplacement_Nb, 1);
        //            test.addAmountData(i, Fsp::Amount_Deplacement_IKValue, 0.97);
        //        }

//        fsp.data(Fsp::Unpaid_PartObligatoire);
//        fsp.data(Fsp::Unpaid_PartComplementaire);
    }

    FspPrinter::Cerfa cerfa()
    {
        if (ui->cerfa->currentIndex() == 1)
            return FspPrinter::S12541_01;
        return FspPrinter::S12541_02;
    }
    void updatePreview()
    {
        FspPrinter printer;
        printer.setDrawRects(false);
        _preview->setPixmap(printer.preview(_fsp, cerfa()).scaledToWidth(700, Qt::SmoothTransformation));
    }

public:
    Ui::FspPrinterDialog *ui;
    Ui::FspPrinterDialogPatient *_patientUi;
    Ui::FspPrinterDialogConds *_condsUi;
    Ui::FspPrinterDialogAmounts * _amountsUi;
    Ui::FspPrinterDialogPrecorded *_preRecUi;
//    QList<Ui::FspPrinterDialogAmounts *> _amountsUi;
    Utils::DetailsWidget *_patientDetailsWidget, *_condDetailsWidget;
    Utils::DetailsWidget *_actDetailsWidget, *_previewDetailsWidget;
    Utils::DetailsWidget *_prerecordedFspDetailsWidget;
    QLabel *_preview;
    Fsp _fsp;

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
    QWidget *aWCont = new QWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(aWCont);
    aWCont->setLayout(lay);
    for(int i=0; i < 4; ++i) {
        QWidget *aW = new QWidget(this);
        d->_amountsUi->setupUi(aW);
        lay->addWidget(aW);
    }
    d->_actDetailsWidget->setWidget(aWCont);

    d->_previewDetailsWidget = new Utils::DetailsWidget(this);
    d->_previewDetailsWidget->setSummaryText("Prévisualisation");
    d->_previewDetailsWidget->setSummaryFontBold(true);
    d->_previewDetailsWidget->setState(Utils::DetailsWidget::Expanded);
    d->_preview = new QLabel(this);
    d->_preview->setAlignment(Qt::AlignCenter);
    d->_previewDetailsWidget->setWidget(d->_preview);

    d->_prerecordedFspDetailsWidget = new Utils::DetailsWidget(this);
    d->_prerecordedFspDetailsWidget->setSummaryText("Cotations pré-enregistrées");
    d->_prerecordedFspDetailsWidget->setSummaryFontBold(true);
    d->_prerecordedFspDetailsWidget->setState(Utils::DetailsWidget::Expanded);
    QWidget *preW = new QWidget(this);
    d->_preRecUi->setupUi(preW);
    d->_prerecordedFspDetailsWidget->setWidget(preW);

    // Change the button box
    QPushButton *button = d->ui->buttonBox->addButton("Imprimer la FSP", QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()), this, SLOT(printFsp()));
    button = d->ui->buttonBox->addButton("Imprimer un chèque", QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()), this, SLOT(printCheque()));

    d->ui->contentLayout->addWidget(d->_patientDetailsWidget);
    d->ui->contentLayout->addWidget(d->_prerecordedFspDetailsWidget);
    d->ui->contentLayout->addWidget(d->_condDetailsWidget);
    d->ui->contentLayout->addWidget(d->_actDetailsWidget);
    d->ui->contentLayout->addWidget(d->_previewDetailsWidget);
    d->ui->contentLayout->addStretch(100);

    connect(d->ui->vueComplexe, SIGNAL(clicked(bool)), this, SLOT(toggleView(bool)));
    toggleView(d->ui->vueComplexe->isChecked());
    d->updatePreview();
    resize(800, 400);
    Utils::centerWidget(this, parent);
}

/*! Destructor of the Tools::Internal::FspPrinterDialog class */
FspPrinterDialog::~FspPrinterDialog()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool FspPrinterDialog::initialize(const Fsp &fsp)
{
    d->fspToUi(fsp);
    d->updatePreview();
    return true;
}

void FspPrinterDialog::toggleView(bool complex)
{
    d->_condDetailsWidget->setVisible(complex);
    d->_actDetailsWidget->setVisible(complex);
//    d->_prerecordedFspDetailsWidget->setVisible(!complex);
}

void FspPrinterDialog::printFsp()
{
    FspPrinter printer;
    printer.setDrawRects(false);
    printer.print(d->_fsp, d->cerfa(), false);
}

void FspPrinterDialog::printCheque()
{}

