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

#include <QFileInfo>
#include <QDir>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QDomDocument>
#include <QtXml>
#include <QStandardItemModel>
#include <QSqlDriver>
#include <QSqlField>

#include <QDebug>

using namespace ZipCodes;

namespace {
const char* const  DB_NAME     = "ZIPCODES";
// this defines the maximum downloaded zipcodes from the page

#ifdef DEBUG
const char* const MAX_ROWS     = "20";
#else // RELEASE
const char* const MAX_ROWS     = "10000";
#endif
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString databaseAbsPath() {
    return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString()
                           + "/geonameszipcodes/zipcodes.db");
}

static inline QString workingPath() {
    return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString()
                           + "/GeonamesZipCodes/") + QDir::separator();
}

static inline QString sqlMasterFileAbsPath() {
    return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString()
                           + "/global_resources/sql/zipcodes/zipcodes.sql");
}

static inline QString sqlImportFileAbsPath()
{
    return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() +
                           "/global_resources/sql/zipcodes/zipcodes-fr-import.sql");
}

}  // end anonymous namespace


// ########################## GenericZipCodesStep ##########################

GenericZipCodesStep::GenericZipCodesStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    m_WithProgress(false),
    m_availableCountriesModel(new QStandardItemModel(this)),
    m_selectedCountriesModel(new QStandardItemModel(this)),
    m_selectedCountriesCounter(0)
{
    setObjectName("GenericZipCodesStep");
    createTemporaryStorage();
}

GenericZipCodesStep::~GenericZipCodesStep()
{
}

bool GenericZipCodesStep::createTemporaryStorage()
{
    // Create the tempPath
    if (!QDir().mkpath(workingPath()))
        LOG_ERROR("Unable to create outputPath:" + workingPath());
    else
        LOG("Tmp dir created");

    // Create database output dir
    if (!QDir().exists(databaseAbsPath())) {
        if (!QDir().mkpath(databaseAbsPath())) {
            LOG_ERROR("Unable to create GenericZipCodesStep database output path: " + databaseAbsPath());
        } else {
            LOG("GenericZipCodesStep database output dir created");
        }
    }
    return true;
}

/*!
 * Downloads the list of available countries.
 * \sa onAvailableCountriesDownloaded()
*/
bool GenericZipCodesStep::downloadFiles(QProgressBar *bar)
{
    Q_UNUSED(bar);
    // TODO: manage progress download
    // TODO: in the automated management of this step all files must be downloaded at once (all countries zipcodes)
    Utils::HttpDownloader *dld = new Utils::HttpDownloader(this);
    dld->setOutputPath(workingPath());
    dld->setUrl(QUrl("http://api.geonames.org/postalCodeCountryInfo?username=freemedforms"));
    dld->startDownload();
    connect(dld, SIGNAL(downloadFinished()), this, SLOT(onAvailableCountriesDownloaded()));
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    return true;

}

/** Automated ZipCode database creation of all available countries in GeoName */
bool GenericZipCodesStep::process()
{
    createDatabaseScheme();
    startDownloadingSelectedCountryData();
    return true;
}

bool GenericZipCodesStep::postProcessDownload()
{
    populateDatabase();
    Q_EMIT postProcessDownloadFinished();
    return true;
}

/** Create the GeoName zipcode database */
bool GenericZipCodesStep::createDatabaseScheme()
{
    LOG(tr("Creating Generic zipcodes database"));
    Q_EMIT progressLabelChanged(tr("Creating Generic zipcodes database scheme"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // try to get the already opened ZIP database connection
    QSqlDatabase db = QSqlDatabase::database(DB_NAME, true);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE", DB_NAME);
        db.setDatabaseName(databaseAbsPath());
        LOG("using zipcodes database: " + databaseAbsPath());
    }
    // ok, here we can be sure that there is a database, now open it
    if (!db.open()) {
        LOG_ERROR("Unable to open database.");
        return false;
    }

    if (db.tables().contains("ZIPS_IMPORT")) {
        Utils::Database::executeSQL("DROP TABLE ZIPS_IMPORT;", db);
    }
    if (!db.tables().contains("ZIPS")) {
        Utils::Database::executeSqlFile(DB_NAME, sqlMasterFileAbsPath());
    }

    LOG(QString("Database schema created"));
    Q_EMIT progress(1);
    return true;
}

/** Download zipcodes for selected countries */
bool GenericZipCodesStep::startDownloadingSelectedCountryData()
{
    Q_EMIT progressLabelChanged(tr("Downloading data for selected countries"));

    // this counter is set to the number of selected countries.
    // it is used as reference counter for the QNetworkReplys and decreased with every finished reply slot
    // when it reaches 0, we can safely delete the netAccessManager.
    m_selectedCountriesCounter = m_selectedCountriesModel->rowCount();

    Q_EMIT progressRangeChanged(0, m_selectedCountriesCounter);
    Q_EMIT progress(0);

    // the netAccessManager is deleted in onSelectedCountryDownloadFinished() when the last reply is finished.
    QNetworkAccessManager *netAccessManager = new QNetworkAccessManager(this);

    const QStandardItem *item;
    for(int i = 0; i < m_selectedCountriesCounter; ++i) {
        item = m_selectedCountriesModel->item(i);
        m_selectedCountry = static_cast<QLocale::Country>(item->data().toInt());
        qDebug() << static_cast<QLocale::Country>(item->data().toInt());

        m_selectedCountryList.append(Utils::countryToIso(m_selectedCountry).toLower());
        //    get list of places that GeoNames has informations for in the given country
        QNetworkRequest request;
        request.setUrl(QUrl(QString("http://api.geonames.org/postalCodeSearch?username=freemedforms&maxRows=%1&style=short&placename=%2")
                            .arg(MAX_ROWS)
                            .arg(Utils::countryToIso(m_selectedCountry).toLower())));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");

        netAccessManager->get(request);
        connect(netAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onSelectedCountryDownloadFinished(QNetworkReply*)));
    }
    return true;
}

bool GenericZipCodesStep::populateDatabase()
{
    Q_EMIT progressLabelChanged(tr("Reading sources..."));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    QSqlDatabase db = QSqlDatabase::database(DB_NAME);

    Q_EMIT progressLabelChanged(tr("Populating the database..."));
    Q_EMIT progress(1);

    // create the new database
    QString req;

    // remove alread recorded codes for selected countries
    foreach(QString countryIso, m_selectedCountryList) {
        req = QString("DELETE FROM `ZIPS` WHERE `ZIPS`.`COUNTRY`='%1';").arg(countryIso.toLower());
        Utils::Database::executeSQL(req, db);
    }
    qDebug() << "postal info number:" << m_postalList.count();
    foreach(const PostalInfo pi, m_postalList) {
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

void GenericZipCodesStep::selectCountry(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QStandardItem *item = m_availableCountriesModel->itemFromIndex(index)->clone();
    m_availableCountriesModel->removeRow(index.row());
    if (m_selectedCountriesModel->findItems(item->data(Qt::DisplayRole).toString()).isEmpty())
        m_selectedCountriesModel->appendRow(item);
    m_selectedCountriesModel->sort(0);
}

void GenericZipCodesStep::deselectCountry(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QStandardItem *item = m_selectedCountriesModel->itemFromIndex(index)->clone();
    m_selectedCountriesModel->removeRow(index.row());
    m_availableCountriesModel->appendRow(item);
    m_availableCountriesModel->sort(0);
}

void GenericZipCodesStep::slotSetProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived)
    Q_UNUSED(bytesTotal)

    //TODO: implementation
}

/*!
 * Called when the downloading of the available countries index from GeoNames is finished,
 * then emits countryListDownloaded().\n
 * Downloads all available zipcodes from GeoNames.
 * Emits downloadFinished() when done.
 */
bool GenericZipCodesStep::onAvailableCountriesDownloaded()
{
    // We can catch error of the Utils::HttpDownloader --> qobject_cast sender()

    // Check if file is downloaded
    // TODO: avoid magic numbers in URL & fileName
    if (!QFileInfo(workingPath() + "/postalCodeCountryInfo").exists()) {
        LOG_ERROR("File not downloaded");
        return false;
    }

    // get XML structure of reply into a parseable format
    QDomDocument doc;
    QXmlInputSource src;
    QString countryIso3166Code/*, countryName*/;
    QMap<QLocale::Country, QString> countryIsoMap; // for e.g. Andorra: <5, "AD">
    QLocale::Country country;
    bool success = false;
    QString flagPath = Core::ICore::instance()->settings()->path(Core::ISettings::SmallPixmapPath) + "/flags/";
    LOG(flagPath);

    src.setData(Utils::readTextFile(workingPath() + "/postalCodeCountryInfo"));
    doc.setContent(src.data());

    // iter over countries
    QDomNodeList countries = doc.elementsByTagName("country");
    for (uint i = 0; i < countries.length(); i++) {

        // get a list of country info subitems
        // (countryCode, countryName, numPostalCodes, minPostalCode, maxPostalCode)
        countryIso3166Code = countries.item(i).firstChildElement("countryCode").toElement().text();
        //            countryName = countries.item(i).firstChildElement("countryName").toElement().text();

        if (countryIso3166Code.isEmpty() || (countryIso3166Code.size() != 2) /*|| countryName.isEmpty()*/) {
            qWarning() << "Country conversion error: wrong countryIso3166Code:" << countryIso3166Code;
            continue;
        }

        // transform ISO3166 to QLocale::Country
        country = Utils::countryIsoToCountry(countryIso3166Code);
        if (country == QLocale::AnyCountry) {
            // error: just skip this unknown country
            qWarning() << "Country conversion error: country" << countryIso3166Code << "has no Qt equivalent.";
            continue;
        }
        // transform ISO-3166-1 2-char countryCode to Qt's internal integer country codes
        countryIsoMap.insert(country, countryIso3166Code);
        qDebug() << country << countryIso3166Code;


        QStandardItem *item = new QStandardItem(
                    QIcon(QString("%1/%2.png").arg(flagPath, countryIso3166Code)),
                    QLocale::countryToString(country));
        item->setData(country);
        m_availableCountriesModel->appendRow(item);
        success = true;
    }

    // find default system country and add it to the selected list
    const QStandardItem *defaultCountryItem = m_availableCountriesModel->findItems(
                QLocale::countryToString(QLocale::system().country())
                ).first();
    if (defaultCountryItem)
        selectCountry(defaultCountryItem->index());
    Q_EMIT countryListDownloaded(success);
    return true;
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

    // decrease the reference counter
    --m_selectedCountriesCounter;

    if (reply->error() > 0) {
        qDebug() << reply->errorString();
    } else {
        bool success = false;

        m_postalList.clear();

        // get XML structure of reply into a parseable format
        QXmlInputSource src;
        QDomDocument doc;

        // get country code from URL, so we know which reply we have
        QString countryIso3166Code = reply->url().queryItemValue("placename").toLower();
        qDebug() << "countryIsoCode from reply:" << countryIso3166Code << reply->url().toString();

        // malformed URL?
        if (countryIso3166Code.isEmpty()) {
            if (m_selectedCountriesCounter == 0)
                reply->parent()->deleteLater();
            Q_EMIT processFinished();
            return;
        }

        // double check if this country is a valid QLocale::Country
        QLocale::Country country = Utils::countryIsoToCountry(countryIso3166Code);
        if (country == QLocale::AnyCountry) {
            LOG("country is not valid: " + countryIso3166Code);
            if (m_selectedCountriesCounter == 0)
                reply->parent()->deleteLater();
            Q_EMIT processFinished();
            return;
        }

        src.setData(reply->readAll());
        doc.setContent(src.data());

        QString postalCode, city;

        // iter over postal code chunks
        QDomNodeList codes = doc.elementsByTagName("code");
        for (uint i = 0; i < codes.length(); i++) {

            // get the postalcode subitem
            // (countryCode, countryName, numPostalCodes, minPostalCode, maxPostalCode)
            postalCode = codes.item(i).firstChildElement("postalcode").toElement().text();
            city = codes.item(i).firstChildElement("name").toElement().text();

            if (postalCode.isEmpty() || (countryIso3166Code.size() != 2) || city.isEmpty()) {
                qWarning() << "postal code conversion error: " << postalCode << city;
                continue;
            }

            m_postalList.append(PostalInfo(postalCode, city, countryIso3166Code));
            qDebug() << "imported" << postalCode << city;

            success = true;
        }
    }
    if (m_selectedCountriesCounter == 0) {
        reply->parent()->deleteLater();
        Q_EMIT processFinished();
    }
}


PostalInfo::PostalInfo(const QString &postalCode, const QString &city, const QString &country, const QString &extraCode)
{
    this->postalCode = postalCode;
    this->city = city;
    this->country = country;
    this->extraCode = extraCode;
}
