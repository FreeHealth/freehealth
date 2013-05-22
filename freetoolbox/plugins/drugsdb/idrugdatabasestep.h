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
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef DRUGSDB_IDRUGDATABASESTEP_H
#define DRUGSDB_IDRUGDATABASESTEP_H

#include <coreplugin/ifullreleasestep.h>
#include <QHash>

/**
 * \file idrugdatabasestep.h
 * \author Eric Maeker
 * \version 0.8.4
 * \date 17 May 2013
*/

namespace DrugsDB {
class Drug;
namespace Internal {
class DrugBaseEssentials;

struct SpcContentResources {
    QString type, name, content;
};

struct SpcContent {
    SpcContent() : did(-1) {}

    int did; // DrugIdentifiant
    QString html, label, url;
    QList<SpcContentResources> resources;
};

class IDrugDatabaseStep : public Core::IFullReleaseStep
{
    Q_OBJECT
public:
    enum ServerOwner {
        Community = 0,
        FrenchAssociation
    };

    enum LicenseType {
        Free = 0,
        NonFree
    };

    explicit IDrugDatabaseStep(QObject *parent = 0);
    ~IDrugDatabaseStep();

    virtual void setDisplayName(const QString &name) {_name=name;}
    virtual void setLicenseType(LicenseType type) {_licenseType=type;}
    virtual void setServerOwner(ServerOwner serverOwner) {_serverOwner=serverOwner;}
    void setTempPath(const QString &absPath);
    void setOutputPath(const QString &absPath);
    void setConnectionName(const QString &connection);
    void setOutputFileName(const QString &fileName);
    void setDownloadUrl(const QString &url);
    void setFinalizationScript(const QString &absPath);
    void setDatabaseDescriptionFile(const QString &absPath);
    void setDatapackDescriptionFile(const QString &absPath);
    void setSpcHtmlFilesDefaultEncoding(const QString &encoding) {_spcDefaultEncoding=encoding;}

    virtual QString displayName() const {return _name;}
    virtual LicenseType licenseType() const {return _licenseType;}
    virtual ServerOwner serverOwner() const {return _serverOwner;}
    QString tempPath() const {return _tempPath;}
    QString outputPath() const {return _outputPath;}
    QString connectionName() const {return _connection;}
    QString outputFileName() const {return _outputFileName;}
    QString absoluteFilePath() const;
    QString downloadUrl() const {return _downloadingUrl;}
    QString finalizationScript() const {return _finalizationScriptPath;}
    QString databaseDescriptionFile() const {return _descriptionFilePath;}
    QString datapackDescriptionFile() const {return _datapackDescriptionFilePath;}
    QString spcHtmlFilesDefaultEncoding() const {return _spcDefaultEncoding;}
    int sourceId() const;

    virtual bool checkDatabase();
    virtual bool canCreateDatabase() const;
    virtual bool createDatabase();
    DrugBaseEssentials *drugEssentialDatabase() const {return _database;}

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

    // Core::IFullReleaseStep interface
    Steps stepNumber() const {return Core::IFullReleaseStep::DrugsDatabase;}
    virtual bool createTemporaryStorage();
    virtual bool cleanTemporaryStorage();
    bool startProcessing(ProcessTiming timing, SubProcess subProcess);

    // Adding specific interface for the UI <-> step connection
    virtual bool startDownload();
    virtual bool unzipFiles();
    virtual bool registerDataPack();
    virtual bool prepareData() = 0;
    virtual bool populateDatabase() = 0; // Cannot be asynchronous
    virtual bool linkMolecules() = 0;  // Cannot be asynchronous

    bool saveDrugSpc(const SpcContent &content);

private Q_SLOTS:
    void onSubProcessFinished();
    bool onAllSpcDownloadFinished();

Q_SIGNALS:
    void spcProcessFinished();

protected:
    DrugBaseEssentials *_database;

private:
    LicenseType _licenseType;
    ServerOwner _serverOwner;
    QString _name, _tempPath, _outputPath, _connection, _outputFileName, _downloadingUrl;
    QString _finalizationScriptPath, _descriptionFilePath, _datapackDescriptionFilePath;
    QString _spcDefaultEncoding;
    int _sid;
    QList<QUrl> _spcUrls;
    ProcessTiming _currentTiming;
    SubProcess _currentSubProcess;
};

}  // namespace Internal
}  // namespace DrugsDB

#endif  // DRUGSDB_IDRUGDATABASESTEP_H

