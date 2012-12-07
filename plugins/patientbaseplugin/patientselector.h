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
#ifndef PATIENTSELECTOR_H
#define PATIENTSELECTOR_H

#include <patientbaseplugin/patientbase_exporter.h>

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
    Q_DECLARE_FLAGS(FieldsToShow, FieldToShow)

    PatientSelector(QWidget *parent = 0, const FieldsToShow fields = None);
    ~PatientSelector();

    void initialize();

    void setSearchMode(const int search);
    void setPatientModel(PatientModel *m);
    void setFieldsToShow(const FieldsToShow fields);
    void setRefreshSearchResultMethod(RefreshSearchResult method);

public Q_SLOTS:
    void setSelectedPatient(const QModelIndex &index);
    void updatePatientActions(const QModelIndex &index);

//Q_SIGNALS:
//    void patientSelected(const QString &uuid);
//    void patientSelected(const QModelIndex &index);

private Q_SLOTS:
    void updateNavigationButton();
    void changeIdentity(const QModelIndex &current, const QModelIndex &previous);
    void refreshFilter();
    void onPatientActivated(const QModelIndex &index);
    void onUserChanged();

protected:
//    void changeEvent(QEvent *e);
    bool event(QEvent *event);

private:
    Internal::PatientSelectorPrivate *d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(PatientSelector::FieldsToShow)

}  // End namespace Patients


#endif // PATIENTSELECTOR_H
