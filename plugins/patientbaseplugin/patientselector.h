/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program (COPYING file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTBASE_PATIENTSELECTOR_H
#define PATIENTBASE_PATIENTSELECTOR_H

#include <patientbaseplugin/patientbase_exporter.h>

#include <QWidget>
#include <QModelIndex>

/**
 * \file patientselector.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 23 May 2013
*/

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
    enum RefreshSearchResult {
        WhileTyping = 0,
        ReturnPress
    };

    enum SearchMethod {
        SearchByName = 0,
        SearchByFirstname,
        SearchByNameFirstname,
        SearchByDOB
    };

    enum FieldToShow {
        None        = 0x0000,
        UsualName   = 0x0001,
        OtherNames  = 0x0002,
        FirstName   = 0x0004,
        FullName    = 0x0008,
        Gender      = 0x0010,
        FullAddress = 0x0020,
        Street      = 0x0040,
        ZipCode     = 0x0080,
        City        = 0x0100,
        Country     = 0x0200,
        Mails       = 0x0400,
        Tels        = 0x0800,
        Faxes       = 0x1000,
        Title       = 0x2000,
        DateOfBirth = 0x4000,
        Default     = FullName | Gender | FullAddress | DateOfBirth
    };
    Q_DECLARE_FLAGS(FieldsToShow, FieldToShow)

    PatientSelector(QWidget *parent = 0, const FieldsToShow fields = None);
    ~PatientSelector();

    void initialize();

    void setSearchMode(const int search);
    void setPatientModel(PatientModel *m);
    void setFieldsToShow(const FieldsToShow fields);
    void setRefreshSearchResultMethod(RefreshSearchResult method);
    void setOnPatientActivatedSetAsCurrent(bool setAsCurrent);

    QString selectedPatientUid() const;

public Q_SLOTS:
    bool setFilterPatientModel(const QString &name, const QString &firstName, const QDate &dateOfBirth) const;
    void setSelectedPatient(const QModelIndex &index);
    void setSelectedPatient(int row);
    void updatePatientActions(const QModelIndex &index);

private Q_SLOTS:
    void updateNavigationButton();
    void changeIdentity(const QModelIndex &current, const QModelIndex &previous);
    void refreshFilter();
    void onPatientActivated(const QModelIndex &index);
    void onPatientDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void onUserChanged();

protected:
    bool event(QEvent *event);

private:
    Internal::PatientSelectorPrivate *d;
};

}  // End namespace Patients

Q_DECLARE_OPERATORS_FOR_FLAGS(Patients::PatientSelector::FieldsToShow)

#endif // PATIENTBASE_PATIENTSELECTOR_H
