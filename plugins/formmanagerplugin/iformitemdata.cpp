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

// Documentation only file

/**
 * \class Form::IFormItemData
 */


/**
 * \enum Form::IFormItemData::ReferenceRepresentation
 */

/**
 * \var Form::IFormItemData::ID_EpisodeDate 
 */

/**
 * \var Form::IFormItemData::ID_EpisodeLabel 
 */

/**
 * \var Form::IFormItemData::ID_UserName 
 */

/**
 * \var Form::IFormItemData::ID_CurrentUuid 
 */

/**
 * \enum Form::IFormItemData::RoleRepresentation 
 */

/**
 * \var Form::IFormItemData::PrintRole  
 */

/**
 * \var Form::IFormItemData::PatientModelRole, 
 */

/**
 * \var Form::IFormItemData::CalculationsRole 
 */

/**
 * \fn void Form::IFormItemData::clear()
 */
 
/**
 * \fn Form::FormItem *Form::IFormItemData::parentItem()
 */

/**
 * \fn bool Form::IFormItemData::isModified()
 */

/**
 * \fn bool Form::IFormItemData::setData(const int ref, const QVariant &data, const int role = Qt::EditRole)
 */

/**
 * \fn QVariant Form::IFormItemData::data(const int ref, const int role = Qt::DisplayRole) const
 */

/**
 * \fn void Form::IFormItemData::setStorableData(const QVariant &data)
 */

/**
 * \fn QVariant Form::IFormItemData::storableData() const
 */

/**
 * \fn void Form::IFormItemData::dataChanged(const int ref)
 */

