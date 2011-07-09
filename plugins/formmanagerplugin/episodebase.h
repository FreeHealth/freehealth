/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#ifndef EPISODEBASE_H
#define EPISODEBASE_H

#include <utils/database.h>

#include <coreplugin/isettings.h>

#include <formmanagerplugin/formmanager_exporter.h>

#include <QObject>

/**
 * \file episodebase.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.5.0
 * \date 17 Mar 2011
*/

namespace Form {
class FormIODescription;
class SubFormInsertionPoint;

namespace Internal {
class EpisodeBasePrivate;

class FORM_EXPORT EpisodeBase : public QObject, public Utils::Database
{
    Q_OBJECT
protected:
    EpisodeBase(QObject *parent = 0);
    bool init();

public:
    // Constructor
    static EpisodeBase *instance();
    virtual ~EpisodeBase();

    // initialize
    bool isInitialized() const {return m_initialized;}

    bool setGenericPatientFormFile(const QString &absPathOrUid);
    QString getGenericFormFile();
    QVector<SubFormInsertionPoint> getSubFormFiles();
    bool addSubForms(const QVector<SubFormInsertionPoint> &insertions);

private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                          const QString &pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString &login, const QString &pass,
                          const int port,
                          CreationOption createOption
                         );
    void populateWithDefaultValues();

public:
    void toTreeWidget(QTreeWidget *tree);

private Q_SLOTS:
    void onCoreDatabaseServerChanged();

private:
    static bool m_initialized;
    static EpisodeBase *m_Instance;
};

}  // End namespace Internal
}  // End namespace Form

#endif // PATIENTBASE_H
