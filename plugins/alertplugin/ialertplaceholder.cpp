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
 *  Main Developers:                                                       *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/

/**
 * \class Alert::IAlertPlaceHolder
 * This object is an alert manager. It receive any information by the Alert::AlertCore and is in charge
 * of creating and managing a widget that presents the registered alerts. This widget can (or can not) allow
 * user to interact with alerts (edit, validate, override...).
 */

/** \fn virtual QString Alert::IAlertPlaceHolder::uuid() const = 0;
 * Return the unique identifier of the object
 */

/** \fn virtual QString Alert::IAlertPlaceHolder::name(const QString &lang = QString::null) const = 0;
 * Return the translated name of the object
 */

/** \fn virtual QString Alert::IAlertPlaceHolder::category(const QString &lang = QString::null) const = 0;
 * Return the translated category name of the object
 */

/** \fn virtual QString Alert::IAlertPlaceHolder::description(const QString &lang = QString::null) const = 0;
 * Return the translated description of the object
 */

/** \fn virtual void Alert::IAlertPlaceHolder::clear() = 0;
 * Clear the alert manager of any registered alerts
 */

/** \fn virtual bool Alert::IAlertPlaceHolder::addAlert(const AlertItem &alert) = 0;
 * If the \e alert can be managed by this object, register the alert and return true. Otherwise return false.
 */

/** \fn virtual bool Alert::IAlertPlaceHolder::updateAlert(const AlertItem &alert) = 0;
 * If the \e alert can be managed by this object, update the alert and return true. Otherwise return false.
 */

/** \fn virtual bool Alert::IAlertPlaceHolder::removeAlert(const AlertItem &alert) = 0;
 * If the \e alert is managed by this object, remove the alert and return true. Otherwise return false.
 */

/** \fn virtual bool Alert::IAlertPlaceHolder::highlightAlert(const AlertItem &alert) = 0;
 * If the \e alert is managed by this object, highlight the alert and return true. Otherwise return false.
 */

/** \fn virtual QWidget *Alert::IAlertPlaceHolder::createWidget(QWidget *parent = 0) = 0;
 * Create the widget in of this alert manager. This widget can be anything and should be updated
 * any time an alert is registered, removed, updated or highlighted.
 */

#include "ialertplaceholder.h"

using namespace Alert;

/** Create an empty object */
IAlertPlaceHolder::IAlertPlaceHolder(QObject *parent) :
    QObject(parent)
{
}
