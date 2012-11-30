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
 *   Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>               *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "frenchzipcodespage.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>

#include <datapackplugin/datapackcore.h>
#include <datapackplugin/datapackquery.h>

#include <utils/global.h>
#include <utils/log.h>
#include <utils/database.h>
#include <utils/httpdownloader.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <quazip/global.h>

#include <extensionsystem/pluginmanager.h>

#include <QFile>
#include <QDir>
#include <QUrl>

#include "ui_frenchzipcodeswidget.h"

// Data are extracted from URL: http://www.galichon.com/codesgeo/

using namespace ZipCodes;
using namespace Trans::ConstantTranslations;

namespace {

const char* const  DB_NAME     = "ZIPCODES";
const char* const  FRENCH_URL  = "http://www.galichon.com/codesgeo/data/insee.zip";

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline DataPackPlugin::DataPackCore *dataPackCore() {return DataPackPlugin::DataPackCore::instance();}

static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/zipcodes/zipcodes.db");}
static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/ZipCodes/") + QDir::separator();}

static inline QString csvFileAbsPath()         {return workingPath() + "/insee.csv";}
static inline QString sqlMasterFileAbsPath()   {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/zipcodes/zipcodes.sql");}
static inline QString sqlImportFileAbsPath()   {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/zipcodes/zipcodes-fr-import.sql");}
static inline QString datapackDescriptionFile(){return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/textfiles/datapack_description/zipcodes/nonfree_french/packdescription.xml");}

}

// #################### FrenchZipCodesPage #######################

FrenchZipCodesPage::FrenchZipCodesPage(QObject *parent) :
        IToolPage(parent)
{
    setObjectName("FrenchZipCodesPage_IToolPage");
}

QWidget *FrenchZipCodesPage::createPage(QWidget *parent)
{
    return new FrenchZipCodesWidget(parent);
}


// #################### FrenchZipCodesStep #######################

FrenchZipCodesStep::FrenchZipCodesStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    m_WithProgress(false)
{
    setObjectName("FrenchZipCodesStep");
    createTemporaryStorage();
}

FrenchZipCodesStep::~FrenchZipCodesStep()
{
}

bool FrenchZipCodesStep::createTemporaryStorage()
{
    // Create the wortking path
    if (!QDir().mkpath(workingPath()))
        LOG_ERROR("Unable to create ZipCodes Working Path: " + workingPath());
    else
        LOG("Tmp dir created");

    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath))
            LOG_ERROR("Unable to create ZipCodes database output path: " + dbpath);
        else
            LOG("ZipCodes database output dir created");
    }
    return true;
}

bool FrenchZipCodesStep::cleanTemporaryStorage()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool FrenchZipCodesStep::startDownload()
{
    // TODO: manage progress download */
    Utils::HttpDownloader *dld = new Utils::HttpDownloader(this);
    dld->setOutputPath(workingPath());
    dld->setUrl(QUrl(FRENCH_URL));
    connect(dld, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()));
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    dld->startDownload();
    return true;
}

bool FrenchZipCodesStep::process()
{
    unzipFiles();
    prepareData();
//    createDatabase();
    populateDatabase();
    Q_EMIT processFinished();
    return true;
}

bool FrenchZipCodesStep::unzipFiles()
{
    Q_EMIT progressLabelChanged(tr("French zip codes database creation: unzipping files..."));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // check file
    QString fileName = workingPath() + QDir::separator() + QFileInfo(FRENCH_URL).fileName();
    if (!QFile(fileName).exists()) {
        LOG_ERROR(QString("No files founded. Please download files."));
        return false;
    }

    LOG(QString("Starting unzipping French Zip Codes file %1").arg(fileName));

    // unzip downloaded using QProcess
    if (!QuaZipTools::unzipFile(fileName, workingPath()))
        return false;

    return true;
}

bool FrenchZipCodesStep::prepareData()
{
    LOG(tr("Preparing raw source files"));
    Q_EMIT progressLabelChanged(tr("Preparing raw source files"));

    if (!QFile(csvFileAbsPath()).exists()) {
        Utils::warningMessageBox(tr("No source file found"), tr("Contact dev team."));
        return false;
    }
//    // correcting wrong chars
//    QString content = Utils::readTextFile(csvFileAbsPath(), "ISO-8859-1");
//    qWarning() << content;
//    content.remove("†");
//    Utils::saveStringToFile(content, csvFileAbsPath());

    return true;
}

bool FrenchZipCodesStep::createDatabase()
{
    LOG(tr("Creating french zipcodes database"));
    Q_EMIT progressLabelChanged(tr("Creating french zipcodes database"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
        .arg(QFileInfo(databaseAbsPath()).fileName())
        .arg(QFileInfo(databaseAbsPath()).absolutePath()));

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", DB_NAME);
    db.setDatabaseName(databaseAbsPath());
    if (!db.open()) {
        LOG_ERROR("Unable to connect database in memory.");
        return false;
    }

    if (db.tables().contains("ZIPS_IMPORT")) {
        if (!Utils::Database::executeSQL("DROP TABLE ZIPS_IMPORT;", db)) {
            LOG_ERROR("Unable to clean database");
            return false;
        }
    }
    if (!db.tables().contains("ZIPS")) {
        if (!Utils::Database::executeSqlFile(DB_NAME, sqlMasterFileAbsPath())) {
            LOG_ERROR("Unable to create database scheme");
            return false;
        }
    }

    LOG(QString("Database schema created"));
    Q_EMIT progress(1);
    return true;
}

bool FrenchZipCodesStep::populateDatabase()
{
    Q_EMIT progressLabelChanged(tr("Reading raw sources..."));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // import the raw source in memory
    createDatabase();
    QSqlDatabase db = QSqlDatabase::database(DB_NAME);

    if (!db.tables().contains("ZIPS_IMPORT")) {
        Utils::Database::executeSqlFile(DB_NAME, sqlImportFileAbsPath());
    }

    if (!Utils::Database::importCsvToDatabase(DB_NAME, csvFileAbsPath(), "ZIPS_IMPORT", ";", true))
        return false;

    Q_EMIT progressLabelChanged(tr("Reading raw sources..."));
    Q_EMIT progress(1);

    // create the new database
    QString req;

    // remove alread recorded french codes
    req = "DELETE FROM `ZIPS` WHERE `ZIPS`.`COUNTRY`='fr';";
    Utils::Database::executeSQL(req, db);

    // import new french codes
    // 4 chars
//    select * from zips where length(ZIP)>5;
//    select substr('00000' || ZIP, -5, 5) from ZIPS where zip like'14%';
    req = "INSERT INTO `ZIPS` (`ZIP`,`CITY`,`EXTRACODE`, `COUNTRY`) \n"
          "SELECT substr('00000' || ZIP, -5, 5), `COMMUNE`, `CODEINSEE`, 'fr' FROM `ZIPS_IMPORT` WHERE LENGTH(ZIP)=4 ORDER BY `ZIP`;";
    Utils::Database::executeSQL(req, db);
    // 5 chars
    req = "INSERT INTO `ZIPS` (`ZIP`,`CITY`,`EXTRACODE`, `COUNTRY`) \n"
          "SELECT substr(`ZIP`, 1, 5) , `COMMUNE`, `CODEINSEE`, 'fr' FROM `ZIPS_IMPORT` WHERE LENGTH(ZIP)>=5 ORDER BY `ZIP`;";
    Utils::Database::executeSQL(req, db);

    // clean database
    req = "DROP TABLE ZIPS_IMPORT;";
    Utils::Database::executeSQL(req, db);
    Utils::Database::vacuum(db.connectionName());

    LOG(QString("Database processed"));

    return true;
}

/** Register the ZipCode datapack to the non-free french associative server */
bool FrenchZipCodesStep::registerDataPack()
{
    QString server = Core::Constants::SERVER_ASSO_NONFREE;
    DataPackPlugin::DataPackQuery query;
    query.setDescriptionFileAbsolutePath(datapackDescriptionFile());
    query.setOriginalContentFileAbsolutePath(databaseAbsPath());
    query.setZipOriginalFile(true);
    if (!dataPackCore()->registerDataPack(query, server)) {
        LOG_ERROR("Unable to register ICD10 datapack");
        return false;
    }
    LOG(QString("Registered ZipCodes datapack in server %1").arg(server));
    return true;
}

// #################### FrenchZipCodesWidget #######################

FrenchZipCodesWidget::FrenchZipCodesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrenchZipCodesWidget)
{
    ui->setupUi(this);
    m_Step = new FrenchZipCodesStep(this);
    m_Step->createTemporaryStorage();
    pluginManager()->addObject(m_Step);
}

FrenchZipCodesWidget::~FrenchZipCodesWidget()
{
    pluginManager()->removeObject(m_Step);
    delete ui;
}

void FrenchZipCodesWidget::on_download_clicked()
{
    ui->download->setEnabled(false);
    ui->download->setText(tr("Download in progress"));
    m_Step->startDownload();
    connect(m_Step, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
}

void FrenchZipCodesWidget::downloadFinished()
{
    ui->download->setText(tr("Download terminated"));
    ui->download->setEnabled(true);
}


void FrenchZipCodesWidget::on_startJob_clicked()
{
//    m_Step->createDir();
//    m_Step->cleanFiles();
//    m_Step->downloadFiles();
    m_Step->process();

//    m_Step->processMessage();

//    m_Step->postProcessDownload();
}



