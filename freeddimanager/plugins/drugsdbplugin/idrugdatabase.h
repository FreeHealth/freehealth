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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric Maeker
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DDIMANAGER_DRUGSDB_IDrugDatabase_H
#define DDIMANAGER_DRUGSDB_IDrugDatabase_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QHash>
#include <QMultiHash>
#include <QStringList>
#include <QLocale>

/**
 * \file idrugdatabase.h
 * \author Eric Maeker
 * \version 0.10.0
 * \date 16 Mar 2014
*/

namespace DrugsDB {
namespace Internal {
class DrugBaseEssentials;
}
}

namespace DrugsDb {
class Drug;
namespace Internal {
class DrugDatabasePopulator;

struct SpcContentResources {
    QString type, name, content;
};

struct SpcContent {
    SpcContent() : did(-1) {}

    int did; // DrugIdentifiant
    QString html, label, url;
    QList<SpcContentResources> resources;
};

class IDrugDatabase : public QObject // public Core::IFullReleaseStep
{
    Q_OBJECT
public:
    enum ProcessTiming {
        PreProcess = 0,
        Process,
        PostProcess
    };

    enum SubProcess {
        Initialization = 0,
        Main,
        DataPackSubProcess,
        Final
    };

    enum ServerOwner {
        Community = 0,
        FrenchAssociation
    };

    enum LicenseType {
        Free = 0,
        NonFree
    };

    explicit IDrugDatabase(QObject *parent = 0);
    ~IDrugDatabase();

    virtual void setDisplayName(const QString &name) {_name=name;}
    virtual void setLicenseType(LicenseType type) {_licenseType=type;}
    virtual void setServerOwner(ServerOwner serverOwner) {_serverOwner=serverOwner;}
    void setDatabaseUid(const QString &dbUid1, const QString dbUid2 = QString::null) {_dbUid1 = dbUid1; _dbUid2 = dbUid2;}
    void setTempPath(const QString &absPath);
    void setOutputPath(const QString &absPath);
    void setConnectionName(const QString &connection);
    void setOutputFileName(const QString &fileName);
    void setDownloadUrl(const QString &url);
    void setManualSourceDownload(const bool state) {_manualSourceDownload = state;}
    void setFinalizationScript(const QString &absPath);
    void setDatabaseDescriptionFile(const QString &absPath);
    void setDatapackDescriptionFile(const QString &absPath);
    void setSpcHtmlFilesDefaultEncoding(const QString &encoding) {_spcDefaultEncoding=encoding;}
    void setDatabaseLanguage(const QString &lang) {_lang = lang;}
    void setCountry(const QLocale::Country &country) {_country = country;} // Use anyCountries for multicountry database

    virtual QString displayName() const {return _name;}
    virtual LicenseType licenseType() const {return _licenseType;}
    virtual ServerOwner serverOwner() const {return _serverOwner;}
    QString databaseUid1() const {return _dbUid1;}
    QString databaseUid2() const {return _dbUid2;}
    QString tempPath() const {return _tempPath;}
    QString outputPath() const {return _outputPath;}
    QString connectionName() const {return _connection;}
    QString outputFileName() const {return _outputFileName;}
    QString absoluteFilePath() const;
    QString downloadUrl() const {return _downloadingUrl;}
    QString finalizationScript() const {return _finalizationScriptPath;}
    QString databaseDescriptionFilePath() const {return _descriptionFilePath;}
    QString datapackDescriptionFilePath() const {return _datapackDescriptionFilePath;}
    QString spcHtmlFilesDefaultEncoding() const {return _spcDefaultEncoding;}
    QString databaseLanguage() const {return _lang;}
    QLocale::Country country() const {return _country;}
    int sourceId() const;
    virtual QString serverUid() const;

    virtual bool checkDatabase();
    virtual bool canCreateDatabase() const;
    virtual bool createDatabase();
    DrugsDB::Internal::DrugBaseEssentials *drugEssentialDatabase() const {return _database;}

    bool addRoutes();
    bool recreateRoutes();
    bool saveDrugDatabaseDescription();
    bool updateDatabaseCompletion(int completion);
    bool saveDrugsIntoDatabase(QVector<Drug *> drugs);
    QHash<int, QString> saveMoleculeIds(const QStringList &molnames);

    bool addAtc();
    bool addDrugDrugInteractions();
    bool addPims();
    bool addPregnancyCheckingData();

    virtual bool downloadSpcContents();

    virtual bool createTemporaryStorage();
    virtual bool cleanTemporaryStorage();

    virtual bool startProcessing(ProcessTiming timing, SubProcess subProcess);

    // Adding specific interface for the UI <-> step connection
    virtual bool startDownload();
    virtual bool unzipFiles();
    virtual bool registerDataPack();
    virtual bool prepareData() = 0;
    virtual bool populateDatabase() = 0; // Cannot be asynchronous
    virtual bool linkDrugsComponentsAndDrugInteractors();  // Cannot be asynchronous

    bool saveDrugSpc(const SpcContent &content);

    // Reporting
    QStringList finalReport() const;
    void clearFinalReport();
    void showReportDialog() const;

private Q_SLOTS:
    void onSubProcessFinished();
    bool onAllSpcDownloadFinished();

Q_SIGNALS:
    void spcProcessFinished();
    void subProcessFinished(ProcessTiming timing, SubProcess subProcess);

    void downloadFinished();
    void postDownloadProcessingFinished();
    void processFinished();

    // Progress dialog management when threaded
    void progressLabelChanged(const QString &label);
    void progress(int value);
    void progressRangeChanged(int min, int max);

protected:
    void addFinalReportMessage(const QString &msg);

    DrugsDB::Internal::DrugBaseEssentials *createDrugDatabase(const QString &absPath, const QString &connection);
    // DrugsDB::Internal::DrugBaseEssentials *drugsBaseFromCache(const QString &connection);

protected:
    DrugsDB::Internal::DrugBaseEssentials *_database;

private:
    LicenseType _licenseType;
    ServerOwner _serverOwner;
    QString _name, _tempPath, _outputPath, _connection, _outputFileName, _downloadingUrl;
    QString _finalizationScriptPath, _descriptionFilePath, _datapackDescriptionFilePath;
    QString _spcDefaultEncoding, _dbUid1, _dbUid2;
    QString _lang;
    int _sid;
    QList<QUrl> _spcUrls;
    ProcessTiming _currentTiming;
    SubProcess _currentSubProcess;
    DrugDatabasePopulator *_databasePopulator;
    QStringList _finalReport;
    QLocale::Country _country;
    bool _manualSourceDownload;
};

}  // namespace Internal
}  // namespace DrugsDb

#endif  // DDIMANAGER_DRUGSDB_IDrugDatabase_H

