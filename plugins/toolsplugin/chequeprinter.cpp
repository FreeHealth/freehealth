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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Tools::ChequePrinter
 * \brief Cheque printer helper.
 *
 * Prepare a cheque printing and start the print process.
 */

#include "chequeprinter.h"
#include "pdftkwrapper.h"

#include <utils/log.h>
#include <utils/global.h>
#include <utils/numbertostring.h>
#include <utils/printaxishelper.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#if QT_VERSION < 0x050000
#include <QPrinter>
#include <QPrintDialog>
#else
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#endif

#include <QPainter>
#include <QSystemLocale>
#include <QPixmap>

#include <QDebug>

enum { DrawChequeRects = false };

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace Tools {
namespace Internal {
class ChequePrinterPrivate
{
public:
    ChequePrinterPrivate(ChequePrinter *parent) :
        _amount(0.),
        q(parent)
    {
    }

    ~ChequePrinterPrivate()
    {
    }

public:
    QString _order, _place;
    QDate _date;
    double _amount;
    Utils::PrintAxisHelper _axisHelper;

private:
    ChequePrinter *q;
};
} // namespace Internal
} // end namespace Tools


/*! Constructor of the Tools::ChequePrinter class */
ChequePrinter::ChequePrinter() :
    d(new ChequePrinterPrivate(this))
{
}

/*! Destructor of the Tools::ChequePrinter class */
ChequePrinter::~ChequePrinter()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool ChequePrinter::initialize()
{
    return true;
}

void ChequePrinter::setOrder(const QString &order)
{
    d->_order = order;
}

void ChequePrinter::setPlace(const QString &place)
{
    d->_place = place;
}

void ChequePrinter::setDate(const QDate &date)
{
    d->_date = date;
}

void ChequePrinter::setAmount(double amount)
{
    d->_amount = amount;
}

bool ChequePrinter::print()
{
    // 0. Amount to readable string
    QString amount;
    QStringList vals = Utils::doubleToHumanReadableString(d->_amount);
    if (vals.count() == 0) {
        LOG_ERROR_FOR("ChecquePrinter", "Wrong amount to string values");
    } else {
        amount = vals.at(0) + " " + QLocale().currencySymbol(QLocale::CurrencyDisplayName) + "s";
        if (vals.count() == 2)
            amount += QString(" %1 %2").arg(tkTr(Trans::Constants::AND)).arg(vals.at(1));
    }

    // Method 1. use pdkcompleter
//    Internal::PdfTkWrapper *pdftk = Internal::PdfTkWrapper::instance();
//    if (!pdftk->initialize())
//        return false;
//    pdftk->beginFdfEncoding();
//    pdftk->addFdfValue("date", QLocale().toString(d->_date, QLocale::ShortFormat));
//    pdftk->addFdfValue("lieu", d->_place);
//    pdftk->addFdfValue("ordre", d->_order);

//    pdftk->addFdfValue("montant1", lines.at(0));
//    if (lines.count() >= 2)
//        pdftk->addFdfValue("montant2", lines.at(1));

//    // Amount to translated number
//    QSystemLocale::CurrencyToStringArgument arg;
//    arg.value = d->_amount;
//    QVariant var;
//    var.setValue<QSystemLocale::CurrencyToStringArgument>(arg);
//    pdftk->addFdfValue("num", QString::number(d->_amount, 'f', 2)); //QSystemLocale().query(QSystemLocale::CurrencyToString, var).toString());

//    QString filename = "/Users/eric/Desktop/cheque.pdf";
//    pdftk->endFdfEncoding(filename);
//    pdftk->fillPdfWithFdf(filename, pdftk->getFdfContent(), "/Users/eric/Desktop/test_cheque.pdf", "ISO-8859-1");

//    return true;

    // Method 2. use QPainter/QPrinter
    // try to use the default printer
    QPrintDialog dlg;
    if (dlg.exec()==QDialog::Rejected)
        return false;
    QPrinter *printer = dlg.printer();
//    printer->setPaperSource(QPrinter::PaperSource);
    printer->setFullPage(true);
    printer->setPaperSize(QPrinter::A4);
    printer->setResolution(150);
    printer->setOrientation(QPrinter::Landscape);
    d->_axisHelper.setPaperSize(printer->paperRect(), printer->paperSize(QPrinter::Millimeter), QPrinter::Millimeter);

    QPainter painter;
    if (!painter.begin(printer)) { // failed to open file
        qWarning("failed to open file, is it writable?");
        return false;
    }

    painter.save();
    QFont font;
    font.setPointSize(10);
    painter.setFont(font);

    painter.translate(d->_axisHelper.pointToPixels(120, 61));
    // 25,17 pour certains
    // 45,13 pour d'autres
    QRectF amountLines(d->_axisHelper.pointToPixels(45,13), d->_axisHelper.sizeToPixels(90, 10));
    // QRect amountLine2(d->_axisHelper.pointToPixels(10,23), d->size(110, 5));
    QRectF orderLine(d->_axisHelper.pointToPixels(10,28), d->_axisHelper.sizeToPixels(110, 5));

    QRectF numberLine(d->_axisHelper.pointToPixels(133,28), d->_axisHelper.sizeToPixels(41, 10));
    QRectF placeLine(d->_axisHelper.pointToPixels(133,38), d->_axisHelper.sizeToPixels(40, 4));
    QRectF dateLine(d->_axisHelper.pointToPixels(133,42), d->_axisHelper.sizeToPixels(40, 4));

    if (DrawChequeRects) {
        painter.drawRect(amountLines);
        painter.drawRect(orderLine);
        painter.drawRect(numberLine);
        painter.drawRect(placeLine);
        painter.drawRect(dateLine);
    }

    if (amount.count() > 50)
        amount = Utils::lineWrapString(amount, 50);
    amount = QString("****%1****").arg(amount.toUpper());

    QFontMetrics metrics(font);
    while (metrics.width(amount) > amountLines.width() || font.pointSize() == 6) {
        font.setPointSizeF(font.pointSizeF() - .1);
        metrics = QFontMetrics(font);
    }
    painter.setFont(font);
    painter.drawText(amountLines, Qt::AlignLeft, amount);
    font.setPointSize(10);
    painter.setFont(font);

//    QStringList lines = amount.split("\n", QString::SkipEmptyParts);
//    painter.drawText(amountLine1, Qt::AlignLeft | Qt::AlignVCenter, lines.at(0));
//    if (lines.count() >= 2)
//        painter.drawText(amountLine2, Qt::AlignLeft | Qt::AlignVCenter, lines.at(1));
    painter.drawText(orderLine, Qt::AlignLeft | Qt::AlignVCenter, d->_order.toUpper());

    font.setBold(true);
    painter.setFont(font);
    painter.drawText(numberLine, Qt::AlignCenter | Qt::AlignVCenter,
                     QString("****%1****").arg(QLocale().toString(d->_amount, 'f', 2))
                     //.arg(QLocale().toCurrencyString(d->_amount, "*"))
                     );//QString::number(d->_amount, 'f', 2)); //QSystemLocale().query(QSystemLocale::CurrencyToString, var).toString());

    font.setPointSize(10);
    font.setBold(false);
    metrics = QFontMetrics(font);
    d->_place = d->_place.toUpper();
    while (metrics.width(d->_place) > placeLine.width() && font.pointSize() > 6) {
        font.setPointSizeF(font.pointSizeF() - .1);
        metrics = QFontMetrics(font);
    }
    painter.setFont(font);
    painter.drawText(placeLine, Qt::AlignLeft | Qt::AlignVCenter, d->_place);

    font.setPointSize(10);
    painter.setFont(font);
    painter.drawText(dateLine, Qt::AlignLeft | Qt::AlignVCenter, d->_date.toString(Qt::SystemLocaleShortDate)); //QLocale().toString(d->_date, QLocale::ShortFormat));

    painter.restore();
    painter.end();
    return true;
}
