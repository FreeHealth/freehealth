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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef TOOLS_INTERNAL_CHEQUEPRINTFORMAT_H
#define TOOLS_INTERNAL_CHEQUEPRINTFORMAT_H

#include <QRectF>
#include <QList>
#include <QHash>
#include <QPixmap>

/**
 * \file chequeprintformat.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 27 Mar 2013
*/

namespace Tools {
namespace Internal {
class ChequePrintFormatPrivate;

class ChequePrintFormat
{
public:
    enum Rect {
        AmountNumbers = 0,
        AmountLetters,
        Date,
        Place,
        PayTo,
        RectCount
    };

    explicit ChequePrintFormat();
    ~ChequePrintFormat();

    void setDefault(bool isDefault) {_default = isDefault;}
    bool isDefault() const {return _default;}

    void setLabel(const QString &label) {_label=label;}
    QString label() const {return _label;}

    void setBackgroundPixmap(const QPixmap &pix) {_pix = pix;}
    const QPixmap &pixmap() const {return _pix;}

    void setSizeMillimeters(const QSizeF &size) {_sizeMillimeters = size;}
    QSizeF sizeMillimeters() const {return _sizeMillimeters;}

    void setRectMillimetersFromTopLeft(Rect rect, const QRectF &rectMm) {_rects.insert(rect,rectMm);}
    QRectF rectMillimetersFromTopLeft(Rect rect) const {return _rects.value(rect, QRectF());}

    QRectF chequePaperRect() const {return QRectF(QPointF(0.,0.), _sizeMillimeters);}

    static QString toXml(const QList<ChequePrintFormat> &set);
    static QList<ChequePrintFormat> fromXmlFile(const QString &file);
    static QList<ChequePrintFormat> fromXml(const QString &xmlContent);

    static bool labelLessThan(const ChequePrintFormat &format1, const ChequePrintFormat &format2);
    static bool defaultLessThan(const ChequePrintFormat &format1, const ChequePrintFormat &format2);

private:
    QString _label;
    QPixmap _pix;
    QSizeF _sizeMillimeters;
    QHash<int, QRectF> _rects;
    bool _default;
};

} // namespace Internal
} // namespace Tools

QDebug operator<<(QDebug dbg, const Tools::Internal::ChequePrintFormat &c);
QDebug operator<<(QDebug dbg, const Tools::Internal::ChequePrintFormat *c);

#endif // TOOLS_INTERNAL_CHEQUEPRINTFORMAT_H

