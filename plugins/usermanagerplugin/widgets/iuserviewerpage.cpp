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
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class UserPlugin::IUserViewerPage
 * Acts like a QWizardPage in the UserPlugin::UserViewer. Each gets
 * included in the menu and in its central view. \n
 * The UserPlugin::UserViewer manages the UserPlugin::IUserViewerWidget
 * objects (that are extracted from the pluginmanager object pool).
*/

/**
 * \class UserPlugin::IUserViewerWidget
 * Acts like a QDataWidgetMapper over the Core::IUser model.
 */

/**
 * \fn UserPlugin::IUserViewerWidget::IUserViewerWidget(QWidget *parent = 0)
 * Ctor of the widget
 */

/**
 * \fn UserPlugin::IUserViewerWidget::~IUserViewerWidget()
 * Dtor of the widget
 */

/**
 * \fn virtual void UserPlugin::IUserViewerWidget::setUserModel(UserModel *model) = 0;
 * Define the UserPlugin::UserModel to use.
 */

/**
 * \fn virtual void UserPlugin::IUserViewerWidget::setUserIndex(const int row) = 0;
 * Define the current index. Model is defined before of course.
 */

/**
 * \fn virtual void UserPlugin::IUserViewerWidget::clear() = 0;
 * Clear the UI content, cache content...
 */

/**
 * \fn virtual bool UserPlugin::IUserViewerWidget::submit() = 0;
 * Submit changes to the model and return the state of error (true = no error, false = error).
 */

