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
 * \class Tools::ChequePrinter
 * \brief Cheque printer helper.
 *
 * Prepare a cheque printing and start the print process.
 */

#include "chequeprinter.h"
#include "../constants.h"
#include "chequeprintformat.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/numbertostring.h>
#include <utils/printaxishelper.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>

#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QSystemLocale>
#include <QPixmap>

#include <QDebug>

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

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
        _drawRects(false),
        q(parent)
    {
    }

    ~ChequePrinterPrivate()
    {
    }

    void drawContent(QPainter &painter, const ChequePrintFormat &format)
    {
        QString amount;
        QStringList vals = Utils::doubleToHumanReadableString(_amount);
        if (vals.count() == 0) {
            LOG_ERROR_FOR("ChecquePrinter", "Wrong amount to string values");
        } else {
            amount = vals.at(0) + " " + QLocale().currencySymbol(QLocale::CurrencyDisplayName) + "s";
            if (vals.count() == 2)
                amount += QString(" %1 %2").arg(tkTr(Trans::Constants::AND)).arg(vals.at(1));
        }
        if (amount.count() > 50)
            amount = Utils::lineWrapString(amount, 50);
        amount = QString("****%1****").arg(amount.toUpper());

        Utils::PrintString s;
        s.autoFontResize = false;
        s.splitChars = false;
        s.minFontPixelSize = 10;
        s.drawBoundingRect = _drawRects;

        if (_drawRects)
            painter.drawRect(QRectF(_axisHelper.pointToPixels(format.chequePaperRect().topLeft()),
                                    _axisHelper.sizeToPixels(format.chequePaperRect().size()))
                             );

        QFont font = painter.font();
        font.setBold(true);
        painter.setFont(font);
        const QRectF &rectAmount = format.rectMillimetersFromTopLeft(Tools::Internal::ChequePrintFormat::AmountNumbers);
        s.topMillimeters = rectAmount.topLeft();
        s.contentSizeMillimeters = rectAmount.size();
        s.alignment = Qt::AlignCenter;
        s.content = QString("****%1****").arg(QLocale().toString(_amount, 'f', 2));
        _axisHelper.printString(&painter, s);
        font.setBold(false);
        painter.setFont(font);

        s.alignment = Qt::AlignLeft | Qt::AlignTop;
        const QRectF &rectAmountLetters = format.rectMillimetersFromTopLeft(Tools::Internal::ChequePrintFormat::AmountLetters);
        s.topMillimeters = rectAmountLetters.topLeft();
        s.contentSizeMillimeters = rectAmountLetters.size();
        s.content = amount;
        _axisHelper.printString(&painter, s);

        s.autoFontResize = true;
        const QRectF &rectDate = format.rectMillimetersFromTopLeft(Tools::Internal::ChequePrintFormat::Date);
        s.topMillimeters = rectDate.topLeft();
        s.contentSizeMillimeters = rectDate.size();
        s.content = _date.toString(Qt::SystemLocaleShortDate); //QLocale().toString(d->_date, QLocale::ShortFormat));
        _axisHelper.printString(&painter, s);

        const QRectF &rectPlace = format.rectMillimetersFromTopLeft(Tools::Internal::ChequePrintFormat::Place);
        s.topMillimeters = rectPlace.topLeft();
        s.contentSizeMillimeters = rectPlace.size();
        s.content = _place.toUpper();
        _axisHelper.printString(&painter, s);

        const QRectF &rectPayTo = format.rectMillimetersFromTopLeft(Tools::Internal::ChequePrintFormat::PayTo);
        s.topMillimeters = rectPayTo.topLeft();
        s.contentSizeMillimeters = rectPayTo.size();
        s.content = _order.toUpper();
        _axisHelper.printString(&painter, s);
    }

public:
    QString _order, _place;
    QDate _date;
    double _amount;
    Utils::PrintAxisHelper _axisHelper;
    bool _drawRects;

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

void ChequePrinter::setDrawRects(bool drawRects)
{
    d->_drawRects = drawRects;
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

bool ChequePrinter::print(const Internal::ChequePrintFormat &format)
{
    // try to use the default printer
    QPrintDialog dlg;
    if (dlg.exec()==QDialog::Rejected)
        return false;
    QPrinter *printer = dlg.printer();
    printer->setFullPage(true);
    printer->setPaperSize(QPrinter::A4);
    printer->setResolution(150);
    printer->setOrientation(QPrinter::Landscape);
    d->_axisHelper.setPageSize(printer->paperRect(), printer->paperSize(QPrinter::Millimeter));

    // Printer correction: not over margins
    qreal l, r, t ,b;
    printer->getPageMargins(&l, &t, &r, &b, QPrinter::DevicePixel);
    d->_axisHelper.setMargins(l, t, r, b);

    QPainter painter;
    if (!painter.begin(printer)) { // failed to open file
        qWarning("failed to open file, is it writable?");
        return false;
    }

    // Center the cheque in the page
    double centerX = (printer->paperSize(QPrinter::Millimeter).width() - format.sizeMillimeters().width());
    double centerY = (printer->paperSize(QPrinter::Millimeter).height() / 2.) - (format.sizeMillimeters().height() / 2.);
    // Printer correction: user defined
    // As we are printing in landscape -> invert x <-> y in the translation
    centerX -= settings()->value(Core::Constants::S_PRINTERCORRECTION_VERTIC_MM).toDouble();
    centerY -= settings()->value(Core::Constants::S_PRINTERCORRECTION_HORIZ_MM).toDouble();
    d->_axisHelper.translateMillimeters(centerX, centerY);

    QFont font;
    font.setPointSize(10);
    painter.setFont(font);

    painter.save();
    d->drawContent(painter, format);
    painter.restore();
    painter.end();
    return true;
}

/**
 * Print the preview in a QPainter
\code
// Example of code using the previewer
   FspPrinter printer;
   printer.setDrawRects(true);
   Utils::ImageViewer viewer;
   QPixmap pix = QPixmap::fromImage(printer.preview(format));
   pix = pix.scaledToWidth(800);
   viewer.setPixmap(pix);
   viewer.exec();
\endcode
 */
QPixmap ChequePrinter::preview(const Internal::ChequePrintFormat &format)
{
    QPixmap image(format.sizeMillimeters().width()*10, format.sizeMillimeters().height()*10);
    image.fill(Qt::white);
    d->_axisHelper.setPageSize(image.rect(), format.sizeMillimeters());
//    d->_axisHelper.setPageSize(image.rect(), QSize(210,297)); // A4

    // Center the cheque in the page
//    double centerX = (printer->paperSize(QPrinter::Millimeter).width() - format.sizeMillimeters().width());
//    double centerY = (printer->paperSize(QPrinter::Millimeter).height() / 2.) - (format.sizeMillimeters().height() / 2.);
//    d->_axisHelper.translateMillimeters(centerX, centerY);

    QPainter painter;
    if (!painter.begin(&image)) { // failed to open file
        qWarning("failed to begin the image device");
        return image;
    }

    QFont font;
    font.setPointSize(28);
    painter.setFont(font);

//    QPixmap background;
//    if (cerfa == S12541_01) {
//        if (!background.load(settings()->path(Core::ISettings::ThemeRootPath) + "/pixmap/others/S3110.png", "PNG"))
//            qWarning() << "ERROR: unable to load background pixmap";
//    } else {
//        if (!background.load(settings()->path(Core::ISettings::ThemeRootPath) + "/pixmap/others/S3110_02.png", "PNG"))
//            qWarning() << "ERROR: unable to load background pixmap";
//    }
//    painter.drawPixmap(image.rect(), background);

    painter.save();
    d->drawContent(painter, format);
    painter.restore();
    painter.end();
    return image;
}
