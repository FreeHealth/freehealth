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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DrugsDb::IDrugDatabase
 * Default drug database creator step.
 */

#include "idrugdatabase.h"
#include "drugdatabasedescription.h"
#include "tools.h"
#include "drug.h"
#include "drugdatabasepopulator.h"

#include <ddiplugin/constants.h>
#include <ddiplugin/routes/routesmodel.h>
#include <ddiplugin/components/componentatcmodel.h>
#include <ddiplugin/components/componentlinkerdata.h>

#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/constants_tokensandsettings.h>

#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>
#include <quazip/global.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/waitforsignal.h>
#include <utils/httpdownloader.h>
#include <utils/httpmultidownloader.h>
#include <datapackutils/constants.h>

#include <QDir>
#include <QFile>
#include <QDomDocument>
#include <QUrl>
#include <QProgressBar>
#include <QTimer>

#include <QDebug>

enum {
    LogDrugSavingChrono = false
};

using namespace DrugsDB;
using namespace DrugsDb;
using namespace DrugsDb::Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

namespace {
QString routeCsvAbsoluteFile()
{
    return QString(settings()->path(Core::ISettings::BundleResourcesPath) + DDI::Constants::ROUTES_CSV_FILENAME);
}
} // anonymous namespace

/*! Constructor of the DrugsDb::IDrugDatabase class */
IDrugDatabase::IDrugDatabase(QObject *parent) :
    QObject(parent),
    _database(0),
    _licenseType(Free),
    _serverOwner(Community),
    _spcDefaultEncoding("UTF-8"),
    _lang("en"),
    _sid(-1),
    _databasePopulator(0),
    _country(QLocale::AnyCountry),
    _manualSourceDownload(false)
{
    setObjectName("FreeDDIManager::IDrugDatabase");
    _outputFileName = "master.db";
    _databasePopulator = new DrugDatabasePopulator(this);
    _databasePopulator->initialize();
}

/*! Destructor of the DrugsDb::IDrugDatabase class */
IDrugDatabase::~IDrugDatabase()
{
    // FIXME: should we delete _database here ?
    delete _database;
    _database = 0;
}

/** Define the absolute temporary path to use.*/
void IDrugDatabase::setTempPath(const QString &absPath)
{
    _tempPath = QDir::cleanPath(absPath);
}

/** Define the absolute output path to use. The resulting database will be copied into this path. */
void IDrugDatabase::setOutputPath(const QString &absPath)
{
    _outputPath = QDir::cleanPath(absPath);
}

/**
 * Define the connection name of this database.
 * This connection name will be the same as the corresponding QSqlDatabase connection name.
 */
void IDrugDatabase::setConnectionName(const QString &connection)
{
    _connection = connection;
}

/**
 * Define the absolute output file name to use.
 * \sa setOutputPath()
 */
void IDrugDatabase::setOutputFileName(const QString &fileName)
{
    _outputFileName = fileName;
}

/**
 * Define one URL for the automatic downloading process.
 * If you define this URL, the URL will be automatically
 * downloaded during the step processing.
 * Otherwise you can overload the downloadFiles().
 * URL should point to the drugs database raw source.
 */
void IDrugDatabase::setDownloadUrl(const QString &url)
{
    _downloadingUrl = url;
}

/**
 * \fn void IDrugDatabase::DrugsDb::Internal::setManualSourceDownload(const bool state)
 * Set the manual source download. User will have to download
 * the drugs database raw source by-hand using a given URL.
 * \sa setDownloadUrl()
 */

/** Define the absolute path to the finalization SQL script to execute. This is obsolete. */
void IDrugDatabase::setFinalizationScript(const QString &absPath)
{
    if (!QFileInfo(absPath).exists())
        LOG_ERROR(tr("Finalization script file does not exist: %1").arg(absPath));
    _finalizationScriptPath = QDir::cleanPath(absPath);
}

/**
 * Define the description file for the database itself.
 * File name must be an absolute path.
 * \sa setDatapackDescriptionFile()
*/
void IDrugDatabase::setDatabaseDescriptionFile(const QString &absPath)
{
    if (!QFileInfo(absPath).exists())
        LOG_ERROR(tr("Description file does not exist: %1").arg(absPath));
    _descriptionFilePath = QDir::cleanPath(absPath);
}

/**
 * Define the description file for the datapack containing this drug database.
 * File name must be an absolute path.
 * \sa setDatapackDescriptionFile(), registerDataPack()
*/
void IDrugDatabase::setDatapackDescriptionFile(const QString &absPath)
{
    if (!QFileInfo(absPath).exists())
        LOG_ERROR(tr("Datapack description file does not exist: %1").arg(absPath));
    _datapackDescriptionFilePath = QDir::cleanPath(absPath);
}

/**
 * \fn void IDrugDatabase::setSpcHtmlFilesDefaultEncoding(const QString &encoding)
 * Define the default encoding to use to open the newly downloaded SPC HTML files. By default,
 * all files are read with the UTF-8 encoding.
 */

/**
 * \fn void IDrugDatabase::setDatabaseLanguage(const QString &lang)
 * Define the language of the database. This language will be used to import ATC data,
 * DDI data, and to link drugs components to Drug interactors/ATC codes. \n
 * Default is "en".
 */
// TODO: drug database language is already defined in the database description, should we use the description?

/** Return the absolute file path of the output database file */
QString IDrugDatabase::absoluteFilePath() const
{
    return QDir::cleanPath(QString("%1/%2").arg(outputPath()).arg(outputFileName()));
}

/** Return the Source Id of the drug database source from the drugs database. */
int IDrugDatabase::sourceId() const
{
    return _sid;
}

/**
 * Returns the server uid according to the licence type and the owner of the server. \n
 * This server uid is used for DataPack Server creation/edition/read.
 * \sa DataPack::Constants::SERVER_ASSO_NONFREE, DataPack::Constants::SERVER_ASSO_FREE
 * \sa DataPack::Constants::SERVER_COMMUNITY_NONFREE, DataPack::Constants::SERVER_COMMUNITY_FREE
 */
QString IDrugDatabase::serverUid() const
{
    if (licenseType() == IDrugDatabase::NonFree) {
        if (serverOwner() == IDrugDatabase::Community)
            return DataPack::Constants::SERVER_COMMUNITY_NONFREE;
        else
            return DataPack::Constants::SERVER_ASSO_NONFREE;

    } else {
        if (serverOwner() == IDrugDatabase::Community)
            return DataPack::Constants::SERVER_COMMUNITY_FREE;
        else
            return DataPack::Constants::SERVER_ASSO_NONFREE;
    }
    return DataPack::Constants::SERVER_COMMUNITY_FREE;
}

/** Check the existence of the database internal pointer and check if the datbase is correctly open */
bool IDrugDatabase::checkDatabase()
{
    if (!_database) {
        LOG_ERROR("Database not created");
        return false;
    }
    QSqlDatabase db = _database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR("Unable to connection database: " + _database->connectionName());
            return false;
        }
    }
    return true;
}

/** Return true if the drugs database can be created */
bool IDrugDatabase::canCreateDatabase() const
{
    return !_connection.isEmpty()
            && !_outputFileName.isEmpty()
            && !_outputPath.isEmpty();
}

/**
 * Create the DrugDatabase pointer according to the settings of the object.
 * If you call this member many times, only one database will be created.
 * Does not populate it with drugs but with non-free data if required.
 * \sa createDrugDatabase()
 */
bool IDrugDatabase::createDatabase()
{
    if (_database)
        return true;
    Q_EMIT progressLabelChanged(tr("Creating database"));
    Q_EMIT progressRangeChanged(0, 4);
    Q_EMIT progress(0);
    _database = createDrugDatabase(absoluteFilePath(), _connection);
    if (!_database) {
        LOG_ERROR("Database not created: " + _connection);
        return false;
    }

    Q_EMIT progressLabelChanged(tr("Creating database: adding source description"));
    Q_EMIT progress(1);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (!saveDrugDatabaseDescription()) {
        Q_EMIT progress(4);
        return false;
    }

    Q_EMIT progressLabelChanged(tr("Creating database: adding routes"));
    Q_EMIT progress(2);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    if (!addRoutes()) {
        Q_EMIT progress(4);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        return false;
    }

    if (licenseType() == NonFree) {
        Q_EMIT progressLabelChanged(tr("Creating database: preparing interaction data"));
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        Q_EMIT progress(3);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
//        if (!ddiCore()->isAtcInstalledInDatabase(_database)) {
//            if (!ddiCore()->addAtcDataToDatabase(_database)) {
//                LOG_ERROR("Unable to add interaction data");
//                Q_EMIT progress(4);
//                qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
//                return false;
//            }
//        }
    }
    Q_EMIT progress(4);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    return true;
}

/**
 * Add drug routes to the database. This function uses the default routes text file.
 * \note This member does not create transaction, does not commit or rollback.
 */
bool IDrugDatabase::addRoutes()
{
    // TODO: move this code into DrugsDb::DrugDatabasePopulator
    if (!checkDatabase())
        return false;

    // Routes already in database ?
    if (_database->count(Constants::Table_ROUTES, Constants::ROUTES_RID) > 0) {
        LOG(tr("Routes already in database"));
        addFinalReportMessage(tr("Routes already in database"));
        return true;
    }

    QString content = Utils::readTextFile(routeCsvAbsoluteFile());
    if (content.isEmpty()) {
        LOG_ERROR(tr("Routes file does not exist. File: %1").arg(routeCsvAbsoluteFile()));
        addFinalReportMessage(tr("Routes file does not exist. File: %1").arg(routeCsvAbsoluteFile()));
        return false;
    }
    LOG("Adding routes to database from " + routeCsvAbsoluteFile());
    QSqlQuery query(_database->database());

    // Read file
    const QStringList &lines = content.split("\n", QString::SkipEmptyParts);
    LOG(QString("Reading %1 lines").arg(lines.count()));
    foreach(const QString &line, lines) {
        if (line.startsWith("--"))
            continue;
        int id = 0;
        int rid = 0;
        QMultiHash<QString, QVariant> trLabels;
        QString systemic;
        // Parse line
        foreach(QString value, line.split(",")) {
            value = value.trimmed();
            if (id==0) {
                // is RID
                rid = value.toInt();
                ++id;
                continue;
            }
            ++id;
            value = value.remove("\"");
            int sep = value.indexOf(":");
            QString lang = value.left(sep);
            if (lang.compare("systemic") == 0) {
                systemic = value.mid(sep + 1);
            } else {
                trLabels.insertMulti(lang, value.mid(sep + 1));
            }
        }
        // Push to database
        int masterLid = Tools::addLabels(_database, -1, trLabels);
        if (masterLid == -1) {
            LOG_ERROR("Route not integrated");
            continue;
        }
        query.prepare(_database->prepareInsertQuery(DrugsDB::Constants::Table_ROUTES));
        query.bindValue(DrugsDB::Constants::ROUTES_RID, rid);
        query.bindValue(DrugsDB::Constants::ROUTES_MASTERLID, masterLid);
        query.bindValue(DrugsDB::Constants::ROUTES_SYSTEMIC, systemic);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
    }
    addFinalReportMessage(tr("Routes included to database."));
    LOG(tr("Routes included to database."));
    return true;
}

/**
 * Recreate drug routes in the database. This function uses the default routes text file.
 * \note This member creates a transaction, does commit or rollback.
 */
bool IDrugDatabase::recreateRoutes()
{
    // TODO: move this code into DrugsDb::DrugDatabasePopulator
    if (!checkDatabase())
        return false;
    QSqlDatabase db = _database->database();
    // TODO: to improve: we need to remove labels also
    db.transaction();
    _database->executeSQL(_database->prepareDeleteQuery(DrugsDB::Constants::Table_ROUTES), db);
    bool ok = addRoutes();
    if (ok)
        db.commit();
    else
        db.rollback();
    return ok;
}

/**
 * Save the database description and create a drug base source ID
 * \note This member does not create transaction, does not commit or rollback.
 */
bool IDrugDatabase::saveDrugDatabaseDescription()
{
    // TODO: move this code into DrugsDb::DrugDatabasePopulator
    if (!checkDatabase())
        return false;
    // Some checks
    if (_descriptionFilePath.isEmpty()) {
        LOG_ERROR("No description file defined");
        return false;
    }
    QFile file(_descriptionFilePath);
    if (!file.exists()) {
        LOG_ERROR("Description file does not exist: " + _descriptionFilePath);
        return false;
    }

    // Read the XML file
    QDomDocument doc;
    int line = 0;
    int col = 0;
    QString error;
    if (!doc.setContent(Utils::readTextFile(_descriptionFilePath, Utils::DontWarnUser), &error, &line, &col)) {
        LOG_ERROR(tkTr(Trans::Constants::ERROR_1_LINE_2_COLUMN_3).arg(error).arg(line).arg(col));
        return false;
    }
    DrugDatabaseDescription descr;
    QDomElement root = doc.firstChildElement("DrugDbDescriptor");
    root = root.firstChildElement("description");
    descr.fromDomElement(root);

    // Connect the database
    QSqlDatabase db = _database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR("Unable to open database");
            return false;
        }
    }

    // Add the description to database
    using namespace DrugsDB::Constants;
    QHash<int, QString> where;
    where.insert(SOURCES_DBUID, QString("='%1'").arg(descr.data(DrugDatabaseDescription::Uuid).toString()));
    QSqlQuery query(db);
    // TODO: use _sid
    if (_database->count(Table_SOURCES, SOURCES_SID, _database->getWhereClause(Table_SOURCES, where)) == 0) {
        // Save
        query.prepare(_database->prepareInsertQuery(Table_SOURCES));
        query.bindValue(SOURCES_SID, QVariant());
        query.bindValue(SOURCES_DBUID, descr.data(DrugDatabaseDescription::Uuid));
        query.bindValue(SOURCES_MASTERLID, QVariant());
        query.bindValue(SOURCES_LANG, descr.data(DrugDatabaseDescription::Language));
        query.bindValue(SOURCES_WEB, descr.data(DrugDatabaseDescription::WebLink));
        query.bindValue(SOURCES_COPYRIGHT, descr.data(DrugDatabaseDescription::Copyright));
        query.bindValue(SOURCES_LICENSE, descr.data(DrugDatabaseDescription::LicenseName));
        query.bindValue(SOURCES_DATE, QDateTime::currentDateTime().toString(Qt::ISODate));
        query.bindValue(SOURCES_DRUGS_VERSION, descr.data(DrugDatabaseDescription::Version));
        query.bindValue(SOURCES_AUTHORS, descr.data(DrugDatabaseDescription::Author));
        query.bindValue(SOURCES_VERSION, descr.data(DrugDatabaseDescription::EHRCompatVersion));
        query.bindValue(SOURCES_PROVIDER, descr.data(DrugDatabaseDescription::Vendor));
        query.bindValue(SOURCES_WEBLINK, descr.data(DrugDatabaseDescription::WebLink));
        query.bindValue(SOURCES_DRUGUID_NAME, descr.data(DrugDatabaseDescription::DrugUidName));
        query.bindValue(SOURCES_ATC, int(descr.data(DrugDatabaseDescription::IsAtcValid).toBool()));
        query.bindValue(SOURCES_INTERACTIONS, int(descr.data(DrugDatabaseDescription::IsDDIValid).toBool()));
        query.bindValue(SOURCES_COMPL_WEBSITE, descr.data(DrugDatabaseDescription::ComplementaryWebLink));
        query.bindValue(SOURCES_PACKUID_NAME, descr.data(DrugDatabaseDescription::PackUidName));
        query.bindValue(SOURCES_COMPLETION, QVariant());
        query.bindValue(SOURCES_AUTHOR_COMMENTS, QVariant());
        query.bindValue(SOURCES_DRUGNAMECONSTRUCTOR, descr.data(DrugDatabaseDescription::DrugNameConstructor));
        query.bindValue(SOURCES_FMFCOMPAT, descr.data(DrugDatabaseDescription::EHRCompatVersion));
        query.bindValue(SOURCES_OPENREACT_COMPAT, QVariant());
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("Tools", query);
            query.finish();
            return false;
        } else {
            _sid = query.lastInsertId().toInt();
        }
        query.finish();
    } else {
        // Update
        query.prepare(_database->prepareUpdateQuery(Table_SOURCES,
                                                    QList<int>()
                                                    << SOURCES_LANG
                                                    << SOURCES_WEB
                                                    << SOURCES_COPYRIGHT
                                                    << SOURCES_LICENSE
                                                    << SOURCES_DATE
                                                    << SOURCES_DRUGS_VERSION
                                                    << SOURCES_AUTHORS
                                                    << SOURCES_VERSION
                                                    << SOURCES_PROVIDER
                                                    << SOURCES_WEBLINK
                                                    << SOURCES_DRUGUID_NAME
                                                    << SOURCES_ATC // 10
                                                    << SOURCES_INTERACTIONS
                                                    << SOURCES_COMPL_WEBSITE
                                                    << SOURCES_PACKUID_NAME
                                                    << SOURCES_COMPLETION
                                                    << SOURCES_AUTHOR_COMMENTS  // 15
                                                    << SOURCES_DRUGNAMECONSTRUCTOR
                                                    << SOURCES_FMFCOMPAT
                                                    << SOURCES_OPENREACT_COMPAT // 18
                                                    , where));
        int i=0;
        query.bindValue(i, descr.data(DrugDatabaseDescription::Language));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::WebLink));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::Copyright));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::LicenseName));
        query.bindValue(++i, QDateTime::currentDateTime().toString(Qt::ISODate));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::Version));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::Author));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::EHRCompatVersion));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::Vendor));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::WebLink));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::DrugUidName));
        query.bindValue(++i, int(descr.data(DrugDatabaseDescription::IsAtcValid).toBool()));
        query.bindValue(++i, int(descr.data(DrugDatabaseDescription::IsDDIValid).toBool()));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::ComplementaryWebLink));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::PackUidName));
        query.bindValue(++i, QVariant());
        query.bindValue(++i, QVariant());
        query.bindValue(++i, descr.data(DrugDatabaseDescription::DrugNameConstructor));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::EHRCompatVersion));
        query.bindValue(++i, QVariant());
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("Tools", query);
            return false;
        }
    }
    query.finish();
    // Save database labels
    QMultiHash<QString, QVariant> trLabels;
    foreach(const QString &lang, descr.availableLanguages())
        trLabels.insert(lang, descr.data(DrugDatabaseDescription::Label));
    // insert labels
    int masterLid = Tools::addLabels(_database, -1, trLabels);
    if (masterLid == -1) {
        LOG_ERROR_FOR("Tools","Unable to add source");
        return false;
    }

    if (_sid==-1) {
        _sid = _database->getSourceId(descr.data(DrugDatabaseDescription::Uuid).toString());
        if (_sid==-1)
            LOG_ERROR("No Source");
    }
    using namespace DrugsDB::Constants;
    query.prepare(_database->prepareUpdateQuery(Table_SOURCES, SOURCES_MASTERLID, where));
    query.bindValue(0, masterLid);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Tools", query);
    }
    query.finish();

    addFinalReportMessage(tr("Saving database description: %1.").arg(descr.data(DrugDatabaseDescription::Uuid).toString()));

    return true;
}

/**
 * Update the drug component to ATC code linking completion percentage
 * \note This member does not create transaction, does not commit or rollback.
 */
bool IDrugDatabase::updateDatabaseCompletion(int completion)
{
    // TODO: move this code into DrugsDb::DrugDatabasePopulator
    if (!checkDatabase())
        return false;
    QHash<int, QString> where;
    where.insert(DrugsDB::Constants::SOURCES_SID, QString("=%1").arg(_sid));
    QSqlQuery query(_database->database());
    using namespace DrugsDB::Constants;
    query.prepare(_database->prepareUpdateQuery(Table_SOURCES, SOURCES_COMPLETION, where));
    query.bindValue(0, completion);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Tools", query);
        return false;
    }
    query.finish();
    LOG(tr("Updating confidence indice to %1.").arg(completion));
    addFinalReportMessage(tr("Updating confidence indice to %1.").arg(completion));
    return true;
}

/**
 * Save a list of drugs in the database. The drugs vector will be sorted.
 * \note This member creates a transaction, does commit or rollback.
 */
bool IDrugDatabase::saveDrugsIntoDatabase(QVector<Drug *> drugs)
{
    // This general chrono is used to log the whole process duration
    QTime chrono;
    chrono.start();

    // TODO: move this code into DrugsDb::DrugDatabasePopulator
    if (!checkDatabase())
        return false;
    if (_sid==-1) {
        LOG_ERROR_FOR("Drug", "NO SID DEFINED");
        return false;
    }
    LOG(tr("Starting to save %1 drugs to database").arg(drugs.count()));

    // Clear database
    QHash<int, QString> w;
    using namespace DrugsDB::Constants;
    QSqlDatabase db = _database->database();
    db.transaction();
    _database->executeSQL(_database->prepareDeleteQuery(Table_MASTER, w), db);
    _database->executeSQL(_database->prepareDeleteQuery(Table_DRUGS, w), db);
    _database->executeSQL(_database->prepareDeleteQuery(Table_COMPO, w), db);
    _database->executeSQL(_database->prepareDeleteQuery(Table_DRUG_ROUTES, w), db);
    _database->executeSQL(_database->prepareDeleteQuery(Table_DRUG_FORMS, w), db);
    _database->executeSQL(_database->prepareDeleteQuery(Table_MOLS, w), db);
    _database->executeSQL(_database->prepareDeleteQuery(Table_LK_MOL_ATC, w), db);
    _database->executeSQL(_database->prepareDeleteQuery(Table_PACKAGING, w), db);
    // TODO: delete COMPOSITION, DRUG_ROUTES, LABELS_LINK, LABELS */

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    db.commit();

    Utils::Log::logTimeElapsed(chrono, this->objectName(), "Save drugs: step 1: Clear db");

    // get distinct component names
    QStringList molnames;
    foreach(Drug *drug, drugs) {
        drug->setData(Drug::SID, _sid);
        foreach(Component *compo, drug->components()) {
            const QString &name = compo->data(Component::Name).toString();
            if (!molnames.contains(name))
                molnames.append(name);
        }
    }
    qSort(molnames);

    db.transaction();
    QHash<int, QString> mids = saveMoleculeIds(molnames);
    db.commit();

    qSort(drugs.begin(), drugs.end(), Drug::lessThanOnNames);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    Utils::Log::logTimeElapsed(chrono, this->objectName(), "Save drugs: step 2: Get all components");

    db.transaction();
    QSqlQuery query(db);
    using namespace DrugsDB::Constants;
    int n = 0;
    Q_EMIT progressRangeChanged(0, drugs.count());
    DDI::RoutesModel *routesModel = new DDI::RoutesModel(this);

    QTime chrono2;
    chrono2.start();
    foreach(Drug *drug, drugs) {
        ++n;
        if (n % 10 == 0) {
            // TODO: manage a "cancelation state"
            Q_EMIT progress(n);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        // Authorizations && get Master_LID
        QMultiHash<QString, QVariant> multiLang;
        foreach(const QString &lang, drug->availableLanguages()) {
            multiLang.insertMulti(lang, drug->data(Drug::Authorization, lang));
        }
        int aidLID = Tools::addLabels(_database, -1, multiLang);
        multiLang.clear();

        if (LogDrugSavingChrono)
            Utils::Log::logTimeElapsed(chrono2, this->objectName(), "   Drug authorization");

        // Table MASTER && DRUGS
        foreach(const QString &lang, drug->availableLanguages()) {
            // Master
            query.prepare(_database->prepareInsertQuery(Table_MASTER));
            query.bindValue(MASTER_DID, QVariant());
            query.bindValue(MASTER_UID1, drug->data(Drug::Uid1, lang).toString());
            query.bindValue(MASTER_UID2, drug->data(Drug::Uid2, lang).toString());
            query.bindValue(MASTER_UID3, drug->data(Drug::Uid3, lang).toString());
            query.bindValue(MASTER_OLDUID, drug->data(Drug::OldUid, lang).toString());
            query.bindValue(MASTER_SID, drug->data(Drug::SID, lang).toString());
            if (query.exec()) {
                drug->setData(Drug::DID, query.lastInsertId(), lang);
            } else {
                LOG_QUERY_ERROR_FOR("Drugs", query);
                query.finish();
                db.rollback();
                return false;
            }
            query.finish();

            // Drugs
            query.prepare(_database->prepareInsertQuery(Table_DRUGS));
            query.bindValue(DRUGS_ID, QVariant());
            query.bindValue(DRUGS_DID, drug->data(Drug::DID, lang).toString());
            query.bindValue(DRUGS_SID, drug->data(Drug::SID, lang).toString().replace("'","''"));
            query.bindValue(DRUGS_NAME, drug->data(Drug::Name, lang).toString().replace("'","''"));
            query.bindValue(DRUGS_ATC_ID, drug->data(Drug::AtcId, lang).toString());
            query.bindValue(DRUGS_STRENGTH, drug->data(Drug::Strength, lang).toString());
            query.bindValue(DRUGS_AID_MASTER_LID, aidLID);
            query.bindValue(DRUGS_VALID, drug->data(Drug::Valid, lang).toInt());
            query.bindValue(DRUGS_MARKET, drug->data(Drug::Marketed, lang).toInt());
            query.bindValue(DRUGS_LINK_SPC, drug->data(Drug::Spc, lang).toString());
            query.bindValue(DRUGS_EXTRA_XML, drug->data(Drug::Xml, lang).toString());

            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR("Drugs", query);
                db.rollback();
                return false;
            }
            query.finish();

        }

        if (LogDrugSavingChrono)
            Utils::Log::logTimeElapsed(chrono2, this->objectName(), "   Drug base data");

        // Composition
        foreach(Component *compo, drug->components()) {
            foreach(const QString &lang, drug->availableLanguages()) {
                query.prepare(_database->prepareInsertQuery(Table_COMPO));
                query.bindValue(COMPO_ID, QVariant());
                query.bindValue(COMPO_DID, drug->data(Drug::DID));
                query.bindValue(COMPO_MID, mids.key(compo->data(Component::Name, lang).toString().toUpper()));
                query.bindValue(COMPO_STRENGTH, compo->data(Component::Strength, lang).toString().replace("'","''"));
                query.bindValue(COMPO_STRENGTH_NID, QVariant());
                query.bindValue(COMPO_DOSE_REF, compo->data(Component::Dose, lang).toString().replace("'","''"));
                query.bindValue(COMPO_REF_NID, QVariant());
                query.bindValue(COMPO_NATURE, compo->data(Component::Nature, lang).toString());
                query.bindValue(COMPO_LK_NATURE, compo->data(Component::NatureLink, lang).toString());
                if (!query.exec()) {
                    LOG_QUERY_ERROR_FOR("Drugs", query);
                    db.rollback();
                    return false;
                }
                query.finish();
            }
        }

        if (LogDrugSavingChrono)
            Utils::Log::logTimeElapsed(chrono2, this->objectName(), "   Drug composition");

        // Routes
        const QStringList &routes = drug->data(Drug::Routes).toStringList();
        if (!routes.isEmpty()) {
            // Create the drugs route links
            QList<int> routesId = routesModel->routesId(routes);

            if (routesId.isEmpty()) {
                LOG_ERROR(QString("Drug routes not found: %1 - %2").arg(drug->data(Drug::Name).toString()).arg(routes.join(";")));
                continue;
            }

            foreach(int rid, routesId) {
                query.prepare(_database->prepareInsertQuery(Table_DRUG_ROUTES));
                query.bindValue(DRUG_ROUTES_DID, drug->data(Drug::DID).toInt());
                query.bindValue(DRUG_ROUTES_RID, rid);
                if (!query.exec()) {
                    LOG_QUERY_ERROR_FOR("Drugs", query);
                    query.finish();
                    db.rollback();
                    return false;
                }
                query.finish();
            }
        }

        if (LogDrugSavingChrono)
            Utils::Log::logTimeElapsed(chrono2, this->objectName(), "   Drug routes");

        // Forms
        // TODO: improve drugs form management (like routes we need to create a specific model & specific set of data)
        foreach(const QString &lang, drug->availableLanguages()) {
            const QStringList &forms = drug->data(Drug::Forms, lang).toStringList();
            if (!forms.isEmpty()) {
                foreach(const QString &f, forms)
                    multiLang.insertMulti(lang, f);
                int formsMasterId = Tools::addLabels(_database, -1, multiLang);
                if (formsMasterId==-1) {
                    LOG_ERROR_FOR("Drug", "Forms not saved");
                }
                // Add formsMasterId to DRUGS record
                query.prepare(_database->prepareInsertQuery(Table_DRUG_FORMS));
                query.bindValue(DRUG_FORMS_DID, drug->data(Drug::DID));
                query.bindValue(DRUG_FORMS_MASTERLID, formsMasterId);
                if (!query.exec()) {
                    LOG_QUERY_ERROR_FOR("Drugs", query);
                    query.finish();
                    db.rollback();
                    return false;
                }
                query.finish();
            }
        }

        if (LogDrugSavingChrono)
            Utils::Log::logTimeElapsed(chrono2, this->objectName(), "   Drug forms");

    }
    query.finish();
    db.commit();

    Utils::Log::logTimeElapsed(chrono, this->objectName(), "Save drugs: step 3: Save all drugs");
    LOG(tr("Added %1 drugs to database.").arg(drugs.count()));
    addFinalReportMessage(tr("Added %1 drugs to database.").arg(drugs.count()));

    return true;
}

/**
 * Save a list of molecules and return a hash containing the MoleculeID
 * as key and the molecule name os value
 * \note This member does not create transaction, does not commit or rollback.
 */
QHash<int, QString> IDrugDatabase::saveMoleculeIds(const QStringList &molnames)
{
    // TODO: move this code into DrugsDb::DrugDatabasePopulator
    // TODO: rename molecule -> component
    QHash<int, QString> mids;
    if (!checkDatabase())
        return mids;
    using namespace DrugsDB::Constants;
    QString req;

    _database->database().transaction();
    QSqlQuery query(_database->database());

    foreach(const QString &name, molnames) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        // Ask for an existing MID
        //        req = QString("SELECT MID FROM MOLS WHERE NAME=\"%1\" AND SID=\"%2\";").arg(name).arg(sid);
        QHash<int, QString> w;
        w.insert(MOLS_NAME, QString("=\"%1\"").arg(name));
        w.insert(MOLS_SID, QString("='%1'").arg(_sid));
        req = _database->select(Table_MOLS, MOLS_MID, w);
        if (query.exec(req)) {
            if (query.next()) {
                // is already in the table MOLS
                mids.insert(query.value(0).toInt(), name);
                continue;
            }
        } else {
            LOG_QUERY_ERROR_FOR("Tools", query);
            query.finish();
            _database->database().rollback();
            mids.clear();
            return mids;
        }
        query.finish();

        // create a new MID
        query.prepare(_database->prepareInsertQuery(Table_MOLS));
        query.bindValue(MOLS_MID, QVariant());
        query.bindValue(MOLS_NAME, name);
        query.bindValue(MOLS_SID, _sid);
        query.bindValue(MOLS_WWW, QVariant());
        if (query.exec()) {
            mids.insert(query.lastInsertId().toInt(), name);
            continue;
        } else {
            LOG_QUERY_ERROR_FOR("Tools", query);
            query.finish();
            _database->database().rollback();
            mids.clear();
            return mids;
        }
        query.finish();
    }
    _database->database().commit();
    addFinalReportMessage(tr("Added %1 components to database.").arg(molnames.count()));
    return mids;
}

/**
 * Add the ATC classification to the drug database. This classification is REQUIRED
 * by all non-free steps (that need ATC to compute interactions).
 * \sa DrugsDB::DrugDrugInteractionCore::addAtcDataToDatabase()
 * \note This member creates a transaction, commit or rollback.
 */
bool IDrugDatabase::addAtc()
{
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progressLabelChanged(tr("Adding ATC classification to database."));
    Q_EMIT progress(0);

    if (!_database)
        return false;
    if (!_database->database().isOpen()) {
        if (!_database->database().open())
            return false;
    }

    _database->database().transaction();
    bool ok = _databasePopulator->saveAtcClassification(_database);
    if (ok) {
        _database->database().commit();
        addFinalReportMessage(tr("Added ATC data to database."));
    } else {
        _database->database().rollback();
        addFinalReportMessage(tr("ERROR: unable to include ATC data to database."));
    }

    return ok;
}

/**
 * Add the drug-drug interaction data to the drug database.
 * \sa DrugsDB::DrugDrugInteractionCore::addDrugDrugInteractionsToDatabase()
 * \note This member creates a transaction, commit or rollback.
 */
bool IDrugDatabase::addDrugDrugInteractions()
{
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progressLabelChanged(tr("Adding ATC classification to database."));
    Q_EMIT progress(0);

    if (!_database)
        return false;
    if (!_database->database().isOpen()) {
        if (!_database->database().open())
            return false;
    }

    _database->database().transaction();
    bool ok = _databasePopulator->saveDrugDrugInteractions(_database);
    if (ok) {
        _database->database().commit();
        addFinalReportMessage(tr("Added drug-drug interactions to database."));
    } else {
        _database->database().rollback();
        addFinalReportMessage(tr("ERROR: unable to include drug-drug interactions to database."));
    }

    return ok;
}

/**
 * Add the PIMs (potentially inappropriate medication in elderly) data to the drug database.
 * \sa DrugsDB::DrugDrugInteractionCore::addPimsToDatabase()
 */
bool IDrugDatabase::addPims()
{
    // TODO: return _databasePopulator->savePIMs(_database);
    return false;
}

/**
 * Add the pregnancy drug compatibility data to the drug database.
 * \sa DrugsDB::DrugDrugInteractionCore::addPregnancyDataToDatabase()
 */
bool IDrugDatabase::addPregnancyCheckingData()
{
    // TODO: return _databasePopulator->savePregnancyData(_database);
    return true;
}

/**
 * Downloads all SPC contents according to the DRUGS_LINK_SPC field.
 * - Get all disctinct SPC contents
 * - Download all contents
 * - Add content to the database (also link drug to spc content)
 */
bool IDrugDatabase::downloadSpcContents()
{
    // TODO: Create specific object and move this code

    // The Summary of Product Characteristics can be automatically downloaded and
    // inserted in the drugs database. The following code will:
    // - read all the recorded spc links in the drugs table,
    // - download all links in a specific output path and create a local cache. If files are
    //   already available in the path they are not re-downloaded. To force a total update, you
    //   just have to remove the output path and its content from your local storage.
    // - populate the database with the SPC contents.

    // Connect the database
    QSqlDatabase db = _database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            return false;
        }
    }

    // Get all drugs SPC links recorded in the database
    using namespace DrugsDB::Constants;
    QSqlQuery query(db);
    QHash<int, QString> where;
    where.insert(Constants::DRUGS_LINK_SPC, "IS NOT NULL");
    where.insert(Constants::DRUGS_SID, QString("='%1'").arg(_sid));
    QString req = _database->selectDistinct(Table_DRUGS, DRUGS_LINK_SPC, where);
    if (query.exec(req)) {
        while (query.next()) {
            if (query.value(0).toString().isEmpty())
                continue;
            _spcUrls << QUrl(query.value(0).toString());
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    LOG(QString("Number of SPC links: %1").arg(_spcUrls.count()));
    addFinalReportMessage(tr("Starting %1 Pharmaceutical Drug Summary of Product Caracteristics saved in the database.").arg(_spcUrls.count()));

    // Create the SPC url downloader
    Utils::HttpMultiDownloader *_multiDownloader = new Utils::HttpMultiDownloader(this);
    _multiDownloader->setUseUidAsFileNames(true);
    QDir().mkpath(tempPath() + "/spc/");
    _multiDownloader->setOutputPath(tempPath() + "/spc/");
    _multiDownloader->readXmlUrlFileLinks();

    // Remove already downloaded files from the queue
    for(int i = _spcUrls.count() - 1; i >= 0; --i) {
        if (_multiDownloader->urls().contains(_spcUrls.at(i)))
            // TODO: does the URL needs to be updated ???
            _spcUrls.removeAt(i);
    }

    // TODO: emit signals for progressbar

    // Start the download
    _multiDownloader->setUrls(_spcUrls);
    connect(_multiDownloader, SIGNAL(allDownloadFinished()), this, SLOT(onAllSpcDownloadFinished()));
    _multiDownloader->startDownload();
    return true;
}

/**
 * When all SPC files are downloaded, populate the drugs database.
 */
bool IDrugDatabase::onAllSpcDownloadFinished()
{
    // Here all SPC files are downloaded.
    // We need to work a bit on HTML files (getting CSS content, JS scripts...)

    // Get the external CSS content of the HTML files (<link ... type="...css"> in the meta/head)
    // We suppose here that all the downloaded content has the same CSS file set.
    // So we read the first available file, extract the CSS file names,
    // download them to the spc output path. Then we must edit all downloaded HTML file and
    // make them point to these CSS files.

    Utils::HttpMultiDownloader *_multiDownloader = qobject_cast<Utils::HttpMultiDownloader *>(sender());
    if (!_multiDownloader) {
        QTimer::singleShot(1, this, SIGNAL(spcProcessFinished()));
        return false;
    }
    disconnect(_multiDownloader, SIGNAL(allDownloadFinished()), this, SLOT(onAllSpcDownloadFinished()));
    addFinalReportMessage(tr("Pharmaceutical Drug Summary of Product Caracteristics downloaded."));

    // TODO: manage progressbar signals

    // Save the HttpMultiDownloader XML cache file (only if there was downloads)
    if (_spcUrls.count() > 0)
        _multiDownloader->saveXmlUrlFileLinks();

    // Nothing downloaded -> return
    const QList<QUrl> &downloadedUrls = _multiDownloader->downloadedUrls();
    if (downloadedUrls.isEmpty()) {
        LOG("No SPC file downloaded");
        QTimer::singleShot(1, this, SIGNAL(spcProcessFinished()));
        return true;
    }
    addFinalReportMessage(tr("%1 Pharmaceutical Drug Summary of Product Caracteristics downloaded.").arg(downloadedUrls.count()));

    // TODO: manage downloader errors

    // As we have supposed that all HMTL have the same CSS set we only screen one file.
    // An improvement can be to screen *all* the HTML files, and to manage database to correctly
    // link HTML and CSS content of SPC documents.


    // Get the first available SPC file in the output dir
    const QUrl &firstUrl = downloadedUrls.at(0);
    QString content = Utils::readTextFile(_multiDownloader->outputAbsoluteFileName(firstUrl), _spcDefaultEncoding);
    QList<QUrl> cssFiles;

    // Extract all stylesheet file from the "<link" tag
    QStringList cssExtractedFileName = Utils::htmlGetLinksToCssContent(content);

    // Manage relative filePath
    foreach(const QString &css, cssExtractedFileName) {
        if (QFileInfo(css).isRelative()) {
             cssFiles << firstUrl.resolved(QUrl(css)).toString();
        } else {
            cssFiles << QUrl(css);
        }
    }

    // Download files the stylesheets files with a new downloader
    Utils::HttpMultiDownloader *_cssMultiDownloader = new Utils::HttpMultiDownloader(this);
    if (!cssFiles.isEmpty()) {
        _cssMultiDownloader->setUseUidAsFileNames(true);
        QDir().mkpath(tempPath() + "/spc/css/");
        _cssMultiDownloader->setOutputPath(tempPath() + "/spc/css/");
        _cssMultiDownloader->clearXmlUrlFileLinks();
        _cssMultiDownloader->setUrls(cssFiles);
        _cssMultiDownloader->startDownload();
        Utils::waitForSignal(_cssMultiDownloader, SIGNAL(allDownloadFinished()), 100000);
    }

    // Connect the database
    QSqlDatabase db = _database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            return false;
        }
    }
    // Get all drugs SPC links recorded in the database
    // This is usefull if the drugs database should not be populated with all
    // the downloaded SPC files (eg: when testing database generation).
    using namespace DrugsDB::Constants;
    QSqlQuery query(db);
    QHash<int, QString> where;
    where.insert(Constants::DRUGS_LINK_SPC, "IS NOT NULL");
    where.insert(Constants::DRUGS_SID, QString("='%1'").arg(_sid));
    QString req = _database->selectDistinct(Table_DRUGS, DRUGS_LINK_SPC, where);
    QList<QUrl> spcUrls;
    if (query.exec(req)) {
        while (query.next()) {
            if (query.value(0).toString().isEmpty())
                continue;
            spcUrls << query.value(0).toString();
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    LOG(QString("Number of SPC links to add in the database: %1").arg(spcUrls.count()));

    // Create the SPC resources content (CSS, JS...)
    QList<SpcContentResources> spcResources;
    int i = 0;
    foreach(const QUrl &url, cssFiles) {
        QString absFilePath = _cssMultiDownloader->outputAbsoluteFileName(url);
        SpcContentResources resource;
        resource.type = "text/css";
        resource.name = cssExtractedFileName.at(i); // use the original filepath (the one extracted from the html file)
        resource.content = Utils::readTextFile(absFilePath, Utils::DontWarnUser);
        spcResources << resource;
        ++i;
    }

    // Create the SPC content and send them to the drugs database
    foreach(const QUrl &url, spcUrls) {
        SpcContent content;
        content.url = url.toString();
        content.html = Utils::readTextFile(_multiDownloader->outputAbsoluteFileName(url), _spcDefaultEncoding, Utils::DontWarnUser);
        content.html = Utils::htmlReplaceAccents(content.html);
        content.resources = spcResources;
        saveDrugSpc(content);
    }

    addFinalReportMessage(tr("%1 Pharmaceutical Drug Summary of Product Caracteristics saved in the database.").arg(spcUrls.count()));

    QTimer::singleShot(1, this, SIGNAL(spcProcessFinished()));
    return true;
}

/**
 * Save the SPC content.
 * \warning code does not manage duplicates
 * \note This member creates a transaction, commit or rollback.
 */
bool IDrugDatabase::saveDrugSpc(const SpcContent &content)
{
    if (content.html.isEmpty()) {
        LOG_ERROR("SPC content is empty");
        return false;
    }

    if (!checkDatabase())
        return false;
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    using namespace DrugsDB::Constants;
    _database->database().transaction();
    QSqlQuery query(_database->database());
    // Get Ids
    int spcId = -1;
    int resourceLinkId = -1;
    if (content.resources.count() > 0)
        resourceLinkId = _database->max(Table_SPC_CONTENT, SPCCONTENT_SPCCONTENT_RESOURCES_LINK_ID).toInt() + 1;

    // Insert the spc content to the SPCCONTENT table
    query.prepare(_database->prepareInsertQuery(Table_SPC_CONTENT));
    query.bindValue(SPCCONTENT_ID, QVariant());
    query.bindValue(SPCCONTENT_LABEL, content.label);
    query.bindValue(SPCCONTENT_URL_SOURCE, content.url);
    query.bindValue(SPCCONTENT_DATEOFDOWNLOAD, QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(SPCCONTENT_HTMLCONTENT, qCompress(content.html.toUtf8(), 9));
    query.bindValue(SPCCONTENT_SPCCONTENT_RESOURCES_LINK_ID, resourceLinkId);
    if (query.exec()) {
        spcId = query.lastInsertId().toInt();
    } else {
        LOG_QUERY_ERROR_FOR("Tools", query);
        query.finish();
        _database->database().rollback();
        return false;
    }
    query.finish();

    // Insert all resources
    foreach(const SpcContentResources &resource, content.resources) {
        int resourceId = -1;
        // Resource already available?
        QHash<int, QString> where;
        where.insert(SPCCONTENTRESOURCES_TYPE, QString("='%1'").arg(resource.type));
        where.insert(SPCCONTENTRESOURCES_NAME, QString("='%1'").arg(resource.name));
        if (query.exec(_database->select(Table_SPC_CONTENTRESOURCE, SPCCONTENTRESOURCES_ID, where))) {
            if (query.next())
                resourceId = query.value(0).toInt();
        } else {
            LOG_QUERY_ERROR_FOR("Tools", query);
            query.finish();
            _database->database().rollback();
            return false;
        }
        query.finish();

        if (resourceId == -1) {
            // Add resource to database
            query.prepare(_database->prepareInsertQuery(Table_SPC_CONTENTRESOURCE));
            query.bindValue(SPCCONTENTRESOURCES_ID, QVariant());
            query.bindValue(SPCCONTENTRESOURCES_TYPE, resource.type);
            query.bindValue(SPCCONTENTRESOURCES_NAME, resource.name);
            query.bindValue(SPCCONTENTRESOURCES_CONTENT, resource.content);
            if (query.exec()) {
                resourceId = query.lastInsertId().toInt();
            } else {
                LOG_QUERY_ERROR_FOR("Tools", query);
                query.finish();
                _database->database().rollback();
                return false;
            }
            query.finish();
        }

        // Populate the resources link table
        query.prepare(_database->prepareInsertQuery(Table_SPC_CONTENTRESOURCE_LINK));
        query.bindValue(SPCCONTENT_RESOURCES_LINK_ID, resourceLinkId);
        query.bindValue(SPCCONTENT_SPCCONTENTRESOURCES_ID, resourceId);
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("Tools", query);
            query.finish();
            _database->database().rollback();
            return false;
        }
        query.finish();
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    // Insert DRUG_SPC table data
    int did = -1;

    QHash<int, QString> where;
    where.insert(DRUGS_LINK_SPC, QString("='%1'").arg(content.url));
    QString req = _database->select(Table_DRUGS, DRUGS_ID, where);
    if (query.exec(req)) {
        if (query.next())
            did = query.value(0).toInt();
    } else {
        LOG_QUERY_ERROR_FOR("Tools", query);
        query.finish();
        _database->database().rollback();
        return false;
    }
    query.finish();
    query.prepare(_database->prepareInsertQuery(Table_DRUG_SPC));
    query.bindValue(DRUG_SPC_DID, did);
    query.bindValue(DRUG_SPC_SPCCONTENT_ID, spcId);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Tools", query);
        query.finish();
        _database->database().rollback();
        return false;
    }
    query.finish();

    _database->database().commit();
    return true;
}

/** Return the final report of the current processing */
QStringList IDrugDatabase::finalReport() const
{
    return _finalReport;
}

/** Create all object path (temp, output, download...) */
bool IDrugDatabase::createTemporaryStorage()
{
    // Create the tempPath
    if (!QDir().mkpath(_tempPath))
        LOG_ERROR("Unable to create outputPath :" + _tempPath);
    else
        LOG("Tmp dir created");

    // Create database output dir
    if (!QDir().exists(_outputPath)) {
        if (!QDir().mkpath(_outputPath)) {
            LOG_ERROR("Unable to create French database output path :" + _outputPath);
        } else {
            LOG("Drugs database output dir created");
        }
    }
    return true;
}

/** Automatically clean the output database (removes the output file). */
bool IDrugDatabase::cleanTemporaryStorage()
{
    QFile(absoluteFilePath()).remove();
    return true;
}

bool IDrugDatabase::startProcessing(ProcessTiming timing, SubProcess subProcess)
{
    bool ok = true;
    _currentTiming = timing;
    _currentSubProcess = subProcess;
    switch (subProcess) {
    case Initialization:
        switch (timing) {
        case PreProcess:
            ok = createTemporaryStorage();
            break;
        case Process:
            connect(this, SIGNAL(downloadFinished()), this, SLOT(onSubProcessFinished()), Qt::UniqueConnection);
            ok = startDownload();
            return ok;
        case PostProcess:
            ok = unzipFiles();
            break;
        } // switch
        break;
    case Main:
        switch (timing) {
        case PreProcess:
            ok = prepareData();
            break;
        case Process:
            ok = populateDatabase();
            if (ok && licenseType() == NonFree) {
                ok = linkDrugsComponentsAndDrugInteractors()
                        && addDrugDrugInteractions()
                        && addPims()
                        && addPregnancyCheckingData();
            }
            break;
        case PostProcess:
            connect(this, SIGNAL(spcProcessFinished()), this, SLOT(onSubProcessFinished()), Qt::UniqueConnection);
            ok = downloadSpcContents();
            return ok;
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
    return ok;
}

void IDrugDatabase::onSubProcessFinished()
{
    WARN_FUNC << _currentTiming << _currentSubProcess;
    Q_EMIT subProcessFinished(_currentTiming, _currentSubProcess);
}

/**
 * Download the URL to the tempPath().
 * \sa setDownloadUrl()
 */
bool IDrugDatabase::startDownload()
{
    Utils::HttpDownloader *dld = new Utils::HttpDownloader;
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    connect(dld, SIGNAL(downloadProgressPermille(int)), this, SIGNAL(progress(int)));

    Q_EMIT progressRangeChanged(0, 1000);
    Q_EMIT progress(0);

    dld->setOutputPath(tempPath());
    dld->setUrl(QUrl(downloadUrl()));
    dld->startDownload();
    addFinalReportMessage(tr("Starting raw source files download."));
    return true;
}

/**
 * Unzip the downloaded file.
 * \sa setDownloadUrl(), downloadFiles()
 */
bool IDrugDatabase::unzipFiles()
{
    // check file
    QString fileName = tempPath() + QDir::separator() + QFileInfo(downloadUrl()).fileName();
    if (!QFile(fileName).exists()) {
        LOG_ERROR(QString("No files to unzip."));
        LOG_ERROR(QString("Please download files."));
        return false;
    }

    // unzip files using QProcess
    LOG(QString("Starting unzipping %1 files to: %2")
        .arg(connectionName())
        .arg(fileName));

    bool ok = QuaZipTools::unzipFile(fileName, tempPath());
    if (ok)
        addFinalReportMessage(tr("Raw source files unzipped."));
    return ok;
}

/**
 * Automatically register the drug database to the DataPackPlugin::DataPackCore according
 * to the DrugsDB::IDrugDatabase::LicenseType and the DrugsDB::IDrugDatabase::ServerOwner
 * of the object.
 * \sa DataPackPlugin::DataPackCore::registerDataPack()
 */
bool IDrugDatabase::registerDataPack()
{
    QString server;
    if (_licenseType == Free) {
        if (_serverOwner == Community) {
            server = DataPack::Constants::SERVER_COMMUNITY_FREE;
        } if (_serverOwner == FrenchAssociation) {
            server = DataPack::Constants::SERVER_ASSO_FREE;
        }
    } else {
        if (_serverOwner == Community) {
            server = DataPack::Constants::SERVER_COMMUNITY_NONFREE;
        } if (_serverOwner == FrenchAssociation) {
            server = DataPack::Constants::SERVER_ASSO_NONFREE;
        }
    }
//    DataPackPlugin::DataPackQuery query;
//    query.setDescriptionFileAbsolutePath(_datapackDescriptionFilePath);
//    query.setOriginalContentFileAbsolutePath(_outputPath + QDir::separator() + _connection + QDir::separator() + _outputFileName);
//    query.setZipOriginalFile(true);
//    if (!dataPackCore()->registerDataPack(query, server)) {
//        LOG_ERROR("Unable to register datapack for drugs database: " + connectionName());
//        return false;
//    }
//    LOG(QString("Registered datapack for drugs database: %1; in server %2").arg(connectionName()).arg(server));
    return true;
}

/** Clear the cached report. */
void IDrugDatabase::clearFinalReport()
{
    _finalReport.clear();
}

void IDrugDatabase::showReportDialog() const
{
    Utils::withButtonsMessageBox(tr("Pharmaceutical drug database report"),
                                 tr("Pharmaceutical drug database processing finished. The full report "
                                    "is available in the detailled part of this dialog. Click 'Details' to "
                                    "access the report."),
                                 _finalReport.join("\n"),
                                 QMessageBox::Ok, QMessageBox::Ok,
                                 tr("Pharmaceutical drug database report"));
}

/** Add a message that will be presented to the user with the final processing dialog. */
void IDrugDatabase::addFinalReportMessage(const QString &msg)
{
    if (!_finalReport.contains(msg))
        _finalReport.append(msg);
}

/** Create the drug database using the absolute path \e absPath, and the connectionName \e connection */
DrugsDB::Internal::DrugBaseEssentials *IDrugDatabase::createDrugDatabase(const QString &absPath, const QString &connection)
{
    // Do not recreate the database
    if (_database)
        return _database;

    // Create a drugessentialbase
    _database = new DrugsDB::Internal::DrugBaseEssentials;
    if (!connection.isEmpty())
        _database->setConnectionName(connection);
    _database->initialize(absPath, true);
    QSqlDatabase db = _database->database();
    if (!db.isOpen() && db.isValid()) {
        LOG_ERROR("Unable to connect to drugs database: " + connection);
        return 0;
    }
    LOG(tr("Drug database created: %1/%2").arg(db.hostName()).arg(db.databaseName()));
    addFinalReportMessage(tr("Drug database created: %1/%2").arg(db.hostName()).arg(db.databaseName()));
    return _database;
}

/**
 * This procedure will
 * - check the database (exists, rights allowed - only works with NonFree databases)
 * - request the internal DDI::ComponentAtcModel to
 *   create the links between drugs components and drugs interactors / ATC codes
 * - send the resulting links to the drugs database
 */
bool IDrugDatabase::linkDrugsComponentsAndDrugInteractors()
{

//    if (licenseType() == Free)
//        return true;

    // Connect to databases
    if (!checkDatabase())
        return false;

    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progressLabelChanged(tr("Linking drugs components to ATC codes. Step 1"));
    Q_EMIT progress(0);
    addFinalReportMessage(tr("*** Starting drug component to ATC linkage ***"));

    // Create component linker data
    DDI::ComponentLinkerData data;
    data.setAtcLanguage(_lang);

    // Get all ATC-codes
    QHash<QString, int> ids;
    QSqlQuery query(_database->database());
    QString req = _database->select(Constants::Table_ATC, QList<int>()
                                    << Constants::ATC_ID
                                    << Constants::ATC_CODE);
    if (query.exec(req)) {
        while (query.next()) {
            ids.insert(query.value(1).toString(), query.value(0).toInt());
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    data.setAtcCodeIds(ids);
    query.finish();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    addFinalReportMessage(tr("  . %1 ATC codes retrieved from database").arg(ids.count()));

    // Get all components ids
    req = _database->select(Constants::Table_MOLS, QList<int>()
                            << Constants::MOLS_MID
                            << Constants::MOLS_NAME);
    ids.clear();
    if (query.exec(req)) {
        while (query.next()) {
            ids.insert(query.value(1).toString(), query.value(0).toInt());
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();
    data.setComponentIds(ids);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    addFinalReportMessage(tr("  . %1 drug components retrieved from database").arg(ids.count()));

    // Get all linked component (compo1 is linked to compo2 using Constants::COMPO_LK_NATURE in compo table)
    // TODO: this part could be improved using SQL specific commands... Any taker?
    // For each drug, if one component is linked to another one, get the link between components
    QTime chrono;
    chrono.start();
    QHash<int, QString> where;
    where.insert(Constants::DRUGS_SID, QString("=%1").arg(_sid));
    req = _database->select(Constants::Table_DRUGS, QList<int>()
                            << Constants::DRUGS_DID, where);
    if (query.exec(req)) {
        while (query.next()) {
            int did = query.value(0).toInt();
            // Get full composition
            QSqlQuery query2(_database->database());
            QHash<int, QString> where2;
            where2.insert(Constants::COMPO_DID, QString("=%1").arg(did));
            QString req2 = _database->select(Constants::Table_COMPO, where2);
            if (query2.exec(req2)) {
                QMultiHash<int, int> linksCompoId;
                QList<int> multi;
                while (query2.next()) {
                    // int compoId = query2.value(Constants::COMPO_ID).toInt();
                    int molId = query2.value(Constants::COMPO_MID).toInt();
                    int lk = query2.value(Constants::COMPO_LK_NATURE).toInt();
                    if (linksCompoId.uniqueKeys().contains(lk))
                        multi << lk;
                    linksCompoId.insertMulti(lk, molId);
                }

                // Set equivalence in the data in class
                for(int i=0; i < multi.count(); ++i) {
                    const QList<int> &eq = linksCompoId.values(multi.at(i));
                    if (eq.count() < 2) {
                        LOG_ERROR(QString("Wrong number of link. DID: %1").arg(did));
                        qWarning() << eq << multi.at(i) << linksCompoId;
                    } else {
                        for(int i = 0; i < eq.count(); ++i) {
                            for(int j = 0; j < eq.count(); ++j) {
                                if (i==j)
                                    continue;
                                data.addComponentEquivalence(eq.at(i), eq.at(j));
                            }
                        }
                    }
                }
            } else {
                LOG_QUERY_ERROR(query2);
                return false;
            }
            query2.finish();
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    } else {
        LOG_QUERY_ERROR(query);
        return false;
    }
    query.finish();


    // TODO: get all drug with 1 ATC code and 1 component -> to add to suggested ATC codes.


    Utils::Log::logTimeElapsed(chrono, "IDrugDatabase", "Retrieve data for ATC / Drug Components computation");

    // qWarning() << data.debugEquivalences();

    Q_EMIT progressLabelChanged(tr("Linking drugs components to ATC codes. Step 2"));
    Q_EMIT progress(1);

    // Create the component to ATC model
    DDI::ComponentAtcModel *linkerModel = new DDI::ComponentAtcModel(this);
    linkerModel->selectDatabase(_dbUid1, _dbUid2);

    // Start the linking processus
    DDI::ComponentLinkerResult result = linkerModel->startComponentLinkage(data);
    addFinalReportMessage(tr("  . Links processed."));
    foreach(const QString &e, result.errors())
        addFinalReportMessage(tr("    . ERROR: %1.").arg(e));
    foreach(const QString &m, result.messages())
        addFinalReportMessage(tr("    . %1.").arg(m));

    // Push data to the drugs database
    updateDatabaseCompletion(result.completionPercentage());
    _databasePopulator->saveComponentAtcLinks(_database, result.componentIdToAtcId(), _sid);
    addFinalReportMessage(tr("  . Links saved to database."));

    LOG(QString("Database processed"));
    Q_EMIT progress(2);

    return true;
}
