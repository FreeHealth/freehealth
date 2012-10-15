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
/**
 * \class Alert::Internal::PatientBarAlertPlaceHolder
 * Manage patient non-blocking alerts. This widget is included in the
 * Patients::PatientBar widget and is accessible in all Core::IMode that
 * shows the patient bar.
 * \sa Alert::AlertPlaceHolderWidget, Patients::PatientBar, Alert plugin
 */

#include "patientbaralertplaceholder.h"

using namespace Alert;
using namespace Internal;

PatientBarAlertPlaceHolder::PatientBarAlertPlaceHolder(QObject *parent) :
    Alert::AlertPlaceHolderWidget(parent)
{
    setIconSize(QSize(16,16));
    setMargin(0);
    setSpacing(2);
    setBorderSize(0);
    setDrawBackgroundUsingAlertPriority(false);
    setAutoSaveOnValidationOrOverriding(true);
    setAutoSaveOnEditing(true);
    setObjectName("Patients::Internal::PatientBarAlertPlaceHolder");
}

QString PatientBarAlertPlaceHolder::uuid() const
{
    return objectName();
}

QString PatientBarAlertPlaceHolder::name(const QString &lang) const
{
    Q_UNUSED(lang);
    return tr("Patient non-blocking alerts");
}

QString PatientBarAlertPlaceHolder::category(const QString &lang) const
{
    Q_UNUSED(lang);
    return tr("Patient alerts");
}

QString PatientBarAlertPlaceHolder::description(const QString &lang) const
{
    Q_UNUSED(lang);
    return tr("Placeholder for patient related non-blocking alerts.");
}
