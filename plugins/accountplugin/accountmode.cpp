/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "accountmode.h"
#include "accountview.h"

#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_menus.h>
#include <coreplugin/constants_icons.h>

using namespace Account::Internal;

static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

AccountMode::AccountMode(QObject *parent) :
    Core::BaseMode(parent)
{
    setName(tr("Accountancy"));
    setIcon(theme()->icon(Core::Constants::ICONACCOUNTMODE, Core::ITheme::BigIcon));
    setPriority(Core::Constants::P_MODE_ACCOUNT);
    setUniqueModeName(Core::Constants::MODE_ACCOUNT);
//    const QList<int> &context;
//    setContext();
    setWidget(new Account::AccountView);

    /** \todo connect actions from the account menu */
    /** \todo connect patient changed and refresh views ? */
}
