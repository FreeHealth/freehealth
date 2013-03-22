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
 * \class Tools::Internal::FspPrinter
 * \brief Prints a french FSP.
 */

#include "fspprinter.h"

#include <utils/printaxishelper.h>
#include <translationutils/constants.h>

#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QSystemLocale>
#include <QPixmap>

#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Tools {
namespace Internal {
class FspPrinterPrivate
{
public:
    FspPrinterPrivate(FspPrinter *parent) :
        _nAPasPayePartObligatoire(false),
        _nAPasPayePartComplementaire(false),
        q(parent)
    {
    }
    
    ~FspPrinterPrivate()
    {
    }

    void printPatientInfo(QPainter &painter, FspPrinter::Cerfa cerfa)
    {
        QRect patientName   (QPoint(55,  29), QSize(140, 4));
        QRect nss           (QPoint(55,  36), QSize(66,  5));
        QRect nssKey        (QPoint(124, 36), QSize(10,  5));
        QRect dob           (QPoint(55,  45), QSize(40,  4));

        QRect assureName    (QPoint(55,  54), QSize(140, 4));
        QRect assureNss     (QPoint(55,  64), QSize(66,  5));
        QRect assureNssKey  (QPoint(124, 64), QSize(10,  5));
        QRect address       (QPoint(55,  73), QSize(110, 4));

        Utils::PrintString s;
        s.autoFontResize = true;
        s.minFontPixelSize = 10;
        s.drawBoundingRect = true; // DEBUG RECTS

        // Print cerfa
        if (cerfa == FspPrinter::S12541_01) {
            s.topMillimeters = QPoint(18, 7);
            s.contentSizeMillimeters = QSize(45, 4);
            s.content = "S12541_01";
            _axisHelper.printString(&painter, s);
        } else {
            s.topMillimeters = QPoint(18, 7);
            s.contentSizeMillimeters = QSize(45, 4);
            s.content = "S12541_02";
            _axisHelper.printString(&painter, s);
        }

        // Facture
        s.splitChars = true;
        if (cerfa == FspPrinter::S12541_02) {
            s.topMillimeters = QPoint(155, 10);
            s.contentSizeMillimeters = QSize(45, 4);
            s.content = _billNumber.left(9);
            _axisHelper.printString(&painter, s);
        }

        if (cerfa == FspPrinter::S12541_02) {
            s.topMillimeters = QPoint(160, 15);
            s.contentSizeMillimeters = QSize(40, 4);
            s.content = _billDate.toString("ddMMyyyy");
        } else {
            s.topMillimeters = QPoint(172.5, 14);
            s.contentSizeMillimeters = QSize(30, 4);
            s.content = _billDate.toString("ddMMyy");
        }
        _axisHelper.printString(&painter, s);

        // Patient data
        s.splitChars = false;
        s.topMillimeters = patientName.topLeft();
        if (cerfa == FspPrinter::S12541_02) {
            s.topMillimeters += _axisHelper.pointToPixels(QPoint(-1, 0));
        }
        s.contentSizeMillimeters = patientName.size();
        s.content = _patient.fullName;
        _axisHelper.printString(&painter, s);

        s.splitChars = true;
        s.topMillimeters = dob.topLeft();
        if (cerfa == FspPrinter::S12541_02) {
            s.topMillimeters += _axisHelper.pointToPixels(QPoint(-1, 0));
        }
        s.contentSizeMillimeters = dob.size();
        s.content = _patient.dob.toString("ddMMyyyy");
        _axisHelper.printString(&painter, s);
        s.splitChars = false;

        s.topMillimeters = assureName.topLeft();
        if (cerfa == FspPrinter::S12541_02) {
            s.topMillimeters += _axisHelper.pointToPixels(QPoint(-1, 0));
        }
        s.contentSizeMillimeters = assureName.size();
        s.content = _patient.assuredName;
        _axisHelper.printString(&painter, s);

        s.splitChars = true;
        s.topMillimeters = assureNss.topLeft();
        if (cerfa == FspPrinter::S12541_02) {
            s.topMillimeters += _axisHelper.pointToPixels(QPoint(-1, -1));
        }
        s.contentSizeMillimeters = assureNss.size();
        s.content = _patient.assuredNss;
        _axisHelper.printString(&painter, s);

        s.topMillimeters = assureNssKey.topLeft();
        if (cerfa == FspPrinter::S12541_02) {
            s.topMillimeters += _axisHelper.pointToPixels(QPoint(-1, -1));
        }
        s.contentSizeMillimeters = assureNssKey.size();
        s.content = _patient.assuredNssKey;
        _axisHelper.printString(&painter, s);
        s.splitChars = false;

        s.topMillimeters = address.topLeft();
        s.contentSizeMillimeters = address.size();
        s.content = _patient.address;
        _axisHelper.printString(&painter, s);

        s.splitChars = true;
        s.topMillimeters = nss.topLeft();
        if (cerfa == FspPrinter::S12541_02) {
            s.topMillimeters += _axisHelper.pointToPixels(QPoint(-1, .5));
        }
        s.contentSizeMillimeters = nss.size();
        s.content = _patient.nss;
        _axisHelper.printString(&painter, s);
        s.topMillimeters = nssKey.topLeft();
        if (cerfa == FspPrinter::S12541_02) {
            s.topMillimeters += _axisHelper.pointToPixels(QPoint(-1, .5));
        }
        s.contentSizeMillimeters = nssKey.size();
        s.content = _patient.nssKey;
        _axisHelper.printString(&painter, s);
        s.splitChars = false;
    }

    void printConditionCerfa01(QPainter &painter)
    {
        Utils::PrintString s;
        s.autoFontResize = true;
        s.minFontPixelSize = 10;
        s.drawBoundingRect = true; // DEBUG RECTS

        painter.save();
        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(10);
        painter.setFont(font);
        s.splitChars = true;
        painter.translate(_axisHelper.pointToPixels(-6, 0.7));

        if (_conds.maladie) {
            s.topMillimeters =          QPoint(12.4, 130.6);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.exonerationTM) {
            s.topMillimeters =          QPoint(100.1, 130.6);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        } else {
            s.topMillimeters =          QPoint(121.7, 130.6);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }

        if (_conds.ald) {
            s.topMillimeters =          QPoint(59.9, 137.4);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.prevention) {
            s.topMillimeters =          QPoint(110.5, 137.4);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.soinL115) {
            s.topMillimeters =          QPoint(171.7, 137.4);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.autre) {
            s.topMillimeters =          QPoint(202.2, 137.4);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }

        if (!_conds.accidentParTiers) {
            s.topMillimeters =          QPoint(69.9, 144.3);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        } else  {
            s.topMillimeters =          QPoint(92.2, 144.3);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (!_conds.accidentParTiersDate.isNull()) {
            s.topMillimeters =          QPoint(119.4, 144.3);
            s.contentSizeMillimeters =  QSize(40.6, 4.1);
            s.content = _conds.accidentParTiersDate.toString("ddMMyyyy");
            _axisHelper.printString(&painter, s);
        }

        if (_conds.maternite) {
            s.topMillimeters =          QPoint(12.4, 151.4);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (!_conds.dateGrossesse.isNull()) {
            s.topMillimeters =          QPoint(167.4, 151.4);
            s.contentSizeMillimeters =  QSize(40.6, 4.1);
            s.content = _conds.dateGrossesse.toString("ddMMyyyy");
            _axisHelper.printString(&painter, s);
        }

        if (_conds.atMP) {
            s.topMillimeters =          QPoint(12.4, 158.4);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (!_conds.atMPNumber.isEmpty()) {
            s.topMillimeters =          QPoint(73.2, 158.4);
            s.contentSizeMillimeters =  QSize(45.7, 4.3);
            s.content = _conds.atMPNumber.left(9);
            _axisHelper.printString(&painter, s);
        }
        if (!_conds.atMPDate.isNull()) {
            s.topMillimeters =          QPoint(167.1, 158.4);
            s.contentSizeMillimeters =  QSize(40.6, 4.1);
            s.content = _conds.atMPDate.toString("ddMMyyyy");
            _axisHelper.printString(&painter, s);
        }

        if (_conds.nouveauMT) {
            s.topMillimeters =          QPoint(96., 166.);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }

        if (!_conds.envoyeParMedecin.isEmpty()) {
            s.splitChars = false;
            s.topMillimeters =          QPoint(66., 177.6);
            s.contentSizeMillimeters =  QSize(140., 5.3);
            s.content = _conds.envoyeParMedecin;
            _axisHelper.printString(&painter, s);
            s.splitChars = true;
        }

        if (_conds.accessSpecific) {
            s.topMillimeters =          QPoint(42.2, 185.9);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.urgence) {
            s.topMillimeters =          QPoint(65.8, 185.9);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.horsResidence) {
            s.topMillimeters =          QPoint(111.3, 185.9);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.medTraitantRemplace) {
            s.topMillimeters =          QPoint(158.5, 185.9);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.accessHorsCoordination) {
            s.topMillimeters =          QPoint(202.2, 185.9);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        painter.restore();
    }

    void printConditionCerfa02(QPainter &painter)
    {
        Utils::PrintString s;
        s.autoFontResize = true;
        s.minFontPixelSize = 10;
        s.drawBoundingRect = true; // DEBUG RECTS

        painter.save();
        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(10);
        painter.setFont(font);
        s.splitChars = true;
        painter.translate(_axisHelper.pointToPixels(-6.8, -1.5));

        if (_conds.maladie) {
            s.topMillimeters =          QPoint(12.4, 130.8);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.ald) {
            s.topMillimeters =          QPoint(59.9, 137.2);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.prevention) {
            s.topMillimeters =          QPoint(104.9, 137.4);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.soinL115) {
            s.topMillimeters =          QPoint(140.2, 131.1);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.autre) {
            s.topMillimeters =          QPoint(129.5, 137.4);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }

        if (!_conds.accidentParTiers) {
            s.topMillimeters =          QPoint(69.6, 144.3);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        } else  {
            s.topMillimeters =          QPoint(92.2, 144.3);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (!_conds.accidentParTiersDate.isNull()) {
            s.topMillimeters =          QPoint(119.4, 144.3);
            s.contentSizeMillimeters =  QSize(40.6, 4.1);
            s.content = _conds.accidentParTiersDate.toString("ddMMyyyy");
            _axisHelper.printString(&painter, s);
        }

        if (_conds.maternite) {
            s.topMillimeters =          QPoint(12.4, 151.4);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (!_conds.dateGrossesse.isNull()) {
            s.topMillimeters =          QPoint(167.1, 151.4);
            s.contentSizeMillimeters =  QSize(40.6, 4.1);
            s.content = _conds.dateGrossesse.toString("ddMMyyyy");
            _axisHelper.printString(&painter, s);
        }

        if (_conds.atMP) {
            s.topMillimeters =          QPoint(12.4, 158.5);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (!_conds.atMPNumber.isEmpty()) {
            s.topMillimeters =          QPoint(73.2, 158.5);
            s.contentSizeMillimeters =  QSize(45.7, 4.3);
            s.content = _conds.atMPNumber.left(9);
            _axisHelper.printString(&painter, s);
        }
        if (!_conds.atMPDate.isNull()) {
            s.topMillimeters =          QPoint(167.1, 158.5);
            s.contentSizeMillimeters =  QSize(40.6, 4.1);
            s.content = _conds.atMPDate.toString("ddMMyyyy");
            _axisHelper.printString(&painter, s);
        }

        if (_conds.nouveauMT) {
            s.topMillimeters =          QPoint(96., 166.);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }

        if (!_conds.envoyeParMedecin.isEmpty()) {
            s.splitChars = false;
            s.topMillimeters =          QPoint(66., 177.6);
            s.contentSizeMillimeters =  QSize(140., 5.3);
            s.content = _conds.envoyeParMedecin;
            _axisHelper.printString(&painter, s);
            s.splitChars = true;
        }

        if (_conds.accessSpecific) {
            s.topMillimeters =          QPoint(42.2, 185.9);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.urgence) {
            s.topMillimeters =          QPoint(65.8, 185.9);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.horsResidence) {
            s.topMillimeters =          QPoint(111.3, 185.9);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.medTraitantRemplace) {
            s.topMillimeters =          QPoint(159, 185.9);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }
        if (_conds.accessHorsCoordination) {
            s.topMillimeters =          QPoint(202.2, 185.9);
            s.contentSizeMillimeters =  QSize(5.3, 3.8);
            s.content = "X";
            _axisHelper.printString(&painter, s);
        }

        if (!_conds.dateAccordPrealable.isNull()) {
            s.topMillimeters =          QPoint(147.4, 196.6);
            s.contentSizeMillimeters =  QSize(40.6, 4.1);
            s.content = _conds.dateAccordPrealable.toString("ddMMyyyy");
            _axisHelper.printString(&painter, s);
        }
        painter.restore();
    }

public:
    QString _billNumber;
    QDate _billDate;
    Fsp_Patient _patient;
    Fsp_Conditions _conds;
    QList<Fsp_AmountLine> _amountLines;
    Utils::PrintAxisHelper _axisHelper;
    bool _nAPasPayePartObligatoire;
    bool _nAPasPayePartComplementaire;

private:
    FspPrinter *q;
};
} // namespace Internal
} // namespace Tools


/*! Constructor of the Tools::Internal::FspPrinter class */
FspPrinter::FspPrinter() :
    d(new FspPrinterPrivate(this))
{
}

/*! Destructor of the Tools::Internal::FspPrinter class */
FspPrinter::~FspPrinter()
{
    if (d)
        delete d;
    d = 0;
}

void FspPrinter::setBillNumber(const QString &uid)
{
    d->_billNumber = uid;
}

void FspPrinter::setBillDate(const QDate &date)
{
    d->_billDate = date;
}

void FspPrinter::setPatient(const Fsp_Patient &patient)
{
    d->_patient = patient;
}

void FspPrinter::setConditions(const Fsp_Conditions &conds)
{
    d->_conds = conds;
}

void FspPrinter::addAmountLine(const Fsp_AmountLine &line)
{
    d->_amountLines.append(line);
}

void FspPrinter::setPayment(bool nAPasPayePartObligatoire, bool nAPasPayePartComplementaire)
{
    d->_nAPasPayePartObligatoire = nAPasPayePartObligatoire;
    d->_nAPasPayePartComplementaire = nAPasPayePartComplementaire;
}

void FspPrinter::clear()
{
    d->_patient = Fsp_Patient();
    d->_conds = Fsp_Conditions();
    d->_amountLines.clear();
}

bool FspPrinter::print(Cerfa cerfa)
{
    QPrintDialog dlg;
    if (dlg.exec()==QDialog::Rejected)
        return false;
    QPrinter *printer = dlg.printer();
    printer->setFullPage(true);
//    printer->setPageMargins(0, 0, 0, 0, QPrinter::Millimeter);
    printer->setPaperSize(QPrinter::A4);
    printer->setResolution(150);
    d->_axisHelper.setPageSize(printer->paperRect(), printer->paperSize(QPrinter::Millimeter));

    QPainter painter;
    if (!painter.begin(printer)) { // failed to open file
        qWarning("failed to open file, is it writable?");
        return false;
    }

    QFont font;
    font.setFamily("Arial Black");
    font.setPointSize(9);
    painter.setFont(font);

    painter.save();

    // TODO: this should be a param of the member; Printer margin correction?
    painter.translate(d->_axisHelper.pointToPixels(-2, 0));

    d->printPatientInfo(painter, cerfa);

    // Conditions
    if (cerfa == S12541_01)
        d->printConditionCerfa01(painter);
    else
        d->printConditionCerfa02(painter);

    painter.restore();
    Utils::PrintString s;
    s.autoFontResize = true;
    s.minFontPixelSize = 10;
    s.splitChars = true;
    s.drawBoundingRect = true; // DEBUG RECTS

    painter.save();
    if (cerfa == S12541_01)
        painter.translate(d->_axisHelper.pointToPixels(-7, 0.1));
    else
        painter.translate(d->_axisHelper.pointToPixels(-8, 2.5));

    // Acts and amount
    double totalAmount = 0.;
    for(int i=0; i < d->_amountLines.count(); ++i) {
        int y = 211.8 + 7.6*i;
        const Fsp_AmountLine &line = d->_amountLines.at(i);
        if (!line.date.isNull()) {
            s.topMillimeters =          QPoint(11.7, y);
            s.contentSizeMillimeters =  QSize(30.7, 5.1);
            s.content = line.date.toString("ddMMyyyy");
            d->_axisHelper.printString(&painter, s);
        }

        if (!line.act.isEmpty()) {
            s.topMillimeters =          QPoint(43.2, y);
            s.contentSizeMillimeters =  QSize(36.6, 5.1);
            s.content = line.act.left(7);
            d->_axisHelper.printString(&painter, s);
        }

        if (line.activity > 0) {
            s.topMillimeters =          QPoint(79.2, y);
            s.contentSizeMillimeters =  QSize(5.8, 5.1);
            s.content = QString::number(line.activity);
            d->_axisHelper.printString(&painter, s);
        }
        s.splitChars = false;
        if (!line.C.isEmpty()) {
            s.topMillimeters =          QPoint(86.4, y);
            s.contentSizeMillimeters =  QSize(10.7, 5.1);
            s.content = line.C;
            d->_axisHelper.printString(&painter, s);
        }
        if (!line.otherAct1.isEmpty()) {
            s.topMillimeters =          QPoint(96., y);
            s.contentSizeMillimeters =  QSize(16.5, 5.1);
            s.content = line.otherAct1;
            d->_axisHelper.printString(&painter, s);
        }
        if (!line.otherAct2.isEmpty()) {
            s.topMillimeters =          QPoint(112.8, y);
            s.contentSizeMillimeters =  QSize(16.5, 5.1);
            s.content = line.otherAct2;
            d->_axisHelper.printString(&painter, s);
        }

        s.splitChars = true;
        if (line.amount > 0.) {
            s.topMillimeters =          QPoint(129.5, y);
            s.contentSizeMillimeters =  QSize(30.2, 5.1);
            s.content = QString::number(line.amount, 'f', 2).remove(".").rightJustified(6, '*');
            d->_axisHelper.printString(&painter, s);

            s.topMillimeters =          QPoint(129.5 + 19, y);
            s.contentSizeMillimeters =  QSize(2.3, 5.1);
            s.content = ",";
            d->_axisHelper.printString(&painter, s);
        }
        if (line.depassement) {
            s.topMillimeters =          QPoint(160., y);
            s.contentSizeMillimeters =  QSize(7.1, 5.1);
            s.content = "X";
            d->_axisHelper.printString(&painter, s);
        }
        if (!line.idMd.isEmpty()) {
            s.topMillimeters =          QPoint(167.4, y);
            s.contentSizeMillimeters =  QSize(9.7, 5.1);
            s.content = line.idMd;
            d->_axisHelper.printString(&painter, s);
        }
        if (line.deplacement_nb) {
            s.topMillimeters =          QPoint(177.5, y);
            s.contentSizeMillimeters =  QSize(9.7, 5.1);
            s.content = QString::number(line.deplacement_nb).rightJustified(2, '*');
            d->_axisHelper.printString(&painter, s);
        }
        if (line.deplacement_amount > 0.) {
            s.topMillimeters =          QPoint(188., y);
            s.contentSizeMillimeters =  QSize(20.1, 5.1);
            s.content = QString::number(line.deplacement_amount, 'f', 2).remove(".").rightJustified(4, '*');
            d->_axisHelper.printString(&painter, s);

            s.topMillimeters =          QPoint(188. + 9, y);
            s.contentSizeMillimeters =  QSize(1.8, 5.1);
            s.content = ",";
            d->_axisHelper.printString(&painter, s);
        }

        totalAmount += line.amount; // add IK
    }

    if (totalAmount > 0.) {
        s.topMillimeters =          QPoint(114.3, 249.5);
        s.contentSizeMillimeters =  QSize(30.7, 5.3);
        s.content = QString::number(totalAmount, 'f', 2).remove(".").rightJustified(6, '*');
        d->_axisHelper.printString(&painter, s);

        s.topMillimeters =          QPoint(114.3 + 19.6, 249.5);
        s.contentSizeMillimeters =  QSize(1.8, 5.1);
        s.content = ",";
        d->_axisHelper.printString(&painter, s);
    }

    if (d->_nAPasPayePartObligatoire) {
        s.topMillimeters =          QPoint(90.9, 259.1);
        s.contentSizeMillimeters =  QSize(5.3, 3.8);
        s.content = "X";
        d->_axisHelper.printString(&painter, s);
    }

    if (d->_nAPasPayePartComplementaire) {
        s.topMillimeters =          QPoint(195.1, 259.1);
        s.contentSizeMillimeters =  QSize(5.3, 3.8);
        s.content = "X";
        d->_axisHelper.printString(&painter, s);
    }

    painter.restore();
    painter.end();
    return true;

}
