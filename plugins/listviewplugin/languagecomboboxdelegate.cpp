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
#include "languagecomboboxdelegate.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/isettings.h>

#include <QDebug>

static inline Core::ITheme *theme() {return Core::ICore::instance()->theme();}
static inline Core::ISettings *settings() {return Core::ICore::instance()->settings();}

using namespace Views;

LanguageComboBoxDelegate::LanguageComboBoxDelegate(QObject *parent, DisplayMode mode) :
        Utils::LanguageComboBoxDelegate(parent)
{
    Q_UNUSED(mode);
    setFlagsIconPath(settings()->path(Core::ISettings::SmallPixmapPath) + "/flags/");
    setTranslationsPath(settings()->path(Core::ISettings::TranslationsPath));
}
