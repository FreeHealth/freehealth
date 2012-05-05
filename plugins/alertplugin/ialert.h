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
 *   Main Developpers:                                                     *
 *       Eric MAEKER, <eric.maeker@gmail.com>,                             *
 *       Pierre-Marie Desombre <pm.desombre@gmail.com>                     *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IALERT
#define IALERT

#include <QObject>
#include <QUuid>
#include <QString>

namespace Alert {

class IAlert : public QObject
{
    Q_OBJECT
public:
    enum TypeOfAlert
    {
        ALL_ALERTS = 0,
        PATIENT_PRIMARY_PREVENTION_ALERTS,
        PATIENT_SECONDARY_PREVENTION_ALERTS,
        PATIENT_PRESCRIPTION_ALERTS,
        PATIENT_NOTES,
        TypeOfAlert_MaxParams
    };

    enum ItemsDefinition
    {
        TYPE_OF_ITEM = 0,
        PARENT_ITEM,
        PARENT_OR_CHILD,
        ICON,
        DATE_DONE,
        DATE_NEXT,
        ABSTRACT,
        ID_ITEM,
        PATIENT_UID,
        USER_UID,
        ItemsDefinition_MaxParam
    };

public:
    IAlert (){}
    virtual ~IAlert (){}

    //methods
    virtual void initialize() = 0;
    virtual QString setAlertUuid() = 0 ;
    virtual void showIHMaccordingToType(int) = 0 ;

};

}  // Alert

#endif

