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

    bool createRawDatabase()
    {
        // try to get the already opened ZIP database connection
        QSqlDatabase db = QSqlDatabase::database(DB_NAME, true);
        if (!db.isValid()) {
            db = QSqlDatabase::addDatabase("QSQLITE", DB_NAME);
            db.setDatabaseName(databaseOutputAbsFilePath());
            LOG_FOR(q, "using zipcodes database: " + databaseOutputAbsFilePath());
        }
        // ok, here we can be sure that there is a database, now open it
        if (!db.open()) {
            LOG_ERROR_FOR(q, "Unable to open database.");
            return false;
        }

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
        // Push CSV to raw database
        QString csvFile = tmpPath() + "/allCountries.txt";
        if (!Utils::Database::importCsvToDatabase(DB_NAME, csvFile, "IMPORT", "\t"))
            LOG_ERROR_FOR(q, "Unable to import CSV file");
        return true;
    }

    bool createOutputDatabase(const QStringList &countryIsoCodes)
    {
        Q_UNUSED(countryIsoCodes);
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

/*!
 * Called when the downloading of the available countries index from GeoNames is finished,
 * then emits countryListDownloaded().\n
 * Downloads all available zipcodes from GeoNames.
 * Emits downloadFinished() when done.
 * \internal
 */
bool GenericZipCodesStep::onAvailableCountriesDownloaded()
{
//    Q_ASSERT(d->m_downloader);
//    if (!d->m_downloader)
//        return false;

//    // Reconnect downloader to the correct slot
//    disconnect(d->m_downloader, SIGNAL(downloadFinished()), this, SLOT(onAvailableCountriesDownloaded()));
//    connect(d->m_downloader, SIGNAL(downloadFinished()), this, SLOT(downloadZipCodesUsingCachedIso()));

//    // Clear cache
//    d->m_availableCountriesModel->clear();
//    d->m_availableIsoCodes.clear();

//    // Check if file is downloaded
//    // TODO: avoid magic numbers in URL & fileName
//    if (!QFileInfo(d->tmpPath() + "/postalCodeCountryInfo").exists()) {
//        LOG_ERROR("File not downloaded");
//        return false;
//    }

//    // Read all available languages
//    QDomDocument doc;
//    int line, col;
//    line = 0;
//    col = 0;
//    QString error;
//    if (!doc.setContent(Utils::readTextFile(tmpPath() + "/postalCodeCountryInfo"), &error, &line, &col)) {
//        LOG_ERROR(QString("XML Error (%1;%2): %3").arg(line).arg(col).arg(error));
//        Utils::warningMessageBox(QString("XML Error (%1;%2): %3").arg(line).arg(col).arg(error), "");
//        return false;
//    }

//    // Start parsing the XML doc
//    const QString &flagPath = Core::ICore::instance()->settings()->path(Core::ISettings::SmallPixmapPath) + "/flags/";
//    QDomElement root = doc.firstChildElement("geonames");
//    QDomElement country = root.firstChildElement("country");
//    while (!country.isNull()) {
//        // Get subitems
//        QDomElement iso = country.firstChildElement("countryCode");
//        const QString &isoCode = iso.text();
//        d->m_availableIsoCodes << isoCode;
////        QDomElement name = country.firstChildElement("countryName");
////        const QString &countryName = name.text();

//        // Check iso code
//        if (isoCode.isEmpty() || (isoCode.size() != 2)) {
//            LOG_ERROR("Country conversion error: wrong ISO: "+ isoCode);
//            country = country.nextSiblingElement("country");
//            continue;
//        }

//        // Transform ISO3166 to QLocale::Country
//        QLocale::Country locale = Utils::countryIsoToCountry(isoCode);
//        if (locale == QLocale::AnyCountry) {
//            LOG_ERROR("Country conversion error: country " + isoCode + " has no Qt equivalent.");
//            country = country.nextSiblingElement("country");
//            continue;
//        }

//        // Create a standard item in the countries model
//        QStandardItem *item = new QStandardItem(QIcon(QString("%1/%2.png").arg(flagPath, isoCode)), QLocale::countryToString(locale));
//        item->setData(locale);
//        d->m_availableCountriesModel->appendRow(item);

//        // process next country
//        country = country.nextSiblingElement("country");
//    }

//    // find default system country and add it to the selected list
//    QList<QStandardItem*> f = d->m_availableCountriesModel->findItems(QLocale::countryToString(QLocale::system().country()));
//    if (f.count() > 0)
//        selectCountry(f.first()->index());
//    Q_EMIT countryListDownloaded();

//    // Start to download all zipcode files
//    downloadZipCodesUsingCachedIso();
    return true;
}

bool GenericZipCodesStep::downloadZipCodesUsingCachedIso()
{
//    // Finished ?
//    if (d->m_availableIsoCodes.isEmpty()) {
//        return true;
//    }

//    QString iso = d->m_availableIsoCodes.takeFirst().toLower();
//    d->m_downloader->setUrl(QUrl(QString("http://api.geonames.org/postalCodeSearch?username=freemedforms&maxRows=%1&style=short&placename=%2")
//                              .arg(MAX_ROWS)
//                              .arg(iso)));
//    d->m_downloader->setOutputFileName(QString("%1.xml").arg(iso));
//    d->m_downloader->startDownload();
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
QStandardItemModel* GenericZipCodesStep::selectedCountriesModel()
{
    return d->m_selectedCountriesModel;
}


/** Download zipcodes for selected countries */
//bool GenericZipCodesStep::startDownloadingSelectedCountryData()
//{
//    //TODO: error handling
//    Q_EMIT progressLabelChanged(tr("Downloading data for selected countries"));

//    // this counter is set to the number of selected countries.
//    // it is used as reference counter for the QNetworkReplys and decreased with every finished reply slot
//    // when it reaches 0, we can safely delete the netAccessManager.
//    m_selectedCountriesCounter = m_selectedCountriesModel->rowCount();

//    Q_EMIT progressRangeChanged(0, m_selectedCountriesCounter);
//    Q_EMIT progress(0);

//    // the netAccessManager is deleted in onSelectedCountryDownloadFinished() when the last reply is finished.
//    QNetworkAccessManager *netAccessManager = new QNetworkAccessManager(this);

//    const QStandardItem *item;
//    for(int i = 0; i < m_selectedCountriesCounter; ++i) {
//        item = m_selectedCountriesModel->item(i);
//        m_selectedCountry = static_cast<QLocale::Country>(item->data().toInt());
//        qDebug() << static_cast<QLocale::Country>(item->data().toInt());

//        m_selectedCountryList.append(Utils::countryToIso(m_selectedCountry).toLower());
//        //    get list of places that GeoNames has informations for in the given country
//        QNetworkRequest request;
//        request.setUrl(QUrl(QString("http://api.geonames.org/postalCodeSearch?username=freemedforms&maxRows=%1&style=short&placename=%2")
//                            .arg(MAX_ROWS)
//                            .arg(Utils::countryToIso(m_selectedCountry).toLower())));
//        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");

//        netAccessManager->get(request);
//        connect(netAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onSelectedCountryDownloadFinished(QNetworkReply*)));
//    }
//    return true;
//}

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

void GenericZipCodesStep::slotSetProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived)
    Q_UNUSED(bytesTotal)

    //TODO: implementation
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
