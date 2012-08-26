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
#ifndef COUNTRYCOMBOBOX_H
#define COUNTRYCOMBOBOX_H

#include <utils/global_exporter.h>
#include <QComboBox>

namespace Utils {

class UTILS_EXPORT CountryComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QLocale::Country currentCountry READ currentCountry WRITE setCurrentCountry NOTIFY currentCountryChanged)
    Q_PROPERTY(QString currentIsoCountry READ currentIsoCountry WRITE setCurrentIsoCountry NOTIFY currentIsoCountryChanged)

public:
    explicit CountryComboBox(QWidget *parent = 0);
    void setFlagPath(const QString &absPath);
    virtual void initialize();

    QLocale::Country currentCountry() const;
    QString currentIsoCountry() const;

public Q_SLOTS:
    void setCurrentIsoCountry(const QString &isoCode);
    void setCurrentCountry(QLocale::Country country);

Q_SIGNALS:
    void currentCountryChanged(QLocale::Country country);
    void currentIsoCountryChanged(const QString &name);

private:
    QString m_FlagPath;
};

}  // End namespace Utils

#endif // COUNTRYCOMBOBOX_H
