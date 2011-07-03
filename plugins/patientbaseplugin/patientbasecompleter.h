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
#ifndef PATIENTBASECOMPLETER_H
#define PATIENTBASECOMPLETER_H

#include <patientbaseplugin/patientbase_exporter.h>

#include <QCompleter>
#include <QValidator>

class QSqlTableModel;

/**
 * \file patientcompleter.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 03 Jul 2011
*/

namespace Patients {
namespace Internal {
class PatientBaseCompleterPrivate;
}

class PATIENT_EXPORT PatientBaseCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit PatientBaseCompleter(QObject *parent = 0);
    ~PatientBaseCompleter();

    QValidator *validator() const;

private:
    Internal::PatientBaseCompleterPrivate *d;
};

}

#endif // PATIENTBASECOMPLETER_H
