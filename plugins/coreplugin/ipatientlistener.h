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
 ***************************************************************************/
#ifndef IPATIENTLISTENER_H
#define IPATIENTLISTENER_H

#include <coreplugin/core_exporter.h>

#include <QtCore/QObject>

/**
 * \file ipatientlistener.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 07 Jun 2010
*/

namespace Core {

/*!
 * \brief The IPatientListener class provides an interface for a listener to patient events.
 *
 * Currently only the currentPatientAboutToChange() method is available.
 * \sa Patients::PatientModel
 */
class CORE_EXPORT IPatientListener : public QObject
{
    Q_OBJECT
public:
    IPatientListener(QObject *parent) : QObject(parent) {}
    virtual ~IPatientListener() {}

    /*! This method is called in the PatientModel \b before the patient is changed.
     *
     * The listener can do last things here like saving data.
     * @return bool \e true if it is safe to change patient. If returned \e false
     * the current patient is \e not changed.
     */
    virtual bool currentPatientAboutToChange() {return true;}
};

}

#endif // IPATIENTLISTENER_H
