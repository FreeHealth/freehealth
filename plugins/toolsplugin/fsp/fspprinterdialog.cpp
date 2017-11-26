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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Tools::Internal::FspPrinterDialog
 * Assistant for French FSP printing.
 */

#include "fspprinterdialog.h"
#include "fsp.h"
#include "fspprinter.h"
#include "fspconstants.h"
#include "fsptemplatemodel.h"

#include "ui_fspprinterdialog.h"
#include "ui_fspprinterdialog_patient.h"
#include "ui_fspprinterdialog_conds.h"
#include "ui_fspprinterdialog_fees.h"
#include "ui_fspprinterdialog_prerecorded.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>

#include <utils/global.h>
#include <utils/widgets/imageviewer.h>
#include <utils/widgets/detailswidget.h>
#include <translationutils/constants.h>

#include <QDir>
#include <QFileInfo>
#include <QPushButton>

#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }

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
        _templateModel(0),
        _patientDetailsWidget(0),
        _condDetailsWidget(0),
        _actDetailsWidget(0),
        _previewDetailsWidget(0),
        _templateDetailsWidget(0),
        _preview(0),
        q(parent)
    {
        ui = new Ui::FspPrinterDialog;
        _patientUi = new Ui::FspPrinterDialogPatient;
        _condsUi = new Ui::FspPrinterDialogConds;
        _feesUi = new Ui::FspPrinterDialogFees;
        _preRecUi = new Ui::FspPrinterDialogPrecorded;
    }

    ~FspPrinterDialogPrivate()
    {
        delete ui;
        delete _patientUi;
        delete _condsUi;
        delete _feesUi;
        delete _preRecUi;
    }

    void clearUi()
    {
        foreach(QLineEdit *edit, q->findChildren<QLineEdit*>()) {
            edit->clear();
        }
        foreach(QCheckBox *check, q->findChildren<QCheckBox*>()) {
            check->setChecked(false);
        }
        foreach(QAbstractSpinBox *spin, q->findChildren<QAbstractSpinBox*>()) {
            spin->clear();
        }
    }

    void fspToUi()
    {
        clearUi();
        _patientUi->billId->setText(_fsp.data(Fsp::Bill_Number).toString());
        _patientUi->billDate->setText(_fsp.data(Fsp::Bill_Date).toString());
        _patientUi->name->setText(_fsp.data(Fsp::Patient_FullName).toString());
        _patientUi->dob->setDate(_fsp.data(Fsp::Patient_DateOfBirth).toDate());
        _patientUi->nns->setText(_fsp.data(Fsp::Patient_Personal_NSS).toString().leftJustified(13, '_') + _fsp.data(Fsp::Patient_Personal_NSSKey).toString().leftJustified(2, '_'));
        _patientUi->orga->setText(_fsp.data(Fsp::Patient_Assurance_Number).toString());
        _patientUi->assureName->setText(_fsp.data(Fsp::Patient_Assure_FullName).toString());
        _patientUi->assureNss->setText(_fsp.data(Fsp::Patient_Assure_NSS).toString().leftJustified(13, '_') + _fsp.data(Fsp::Patient_Assure_NSSKey).toString().leftJustified(2, '_'));
        _patientUi->address->setText(_fsp.data(Fsp::Patient_FullAddress).toString());

        if (!_fsp.data(Fsp::Condition_Maladie).isNull()
                && _fsp.data(Fsp::Condition_Maladie).toBool())
            _condsUi->maladie->setChecked(true);

        if (!_fsp.data(Fsp::Condition_Maladie_ETM).isNull()
                && _fsp.data(Fsp::Condition_Maladie_ETM).toBool()) {
            _condsUi->exoTM_Oui->setChecked(true);
            _condsUi->exoTM_non->setChecked(false);
        } else {
            _condsUi->exoTM_Oui->setChecked(false);
            _condsUi->exoTM_non->setChecked(true);
        }

        if (!_fsp.data(Fsp::Condition_Maladie_ETM_Ald).isNull()
                && _fsp.data(Fsp::Condition_Maladie_ETM_Ald).toBool())
            _condsUi->ald->setChecked(true);

        if (!_fsp.data(Fsp::Condition_Maladie_ETM_Autre).isNull()
                && _fsp.data(Fsp::Condition_Maladie_ETM_Autre).toBool())
            _condsUi->autre->setChecked(true);

        if (!_fsp.data(Fsp::Condition_Maladie_ETM_L115).isNull()
                && _fsp.data(Fsp::Condition_Maladie_ETM_L115).toBool())
            _condsUi->l115->setChecked(true);

        if (!_fsp.data(Fsp::Condition_Maladie_ETM_Prevention).isNull()
                && _fsp.data(Fsp::Condition_Maladie_ETM_Prevention).toBool())
            _condsUi->prevention->setChecked(true);

        if (_fsp.data(Fsp::Condition_Maladie_ETM_AccidentParTiers_Oui).isNull()) {
            _condsUi->accident_oui->setChecked(false);
            _condsUi->accident_non->setChecked(false);
        } else {
            if (_fsp.data(Fsp::Condition_Maladie_ETM_AccidentParTiers_Oui).toBool()) {
                _condsUi->accident_oui->setChecked(true);
                _condsUi->accident_non->setChecked(false);
            } else {
                _condsUi->accident_oui->setChecked(false);
                _condsUi->accident_non->setChecked(true);
            }
        }
        _condsUi->accidentDate->setDate(_fsp.data(Fsp::Condition_Maladie_ETM_AccidentParTiers_Date).toDate());

        if (!_fsp.data(Fsp::Condition_Maternite).isNull()
                && _fsp.data(Fsp::Condition_Maternite).toBool())
            _condsUi->mater->setChecked(true);
        _condsUi->materDate->setDate(_fsp.data(Fsp::Condition_Maternite).toDate());

        if (!_fsp.data(Fsp::Condition_ATMP).isNull()
                && _fsp.data(Fsp::Condition_ATMP).toBool())
            _condsUi->at->setChecked(true);
        _condsUi->atNb->setText(_fsp.data(Fsp::Condition_ATMP_Number).toString());
        _condsUi->atDate->setDate(_fsp.data(Fsp::Condition_ATMP_Date).toDate());

        if (!_fsp.data(Fsp::Condition_NouveauMedTraitant).isNull()
                && _fsp.data(Fsp::Condition_NouveauMedTraitant).toBool())
            _condsUi->nouveauMT->setChecked(true);
        _condsUi->envoyeur->setText(_fsp.data(Fsp::Condition_MedecinEnvoyeur).toString());

        _condsUi->accessspe->setChecked(false);
        _condsUi->urgence->setChecked(false);
        _condsUi->horsresidence->setChecked(false);
        _condsUi->medtrempla->setChecked(false);
        _condsUi->horscoord->setChecked(false);
        if (_fsp.data(Fsp::Condition_AccesSpecifique).toBool()) {
            _condsUi->accessspe->setChecked(true);
        } else if (_fsp.data(Fsp::Condition_Urgence).toBool()) {
            _condsUi->urgence->setChecked(true);
        } else if (_fsp.data(Fsp::Condition_HorsResidence).toBool()) {
            _condsUi->horsresidence->setChecked(true);
        } else if (_fsp.data(Fsp::Condition_Remplace).toBool()) {
            _condsUi->medtrempla->setChecked(true);
        } else if (_fsp.data(Fsp::Condition_HorsCoordination).toBool()) {
            _condsUi->horscoord->setChecked(true);
        }

        if (!_fsp.data(Fsp::Unpaid_PartObligatoire).isNull()
                && _fsp.data(Fsp::Unpaid_PartObligatoire).toBool())
            _feesUi->partOblig->setChecked(true);
        if (!_fsp.data(Fsp::Unpaid_PartComplementaire).isNull()
                && _fsp.data(Fsp::Unpaid_PartComplementaire).toBool())
            _feesUi->partOblig->setChecked(true);

        // Fees
        QStringList objNames;
        objNames << "date_" << "code_" << "activite_" << "cv_"
                 << "autres_1" << "autres_2"
                 << "amount_" << "depassement_"
                 << "idmd_" << "ikmd_" << "ikAmount_";
        for(int i=0; i < 4; ++i) {
            for(int j = Fsp::Amount_Date; j < Fsp::Amount_MaxData; ++j) {
                int objId = j - Fsp::Amount_Date;
                switch (j) {
                case Fsp::Amount_Date:
                {
                    Utils::ModernDateEditor *date = q->findChild<Utils::ModernDateEditor*>(objNames.at(objId) + QString::number(i + 1));
                    if (date)
                        date->setDate(_fsp.amountLineData(i, j).toDate());
                    break;
                }
                case Fsp::Amount_Amount:
                {
                    QDoubleSpinBox *date = q->findChild<QDoubleSpinBox*>(objNames.at(objId) + QString::number(i + 1));
                    if (date)
                        date->setValue(_fsp.amountLineData(i, j).toDouble());
                    break;
                }
                default:
                {
                    QLineEdit *line = q->findChild<QLineEdit*>(objNames.at(objId) + QString::number(i + 1));
                    if (line)
                        line->setText(_fsp.amountLineData(i, j).toString());
                }
                }
            }
        }

        // Total
        _feesUi->total->setValue(_fsp.data(Fsp::TotalAmount).toDouble());
    }

    void uiToFsp()
    {
        _fsp.clear();
        _fsp.setData(Fsp::Bill_Number, _patientUi->billId->text());
        _fsp.setData(Fsp::Bill_Date, _patientUi->billDate->text());
        _fsp.setData(Fsp::Patient_FullName, _patientUi->name->text());
        _fsp.setData(Fsp::Patient_DateOfBirth, _patientUi->dob->date());
        _fsp.setData(Fsp::Patient_Personal_NSS, _patientUi->nns->text().remove(" ").left(13).leftJustified(13, '_'));
        _fsp.setData(Fsp::Patient_Personal_NSSKey, _patientUi->nns->text().remove(" ").mid(13).leftJustified(2, '_'));
        _fsp.setData(Fsp::Patient_Assurance_Number, _patientUi->orga->text());
        _fsp.setData(Fsp::Patient_Assure_FullName, _patientUi->assureName->text());
        _fsp.setData(Fsp::Patient_Assure_NSS, _patientUi->assureNss->text().remove(" ").left(13).leftJustified(13, '_'));
        _fsp.setData(Fsp::Patient_Assure_NSSKey, _patientUi->assureNss->text().remove(" ").mid(13).leftJustified(2, '_'));
        _fsp.setData(Fsp::Patient_FullAddress, _patientUi->address->text());

        if (_condsUi->maladie->isChecked())
                _fsp.setData(Fsp::Condition_Maladie, true);
        if (_condsUi->exoTM_Oui->isChecked())
                _fsp.setData(Fsp::Condition_Maladie_ETM, true);
        else if (_condsUi->exoTM_non->isChecked())
            _fsp.setData(Fsp::Condition_Maladie_ETM, false);
        if (_condsUi->ald->isChecked())
            _fsp.setData(Fsp::Condition_Maladie_ETM_Ald, true);
        if (_condsUi->autre->isChecked())
            _fsp.setData(Fsp::Condition_Maladie_ETM_Autre, true);
        if (_condsUi->l115->isChecked())
            _fsp.setData(Fsp::Condition_Maladie_ETM_L115, true);
        if (_condsUi->prevention->isChecked())
            _fsp.setData(Fsp::Condition_Maladie_ETM_Prevention, true);

        if (_condsUi->accident_oui->isChecked())
            _fsp.setData(Fsp::Condition_Maladie_ETM_AccidentParTiers_Oui, true);
        _fsp.setData(Fsp::Condition_Maladie_ETM_AccidentParTiers_Date, _condsUi->accidentDate->date());

        if (_condsUi->mater->isChecked())
            _fsp.setData(Fsp::Condition_Maternite, true);
        _fsp.setData(Fsp::Condition_Maternite, _condsUi->materDate->date());

        if (_condsUi->at->isChecked())
            _fsp.setData(Fsp::Condition_ATMP, true);
        _fsp.setData(Fsp::Condition_ATMP_Number, _condsUi->atNb->text());
        _fsp.setData(Fsp::Condition_ATMP_Date, _condsUi->atDate->date());

        if (_condsUi->nouveauMT->isChecked())
            _fsp.setData(Fsp::Condition_NouveauMedTraitant, true);
        _fsp.setData(Fsp::Condition_MedecinEnvoyeur, _condsUi->envoyeur->text());

        if (_condsUi->accessspe->isChecked()) {
            _fsp.setData(Fsp::Condition_AccesSpecifique, true);
        } else if (_condsUi->urgence->isChecked()) {
            _fsp.setData(Fsp::Condition_Urgence, true);
        } else if (_condsUi->horsresidence->isChecked()) {
            _fsp.setData(Fsp::Condition_HorsResidence, true);
        } else if (_condsUi->medtrempla->isChecked()) {
            _fsp.setData(Fsp::Condition_Remplace, true);
        } else if (_condsUi->horscoord->isChecked()) {
            _fsp.setData(Fsp::Condition_HorsCoordination, true);
        }

        if (_feesUi->partOblig->isChecked())
            _fsp.setData(Fsp::Unpaid_PartObligatoire, true);
        if (_feesUi->partOblig->isChecked())
            _fsp.setData(Fsp::Unpaid_PartComplementaire, true);

        // Fees
        QStringList objNames;
        objNames << "date_" << "code_" << "activite_" << "cv_"
                 << "autres_1" << "autres_2"
                 << "amount_" << "depassement_"
                 << "idmd_" << "ikmd_" << "ikAmount_";
        for(int i=0; i < 4; ++i) {
            for(int j = Fsp::Amount_Date; j < Fsp::Amount_MaxData; ++j) {
                int objId = j - Fsp::Amount_Date;
                switch (j) {
                case Fsp::Amount_Date:
                {
                    Utils::ModernDateEditor *date = q->findChild<Utils::ModernDateEditor*>(objNames.at(objId) + QString::number(i + 1));
                    if (date && date->date().isValid() && !date->date().isNull())
                        _fsp.addAmountData(i, j, date->date());
                    break;
                }
                case Fsp::Amount_Amount:
                {
                    QDoubleSpinBox *spin = q->findChild<QDoubleSpinBox*>(objNames.at(objId) + QString::number(i + 1));
                    if (spin && spin->value() > 0.0)
                        _fsp.addAmountData(i, j, spin->value());
                    break;
                }
                default:
                {
                    QLineEdit *line = q->findChild<QLineEdit*>(objNames.at(objId) + QString::number(i + 1));
                    if (line && !line->text().isEmpty())
                        _fsp.addAmountData(i, j, line->text().remove(" "));
                }
                }
            }
        }

        // Total
        _fsp.setData(Fsp::TotalAmount, _feesUi->total->value());
    }

    FspPrinter::Cerfa cerfa()
    {
        if (ui->cerfa->currentIndex() == 0)
            return FspPrinter::S12541_01;
        else if (ui->cerfa->currentIndex() == 1)
            return FspPrinter::S12541_02;
        else if (ui->cerfa->currentIndex() == 2)
            return FspPrinter::S12541_02_2;
        return FspPrinter::S12541_01;
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
    Ui::FspPrinterDialogFees * _feesUi;
    Ui::FspPrinterDialogPrecorded *_preRecUi;
    FspTemplateModel *_templateModel;

    Utils::DetailsWidget *_patientDetailsWidget, *_condDetailsWidget;
    Utils::DetailsWidget *_actDetailsWidget, *_previewDetailsWidget;
    Utils::DetailsWidget *_templateDetailsWidget;
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
    setWindowTitle(tr("French 'FSP' printing assistant"));

    // Templates
    d->_templateModel = new FspTemplateModel(this);
    d->_templateModel->initialize();
    d->_templateDetailsWidget = new Utils::DetailsWidget(this);
    d->_templateDetailsWidget->setSummaryText("Cotations pré-enregistrées");
    d->_templateDetailsWidget->setSummaryFontBold(true);
    d->_templateDetailsWidget->setState(Utils::DetailsWidget::Expanded);
    QWidget *preW = new QWidget(this);
    d->_preRecUi->setupUi(preW);
    d->_preRecUi->treeView->setModel(d->_templateModel);
    d->_preRecUi->treeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    connect(d->_preRecUi->treeView, SIGNAL(activated(QModelIndex)), this, SLOT(useTemplate(QModelIndex)));
    d->_templateDetailsWidget->setWidget(preW);

    // Patient
    d->_patientDetailsWidget = new Utils::DetailsWidget(this);
    d->_patientDetailsWidget->setSummaryText("Information patient");
    d->_patientDetailsWidget->setSummaryFontBold(true);
    d->_patientDetailsWidget->setState(Utils::DetailsWidget::Expanded);
    QWidget *pWCont = new QWidget(this);
    QVBoxLayout *pWlay = new QVBoxLayout(pWCont);
    pWCont->setLayout(pWlay);
    QWidget *pW = new QWidget(pWCont);
    d->_patientUi->setupUi(pW);
    d->_patientUi->backgroundLabel->setPixmap(QPixmap(settings()->path(Core::ISettings::ThemeRootPath) + "/pixmap/others/S3110_patient.png"));
    pWlay->addWidget(pW);
    d->_patientDetailsWidget->setWidget(pWCont);

    // Conditions
    d->_condDetailsWidget = new Utils::DetailsWidget(this);
    d->_condDetailsWidget->setSummaryText("Conditions");
    d->_condDetailsWidget->setSummaryFontBold(true);
    d->_condDetailsWidget->setState(Utils::DetailsWidget::Expanded);
    QWidget *cWCont = new QWidget(this);
    QGridLayout *cWlay = new QGridLayout(cWCont);
    cWCont->setLayout(cWlay);
    QWidget *cW = new QWidget(cWCont);
    d->_condsUi->setupUi(cW);
    cWlay->addWidget(cW);
    d->_condsUi->conditionsLabel->setPixmap(QPixmap(settings()->path(Core::ISettings::ThemeRootPath) + "/pixmap/others/S3110_conditions.png"));
    d->_condDetailsWidget->setWidget(cWCont);

    d->_actDetailsWidget = new Utils::DetailsWidget(this);
    d->_actDetailsWidget->setSummaryText("Actes");
    d->_actDetailsWidget->setSummaryFontBold(true);
    d->_actDetailsWidget->setState(Utils::DetailsWidget::Expanded);
    QWidget *aWCont = new QWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(aWCont);
    aWCont->setLayout(lay);
    QWidget *aW = new QWidget(this);
    d->_feesUi->setupUi(aW);
    d->_feesUi->conditionsLabel->setPixmap(QPixmap(settings()->path(Core::ISettings::ThemeRootPath) + "/pixmap/others/S3110_honoraires.png"));

    QFont font;
    font.setBold(true);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 4);
    d->_feesUi->code_1->setFont(font);
    d->_feesUi->code_2->setFont(font);
    d->_feesUi->code_3->setFont(font);
    d->_feesUi->code_4->setFont(font);

    lay->addWidget(aW);
    d->_actDetailsWidget->setWidget(aWCont);

    d->_previewDetailsWidget = new Utils::DetailsWidget(this);
    d->_previewDetailsWidget->setSummaryText("Prévisualisation");
    d->_previewDetailsWidget->setSummaryFontBold(true);
    d->_previewDetailsWidget->setState(Utils::DetailsWidget::Expanded);
    d->_preview = new QLabel(this);
    d->_preview->setAlignment(Qt::AlignCenter);
    d->_previewDetailsWidget->setWidget(d->_preview);

    // Change the button box
    QPushButton *button = d->ui->buttonBox->addButton("Imprimer la FSP", QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()), this, SLOT(printFsp()));
    button = d->ui->buttonBox->addButton("Prévisualisation de la FSP", QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()), this, SLOT(previewFsp()));
    button = d->ui->buttonBox->addButton("Imprimer un chèque", QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()), this, SLOT(printCheque()));

    d->ui->contentLayout->addWidget(d->_templateDetailsWidget);
    d->ui->contentLayout->addWidget(d->_patientDetailsWidget);
    d->ui->contentLayout->addWidget(d->_condDetailsWidget);
    d->ui->contentLayout->addWidget(d->_actDetailsWidget);
    d->ui->contentLayout->addWidget(d->_previewDetailsWidget);
    // d->ui->contentLayout->addStretch(100);

    // Read settings
    if (settings()->value(Constants::S_DEFAULTCERFA) == Constants::S_CERFA_01)
        d->ui->cerfa->setCurrentIndex(0);
    else if (settings()->value(Constants::S_DEFAULTCERFA) == Constants::S_CERFA_02)
        d->ui->cerfa->setCurrentIndex(1);
    else if (settings()->value(Constants::S_DEFAULTCERFA) == Constants::S_CERFA_02_V2)
        d->ui->cerfa->setCurrentIndex(2);

    // Connections
    connect(d->ui->vueComplexe, SIGNAL(clicked(bool)), this, SLOT(toggleView(bool)));
    connect(d->ui->cerfa, SIGNAL(activated(int)), this, SLOT(updatePreview()));
    connect(d->_preRecUi->treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(expandChildren(QModelIndex)));

    toggleView(d->ui->vueComplexe->isChecked());
    d->updatePreview();
    resize(850, 400);
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
    d->_fsp = fsp;
    d->fspToUi();
    d->updatePreview();
    return true;
}

/** Returns true if the FSP print dialog is available for this configuration */
bool FspPrinterDialog::isAvailable()  // static
{
    // Datapack is not installed -> return false
    const QString &path = datapackPath();
    if (path.isEmpty())
        return false;

    // Check the content of the datapack path
    QFileInfoList files = Utils::getFiles(QDir(path), "*.xml", Utils::Recursively);
    return !files.isEmpty();
}

QString FspPrinterDialog::datapackPath() // static
{
    QString path;
    // Check the DataPackInstallPath path first
    path = settings()->path(Core::ISettings::DataPackInstallPath) + Constants::DATAPACK_PATH;
    if (QDir(path).exists())
        return path;

    // Else check the DataPackApplicationInstalled path
    path = settings()->path(Core::ISettings::DataPackApplicationPath) + Constants::DATAPACK_PATH;
    if (QDir(path).exists())
        return path;

    // Not installed
    return QString::null;
}

void FspPrinterDialog::toggleView(bool complex)
{
    d->_condDetailsWidget->setVisible(complex);
    d->_actDetailsWidget->setVisible(complex);
//    d->_prerecordedFspDetailsWidget->setVisible(!complex);
}

void FspPrinterDialog::expandChildren(const QModelIndex &index)
{
    if (index.parent() == QModelIndex()) {
        for(int i = 0; i < d->_templateModel->rowCount(index); ++i) {
            QModelIndex idx = d->_templateModel->index(i, 0, index);
            d->_preRecUi->treeView->expand(idx);
            expandChildren(idx);
        }
    }
}

void FspPrinterDialog::useTemplate(const QModelIndex &index)
{
    d->_fsp = d->_templateModel->fsp(index);
    d->_fsp.populateWithCurrentPatientData();
    d->_fsp.populateAmountsWithCurrentDate();
    d->fspToUi();
    d->updatePreview();
    printFsp();
}

void FspPrinterDialog::printFsp()
{
    d->uiToFsp();
    FspPrinter printer;
    printer.setDrawRects(false);
    printer.print(d->_fsp, d->cerfa(), false);
}

void FspPrinterDialog::previewFsp()
{
    d->uiToFsp();
    Utils::ImageViewer viewer(this);
    viewer.setPixmap(QPixmap(*d->_preview->pixmap()));
    viewer.exec();
}

void FspPrinterDialog::updatePreview()
{
    d->updatePreview();
}

