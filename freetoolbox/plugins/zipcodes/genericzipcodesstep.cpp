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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com     *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "genericzipcodesstep.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <datapackplugin/datapackcore.h>
#include <datapackplugin/datapackquery.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <utils/httpdownloader.h>
#include <quazip/global.h>
#include <datapackutils/constants.h>

#include <extensionsystem/pluginmanager.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QTimer>

#include <QDebug>

using namespace ZipCodes;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DataPackPlugin::DataPackCore *dataPackCore() {return DataPackPlugin::DataPackCore::instance();}

namespace {
const char* const  DB_NAME     = "ZIPCODES";

// Dump URL: http://download.geonames.org/export/zip/allCountries.zip
// Request URL: Available countries "http://api.geonames.org/postalCodeCountryInfo?username=freemedforms"
// Request URL: Available countries zipcodes "http://api.geonames.org/postalCodeSearch?username=freemedforms&maxRows=%1&style=short&placename=%2"
}  // end anonymous namespace

namespace ZipCodes {
namespace Internal {
class GenericZipCodesStepPrivate
{
public:
    explicit GenericZipCodesStepPrivate(GenericZipCodesStep *parent) :
        m_WithProgress(false),
        m_availableCountriesModel(0),
        m_selectedCountriesModel(0),
        m_selectedCountry(QLocale().country()),
        m_selectedCountriesCounter(-1),
        m_downloader(0),
        _currentTiming(Core::IFullReleaseStep::PreProcess),
        _currentSubProcess(Core::IFullReleaseStep::Initialization),
        q(parent)
    {
    }

    ~GenericZipCodesStepPrivate() {}

    // Source URL for geonames zipcodes
    QUrl url() const {return QUrl("http://download.geonames.org/export/zip/allCountries.zip");}

    // Temporary path
    QString tmpPath() const {return settings()->value(Core::Constants::S_TMP_PATH).toString() + "/GeonamesZipCodes/";}

    // This database contains **all** zipcodes available for **all** countries
    QString databaseOutputAbsFilePath() const
    {
        return settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() +
                "/geonameszipcodes/zipcodes.db";
    }

    // Return the path for a database containing \e countries usable for a datapack creation
    // This database is different from the databaseOutputAbsFilePath()
    QString databaseForDatapackOutputAbsFilePath(const QStringList &countries) const
    {
        QFileInfo info(databaseOutputAbsFilePath());
        return info.absolutePath() + QDir::separator() + countries.join("-") + QDir::separator() + "zipcodes.db";
    }

    // ZipCodes database SQL scheme file
    QString sqlMasterFileAbsPath() const
    {
        return settings()->value(Core::Constants::S_GITFILES_PATH).toString() +
                "/global_resources/sql/zipcodes/zipcodes.sql";
    }

    // When user wants to create multiple country datapacks
    // This file describes all countries to include
    QString multiCountryDefinitionFile() const
    {
        return settings()->value(Core::Constants::S_GITFILES_PATH).toString() +
                QString("%1/%2")
                .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                .arg("/geonames/multi.csv");
    }

    // Path to generic datapack description file
    QString genericDatapackDescriptionFile() const
    {
        return settings()->value(Core::Constants::S_GITFILES_PATH).toString() +
                QString("%1/%2/%3")
                .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                .arg("geonames")
                .arg(DataPack::Constants::PACKDESCRIPTION_FILENAME);}

    // Connect the raw source database
    bool connectRawDatabase()
    {
        QSqlDatabase db;
        if (QSqlDatabase::connectionNames().contains(DB_NAME)) {
            db = QSqlDatabase::database(DB_NAME);
        } else {
            db = QSqlDatabase::addDatabase("QSQLITE", DB_NAME);
            db.setDatabaseName(databaseOutputAbsFilePath());

            // Improve speed when creating the drug database
            db.exec("PRAGMA synchronous = OFF");
            db.exec("PRAGMA journal_mode = MEMORY");

            LOG_FOR(q, "Using zipcodes database: " + databaseOutputAbsFilePath());
        }
        if (!db.isOpen()) {
            if (!db.open()) {
                LOG_ERROR_FOR(q, "Unable to open database: " + databaseOutputAbsFilePath());
                return false;
            }
        }
        return true;
    }

    // If the raw source database does not exists -> create it
    bool createRawDatabase()
    {
        if (!connectRawDatabase())
            return false;
        QSqlDatabase db = QSqlDatabase::database(DB_NAME);
        if (!db.tables().contains("ZIPS"))
            Utils::Database::executeSqlFile(DB_NAME, sqlMasterFileAbsPath());
        LOG_FOR(q, QString("Database schema created"));
        return true;
    }

    // Populate the raw source database wih downloaded data
    bool populateRawDatabase()
    {
        if (!connectRawDatabase())
            return false;

        // Push CSV to raw database
        QString csvFile = tmpPath() + "/allCountries.txt";
        if (!Utils::Database::importCsvToDatabase(DB_NAME, csvFile, "IMPORT", "\t"))
            LOG_ERROR_FOR(q, "Unable to import CSV file");
        else
            LOG_FOR(q, "CSV data imported to database");
        return true;
    }

    // Read all available countries from raw database and store them in m_availableIsoCodes
    bool getAllAvailableCountriesFromRawDatabase()
    {
        if (!m_availableIsoCodes.isEmpty())
            return true;

        // Connect raw database
        if (!connectRawDatabase())
            return false;
        QSqlDatabase db = QSqlDatabase::database(DB_NAME);
        QSqlQuery query(db);
        if (query.exec("SELECT DISTINCT `COUNTRY` FROM `IMPORT`;")) {
            while (query.next()) {
                const QString &isoCode = query.value(0).toString();
                // Check iso code
                if (isoCode.isEmpty() || (isoCode.size() != 2)) {
                    LOG_ERROR_FOR(q, "Country conversion error: wrong ISO: "+ isoCode);
                    continue;
                }
                // Transform ISO3166 to QLocale::Country
                QLocale::Country locale = Utils::countryIsoToCountry(isoCode);
                if (locale == QLocale::AnyCountry) {
                    LOG_ERROR_FOR(q, "Country conversion error: country " + isoCode + " has no Qt equivalent.");
                    continue;
                }
                m_availableIsoCodes << isoCode;
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            return false;
        }
        return true;
    }

    // Get all available countries from database and populate the model
    bool populateCountryModel()
    {
        m_availableCountriesModel->clear();
        const QString &flagPath = Core::ICore::instance()->settings()->path(Core::ISettings::SmallPixmapPath) + "/flags/";
        if (!getAllAvailableCountriesFromRawDatabase())
            return false;
        foreach(const QString &isoCode, m_availableIsoCodes) {
            // Create a standard item in the countries model
            QLocale::Country locale = Utils::countryIsoToCountry(isoCode);
            QStandardItem *item = new QStandardItem(QIcon(QString("%1/%2.png").arg(flagPath, isoCode)), QLocale::countryToString(locale));
            item->setData(locale);
            m_availableCountriesModel->appendRow(item);
        }
        return true;
    }

    // Create all datapacks using the 'multi-country' description file
    bool createAllDatapackDatabase()
    {
        // Read the multi-country definition file
        QString multiContent = Utils::readTextFile(multiCountryDefinitionFile());
        QStringList multi;
        foreach(QString line, multiContent.split("\n")) {
            // clean line
            line = line.trimmed();
            if (line.isEmpty())
                continue;
            // remove comment line
            if (line.startsWith("--") || line.startsWith("#"))
                continue;
            multi << line;
        }
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        // Read available countries
        if (!getAllAvailableCountriesFromRawDatabase())
            return false;
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        // Manage progress
        int max = m_availableIsoCodes.count() + multi.count();
        int min = 0;
        Q_EMIT q->progressLabelChanged(QString("Creating %1 geonames datapacks").arg(max));
        Q_EMIT q->progressRangeChanged(min, max);
        Q_EMIT q->progress(min);

        // Create the one-country datapacks
        foreach(const QString &iso, m_availableIsoCodes) {
            ++min;
            Q_EMIT q->progress(min);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            if (!createDatabaseForDatapack(QStringList() << iso)) {
                LOG_ERROR_FOR(q, "Unable to create zipcodes datapack: " + iso);
                continue;
            }
            LOG_FOR(q, "ZipCodes datapack created: " + iso);
        }
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        // Create the one-country datapacks
        foreach(const QString &iso, multi) {
            ++min;
            Q_EMIT q->progress(min);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            if (!createDatabaseForDatapack(iso.split(";"))) {
                LOG_ERROR_FOR(q, "Unable to create zipcodes datapack: " + iso);
                continue;
            }
            LOG_FOR(q, "ZipCodes datapack created: " + iso);
        }

        Q_EMIT q->progress(max);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        return true;
    }

    // Create the database corresponding to listed countries
    bool createDatabaseForDatapack(const QStringList &countryIsoCodes)
    {
        Q_UNUSED(countryIsoCodes);
        QFileInfo file(databaseForDatapackOutputAbsFilePath(countryIsoCodes));

        // Check dir
        if (!QDir().mkpath(file.absolutePath())) {
            LOG_ERROR_FOR(q, "Unable to create path: " + file.absolutePath());
            return false;
        }

        // Copy the raw source database into the path
        if (file.exists()) {
            QFile removeMe(file.absoluteFilePath());
            removeMe.remove();
        }
        if (!QFile::copy(databaseOutputAbsFilePath(), file.absoluteFilePath())) {
            LOG_ERROR_FOR(q, "Unable to copy file: " + file.absoluteFilePath());
            return false;
        }
        LOG_FOR(q, "Raw database copied into: " + file.absoluteFilePath());
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        // Connection & database preparation
        QString connection = QString("%1_%2").arg(DB_NAME).arg(countryIsoCodes.join("_"));
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connection);
        db.setDatabaseName(file.absoluteFilePath());
        if (!db.isOpen()) {
            if (!db.open()) {
                LOG_ERROR_FOR(q, "Unable to open database: " + file.absoluteFilePath());
                return false;
            }
        }

        // Remove uneeded data
        // DELETE FROM `IMPORT` WHERE (`COUNTRY` NOT IN ('FR', 'BE'))
        QString req = QString("DELETE FROM `IMPORT` WHERE (`COUNTRY` NOT IN ('%1'))")
                .arg(countryIsoCodes.join("', '"));
        if (!Utils::Database::executeSQL(req, db))
            return false;

        // Recreate old fashion zips table to avoid version breaking for zipcode autocompleters
//        foreach(const QString &iso, countryIsoCodes) {
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            req =   "INSERT INTO `ZIPS` (`ZIP`, `CITY`, `COUNTRY`) "
                    "SELECT `IMPORT`.`ZIP`, `IMPORT`.`CITY`, `IMPORT`.`COUNTRY` FROM `IMPORT`";
            if (!Utils::Database::executeSQL(req, db)) {
                LOG_ERROR_FOR(q, "Unable to create the old database version");
            }
//        }

        // Add version
        if (!setDatabaseVersion(connection, QString(PACKAGE_VERSION), QDate::currentDate())) {
            LOG_ERROR_FOR(q, "Unable to set version to database: " + file.absoluteFilePath());
            return false;
        }
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        // Copy the datapack description file
        QStringList names;
        QString content = Utils::readTextFile(genericDatapackDescriptionFile());
        foreach(const QString &iso, countryIsoCodes) {
            // If there is a > 2char iso, the user want this as country name (like EUROPE)
            if (iso.size() > 2) {
                names.clear();
                names.append(iso);
                break;
            }
            // Otherwise add country name
            QLocale::Country locale = Utils::countryIsoToCountry(iso);
            names << QLocale::countryToString(locale);
        }
        content = content.replace("__COUNTRY__", names.join(", "));
        content = content.replace("__COUNTRY_ISO__", countryIsoCodes.join(", "));
        Utils::saveStringToFile(content, QString("%1/%2").arg(file.absolutePath()).arg(DataPack::Constants::PACKDESCRIPTION_FILENAME), Utils::Overwrite, Utils::DontWarnUser);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        // Vacuum
        if (!Utils::Database::vacuum(connection))
            return false;

        return true;
    }

    // Tag the database with the current version
    bool setDatabaseVersion(const QString &connection, const QString &version, const QDate &date)
    {
        QSqlDatabase db = QSqlDatabase::database(connection);
        if (!db.isOpen()) {
            LOG_ERROR_FOR(q, "Database not open");
            return false;
        }
        if (!Utils::Database::executeSQL("DELETE FROM `VERSION`;", db))
            return false;
        if (!Utils::Database::executeSQL(QString("INSERT INTO `VERSION` (`CURRENT`, `DATE`) "
                                                 "VALUES ('%1', '%2');").arg(version).arg(date.toString(Qt::ISODate)), db))
            return false;
        return true;
    }

public:
    QStringList m_Errors;
    bool m_WithProgress;
    QStandardItemModel *m_availableCountriesModel;
    QStandardItemModel *m_selectedCountriesModel;
    QLocale::Country m_selectedCountry;
    QStringList m_selectedCountryList, m_availableIsoCodes;
    int m_selectedCountriesCounter;
    QList<PostalInfo> m_postalList;
    Utils::HttpDownloader *m_downloader;
    Core::IFullReleaseStep::ProcessTiming _currentTiming;
    Core::IFullReleaseStep::SubProcess _currentSubProcess;

private:
    GenericZipCodesStep *q;
};
}  // namespace Internal
}  // namespace ZipCodes


// ########################## GenericZipCodesStep ##########################

GenericZipCodesStep::GenericZipCodesStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    d(new Internal::GenericZipCodesStepPrivate(this))
{
    setObjectName("GenericZipCodesStep");

    d->m_WithProgress = false;
    d->m_availableCountriesModel = new QStandardItemModel(this);
    d->m_selectedCountriesModel = new QStandardItemModel(this);
    d->m_selectedCountriesCounter = 0;
    d->m_downloader = 0;
}

GenericZipCodesStep::~GenericZipCodesStep()
{
}

bool GenericZipCodesStep::createTemporaryStorage()
{
    // Create the tempPath
    if (!QDir().mkpath(d->tmpPath()))
        LOG_ERROR("Unable to create outputPath:" + d->tmpPath());
    else
        LOG("Tmp dir created");

    // Create database output dir
    QString outputDir = QFileInfo(d->databaseOutputAbsFilePath()).absolutePath();
    if (!QDir().exists(outputDir)) {
        if (!QDir().mkpath(outputDir)) {
            LOG_ERROR("Unable to create GenericZipCodesStep database output path: " + outputDir);
        } else {
            LOG("GenericZipCodesStep database output dir created");
        }
    }
    return true;
}

/** Clear the temporary path */
bool GenericZipCodesStep::cleanTemporaryStorage()
{
    Utils::removeDirRecursively(d->tmpPath(), 0);
    if (!QDir().mkpath(d->tmpPath()))
        LOG_ERROR("Unable to create outputPath:" + d->tmpPath());
    return true;
}

bool GenericZipCodesStep::startProcessing(ProcessTiming timing, SubProcess subProcess)
{
    bool ok = true;
    d->_currentTiming = timing;
    d->_currentSubProcess = subProcess;
    switch (subProcess) {
    case Initialization:
        switch (timing) {
        case PreProcess:
            ok = createTemporaryStorage();
            break;
        case Process:
            ok = startDownload();
            connect(this, SIGNAL(downloadFinished()), this, SLOT(onSubProcessFinished()), Qt::UniqueConnection);
            return ok;
            break;
        case PostProcess:
            ok = postDownloadProcessing();
            break;
        } // switch
        break;
    case Main:
        switch (timing) {
        case PreProcess:
            break;
        case Process:
            ok = process();
            break;
        case PostProcess:
            break;
        } // switch
        break;
    case DataPackSubProcess:
        switch (timing) {
        case PreProcess:
            break;
        case Process:
            ok = registerDataPack();
            break;
        case PostProcess:
            break;
        } // switch
        break;
    case Final:
        switch (timing) {
        case PreProcess:
            break;
        case Process:
            break;
        case PostProcess:
            break;
        } // switch
        break;
    } // switch
    QTimer::singleShot(1, this, SLOT(onSubProcessFinished()));
    return true;
}

void GenericZipCodesStep::onSubProcessFinished()
{
    Q_EMIT subProcessFinished(d->_currentTiming, d->_currentSubProcess);
}

/*!
 * Downloads the list of available countries.
 * You can connect the progress() signal to a QComboBox (with range 0..1000) to watch the
 * download progress.
 * \sa downloadFinished(), progress()
*/
bool GenericZipCodesStep::startDownload()
{
    // TODO: manage progress download
    if (!d->m_downloader) {
        d->m_downloader = new Utils::HttpDownloader(this);
        connect(d->m_downloader, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()), Qt::UniqueConnection);
        connect(d->m_downloader, SIGNAL(downloadFinished()), d->m_downloader, SLOT(deleteLater()));
        connect(d->m_downloader, SIGNAL(downloadProgressPermille(int)), this, SIGNAL(progress(int)));
    }

    Q_EMIT progressRangeChanged(0, 1000);
    Q_EMIT progress(0);

    d->m_downloader->setOutputPath(d->tmpPath());
    d->m_downloader->setUrl(d->url());
    return d->m_downloader->startDownload();
}


QString GenericZipCodesStep::processMessage() const
{
    return tr("Generic zip codes database creation");
}

/**
 * General processing: \n
 * Automated ZipCode database creation of all available countries in GeoName.\n
 * This step will create one datapack per available country and store them in the
 * DataPack::Constants::SERVER_COMMUNITY_FREE datapack server. \n
 *
 * Multi-country datapacks: \n
 * If you want to define multi-country datapacks, you can use the file in the
 * source global_resources to add multi-country definition. You can find the file here:
 * <i>global_resources/datapack_description/zipcodes/geonames/multi.csv</i>.
 * This file is self-documented.
 */
bool GenericZipCodesStep::process()
{
    LOG(tr("Creating geoname zipcodes raw database"));
    Q_EMIT progressLabelChanged(tr("Creating geoname zipcodes raw database"));
    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progress(0);

    if (!d->createRawDatabase())
        return false;
    Q_EMIT progress(1);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    Q_EMIT progressLabelChanged(tr("Populating geoname zipcodes raw database"));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (!d->populateRawDatabase())
        return false;

    if (!d->createAllDatapackDatabase())
        return false;

    Q_EMIT progress(2);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    Q_EMIT processFinished();
    return true;
}

/** Unzip the downloaded file */
bool GenericZipCodesStep::postDownloadProcessing()
{
    // check file
    QString fileName = d->tmpPath() + QDir::separator() + QFileInfo(d->url().toString()).fileName();
    if (!QFileInfo(fileName).exists()) {
        LOG_ERROR(QString("No files to unzip."));
        LOG_ERROR(QString("Please download files."));
        return false;
    }

    // unzip files using QProcess
    bool ok = QuaZipTools::unzipFile(fileName, d->tmpPath());

    // uppercase the whole file
    Utils::saveStringToFile(Utils::readTextFile(d->tmpPath() + "/allCountries.txt").toUpper(), d->tmpPath() + "/allCountries.txt", Utils::Overwrite, Utils::DontWarnUser);

    Q_EMIT postDownloadProcessingFinished();
    return ok;
}

QStandardItemModel* GenericZipCodesStep::availableCountriesModel()
{
    return d->m_availableCountriesModel;
}

bool GenericZipCodesStep::populateCountryModel() const
{
    return d->populateCountryModel();
}

QStandardItemModel* GenericZipCodesStep::selectedCountriesModel()
{
    return d->m_selectedCountriesModel;
}

/** Populate the raw database with the downloaded CSV file */
bool GenericZipCodesStep::populateDatabase()
{
    Q_EMIT progressLabelChanged(tr("Creating geoname zipcodes raw database"));
    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progress(0);
    if (!d->connectRawDatabase())
        return false;
    QSqlDatabase db = QSqlDatabase::database(DB_NAME);

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (!d->createRawDatabase())
        return false;
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    Q_EMIT progress(1);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    if (!d->populateRawDatabase())
        return false;
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progress(2);
    return true;
}

bool GenericZipCodesStep::registerDataPack()
{
    QString server = DataPack::Constants::SERVER_ASSO_NONFREE;
    QString dbFileName = QFileInfo(d->databaseOutputAbsFilePath()).fileName();

    // Find all created datapacks
    QFileInfoList files = Utils::getFiles(QFileInfo(d->databaseOutputAbsFilePath()).absolutePath(), DataPack::Constants::PACKDESCRIPTION_FILENAME);
    foreach(const QFileInfo &file, files) {
        DataPackPlugin::DataPackQuery query;
        query.setDescriptionFileAbsolutePath(file.absoluteFilePath());
        query.setOriginalContentFileAbsolutePath(file.absolutePath() + QDir::separator() + dbFileName);
        query.setZipOriginalFile(true);
        if (!dataPackCore()->registerDataPack(query, server)) {
            LOG_ERROR("Unable to register ZipCode datapack: " + file.absoluteFilePath());
            return false;
        }
        LOG(QString("Registered ZipCodes %1 datapack in server %2").arg(file.absoluteFilePath()).arg(server));
    }
    return true;
}

// TODO: OBSOLETE
QStringList GenericZipCodesStep::errors() const
{
    return d->m_Errors;
}

PostalInfo::PostalInfo(const QString &postalCode, const QString &city, const QString &country, const QString &extraCode)
{
    this->postalCode = postalCode;
    this->city = city;
    this->country = country;
    this->extraCode = extraCode;
}
