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
#ifndef PATIENTS_IDENTITYEDITORWIDGET_H
#define PATIENTS_IDENTITYEDITORWIDGET_H

#include <patientbaseplugin/patientbase_exporter.h>
#include <identityplugin/identityeditorwidget.h>

/**
 * \file identityeditorwidget.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 12 Dec 2012
*/

namespace Patients {
class PatientModel;

namespace Internal {
class IdentityEditorWidgetPrivate;

class PATIENT_EXPORT IdentityEditorWidget : public Identity::IdentityEditorWidget
{
    Q_OBJECT
    friend class Internal::IdentityEditorWidgetPrivate;

public:
    explicit IdentityEditorWidget(QWidget *parent = 0);
    ~IdentityEditorWidget();

    bool isIdentityValid() const;

private:
    Internal::IdentityEditorWidgetPrivate *d;
};

}  // End namespace Internal
}  // End namespace Patients

#endif // PATIENTS_IDENTITYEDITORWIDGET_H
