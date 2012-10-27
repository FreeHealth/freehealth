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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DrugsDB::IDrugDatabaseStep
 * Default drug database creator step.
 */

#include "idrugdatabasestep.h"
#include "routesmodel.h"
#include "tools.h"
#include "drugsdbcore.h"
#include "drugdatabasedescription.h"
#include "drug.h"

#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <translationutils/constants.h>
#include <quazip/global.h>
#include <utils/httpdownloader.h>
#include <utils/log.h>
#include <utils/global.h>

#include <QDir>
#include <QFile>
#include <QDomDocument>

#include <QDebug>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsDBCore *dbCore() {return DrugsDB::DrugsDBCore::instance();}

/*! Constructor of the DrugsDB::IDrugDatabaseStep class */
IDrugDatabaseStep::IDrugDatabaseStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    _database(0),
    _sid(-1)
{
    _outputFileName = "master.db";
}

/*! Destructor of the DrugsDB::IDrugDatabaseStep class */
IDrugDatabaseStep::~IDrugDatabaseStep()
{
    // FIXME: should we delete _database here ?
    delete _database;
    _database = 0;
}

void IDrugDatabaseStep::setTempPath(const QString &absPath)
{
    _tempPath = QDir::cleanPath(absPath);
}

void IDrugDatabaseStep::setOutputPath(const QString &absPath)
{
    _outputPath = QDir::cleanPath(absPath);
}

void IDrugDatabaseStep::setConnectionName(const QString &connection)
{
    _connection = connection;
}

void IDrugDatabaseStep::setOutputFileName(const QString &fileName)
{
    _outputFileName = fileName;
}

void IDrugDatabaseStep::setDownloadUrl(const QString &url)
{
    _downloadingUrl = url;
}

void IDrugDatabaseStep::setFinalizationScript(const QString &absPath)
{
    // TODO: add some checks
    _finalizationScriptPath = absPath;
}

void IDrugDatabaseStep::setDescriptionFile(const QString &absPath)
{
    // TODO: add some checks
    _descriptionFilePath = absPath;
}

/** Return the absolute file path of the output database file */
QString IDrugDatabaseStep::absoluteFilePath() const
{
    return QString("%1/%2").arg(_outputPath).arg(outputFileName());
}

int IDrugDatabaseStep::sourceId() const
{
    return _sid;
}

/** Check the existence of the database internal pointer and check if the datbase is correctly open */
bool IDrugDatabaseStep::checkDatabase()
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
bool IDrugDatabaseStep::canCreateDatabase() const
{
    return !_connection.isEmpty()
            && !_outputFileName.isEmpty()
            && !_outputPath.isEmpty();
}

/**
 * Create the DrugDatabase pointer according to the settings of the object.
 * If you call this member many times, only one database will be created.
 * Does not populate it with drugs but with non-free data if required.
 * \sa DrugDB::DrugsDBCore::createDrugDatabase()
 */
bool IDrugDatabaseStep::createDatabase()
{
    if (_database)
        return true;
    _database = dbCore()->createDrugDatabase(absoluteFilePath(), _connection);
    if (!_database) {
        LOG_ERROR("Database not created: " + _connection);
        return false;
    }

    if (!saveDrugDatabaseDescription())
        return false;
    if (!addRoutes())
        return false;

    if (licenseType() == NonFree) {
        if (!dbCore()->addInteractionData(_connection)) {
            LOG_ERROR("Unable to add interaction data");
            return false;
        }
    }
    return true;
}

/** Add drug routes to the database. This function uses the default routes text file. */
bool IDrugDatabaseStep::addRoutes()
{
    if (!checkDatabase())
        return false;

    // Routes already in database ?
    if (_database->count(Constants::Table_ROUTES, Constants::ROUTES_RID) > 0) {
        LOG("Routes already in database");
        return true;
    }

    QString content = Utils::readTextFile(RoutesModel::routeCsvAbsoluteFile());
    if (content.isEmpty()) {
        LOG_ERROR_FOR("Tools","Routes file does not exist.\n" + RoutesModel::routeCsvAbsoluteFile());
        return false;
    }
    LOG_FOR("Tools", "Adding routes to database from " + RoutesModel::routeCsvAbsoluteFile());
    QSqlQuery query(_database->database());

    // Read file
    foreach(const QString &line, content.split("\n", QString::SkipEmptyParts)) {
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
            LOG_ERROR_FOR("Tools", "Route not integrated");
            continue;
        }
        //        QString req = QString("INSERT INTO ROUTES (RID, MASTER_LID, SYSTEMIC_STATUS) VALUES (NULL, %1, '%2')")
        //                .arg(masterLid)
        //                .arg(systemic)
        //                ;
        query.prepare(_database->prepareInsertQuery(DrugsDB::Constants::Table_ROUTES));
        query.bindValue(DrugsDB::Constants::ROUTES_RID, rid);
        query.bindValue(DrugsDB::Constants::ROUTES_MASTERLID, masterLid);
        query.bindValue(DrugsDB::Constants::ROUTES_SYSTEMIC, systemic);
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("Tools", query);
            return false;
        }
        query.finish();
    }
    return true;
}

/** Recreate drug routes in the database. This function uses the default routes text file. */
bool IDrugDatabaseStep::recreateRoutes()
{
    if (!checkDatabase())
        return false;
    Tools::executeSqlQuery(_database->prepareDeleteQuery(DrugsDB::Constants::Table_ROUTES), _connection);
    return addRoutes();
}

/** Save the database description and create a drug base source ID */
bool IDrugDatabaseStep::saveDrugDatabaseDescription()
{
    if (!checkDatabase())
        return false;
    // Some checks
    if (_descriptionFilePath.isEmpty())
        return false;
    QFile file(_descriptionFilePath);
    if (!file.exists())
        return false;

    // Read the XML file
    QDomDocument doc;
    int line = 0;
    int col = 0;
    QString error;
    if (!doc.setContent(Utils::readTextFile(_descriptionFilePath, Utils::DontWarnUser), &error, &line, &col)) {
        LOG_ERROR_FOR("Tools", QString("Wrong XML: (%1;%2): %3 in file %4").arg(line).arg(col).arg(error).arg(_descriptionFilePath));
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
        query.bindValue(SOURCES_VERSION, descr.data(DrugDatabaseDescription::FreeMedFormsCompatVersion));
        query.bindValue(SOURCES_PROVIDER, descr.data(DrugDatabaseDescription::Vendor));
        query.bindValue(SOURCES_WEBLINK, descr.data(DrugDatabaseDescription::WebLink));
        query.bindValue(SOURCES_DRUGUID_NAME, descr.data(DrugDatabaseDescription::DrugUidName));
        query.bindValue(SOURCES_ATC, descr.data(DrugDatabaseDescription::IsAtcValid));
        query.bindValue(SOURCES_INTERACTIONS, descr.data(DrugDatabaseDescription::IsDDIValid));
        query.bindValue(SOURCES_COMPL_WEBSITE, descr.data(DrugDatabaseDescription::ComplementaryWebLink));
        query.bindValue(SOURCES_PACKUID_NAME, descr.data(DrugDatabaseDescription::PackUidName));
        query.bindValue(SOURCES_COMPLETION, QVariant());
        query.bindValue(SOURCES_AUTHOR_COMMENTS, QVariant());
        query.bindValue(SOURCES_DRUGNAMECONSTRUCTOR, descr.data(DrugDatabaseDescription::DrugNameConstructor));
        query.bindValue(SOURCES_FMFCOMPAT, descr.data(DrugDatabaseDescription::FreeMedFormsCompatVersion));
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
        query.bindValue(++i, descr.data(DrugDatabaseDescription::FreeMedFormsCompatVersion));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::Vendor));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::WebLink));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::DrugUidName));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::IsAtcValid));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::IsDDIValid));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::ComplementaryWebLink));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::PackUidName));
        query.bindValue(++i, QVariant());
        query.bindValue(++i, QVariant());
        query.bindValue(++i, descr.data(DrugDatabaseDescription::DrugNameConstructor));
        query.bindValue(++i, descr.data(DrugDatabaseDescription::FreeMedFormsCompatVersion));
        query.bindValue(++i, QVariant());
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("Tools", query);
            return false;
        }
    }
    query.finish();

    // Save database labels
    QMultiHash<QString, QVariant> trLabels;
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

    return true;
}

/** Update the INN <-> molecules linking completion percentage */
bool IDrugDatabaseStep::updateDatabaseCompletion(int completion)
{
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
    return true;
}

/** Save a list of drugs in the database. The drugs vector will be sorted. */
bool IDrugDatabaseStep::saveDrugsIntoDatabase(QVector<Drug *> drugs)
{
    if (!checkDatabase())
        return false;
    if (_sid==-1) {
        LOG_ERROR_FOR("Drug", "NO SID DEFINED");
        return false;
    }

    // Clear database
    QHash<int, QString> w;
    using namespace DrugsDB::Constants;
    QSqlDatabase db = _database->database();
    db.transaction();
    Tools::executeSqlQuery(_database->prepareDeleteQuery(Table_MASTER, w), _connection);
    Tools::executeSqlQuery(_database->prepareDeleteQuery(Table_DRUGS, w), _connection);
    Tools::executeSqlQuery(_database->prepareDeleteQuery(Table_COMPO, w), _connection);
    Tools::executeSqlQuery(_database->prepareDeleteQuery(Table_DRUG_ROUTES, w), _connection);
    Tools::executeSqlQuery(_database->prepareDeleteQuery(Table_DRUG_FORMS, w), _connection);
    Tools::executeSqlQuery(_database->prepareDeleteQuery(Table_MOLS, w), _connection);
    Tools::executeSqlQuery(_database->prepareDeleteQuery(Table_LK_MOL_ATC, w), _connection);
    Tools::executeSqlQuery(_database->prepareDeleteQuery(Table_PACKAGING, w), _connection);
    // TODO: delete COMPOSITION, DRUG_ROUTES, LABELS_LINK, LABELS */
    db.commit();

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
    QHash<int, QString> mids = saveMoleculeIds(molnames);
    qSort(drugs.begin(), drugs.end(), Drug::lessThanOnNames);

    db.transaction();
    QSqlQuery query(db);
    using namespace DrugsDB::Constants;
    int n = 0;
    foreach(Drug *drug, drugs) {
        ++n;
        if (n % 1000)
            LOG(QString("Saved %1 drugs").arg(n));

        // Authorizations && get Master_LID
        QMultiHash<QString, QVariant> multiLang;
        foreach(const QString &lang, drug->availableLanguages()) {
            multiLang.insertMulti(lang, drug->data(Drug::Authorization, lang));
        }
        int aidLID = Tools::addLabels(_database, -1, multiLang);
        multiLang.clear();

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

        // Routes
        const QStringList &routes = drug->data(Drug::Routes).toStringList();
//        routeId
        if (!routes.isEmpty()) {
            // Create the drugs route links
            QList<int> routesId = dbCore()->routesModel()->routeId(routes);
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

        // Forms
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
    }
    query.finish();
    db.commit();

    return true;
}

/** Save a list of molecules and return a hash containing the MoleculeID as key and the molecule name os value */
QHash<int, QString> IDrugDatabaseStep::saveMoleculeIds(const QStringList &molnames)
{
    QHash<int, QString> mids;
    if (!checkDatabase())
        return mids;
    using namespace DrugsDB::Constants;
    QString req;
    _database->database().transaction();
    QSqlQuery query(_database->database());

    foreach(const QString &name, molnames) {
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
    return mids;
}

/** Create all object working path */
bool IDrugDatabaseStep::createDir()
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

/**
 * Download the URL to the tempPath().
 * \sa setDownloadUrl()
 */
bool IDrugDatabaseStep::downloadFiles(QProgressBar *bar)
{
    Utils::HttpDownloader *dld = new Utils::HttpDownloader;
    dld->setProgressBar(bar);
//    dld->setMainWindow(mainwindow());
    dld->setOutputPath(tempPath());
    dld->setUrl(QUrl(downloadUrl()));
    dld->startDownload();
    connect(dld, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()));
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    return true;
}

/**
 * Unzip the downloaded file.
 * \sa setDownloadUrl(), downloadFiles()
 */
bool IDrugDatabaseStep::unzipFiles()
{
    // check file
    QString fileName = tempPath() + QDir::separator() + QFileInfo(downloadUrl()).fileName();
    if (!QFile(fileName).exists()) {
        LOG_ERROR(QString("No files to unzip."));
        LOG_ERROR(QString("Please download files."));
        return false;
    }

    // unzip files using QProcess
    LOG(QString("Starting unzipping %1 files %1")
        .arg(connectionName())
        .arg(fileName));

    return QuaZipTools::unzipFile(fileName, tempPath());
}
