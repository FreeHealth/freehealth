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
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com     *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "genericzipcodesstep.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <utils/httpdownloader.h>
#include <quazip/global.h>

#include <extensionsystem/pluginmanager.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardItemModel>

#include <QDebug>

using namespace ZipCodes;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

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
        q(parent)
    {
    }

    ~GenericZipCodesStepPrivate() {}

    QUrl url() const {return QUrl("http://download.geonames.org/export/zip/allCountries.zip");}

    QString tmpPath() const {return settings()->value(Core::Constants::S_TMP_PATH).toString() + "/GeonamesZipCodes/";}

    // This database contains **all** zipcodes available for **all** countries
    QString databaseOutputAbsFilePath() const {return settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/geonameszipcodes/zipcodes.db";}

    // Return the path for a database containing \e countries usable for a datapack creation
    // This database is different from the databaseOutputAbsFilePath()
    QString databaseForDatapackOutputAbsFilePath(const QStringList &countries) const
    {
        QFileInfo info(databaseOutputAbsFilePath());
        return info.absolutePath() + QDir::separator() + countries.join("-") + QDir::separator() + "zipcodes.db";
    }

    QString sqlMasterFileAbsPath() const {return settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/zipcodes/zipcodes.sql";}

    bool connectRawDatabase()
    {
        QSqlDatabase db;
        if (QSqlDatabase::connectionNames().contains(DB_NAME)) {
            db = QSqlDatabase::database(DB_NAME);
        } else {
            db = QSqlDatabase::addDatabase("QSQLITE", DB_NAME);
            db.setDatabaseName(databaseOutputAbsFilePath());
            LOG_FOR(q, "using zipcodes database: " + databaseOutputAbsFilePath());
        }
        if (!db.isOpen()) {
            if (!db.open()) {
                LOG_ERROR_FOR(q, "Unable to open database: " + databaseOutputAbsFilePath());
                return false;
            }
        }
        return true;
    }

    bool createRawDatabase()
    {
        if (!connectRawDatabase())
            return false;
        QSqlDatabase db = QSqlDatabase::database(DB_NAME);

        if (db.tables().contains("IMPORT")) {
            Utils::Database::executeSQL("DROP TABLE IMPORT;", db);
        }
        if (!db.tables().contains("ZIPS")) {
            Utils::Database::executeSqlFile(DB_NAME, sqlMasterFileAbsPath());
        }
        LOG_FOR(q, QString("Database schema created"));
        return true;
    }

    bool populateRawDatabase()
    {
        if (!connectRawDatabase())
            return false;

        // Push CSV to raw database
        QString csvFile = tmpPath() + "/allCountries.txt";
        if (!Utils::Database::importCsvToDatabase(DB_NAME, csvFile, "IMPORT", "\t"))
            LOG_ERROR_FOR(q, "Unable to import CSV file");
        return true;
    }

    // Get all available countries from database and populate the model
    bool populateCountryModel()
    {
        qWarning() << "wwwwwwwwwwwwwwwww";
        // Clear cache
        m_availableCountriesModel->clear();
        m_availableIsoCodes.clear();

        // Connect raw database
        if (!connectRawDatabase())
            return false;
        QSqlDatabase db = QSqlDatabase::database(DB_NAME);

        // Read database
        const QString &flagPath = Core::ICore::instance()->settings()->path(Core::ISettings::SmallPixmapPath) + "/flags/";
        QSqlQuery query(db);
        if (query.exec("SELECT DISTINCT `COUNTRY` FROM `IMPORT`;")) {
            while (query.next()) {
                const QString &isoCode = query.value(0).toString();
                // Check iso code
                if (isoCode.isEmpty() || (isoCode.size() != 2)) {
                    LOG_ERROR_FOR(q, "Country conversion error: wrong ISO: "+ isoCode);
                    continue;
                }
                m_availableIsoCodes << isoCode;

                // Transform ISO3166 to QLocale::Country
                QLocale::Country locale = Utils::countryIsoToCountry(isoCode);
                if (locale == QLocale::AnyCountry) {
                    LOG_ERROR_FOR(q, "Country conversion error: country " + isoCode + " has no Qt equivalent.");
                    continue;
                }

                // Create a standard item in the countries model
                QStandardItem *item = new QStandardItem(QIcon(QString("%1/%2.png").arg(flagPath, isoCode)), QLocale::countryToString(locale));
                item->setData(locale);
                m_availableCountriesModel->appendRow(item);
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            return false;
        }
        return true;
    }

    bool createAllDatapackDatabase()
    {
        return true;
    }

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
        if (!QFile::copy(databaseOutputAbsFilePath(), file.absoluteFilePath())) {
            LOG_ERROR_FOR(q, "Unable to copy file: " + file.absoluteFilePath());
            return false;
        }

        // Connection & database preparation
        QString connection = QString("%1_%2").arg(DB_NAME).arg(countryIsoCodes.join("_"));
        QSqlDatabase db = QSqlDatabase::addDatabase("QSLITE", connection);
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

        // Vacuum
        if (!Utils::Database::vacuum(connection))
            return false;

        return true;
    }

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

/*!
 * Downloads the list of available countries.
 * \sa onAvailableCountriesDownloaded()
*/
bool GenericZipCodesStep::startDownload()
{
    // TODO: manage progress download
    if (!d->m_downloader) {
        d->m_downloader = new Utils::HttpDownloader(this);
        connect(d->m_downloader, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()), Qt::UniqueConnection);
        connect(d->m_downloader, SIGNAL(downloadProgressPercentsChanged(int)), this, SIGNAL(progress(int)));
    }
    Q_EMIT progressRangeChanged(0, 100);
    Q_EMIT progress(0);
    d->m_downloader->setOutputPath(d->tmpPath());
    d->m_downloader->setUrl(d->url());
    d->m_downloader->startDownload();
    return true;
}


QString GenericZipCodesStep::processMessage() const
{
    return tr("Generic zip codes database creation");
}

/** Automated ZipCode database creation of all available countries in GeoName */
bool GenericZipCodesStep::process()
{
    LOG(tr("Creating geoname zipcodes raw database"));
    Q_EMIT progressLabelChanged(tr("Creating geoname zipcodes raw database"));
    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progress(0);

    if (!d->createRawDatabase())
        return false;
    Q_EMIT progress(1);

    Q_EMIT progressLabelChanged(tr("Populating geoname zipcodes raw database"));
    if (!d->populateRawDatabase())
        return false;

    if (!d->createAllDatapackDatabase())
        return false;

    Q_EMIT progress(2);

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

bool GenericZipCodesStep::populateDatabase()
{
    //TODO: error handling
    Q_EMIT progressLabelChanged(tr("Reading sources..."));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    QSqlDatabase db = QSqlDatabase::database(DB_NAME);

    Q_EMIT progressLabelChanged(tr("Populating the database..."));
    Q_EMIT progress(1);

    // create the new database
    QString req;

    // remove alread recorded codes for selected countries
    foreach(QString countryIso, d->m_selectedCountryList) {
        req = QString("DELETE FROM `ZIPS` WHERE `ZIPS`.`COUNTRY`='%1';").arg(countryIso.toLower());
        Utils::Database::executeSQL(req, db);
    }
    qDebug() << "postal info number:" << d->m_postalList.count();
    foreach(const PostalInfo pi, d->m_postalList) {
        req = QString("INSERT INTO ZIPS (`ZIP`, `CITY`, `EXTRACODE`, `COUNTRY`)\n"
                      "VALUES ('%1', '%2', '%3', '%4');").arg(pi.postalCode, pi.city, pi.extraCode, pi.country);
        Utils::Database::executeSQL(req, db);
    }

    LOG(QString("Database processed"));
    return true;
}

bool GenericZipCodesStep::registerDataPack()
{
    // TODO: register zipcodes datapack
    return true;
}

// TODO: OBSOLETE
QStringList GenericZipCodesStep::errors() const
{
    return d->m_Errors;
}

void GenericZipCodesStep::selectCountry(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QStandardItem *item = d->m_availableCountriesModel->itemFromIndex(index)->clone();
    d->m_availableCountriesModel->removeRow(index.row());
    if (d->m_selectedCountriesModel->findItems(item->data(Qt::DisplayRole).toString()).isEmpty())
        d->m_selectedCountriesModel->appendRow(item);
    d->m_selectedCountriesModel->sort(0);
}

void GenericZipCodesStep::deselectCountry(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QStandardItem *item = d->m_selectedCountriesModel->itemFromIndex(index)->clone();
    d->m_selectedCountriesModel->removeRow(index.row());
    d->m_availableCountriesModel->appendRow(item);
    d->m_availableCountriesModel->sort(0);
}

/*!
 * \brief Called when a QNetworkReply that started a download of a selected country is finished.
 *
 * This function queries the GeoNames API and fetches all postalcode infos for the \e country
 * value found in the URL of the \e reply.
 *
 * A typical xml code of this reply looks like this:
 * \code
 * <code>
 * <postalcode>1070</postalcode>
 * <name>Wien</name>
 * <countryCode>AT</countryCode>
 * <lat>48.2</lat>
 * <lng>16.35</lng>
 * <adminName2>Politischer Bezirk Wien (Stadt)</adminName2>
 * <adminCode3>907</adminCode3>
 * <adminName3>Wien</adminName3>
 * </code>
 * \endcode
 * We extract just the \e name and the \e postalcode, as FreeMedForms does not use more data.
 * \param reply The QNetworkreply that was finished. Contains the URL and
 * the used parameters there.
 * \sa QNetworkReply::url(), QNetworkReply::queryItemValue()
 */
void GenericZipCodesStep::onSelectedCountryDownloadFinished(QNetworkReply *reply)
{

//    // decrease the reference counter
//    --d->m_selectedCountriesCounter;

//    if (reply->error() > 0) {
//        qDebug() << reply->errorString();
//    } else {
//        bool success = false;

//        d->m_postalList.clear();

//        // get XML structure of reply into a parseable format
//        QXmlInputSource src;
//        QDomDocument doc;

//        // get country code from URL, so we know which reply we have
//        QString countryIso3166Code = reply->url().queryItemValue("placename").toLower();
//        qDebug() << "countryIsoCode from reply:" << countryIso3166Code << reply->url().toString();

//        // malformed URL?
//        if (countryIso3166Code.isEmpty()) {
//            if (d->m_selectedCountriesCounter == 0)
//                reply->parent()->deleteLater();
//            Q_EMIT processFinished();
//            return;
//        }

//        // double check if this country is a valid QLocale::Country
//        QLocale::Country country = Utils::countryIsoToCountry(countryIso3166Code);
//        if (country == QLocale::AnyCountry) {
//            LOG("country is not valid: " + countryIso3166Code);
//            if (d->m_selectedCountriesCounter == 0)
//                reply->parent()->deleteLater();
//            Q_EMIT processFinished();
//            return;
//        }

//        src.setData(reply->readAll());
//        doc.setContent(src.data());

//        QString postalCode, city;

//        // iter over postal code chunks
//        QDomNodeList codes = doc.elementsByTagName("code");
//        for (uint i = 0; i < codes.length(); i++) {

//            // get the postalcode subitem
//            // (countryCode, countryName, numPostalCodes, minPostalCode, maxPostalCode)
//            postalCode = codes.item(i).firstChildElement("postalcode").toElement().text();
//            city = codes.item(i).firstChildElement("name").toElement().text();

//            if (postalCode.isEmpty() || (countryIso3166Code.size() != 2) || city.isEmpty()) {
//                qWarning() << "postal code conversion error: " << postalCode << city;
//                continue;
//            }

//            // do some vulnerability fixes: remove the ' char as it could be used for SQL injection
//            postalCode = postalCode.remove("'");
//            city = city.remove("'");
//            countryIso3166Code.remove("'");

//            d->m_postalList.append(PostalInfo(postalCode, city, countryIso3166Code));
//            qDebug() << "imported" << postalCode << city;

//            success = true;
//        }
//    }
//    if (d->m_selectedCountriesCounter == 0) {
//        reply->parent()->deleteLater();
//        Q_EMIT processFinished();
//    }
}


PostalInfo::PostalInfo(const QString &postalCode, const QString &city, const QString &country, const QString &extraCode)
{
    this->postalCode = postalCode;
    this->city = city;
    this->country = country;
    this->extraCode = extraCode;
}
