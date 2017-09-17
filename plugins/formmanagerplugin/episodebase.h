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
#ifndef FORMMANAGER_INTERNAL_EPISODEBASE_H
#define FORMMANAGER_INTERNAL_EPISODEBASE_H

#include <coreplugin/isettings.h>
#include <formmanagerplugin/formmanager_exporter.h> // Exporter needed by MainWindow::VirtualDatabasePreferences
#include <utils/database.h>

#include <QObject>
#include <QDate>
#include <QVariant>

/**
 * \file episodebase.h
 * \author Eric Maeker
 * \version 0.9.0
 * \date 02 Sept 2013
*/

namespace Form {
class FormIODescription;
class SubFormInsertionPoint;
class FormCore;

namespace Internal {
class FormManagerPlugin;
class EpisodeBasePrivate;
class EpisodeData;
class EpisodeValidationData;

class EpisodeBaseQuery
{
public:
    EpisodeBaseQuery() :
        m_Valid(true), m_Deleted(false),
        m_FilterValidated(false), m_Validated(true), m_NonValidated(true),
        m_Limit(false), m_Start(0), m_End(-1),
        m_UseUserDateRange(false),
        m_GetContent(false)
    {}

    ~EpisodeBaseQuery() {}

    void setPatientUid(const QVariant &patientUid) {m_PatientUid= patientUid;}
    void setUserUid(const QVariant &userUid) {m_UserUid = userUid;}

    void setValidEpisodes(bool valid) {m_Valid=valid;}
    void setDeletedEpisodes(bool del) {m_Deleted=del;}

    void seFilterValidated(bool v) {m_FilterValidated=v;}
    void setValidatedEpisodes(bool v) {m_Validated=v;}
    void setNonValidated(bool v) {m_NonValidated=v;}

    void setUserDateRange(const QDate &start, const QDate &end) {m_UseUserDateRange=true; m_UserDateStart=start; m_UserDateEnd=end;}

    void setFormUids(const QStringList &list) {m_FormUids=list;}

    void setGetEpisodeContent(const bool get) {m_GetContent=get;}

    void setLimitRange(int start, int end) {m_Start=start; m_End=end; m_Limit=true;}

    QVariant patientUid() const {return m_PatientUid;}
    QVariant userUid() const {return m_UserUid;}

    bool validEpisodes() const {return m_Valid;}
    bool deletedEpisodes() const {return m_Deleted;}

    bool filterValidated() const {return m_FilterValidated;}
    bool validatedEpisodes() const {return m_Validated;}
    bool nonValidatedEpisodes() const {return m_NonValidated;}

    bool useUserDateRange() const {return m_UseUserDateRange;}
    QDate userDateStart() const {return m_UserDateStart;}
    QDate userDateEnd() const {return m_UserDateEnd;}

    QStringList formUids() const {return m_FormUids;}

    bool getEpisodeContent() const {return m_GetContent;}

    bool useLimit() const {return m_Limit;}
    int limitStart() const {return m_Start;}
    int limitEnd() const {return m_End;}

private:
    QVariant m_UserUid, m_PatientUid;
    QStringList m_FormUids;
    bool m_Valid, m_Deleted, m_FilterValidated, m_Validated, m_NonValidated, m_Limit;
    int m_Start, m_End;
    bool m_UseUserDateRange, m_GetContent;
    QDate m_UserDateStart, m_UserDateEnd;
};


class FORM_EXPORT EpisodeBase : public QObject, public Utils::Database
{
    Q_OBJECT
    friend class Form::Internal::FormManagerPlugin;
    friend class Form::FormCore;

protected:
    EpisodeBase(QObject *parent = 0);
    bool initialize();

public:
    // Constructor
    static EpisodeBase *instance();
    virtual ~EpisodeBase();

    // initialize
    bool isInitialized() const {return m_initialized;}

    // Forms
    bool setGenericPatientFormFile(const QString &absPathOrUid);
    QString getGenericFormFile();
    QVector<SubFormInsertionPoint> getSubFormFiles();
    bool addSubForms(const QVector<SubFormInsertionPoint> &insertions);

    // Episodes
    bool saveEpisode(EpisodeData *episode);
    bool saveEpisode(const QList<EpisodeData *> &episode);
    bool saveEpisodeContent(const QVariant &uid, const QString &xml);
    bool saveEpisodeValidation(EpisodeValidationData *validation);
    bool removeEpisode(const QVariant &uid);
    bool removeAllEpisodeForForm(const QVariant &formUid, const QString &patientUid);

    QList<EpisodeData *> getEpisodes(const EpisodeBaseQuery &query);
    bool getEpisodeContent(EpisodeData *episode);
    int getNumberOfEpisodes(const QString &formUid, const QStringList &equivalents);
    QString getEpisodeContent(const QVariant &uid);
    QList<EpisodeValidationData *> getEpisodeValidations(const QVariant &uid);

private:
    bool createDatabase(const QString &connectionName, const QString &dbName,
                          const QString &pathOrHostName,
                          TypeOfAccess access, AvailableDrivers driver,
                          const QString &login, const QString &pass,
                          const int port,
                          CreationOption createOption
                         );
    QString getOldVersionField() const;
    int currentDatabaseVersion() const;
    bool updateDatabase();
    void populateWithDefaultValues();
    /*bool checkDatabaseVersion();*/

    // Episodes
    bool saveEpisodeValidations(Internal::EpisodeData *episode);
    bool saveEpisodeModifications(Internal::EpisodeData *episode);

public:
    void toTreeWidget(QTreeWidget *tree) const;

private Q_SLOTS:
    void onCoreDatabaseServerChanged();
    void onCoreFirstRunCreationRequested();

private:
    bool m_initialized;
    static EpisodeBase *m_Instance;
};

}  // End namespace Internal
}  // End namespace Form

#endif // FORMMANAGER_INTERNAL_EPISODEBASE_H
