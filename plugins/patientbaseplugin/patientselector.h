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
#ifndef PATIENTSELECTOR_H
#define PATIENTSELECTOR_H

#include <patientbaseplugin/patientbase_exporter.h>
#include <coreplugin/modemanager/basemode.h>

#include <QWidget>
#include <QModelIndex>

namespace Patients {
class PatientModel;

namespace Internal {
class PatientSelectorPrivate;
}  // End namespace Internal

class PATIENT_EXPORT PatientSelector : public QWidget
{
    Q_OBJECT
    friend class Internal::PatientSelectorPrivate;

public:
    enum SearchMethod {
        SearchByName = 0,
        SearchByFirstname,
        SearchByNameFirstname,
        SearchByDOB
    };

    enum FieldToShow {
        None        = 0x0000,
        BirthName   = 0x0001,
        SecondName  = 0x0002,
        FirstName   = 0x0004,
        FullName    = 0x0008,
        Gender      = 0x0010,
        FullAdress  = 0x0020,
        Street      = 0x0040,
        ZipCode     = 0x0080,
        City        = 0x0100,
        Country     = 0x0200,
        Mails       = 0x0400,
        Tels        = 0x0800,
        Faxes       = 0x1000,
        Title       = 0x2000,
        DateOfBirth = 0x4000,
        Default     = Title | FullName | Gender | FullAdress
    };
    Q_DECLARE_FLAGS(FieldsToShow, FieldToShow);


    PatientSelector(QWidget *parent = 0, const FieldsToShow fields = None);
    ~PatientSelector();

    void init();

    void setSearchMode(const int search);
    void setPatientModel(PatientModel *m);
    void setFieldsToShow(const FieldsToShow fields);
    void setSelectedPatient(const QModelIndex &index);


Q_SIGNALS:
    void patientSelected(const QString &uuid);
    void patientSelected(const QModelIndex &index);

private Q_SLOTS:
    void changeIdentity(const QModelIndex &current, const QModelIndex &previous);
    void refreshFilter(const QString &);

protected:
    void changeEvent(QEvent *e);

private:
    Internal::PatientSelectorPrivate *d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(PatientSelector::FieldsToShow)

}  // End namespace Patients


#endif // PATIENTSELECTOR_H
