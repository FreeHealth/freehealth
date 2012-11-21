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

/**
 * \file ioptionspage.h
 * \author Christian A. Reiter <christian.a.reiter@gmail.com>
 * \version 0.8.0
 * \date 2012-11-19
 * \class Core::IOptionsPage
 * \brief Represents an application preferences page. Derive objects from this interface and set it inside the PluginManager object pool to get the page in the Core::SettingsDialog.
*/

#include "ioptionspage.h"

/*!
  \class Core::IOptionsPage
  \brief The IOptionsPage is an interface for providing options pages.

  You need to subclass this interface and put an instance of your subclass
  into the plugin manager object pool (e.g. ExtensionSystem::PluginManager::addObject).
  Guidelines for implementing:
  \list
  \o id() is a unique identifier for referencing this page
  \o displayName() is the (translated) name for display
  \o category() is the unique id for the category that the page should be displayed in
  \o displayCategory() is the translated name of the category
  \o createPage() is called to retrieve the widget to show in the preferences dialog
     The widget will be destroyed by the widget hierarchy when the dialog closes
  \o apply() is called to store the settings. It should detect if any changes have been
         made and store those
  \o finish() is called directly before the preferences dialog closes
  \o matches() is used for the options dialog search filter
  \endlist
*/

/*!
 * fn IOptionsPage::checkSettingsValidity()
 * \brief Check the validity of settings. You don't need to sync the settings. This will be automatically done.
 */

/*!
 * fn IOptionsPage::applyChanges()
 * \brief Apply changes to the settings. You don't need to sync the settings. This will be automatically done.
 */
