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
#ifndef PATIENTWIDGETMANAGER_H
#define PATIENTWIDGETMANAGER_H

#include <coreplugin/contextmanager/icontext.h>

#include <patientbaseplugin/patientbase_exporter.h>
#include <patientbaseplugin/patientselector.h>

#include <QWidget>
#include <QObject>
#include <QAction>
#include <QPointer>

/**
 * \file patientwidgetmanager.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.0
 * \date 05 Dec 2012
*/

namespace Patients {
class PatientCore;
namespace Internal {
class PatientContext : public Core::IContext
{
public:
    PatientContext(QWidget *w) : Core::IContext(w)
    {
        setObjectName("PatientContext");
        setWidget(w);
    }
};

class PatientActionHandler : public QObject
{
    Q_OBJECT
public:
    PatientActionHandler(QObject *parent = 0);
    virtual ~PatientActionHandler() {}

    void setCurrentView(PatientSelector *view);

private Q_SLOTS:
    void searchActionChanged(QAction *action);
    void viewPatientInformation();
    void printPatientsInformation();

    void showPatientDatabaseInformation();

private:
    void updateActions();

protected:
    QAction *aSearchName;
    QAction *aSearchFirstname;
    QAction *aSearchNameFirstname;
    QAction *aSearchDob;
    QAction *aViewPatientInformation;
    QAction *aPrintPatientInformation;
    QAction *aShowPatientDatabaseInformation;
    QActionGroup *gSearchMethod;
    // setDeceased
    // writeALetter...
    QPointer<PatientSelector> m_CurrentView;
};

class PatientWidgetManager : public Internal::PatientActionHandler
{
    Q_OBJECT
    friend class Patients::PatientCore;

protected:
    PatientWidgetManager(QObject *parent = 0);
    void postCoreInitialization();

public:
    ~PatientWidgetManager() {}

    PatientSelector *selector() const;

private Q_SLOTS:
    void updateContext(Core::IContext *object, const Core::Context &additionalContexts);
};

}  // namespace Internal
}  // namespace Patients

#endif // PATIENTWIDGETMANAGER_H
