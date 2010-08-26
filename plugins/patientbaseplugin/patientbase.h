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
 ***************************************************************************/
#ifndef PATIENTBASE_H
#define PATIENTBASE_H

#include <utils/database.h>

#include <coreplugin/isettings.h>

#include <patientbaseplugin/patientbase_exporter.h>

#include <QObject>

/**
 * \file patientbase.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.4.0
 * \date 18 Apr 2010
*/

/** \todo create a sqlite db in memory to improve speed ? */

namespace Patients {
namespace Internal {
class PatientBasePrivate;
class PatientData;

class PATIENT_EXPORT PatientBase : public QObject, public Utils::Database
{
    Q_OBJECT
protected:
    PatientBase(QObject *parent = 0);
    bool init();

public:
    // Constructor
    static PatientBase *instance();
    virtual ~PatientBase();

//    // initialize
    bool initialize(Core::ISettings *settings);
    bool isInitialized() const {return m_initialized;}

//    bool isNewlyCreated() const;
//    bool isEmpty() const;
//
//    // retreivers (pointers)
//    PatientData *getPatientByUuid(const QString &uuid) const;
//
//    // datas retrievers
//    QString getUuid(const QString &name, const QString &firstname);
//    QString createNewPatient();
//
//    // savers
//    bool     savePatient(PatientData *patient);
//    bool     deletePatient(const QString &uuid);
//
//private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                          const QString &pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString &login, const QString &pass,
                          const int port,
                          CreationOption createOption
                         );

//    // privates retreivers
//    PatientData *getPatient(const QHash<int, QString> &conditions) const;

    void toTreeWidget(QTreeWidget *tree);

private Q_SLOTS:
    void onCoreDatabaseServerChanged();

private:
    static bool m_initialized;
    static PatientBase *m_Instance;
//    QList<PatientData *> m_OrderedHistory;
//    bool m_IsNewlyCreated;
};

}  // End namespace Internal
}  // End namespace Patients

#endif // PATIENTBASE_H
