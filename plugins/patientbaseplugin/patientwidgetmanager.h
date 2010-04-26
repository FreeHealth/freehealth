/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
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
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 23 Apr 2010
 * \internal
*/

namespace Patients {
namespace Internal {
class PatientContext : public Core::IContext
{
public:
    PatientContext(QWidget *w) : Core::IContext(w), wgt(w) {}
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
    void viewPatientInformations();
    void printPatientsInformations();

    void showPatientDatabaseInformations();

private:
    void updateActions();

protected:
    QAction *aViewPatientInformations;
    QAction *aPrintPatientInformations;
    QAction *aShowPatientDatabaseInformations;

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

    PatientSelector  *selector() const;

private Q_SLOTS:
    void updateContext(Core::IContext *object);

private:
    PatientWidgetManager(QObject *parent = 0);
    static PatientWidgetManager *m_Instance;
};

}  // End namespace Patients

#endif // PATIENTWIDGETMANAGER_H
