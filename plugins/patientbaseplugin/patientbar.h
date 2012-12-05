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
#ifndef PATIENTBAR_H
#define PATIENTBAR_H

#include <patientbaseplugin/patientbase_exporter.h>

#include <QWidget>
#include <QModelIndex>

namespace Patients {
class PatientModel;

namespace Internal {
class PatientBarPrivate;
}

class PATIENT_EXPORT PatientBar : public QWidget
{
    Q_OBJECT

    PatientBar(QWidget *parent = 0);
public:
    static PatientBar *instance(QWidget *parent = 0);
    ~PatientBar();

    void addBottomWidget(QWidget *widget);

private Q_SLOTS:
    void onCurrentPatientChanged();
    void patientDataChanged(const QModelIndex &top, const QModelIndex &bottom);

protected:
    void paintEvent(QPaintEvent *);
    void changeEvent(QEvent *event);

private:
    Internal::PatientBarPrivate *d;
    static PatientBar *m_Instance;
};

}  // End namespace Patients

#endif // PATIENTBAR_H
