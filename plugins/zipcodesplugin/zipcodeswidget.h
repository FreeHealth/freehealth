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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef ZIPCODES_ZIPCODESWIDGET_H
#define ZIPCODES_ZIPCODESWIDGET_H

#include <zipcodesplugin/zipcodes_exporter.h>
#include <QWidget>
QT_BEGIN_NAMESPACE
class QDataWidgetMapper;
QT_END_NAMESPACE

/**
 * \file zipcodeswidget.h
 * \author Eric Maeker
 * \version 0.8.2
 * \date 07 Dec 2012
*/

namespace ZipCodes {
namespace Internal {
class ZipCodesWidgetPrivate;
}  // namespace Internal

class ZIPCODES_EXPORT ZipCodesWidget : public QWidget
{
    Q_OBJECT
public:
    enum TypeOfLayout {
//        GridLayoutOnePerLine,
//        GridLayoutTwoPerLine,
        GridLayout,
        FormLayout
    };

    enum Mapping {
        StreetPlainText,
        CityPlainText,
        CountryLocale,
        CountryIso,
        StateProvincePlainText,
        ZipcodePlainText
    };

    explicit ZipCodesWidget(QWidget *parent = 0);
    ~ZipCodesWidget();
    bool initialize(TypeOfLayout layout);

    void setReadOnly(bool readOnly);

    void addMapping(QDataWidgetMapper *mapper, const int section, Mapping mapping) const;

    void clear();
    void setStreet(const QString &street);
    void setCity(const QString &city);
    void setStateProvince(const QString &state);
    void setCountry(const QString &twoCharIsoCode);
    void setZipCode(const QString &zip);

    QString street() const;
    QString city() const;
    QString stateProvince() const;
    QString countryName() const;
    QString countryIso() const;
    QString zipCode() const;

private:
    void retranslateUi();
    void changeEvent(QEvent *e);

private:
    Internal::ZipCodesWidgetPrivate *d;
};

} // namespace ZipCodes

#endif  // ZIPCODES_ZIPCODESWIDGET_H

