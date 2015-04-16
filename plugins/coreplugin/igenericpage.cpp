/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2015 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 * \brief Represents an application preferences page.
 *
 * Derive objects from this interface and set it inside the PluginManager object pool to get a
 * paged widget Core::PageWidget.
 *
 * Implementation hints:
 * \list
 * \o id() is a unique identifier for referencing this page
 * \o displayName() is the (translated) name for display
 * \o title() returns the title of the page to show in the widget
 * \o category() is the category to use. You can use a path encoded tree, e.g.: <em>first/second</em>
 * \o displayCategory() is the translated name of the category
 * \o createPage() is called to retrieve the widget to show in the preferences dialog. Implement
 *    this to create and return your custom widget.
 *    The widget will be destroyed by the widget hierarchy when the dialog closes.
 * \o sortIndex() the sort index of the page. The larger the index, the more is the element at the end of the list.
 * \endlist
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

bool Core::IGenericPage::sortIndexLessThan(IGenericPage *one, IGenericPage *two)
{
    return one->sortIndex() < two->sortIndex();
}
