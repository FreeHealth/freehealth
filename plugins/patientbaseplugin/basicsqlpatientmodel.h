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
 *   Main developers : Eric Maeker
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef PATIENTS_INTERNAL_BASICSQLPATIENTMODEL_H
#define PATIENTS_INTERNAL_BASICSQLPATIENTMODEL_H

#include <QAbstractListModel>

/**
 * \file basicsqlpatientmodel.h
 * \author Eric Maeker
 * \version 0.8.0
 * \date 04 Dec 2012
*/

namespace Patients {
class PatientCore;
namespace Internal {
class BasicSqlPatientModelPrivate;

class BasicSqlPatientFilter
{
public:
    BasicSqlPatientFilter();

    bool isNull() const {return _birthName.isEmpty() && _secondName.isEmpty() && _firstName.isEmpty();}

    void setBirthName(const QString &name) {_birthName = name;}
    QString birthName() const {return _birthName;}

    void setSecondName(const QString &name) {_secondName = name;}
    QString secondName() const {return _secondName;}

    void setFirstName(const QString &name) {_firstName = name;}
    QString firstName() const {return _firstName;}

private:
    QString _birthName, _secondName, _firstName;
};

class BasicSqlPatientModel : public QAbstractListModel
{
    Q_OBJECT
    friend class Patients::PatientCore;

protected:
    explicit BasicSqlPatientModel(QObject *parent = 0);
    bool initialize();

public:
    ~BasicSqlPatientModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &) const;
    int numberOfFilteredPatients() const;

    void fetchMore(const QModelIndex &parent);
    bool canFetchMore(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    bool setFilter(const BasicSqlPatientFilter &filter);

private Q_SLOTS:
    void onCoreDatabaseServerChanged();
    
private:
    Internal::BasicSqlPatientModelPrivate *d;
};

} // namespace Internal
} // namespace Patients

#endif // PATIENTS_INTERNAL_BASICSQLPATIENTMODEL_H

