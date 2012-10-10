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
#include "igenericpage.h"

#include <QDebug>

/**
 * \class Core::IGenericPage
 * \brief Represents an application preferences page. Derive objects from this interface and set it inside the PluginManager object pool to get a paged widget Core::PageWidget.
 */

/** Use this to sort page list by category && sort index. */
bool Core::IGenericPage::lessThan(IGenericPage *one, IGenericPage *two)
{
//    qWarning() << "LESS THAN" << one->category() << two->category() << one->sortIndex() << two->sortIndex();
    if (one->category() < two->category())
        return true;
    else if (one->category() == two->category()
             && one->sortIndex() < two->sortIndex())
             return true;
//    qWarning() << "    FALSE";
    return false;
}

