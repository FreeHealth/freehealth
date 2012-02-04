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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
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
 * \version 0.4.0
 * \date 23 Apr 2010
 * \internal
*/

namespace Patients {
namespace Internal {
class PatientContext : public Core::IContext
{
public:
    PatientContext(QWidget *w) : Core::IContext(w), wgt(w) { setObjectName("PatientContext"); }
    void setContext(QList<int> c) { ctx = c; }
    QList<int> context() const { return ctx; }
    QWidget *widget() { return wgt; }
private:
    QWidget *wgt;
    QList<int> ctx;
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
    void viewPatientInformations();
    void printPatientsInformations();

    void showPatientDatabaseInformations();

private:
    void updateActions();

protected:
    QAction *aSearchName;
    QAction *aSearchFirstname;
    QAction *aSearchNameFirstname;
    QAction *aSearchDob;
    QAction *aViewPatientInformations;
    QAction *aPrintPatientInformations;
    QAction *aShowPatientDatabaseInformations;

    QActionGroup *gSearchMethod;

    // setDeceased
    // writeALetter...

    QPointer<PatientSelector> m_CurrentView;

};

}  // End Internal



class PATIENT_EXPORT PatientWidgetManager : public Internal::PatientActionHandler
{
    Q_OBJECT
public:
    static PatientWidgetManager *instance();
    ~PatientWidgetManager() {}

    void postCoreInitialization();

    PatientSelector  *selector() const;

private Q_SLOTS:
    void updateContext(Core::IContext *object);

private:
    PatientWidgetManager(QObject *parent = 0);
    static PatientWidgetManager *m_Instance;
};

}  // End namespace Patients

#endif // PATIENTWIDGETMANAGER_H
