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
#ifndef PATIENTBASE_H
#define PATIENTBASE_H

#include <utils/database.h>

#include <coreplugin/isettings.h>

#include <patientbaseplugin/patientbase_exporter.h>

#include <QObject>
#include <QString>
#include <QDate>
#include <QUuid>

/**
 * \file patientbase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.8.4
 * \date 08 Mar 2013
*/

namespace Patients {
class PatientCore;
namespace Internal {
class PatientBasePrivate;
class PatientData;

class PATIENT_EXPORT PatientBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class Patients::PatientCore;

protected:
    PatientBase(QObject *parent = 0);
    bool initialize();

public:
    // Constructor
    static PatientBase *instance();
    virtual ~PatientBase();

//    // initialize
//    bool initialize(Core::ISettings *settings);
    bool isInitialized() const {return m_initialized;}

    bool createVirtualPatient(const QString &usualName,
                              const QString &otherNames,
                              const QString &firstname,
                              const QString &gender,
                              const int title,
                              const QDate &dob,
                              const QString &country,
                              const QString &note,
                              const QString &street,
                              const QString &zip,
                              const QString &city,
                              QString uuid,
                              const int lkid,
                              const QString &mobilePhone = QString(),
                              const QString &workPhone = QString(),
                              const QString &photoFile = QString(),
                              const QDate &death = QDate(),
                              const QUuid &userUuid = QUuid());

    QString patientUuid(const QString &usualname,
                        const QString &othernames,
                        const QString &firstname,
                        const QString &gender,
                        const QDate &dob) const;
    bool isPatientExists(const QString &usualname,
                         const QString &othernames,
                         const QString &firstname,
                         const QString &gender,
                         const QDate &dob) const;

    bool setPatientActiveProperty(const QString &uuid, bool active);

    void toTreeWidget(QTreeWidget *tree) const;

    bool updateDatabase();
    quint32 getSchemaVersionNumber() const;
    bool executeQueryFile(QFile &file, QSqlDatabase &db) const;
    QString getOldVersionField() const;
    bool transferPhone();
    bool insertMobilePhone(QHash<QString, QString> mP);
    bool insertWorkPhone(QHash<QString, QString> wP);
private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                          const QString &pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString &login, const QString &pass,
                          const int port,
                          CreationOption createOption
                         );

private Q_SLOTS:
    void onCoreDatabaseServerChanged();
    void onCoreFirstRunCreationRequested();

private:
    bool m_initialized;
    static PatientBase *m_Instance;
};

}  // End namespace Internal
}  // End namespace Patients

#endif // PATIENTBASE_H
