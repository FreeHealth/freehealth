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
 ***************************************************************************/
/**
 * \class Views::CountryComboBox
 * Ready to use country combobox, with flags...\n
 * Path the theme and initialization are set/done in the ctor.
 * \sa Utils::CountryComboBox
 */

#include "countrycombobox.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

using namespace Views;

static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

CountryComboBox::CountryComboBox(QWidget *parent) :
    Utils::CountryComboBox(parent)
{
    setFlagPath(settings()->path(Core::ISettings::SmallPixmapPath) + "/flags/");
    initialize();
}
