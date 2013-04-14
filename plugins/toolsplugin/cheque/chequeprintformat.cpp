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
 * \class Tools::Internal::ChequePrintFormat
 * Stores all information used for the cheque printing process.
 */

#include "chequeprintformat.h"

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QDomDocument>

#include <QDebug>

using namespace Tools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
const char * const XML_TAG_CHEQUESET = "ChqSet";
const char * const XML_TAG_CHEQUE = "Chq";
const char * const XML_ATTRIB_LABEL = "name";
const char * const XML_ATTRIB_DEFAULT = "default";
const char * const XML_TAG_RECT = "Rect";
const char * const XML_ATTRIB_RECT = "r";
const char * const XML_ATTRIB_POINTX = "x";
const char * const XML_ATTRIB_POINTY = "y";
const char * const XML_ATTRIB_WIDTH = "w";
const char * const XML_ATTRIB_HEIGHT = "h";
const char * const XML_TAG_SIZE = "Size";
const char * const XML_TAG_BACKGROUNDPIX = "Pix";

QString rectToXml(Tools::Internal::ChequePrintFormat::Rect rect)
{
    switch (rect) {
    case Tools::Internal::ChequePrintFormat::AmountNumbers: return "an";
    case Tools::Internal::ChequePrintFormat::AmountLetters: return "al";
    case Tools::Internal::ChequePrintFormat::Date: return "dt";
    case Tools::Internal::ChequePrintFormat::Place: return "pl";
    case Tools::Internal::ChequePrintFormat::PayTo: return "pt";
    default: return QString::null;
    }
    return QString::null;
}

Tools::Internal::ChequePrintFormat::Rect rectFromXml(const QString &s)
{
    if (s == "an")
        return Tools::Internal::ChequePrintFormat::AmountNumbers;
    if (s == "al")
        return Tools::Internal::ChequePrintFormat::AmountLetters;
    if (s == "dt")
        return Tools::Internal::ChequePrintFormat::Date;
    if (s == "pl")
        return Tools::Internal::ChequePrintFormat::Place;
    if (s == "pt")
        return Tools::Internal::ChequePrintFormat::PayTo;
    return Tools::Internal::ChequePrintFormat::PayTo;
}

}

/*! Constructor of the Tools::Internal::ChequePrintFormat class */
ChequePrintFormat::ChequePrintFormat() :
    _default(false)
{
}

/*! Destructor of the Tools::Internal::ChequePrintFormat class */
ChequePrintFormat::~ChequePrintFormat()
{
}

QString ChequePrintFormat::toXml(const QList<ChequePrintFormat> &set)
{
    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE FreeMedForms>\n";
    QDomDocument doc;
    QDomElement root = doc.createElement(::XML_TAG_CHEQUESET);
    doc.appendChild(root);
    foreach(const ChequePrintFormat &format, set) {
        QDomElement cheque = doc.createElement(::XML_TAG_CHEQUE);
        cheque.setAttribute(::XML_ATTRIB_LABEL, format.label());
        cheque.setAttribute(::XML_ATTRIB_DEFAULT, format.isDefault());
        root.appendChild(cheque);

        // Append rects
        for(int i=0; i < RectCount; ++i) {
            const QRectF &rect = format.rectMillimetersFromTopLeft(Rect(i));
            if (rect.isNull())
                continue;
            QDomElement item = doc.createElement(::XML_TAG_RECT);
            item.setAttribute(::XML_ATTRIB_RECT, ::rectToXml(Rect(i)));
            item.setAttribute(::XML_ATTRIB_POINTX, rect.x());
            item.setAttribute(::XML_ATTRIB_POINTY, rect.x());
            item.setAttribute(::XML_ATTRIB_WIDTH, rect.width());
            item.setAttribute(::XML_ATTRIB_HEIGHT, rect.height());
            cheque.appendChild(item);
        }

        // Append size
        QDomElement item = doc.createElement(::XML_TAG_SIZE);
        item.setAttribute(::XML_ATTRIB_WIDTH, format.sizeMillimeters().width());
        item.setAttribute(::XML_ATTRIB_HEIGHT, format.sizeMillimeters().height());
        cheque.appendChild(item);

        // Append background pix
        if (!format.pixmap().isNull()) {
            QDomElement item = doc.createElement(::XML_TAG_BACKGROUNDPIX);
            item.setNodeValue(Utils::pixmapToBase64(format.pixmap()));
            cheque.appendChild(item);
        }
    }
    return xml;
}

QList<ChequePrintFormat> ChequePrintFormat::fromXmlFile(const QString &file)
{
    return fromXml(Utils::readTextFile(file));
}

QList<ChequePrintFormat> ChequePrintFormat::fromXml(const QString &xmlContent)
{
    QList<ChequePrintFormat> set;
    QDomDocument doc;
    QString error;
    int col, line;
    if (!doc.setContent(xmlContent, &error, &line, &col)) {
        Utils::warningMessageBox("Erreur XML",
                                 QString("Une erreur XML a été détectée :<br />"
                                         "(%1; %2) %3<br />")
                                 .arg(line).arg(col).arg(error));
        return set;
    }
    QDomElement root = doc.firstChildElement(::XML_TAG_CHEQUESET);
    root = root.firstChildElement(::XML_TAG_CHEQUE);
    while (!root.isNull()) {
        ChequePrintFormat format;
        // Read Label
        format.setLabel(root.attribute(::XML_ATTRIB_LABEL));
        format.setDefault(root.attribute(::XML_ATTRIB_DEFAULT) == "true");

        // Read pix
        QDomElement pix = root.firstChildElement(::XML_TAG_BACKGROUNDPIX);
        if (!pix.isNull())
            format.setBackgroundPixmap(Utils::pixmapFromBase64(pix.text().toUtf8()));

        // Read rects
        QDomElement rect = root.firstChildElement(::XML_TAG_RECT);
        while (!rect.isNull()) {
            QRectF rectF;
            rectF.setX(rect.attribute(::XML_ATTRIB_POINTX).toDouble());
            rectF.setY(rect.attribute(::XML_ATTRIB_POINTY).toDouble());
            rectF.setWidth(rect.attribute(::XML_ATTRIB_WIDTH).toDouble());
            rectF.setHeight(rect.attribute(::XML_ATTRIB_HEIGHT).toDouble());
            format.setRectMillimetersFromTopLeft(ChequePrintFormat::Rect(::rectFromXml(rect.attribute(::XML_ATTRIB_RECT))), rectF);
            rect = rect.nextSiblingElement(::XML_TAG_RECT);
        }

        QDomElement size = root.firstChildElement(::XML_TAG_SIZE);
        if (!size.isNull()) {
            QSizeF sizeF;
            sizeF.setWidth(size.attribute(::XML_ATTRIB_WIDTH).toDouble());
            sizeF.setHeight(size.attribute(::XML_ATTRIB_HEIGHT).toDouble());
            format.setSizeMillimeters(sizeF);
        } else {
            LOG_ERROR_FOR("ChequePrintFormat", "No size");
        }
        set << format;
        root = root.nextSiblingElement(::XML_TAG_CHEQUE);
    }
    return set;
}

bool ChequePrintFormat::labelLessThan(const ChequePrintFormat &format1, const ChequePrintFormat &format2)
{
    return format1.label() < format2.label();
}

bool ChequePrintFormat::defaultLessThan(const ChequePrintFormat &format1, const ChequePrintFormat &format2)
{
    Q_UNUSED(format1);
    if (format2.isDefault())
        return false;
    return true;
}

QDebug operator<<(QDebug dbg, const Tools::Internal::ChequePrintFormat &c)
{
    QStringList t;
    for(int i=0; i < Tools::Internal::ChequePrintFormat::RectCount; ++i) {
        const QRectF &rect = c.rectMillimetersFromTopLeft(Tools::Internal::ChequePrintFormat::Rect(i));
        if (rect.isNull())
            continue;
        t << QString("Rect: %1; %2; %3; %4; %5")
             .arg(rectToXml(Tools::Internal::ChequePrintFormat::Rect(i)))
             .arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
    }
    dbg.nospace() << "ChequePrintFormat("
                  << QString("Size: %1;%2").arg(c.sizeMillimeters().width()).arg(c.sizeMillimeters().height())
                  << t.join(" - ")
                  << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Tools::Internal::ChequePrintFormat *c)
{
    if (!c) {
        dbg.nospace() << "ChequePrintFormat(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}
