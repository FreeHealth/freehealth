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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DrugsDB::IDrugDatabaseStep
 * Default drug database creator step.
 * \n FreeToolBox specific class.
 */

#include "idrugdatabasestep.h"
#include "routesmodel.h"
#include "tools.h"
#include "drugsdbcore.h"
#include "drugdatabasedescription.h"
#include "drug.h"
#include <drugsdb/ddi/drugdruginteractioncore.h>

#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <coreplugin/ftb_constants.h>

#include <datapackplugin/datapackcore.h>
#include <datapackplugin/datapackquery.h>

#include <translationutils/constants.h>
#include <quazip/global.h>
#include <utils/log.h>
#include <utils/global.h>
#include <utils/waitforsignal.h>
#include <utils/httpdownloader.h>
#include <utils/httpmultidownloader.h>

#include <QDir>
#include <QFile>
#include <QDomDocument>
#include <QUrl>
#include <QProgressBar>
#include <QTimer>

#include <QDebug>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsDBCore *dbCore() {return DrugsDB::DrugsDBCore::instance();}
static inline DrugsDB::DrugDrugInteractionCore *ddiCore() {return dbCore()->ddiCore();}
static inline DataPackPlugin::DataPackCore *dataPackCore() {return DataPackPlugin::DataPackCore::instance();}

/*! Constructor of the DrugsDB::IDrugDatabaseStep class */
IDrugDatabaseStep::IDrugDatabaseStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    _database(0),
    _licenseType(Free),
    _serverOwner(Community),
    _sid(-1)
{
    setObjectName("IDrugDatabaseStep");
    _outputFileName = "master.db";
}

/*! Destructor of the DrugsDB::IDrugDatabaseStep class */
IDrugDatabaseStep::~IDrugDatabaseStep()
{
    // FIXME: should we delete _database here ?
    delete _database;
    _database = 0;
}

/** Define the absolute temporary path to use.*/
void IDrugDatabaseStep::setTempPath(const QString &absPath)
{
    _tempPath = QDir::cleanPath(absPath);
}

/** Define the absolute output path to use. The resulting database will be copied into this path. */
void IDrugDatabaseStep::setOutputPath(const QString &absPath)
{
    _outputPath = QDir::cleanPath(absPath);
}

/**
 * Define the connection name of this database.
 * This connection name will be the same as the corresponding QSqlDatabase connection name.
 */
void IDrugDatabaseStep::setConnectionName(const QString &connection)
{
    _connection = connection;
}

/**
 * Define the absolute output file name to use.
 * \sa setOutputPath()
 */
void IDrugDatabaseStep::setOutputFileName(const QString &fileName)
{
    _outputFileName = fileName;
}

/**
 * Define one URL for the automatic downloading process.
 * If you define this URL, the URL will be automatically downloaded during the step processing.
 * Otherwise you can overload the downloadFiles().
 */
void IDrugDatabaseStep::setDownloadUrl(const QString &url)
{
    _downloadingUrl = url;
}

/** Define the absolute path to the finalization SQL script to execute. This is obsolete. */
void IDrugDatabaseStep::setFinalizationScript(const QString &absPath)
{
    // TODO: add some checks
    _finalizationScriptPath = absPath;
}

/**
 * Define the description file for the database itself.
 * File name must be an absolute path.
 * \sa setDatapackDescriptionFile()
*/
void IDrugDatabaseStep::setDatabaseDescriptionFile(const QString &absPath)
{
    // TODO: add some checks
    _descriptionFilePath = absPath;
}

/**
 * Define the description file for the datapack containing this drug database.
 * File name must be an absolute path.
 * \sa setDatapackDescriptionFile(), registerDataPack()
*/
void IDrugDatabaseStep::setDatapackDescriptionFile(const QString &absPath)
{
    // TODO: add some checks
    _datapackDescriptionFilePath = absPath;
}

/** Return the absolute file path of the output database file */
QString IDrugDatabaseStep::absoluteFilePath() const
{
    return QString("%1/%2").arg(_outputPath).arg(outputFileName());
}

/** Return the Source Id of the drug database source from the drugs database. */
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
    Q_EMIT progressLabelChanged(tr("Creating database"));
    Q_EMIT progressRangeChanged(0, 4);
    Q_EMIT progress(0);
    _database = dbCore()->createDrugDatabase(absoluteFilePath(), _connection);
    if (!_database) {
        LOG_ERROR("Database not created: " + _connection);
        return false;
    }

    Q_EMIT progressLabelChanged(tr("Creating database: adding source description"));
    Q_EMIT progress(1);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    if (!saveDrugDatabaseDescription())
        return false;

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
        if (!ddiCore()->isAtcInstalledInDatabase(_database)) {
            if (!ddiCore()->addAtcDataToDatabase(_database)) {
                LOG_ERROR("Unable to add interaction data");
                Q_EMIT progress(4);
                qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
                return false;
            }
        }
    }
    Q_EMIT progress(4);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
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
        LOG_ERROR(QString("Routes file does not exist. File: %1").arg(RoutesModel::routeCsvAbsoluteFile()));
        return false;
    }
    LOG("Adding routes to database from " + RoutesModel::routeCsvAbsoluteFile());
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
    LOG("Routes saved");
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
        query.bindValue(SOURCES_ATC, int(descr.data(DrugDatabaseDescription::IsAtcValid).toBool()));
        query.bindValue(SOURCES_INTERACTIONS, int(descr.data(DrugDatabaseDescription::IsDDIValid).toBool()));
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
        query.bindValue(++i, int(descr.data(DrugDatabaseDescription::IsAtcValid).toBool()));
        query.bindValue(++i, int(descr.data(DrugDatabaseDescription::IsDDIValid).toBool()));
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

    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
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
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    db.transaction();
    QSqlQuery query(db);
    using namespace DrugsDB::Constants;
    int n = 0;
    Q_EMIT progressRangeChanged(0, drugs.count());

    foreach(Drug *drug, drugs) {
        ++n;
        if (n % 10 == 0) {
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
    LOG(QString("Saved %1 drugs").arg(drugs.count()));
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
    return mids;
}

/**
 * Add the ATC classification to the drug database. This classification is REQUIRED
 * by all non-free steps (that need ATC to compute interactions).
 * \sa DrugsDB::DrugDrugInteractionCore::addAtcDataToDatabase()
 */
bool IDrugDatabaseStep::addAtc()
{
    if (licenseType() == IDrugDatabaseStep::Free)
        return false;
    return ddiCore()->addAtcDataToDatabase(_database);
}

/**
 * Add the drug-drug interaction data to the drug database.
 * \sa DrugsDB::DrugDrugInteractionCore::addDrugDrugInteractionsToDatabase()
 */
bool IDrugDatabaseStep::addDrugDrugInteractions()
{
    if (licenseType() == IDrugDatabaseStep::Free)
        return false;
    return ddiCore()->addDrugDrugInteractionsToDatabase(_database);
}

/**
 * Add the PIMs (potentially inappropriate medication in elderly) data to the drug database.
 * \sa DrugsDB::DrugDrugInteractionCore::addPimsToDatabase()
 */
bool IDrugDatabaseStep::addPims()
{
    if (licenseType() == IDrugDatabaseStep::Free)
        return false;
    return ddiCore()->addPimsToDatabase(_database);
}

/**
 * Add the pregnancy drug compatibility data to the drug database.
 * \sa DrugsDB::DrugDrugInteractionCore::addPregnancyDataToDatabase()
 */
bool IDrugDatabaseStep::addPregnancyCheckingData()
{
    if (licenseType() == IDrugDatabaseStep::Free)
        return false;
//    return ddiCore()->addPregnancyDataToDatabase(_database);
    return true;
}

/**
 * Downloads all SPC contents according to the DRUGS_LINK_SPC field.
 * - Get all disctinct SPC contents
 * - Download all contents
 * - Add content to the database (also link drug to spc content)
 */
bool IDrugDatabaseStep::downloadSpcContents()
{
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

    // Create the SPC url downloader
    Utils::HttpMultiDownloader *_multiDownloader = new Utils::HttpMultiDownloader(this);
    _multiDownloader->setUseUidAsFileNames(true);
    QDir().mkpath(tempPath() + "/spc/");
    _multiDownloader->setOutputPath(tempPath() + "/spc/");
    _multiDownloader->readXmlUrlFileLinks();

    // Remove already downloaded files from the queue
    for(int i = _spcUrls.count() - 1; i >= 0; --i) {
        if (_multiDownloader->urls().contains(_spcUrls.at(i)))
            _spcUrls.removeAt(i);
    }

    // Start the download
    _multiDownloader->setUrls(_spcUrls);
    connect(_multiDownloader, SIGNAL(allDownloadFinished()), this, SLOT(onAllSpcDownloadFinished()));
    _multiDownloader->startDownload();
    return true;
}

/**
 * When all SPC files are downloaded, populate the drugs database.
 */
bool IDrugDatabaseStep::onAllSpcDownloadFinished()
{
    // Here all SPC files are downloaded.
    // We need to work a bit on HTML files (getting CSS content, JS scripts...)

    // Get the external CSS content of the HTML files (<link ... type="...css"> in the meta/head)
    // We suppose here that all the downloaded content has the same CSS file set.
    // So we read the first available file, extract the CSS file names,
    // download them to the spc output path. Then we must edit all downloaded HTML file and
    // make them point to these CSS files.

    Utils::HttpMultiDownloader *_multiDownloader = qobject_cast<Utils::HttpMultiDownloader *>(sender());
    if (!_multiDownloader)
        return false;
    disconnect(_multiDownloader, SIGNAL(allDownloadFinished()), this, SLOT(onAllSpcDownloadFinished()));
    // Save the HttpMultiDownloader XML cache file (only if there was downloads)
    if (_spcUrls.count() > 0)
        _multiDownloader->saveXmlUrlFileLinks();
    //return true;

    // TODO: manage downloader errors

    // As we have supposed that all HMTL have the same CSS set we only screen one file.
    // An improvement can be to screen *all* the HTML files, and to manage database to correctly
    // link HTML and CSS content of SPC documents.

    // Get the first available SPC file in the output dir
    const QUrl &firstUrl = _multiDownloader->urls().at(0);
    QString content = Utils::readTextFile(_multiDownloader->outputAbsoluteFileName(firstUrl), "ISO-8859-1");
    QList<QUrl> cssFiles;

    // Extract all stylesheet file from the "<link" tag
    QStringList tmp = Utils::htmlGetLinksToCssContent(content);

    // Manage relative filePath
    foreach(const QString &css, tmp) {
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

    // Create the SPC resources content (CSS, JS...)
    QList<SpcContentResources> spcResources;
    foreach(const QUrl &url, cssFiles) {
        QString absFilePath = _cssMultiDownloader->outputAbsoluteFileName(url);
        SpcContentResources resource;
        resource.type = "text/css";
        resource.name = QFileInfo(absFilePath).fileName();
        resource.content = Utils::readTextFile(absFilePath, Utils::DontWarnUser);
        spcResources << resource;
    }

    // Create the SPC content and send them to the drugs database
    foreach(const QUrl &url, _multiDownloader->urls()) {
        SpcContent content;
        content.url = url.toString();
        content.html = Utils::readTextFile(_multiDownloader->outputAbsoluteFileName(url), Utils::DontWarnUser);
        content.resources = spcResources;
        saveDrugSpc(content);
    }

    QTimer::singleShot(1, this, SIGNAL(spcProcessFinished()));
    return true;
}

/**
 * Save the SPC content.
 * \warning code does not manage duplicates
 */
bool IDrugDatabaseStep::saveDrugSpc(const SpcContent &content)
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
        _database->max(Table_SPC_CONTENT, SPCCONTENT_SPCCONTENT_RESOURCES_LINK_ID).toInt() + 1;

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

/** Create all object path (temp, output, download...) */
bool IDrugDatabaseStep::createTemporaryStorage()
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
bool IDrugDatabaseStep::cleanTemporaryStorage()
{
    QFile(absoluteFilePath()).remove();
    return true;
}

bool IDrugDatabaseStep::startProcessing(ProcessTiming timing, SubProcess subProcess)
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
                ok = linkMolecules()
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

void IDrugDatabaseStep::onSubProcessFinished()
{
    WARN_FUNC << _currentTiming << _currentSubProcess;
    Q_EMIT subProcessFinished(_currentTiming, _currentSubProcess);
}

/**
 * Download the URL to the tempPath().
 * \sa setDownloadUrl()
 */
bool IDrugDatabaseStep::startDownload()
{
    Utils::HttpDownloader *dld = new Utils::HttpDownloader;
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    connect(dld, SIGNAL(downloadProgressPermille(int)), this, SIGNAL(progress(int)));

    Q_EMIT progressRangeChanged(0, 1000);
    Q_EMIT progress(0);

    dld->setOutputPath(tempPath());
    dld->setUrl(QUrl(downloadUrl()));
    dld->startDownload();
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
    LOG(QString("Starting unzipping %1 files to: %2")
        .arg(connectionName())
        .arg(fileName));

    return QuaZipTools::unzipFile(fileName, tempPath());
}

/**
 * Automatically register the drug database to the DataPackPlugin::DataPackCore according
 * to the DrugsDB::IDrugDatabaseStep::LicenseType and the DrugsDB::IDrugDatabaseStep::ServerOwner
 * of the object.
 * \sa DataPackPlugin::DataPackCore::registerDataPack()
 */
bool IDrugDatabaseStep::registerDataPack()
{
    QString server;
    if (_licenseType == Free) {
        if (_serverOwner == Community) {
            server = Core::Constants::SERVER_COMMUNITY_FREE;
        } if (_serverOwner == FrenchAssociation) {
            server = Core::Constants::SERVER_ASSO_FREE;
        }
    } else {
        if (_serverOwner == Community) {
            server = Core::Constants::SERVER_COMMUNITY_NONFREE;
        } if (_serverOwner == FrenchAssociation) {
            server = Core::Constants::SERVER_ASSO_NONFREE;
        }
    }
    DataPackPlugin::DataPackQuery query;
    query.setDescriptionFileAbsolutePath(_datapackDescriptionFilePath);
    query.setOriginalContentFileAbsolutePath(_outputPath + QDir::separator() + _connection + QDir::separator() + _outputFileName);
    query.setZipOriginalFile(true);
    if (!dataPackCore()->registerDataPack(query, server)) {
        LOG_ERROR("Unable to register datapack for drugs database: " + connectionName());
        return false;
    }
    LOG(QString("Registered datapack for drugs database: %1; in server %2").arg(connectionName()).arg(server));
    return true;
}
