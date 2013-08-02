/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef PATIENTBASE_PATIENTBAR_H
#define PATIENTBASE_PATIENTBAR_H

#include <coreplugin/ipatientbar.h>

#include <QModelIndex>

/**
 * \file patientbar.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 04 Mar 2013
*/

namespace Patients {
class PatientModel;
class PatientCore;

namespace Internal {
class PatientBarPrivate;
}

class PatientBar : public Core::IPatientBar
{
    Q_OBJECT
    friend class Patients::PatientCore;

protected:
    PatientBar(QWidget *parent = 0);

public:
    ~PatientBar();

    void addBottomWidget(QWidget *widget);
    void showMessage(const QString &message, int duration_ms = 2000, const QString &css = QString::null);

private Q_SLOTS:
    void onCurrentPatientChanged();
    void onPatientDataChanged(const QModelIndex &top, const QModelIndex &bottom);

protected:
    void paintEvent(QPaintEvent *);
    void changeEvent(QEvent *event);

private:
    Internal::PatientBarPrivate *d;
};

}  // End namespace Patients

#endif // PATIENTBASE_PATIENTBAR_H
