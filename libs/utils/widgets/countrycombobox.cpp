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
#include "countrycombobox.h"
#include <utils/global.h>

#include <QLocale>
#include <QMap>
#include <QFileInfo>

#include <QDebug>

using namespace Utils;

CountryComboBox::CountryComboBox(QWidget *parent) :
    QComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(on_currentIndexChanged(int)));
}

void CountryComboBox::setFlagPath(const QString &absPath)
{
    if (QFileInfo(absPath).exists())
        m_FlagPath = absPath;
    else
        m_FlagPath.clear();
}

void CountryComboBox::initialize()
{
    // populate with available countries
    for(int i = 1; i < 246; ++i) {
        addCountry(QLocale::Country(i));
    }
    setCurrentCountry(QLocale::system().country());
}

void CountryComboBox::addCountry(QLocale::Country country)
{
    if (country == QLocale::AnyCountry)
        return;
    const QString &countryName = QLocale::countryToString(country);
    if (countryName.isEmpty())
        return;
    QString flag = Utils::countryToIso(country);
    addItem(QIcon(QString("%1/%2.png").arg(m_FlagPath, flag)), countryName, QVariant(country));

}

void CountryComboBox::removeCountry(QLocale::Country country)
{
    int index = findData(QVariant(country));
    if (index == -1)
        return;
    removeItem(index);
}

QLocale::Country CountryComboBox::currentCountry() const
{
    bool ok;
    int country = itemData(currentIndex()).toInt(&ok);
    return ok? QLocale::Country(country) : QLocale::AnyCountry;
}

/** Return the translated, human readable country name */
QString CountryComboBox::currentCountryName() const
{
    return QLocale::countryToString(currentCountry());
}

/*! Returns an uppercase 2-char ISO name of the current selected country. */
QString CountryComboBox::currentIsoCountry() const
{
    bool ok;
    int country = itemData(currentIndex()).toInt(&ok);
    return ok? Utils::countryToIso(QLocale::Country(country)).toUpper() : QString();
}

/*! Set the Combobox index to the country specified by the given 2-char ISO code country */
void CountryComboBox::setCurrentIsoCountry(const QString &isoCode)
{
    setCurrentIndex(findData(QVariant(Utils::countryIsoToCountry(isoCode))));
}

/*! Set the Combobox index to the country specified by the given \e country */
void CountryComboBox::setCurrentCountry(QLocale::Country country)
{
    setCurrentIndex(findData(QVariant(country)));
}


void CountryComboBox::on_currentIndexChanged(int index)
{
    Q_EMIT currentCountryChanged(QLocale::Country(itemData(index).toInt()));
}
