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
#ifndef PATIENTS_PATIENTMODEL_H
#define PATIENTS_PATIENTMODEL_H

#include <patientbaseplugin/patientbase_exporter.h>

#include <QAbstractTableModel>
QT_BEGIN_NAMESPACE
class QSqlRecord;
QT_END_NAMESPACE

/**
 * \file patientmodel.h
 * \author Eric MAEKER
 * \version 0.8.2
 * \date 04 Feb 2013
*/

namespace Patients {
class PatientCore;
namespace Internal {
class PatientModelPrivate;
}  // End namespace Internal

class PATIENT_EXPORT PatientModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class Internal::PatientModelPrivate;
    friend class Patients::PatientCore;

public:
    // Column data representation is stored in Core::IPatient

    enum FilterOn {
        FilterOnName,
        FilterOnFullName,
        FilterOnCity,
        FilterOnUuid
    };

    PatientModel(QObject *parent);
    ~PatientModel();

    QModelIndex currentPatient() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void setFilter(const QString &name, const QString &firstname, const QString &uuid = QString::null, const FilterOn on = FilterOnFullName);
    QString filter() const;
    int numberOfFilteredPatients() const;

    void emitPatientCreationOnSubmit(bool state);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;

    static QHash<QString, QString> patientName(const QList<QString> &uuids);

    QString patientUuid(const QModelIndex &index) const;

public Q_SLOTS:
    bool submit();
    bool refreshModel();

Q_SIGNALS:
    void patientAboutToChange(const QString &newUuid);
    void patientAboutToBeDeleted(const QString &uuid);
    void patientAboutToBeCreated(const QString &uuid);

    void currentPatientChanged(const QModelIndex &index);
    void currentPatientChanged(const QString &uuid);
    void patientDeleted(const QString &uuid);
    void patientCreated(const QString &uuid);

protected:
    bool beginChangeCurrentPatient();
    bool setCurrentPatient(const QModelIndex &index);
    void endChangeCurrentPatient();

private Q_SLOTS:
//    void onPrimeInsert(int row, QSqlRecord &record);
    void onCoreDatabaseServerChanged();

private:
    Internal::PatientModelPrivate *d;
};

}  // End namespace Patients

#endif // PATIENTS_PATIENTMODEL_H
