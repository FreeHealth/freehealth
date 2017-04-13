/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

// Documentation only file

/**
 * \class Form::IFormItemData
 * \brief Manages data propagation between form widget and the patient model.\n
 *
 * Life cycle:
 * This form item data manager can be created by the Form::FormItem or the Form::IFormWidget.
 * It is purely internal to your form item plugin implementation. In the BaseWidgets plugin, the
 * data managers are created and managed by the Form::IFormWidget. This object is deleted
 * by its parent Form::FormItem in its dtor.
 *
 * Automatic access to the episode database:
 * You can store any stringified value in the database and receive it exactly as you set it in return.
 * The storable data can be anything you want: serialized values, XML, HTML, JSON... But only strings.
 * This will evolve in future version using QVariant.\n
 * The storable data is saved to the database after being tagged with the Form::FormItem uuid. This is
 * reason why the form item uuid MUST BE persistent in time and unique in all forms.\n
 * The storable data is kept inside the central episode database. When an episode is activated in the
 * form data widget mapper, the data manager will receive its storable data (anything stringified using
 * setStorableData()). It can then manage its Form::IFormWidget. When an episode is submitted from
 * the model (or the mapper) to the database, its storable data is saved as is.
 * You can return the storable data with storableData().
 * This mecanism is automatic, safe and secure. You don't have to worry about SQL commands or anything.
 * Just manage your string internally. \n
 * Example:
 * \code
 * QVariant MyFormData::storableData() const {
 *     return QString("I want this back as is: h=1;v=4;<widgetvalue>%1</widgetvalue>")
 *               .arg(_linkedwidget->currentValue());
 * }
 *
 * void MyFormData::setStorableData(const QVariant &dbValue) {
 *     qWarning() << dbValue.toString();
 *     // will output
 *     // "I want this back as is: h=1;v=4;<widgetvalue>...</widgetvalue>"
 * }
 * \endcode
 *
 * \sa Form::FormItem, Form::IFormWidget, Form::EpisodeModel, Form::Internal::FormDataWidgetMapper
 */


/**
 * \enum Form::IFormItemData::ReferenceRepresentation
 */

/**
 * \var Form::IFormItemData::ID_EpisodeDateTime
 */

/**
 * \var Form::IFormItemData::ID_EpisodeLabel
 */

/**
 * \var Form::IFormItemData::ID_UserName
 */

/**
 * \var Form::IFormItemData::ID_Priority
 */


/**
 * \var Form::IFormItemData::ID_CurrentUuid
 */

/**
 * \enum Form::IFormItemData::RoleRepresentation
 */

/**
 * \var Form::IFormItemData::PrintRole
 * Unused actually.
 */

/**
 * \var Form::IFormItemData::PatientModelRole,
 * Value to use for the patient model (if the Form::FormItem has got a patient data representation).
 */

/**
 * \var Form::IFormItemData::CalculationsRole
 * Value to use for calculations.
 */

/**
 * \fn void Form::IFormItemData::clear()
 * Clear the data manager (return to the default value using the form item Form::FormItemValues).
 */

/**
 * \fn Form::FormItem *Form::IFormItemData::parentItem()
 * Return the parent Form::FormItem.
 */

/**
 * \fn bool Form::IFormItemData::isModified()
 * Return true if the data content is modified (usually when the form widget was edited).
 * \sa setModified(), Form::EpisodeModel::submit(), Form::Internal::FormDataWidgetMapper::isDirty()
 */

/**
 * \fn void Form::IFormItemData::setModified(bool modified)
 * Set the modified state of the item data.
 * \sa isModified(), Form::EpisodeModel::submit()
 */

/**
 * \fn bool Form::IFormItemData::setData(const int ref, const QVariant &data, const int role = Qt::EditRole)
 * Set the data internally and to the widget. The \e ref is the patient data representation (Core::IPatient::PatientDataRepresentation),
 * \e data the data, \e role the associated role (see QAbstractItemModel roles and the Form::IFormItemData::RoleRepresentation).
 */

/**
 * \fn QVariant Form::IFormItemData::data(const int ref, const int role = Qt::DisplayRole) const
 * Return the data.  The \e ref is the patient data representation (Core::IPatient::PatientDataRepresentation)
 * and \e role the associated role (see QAbstractItemModel roles and the Form::IFormItemData::RoleRepresentation).
 */

/**
 * \fn void Form::IFormItemData::setStorableData(const QVariant &data)
 * Receive the stored data for this item.
 */

/**
 * \fn QVariant Form::IFormItemData::storableData() const
 * Return the data to be saved into database.
 */

/**
 * \fn void Form::IFormItemData::dataChanged(const int ref)
 * Signal must be emitted when the internal data changed whatever is the modified role.
 * The \e ref is the patient data representation (Core::IPatient::PatientDataRepresentation).
 */

