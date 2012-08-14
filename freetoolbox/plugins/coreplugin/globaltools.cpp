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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@gmail.com>             *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "globaltools.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/drugdatabasedescription.h>

#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QStringList>
#include <QProcess>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTime>
#include <QVariant>
#include <QProgressDialog>
#include <QAbstractTableModel>
#include <QDomDocument>
#include <QDomElement>

#include <QDebug>

#include <quazip/quazip/quazip.h>
#include <quazip/quazip/quazipfile.h>

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline QString masterDatabaseSqlSchema() {return settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + QString(Core::Constants::FILE_MASTER_DATABASE_SCHEME);}
static inline QString routesCsvAbsFile() {return settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + QString(Core::Constants::FILE_DRUGS_ROUTES);}

namespace Core {
namespace Tools {

QString noAccent(const QString & s)
{
    QString tmp = s;
    tmp.replace( QString::fromUtf8( "é" ) , "e" );
    tmp.replace( QString::fromUtf8( "è" ) , "e" );
    tmp.replace( QString::fromUtf8( "ê" ) , "e" );
    tmp.replace( QString::fromUtf8( "ë" ) , "e" );
    tmp.replace( QString::fromUtf8( "î" ) , "i" );
    tmp.replace( QString::fromUtf8( "ï" ) , "i" );
    tmp.replace( QString::fromUtf8( "É" ) , "E" );
    tmp.replace( QString::fromUtf8( "È" ) , "E" );
    tmp.replace( QString::fromUtf8( "Ê" ) , "E" );
    tmp.replace( QString::fromUtf8( "Ë" ) , "E" );
    tmp.replace( QString::fromUtf8( "Î" ) , "I" );
    tmp.replace( QString::fromUtf8( "Ï" ) , "I" );
    return tmp;
}

QString getBlock(const QString &content, const int posStart, int &posEnd, const QString &delimiter)
{
    // find first delimiter starting at pos posStart
    int begin = content.indexOf( delimiter, posStart );
    int end = content.indexOf( delimiter, begin + delimiter.length() + 1 );

    // modify posEnd for next block process
    if (begin == -1) {
        posEnd = posStart;
        return QString::null;
    } else {
        posEnd = end;
    }

    // return block
    QString tmp = content.mid( begin, end - begin );
    tmp.replace( "\r" , "" );
    tmp.replace( "\n" , "" );
    return tmp;
}

QString getBlock(const QString &content, const int posStart, int &posEnd, const QRegExp &delimiter)
{
    // find first delimiter starting at pos posStart
    int begin = content.indexOf( delimiter, posStart );
    int end = content.indexOf( delimiter, begin + 4 );

    // modify posEnd for next block process
    if (begin == -1) {
        posEnd = posStart;
        return QString::null;
    } else {
        posEnd = end;
    }

    // return block
    QString tmp = content.mid( begin, end - begin );
    tmp.replace( "\r" , "" );
    //      tmp.replace( "\n" , "" );
    return tmp;
}

bool executeProcess(const QString &proc)
{
    QProcess process;
    process.start(proc, QIODevice::ReadOnly);

    LOG_FOR("Tools", QString("Executing process : %1").arg(proc));

    if (!process.waitForStarted())
        LOG_ERROR_FOR("Tools", QString("Process %1 can not start").arg(proc.left(20)));

    if (!process.waitForFinished(100000))
        LOG_ERROR_FOR("Tools", QString("Process %1 does not end").arg(proc.left(20)));

    QString error = process.readAllStandardError();
    if (!error.isEmpty()) {
        LOG_ERROR_FOR("Tools", QString("ERROR : %1").arg(proc));
        LOG_ERROR_FOR("Tools", QString("ERROR : %1").arg(error));
        return false;
    }
    LOG_FOR("Tools", QString("Process done : %1, output : %2").arg(proc.left(20)).arg(QString(process.readAllStandardOutput())));
    return true;
}

bool executeSqlFile(const QString &connectionName, const QString &fileName, QProgressDialog *dlg)
{
    if (!QFile::exists(fileName)) {
        LOG_ERROR_FOR("Tools", QString("ERROR: missing database schema file : %1.").arg(fileName));
        return false;
    }

    // execute all sql queries
    QString req = Utils::readTextFile(fileName);
    if (req.isEmpty()) {
        LOG_ERROR_FOR("Tools", "File is empty : " + fileName);
        return false;
    }

    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.remove("AUTO_INCREMENT"); // SQLite compatibility

    QStringList list = req.split("\n");
    QSqlDatabase DB = QSqlDatabase::database(connectionName);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR_FOR("Tools", "Database not opened");
            return false;
        }
    }
    DB.transaction();

    req.clear();
    QStringList queries;
    // Reconstruct req : removes comments
    foreach(const QString &s, list) {
        if (s.startsWith("--")) {
            LOG_FOR("Tools", s);
            continue;
        }
        req += s + " \n";
        if (s.endsWith(";")) {
            queries.append(req);
            req.clear();
        }
    }

    // Execute queries
    if (dlg)
        dlg->setRange(0, queries.count());

    foreach(const QString &sql, queries) {
        QTime time;
        time.start();

        QString q = sql.simplified();
        // Do not processed empty strings
        if (q.isEmpty())
            continue;

        // No SQLite extra commands
        if (q.startsWith("."))
            continue;

        // No BEGIN, No COMMIT
        if (q.startsWith("BEGIN", Qt::CaseInsensitive) || q.startsWith("COMMIT", Qt::CaseInsensitive))
            continue;

        qWarning() << sql;

        QSqlQuery query(sql, DB);
        if (!query.isActive()) {
            LOG_ERROR_FOR("Tools", QString("SQL ERROR : %1 \"%2\"").arg(query.lastError().text(), sql));
//            DB.rollback();
            return false;
        } else {
            LOG_FOR("Tools", QString("Query correctly done (%1 ms)").arg(time.elapsed()));
        }

        if (dlg)
            dlg->setValue(dlg->value()+1);
    }
    DB.commit();
    return true;
}

bool executeSqlQuery(const QString &sql, const QString &dbName, const QString &file, int line)
{
    QSqlDatabase DB = QSqlDatabase::database(dbName);
    if (!DB.isOpen()) {
        if (file.isEmpty())
            LOG_ERROR_FOR("Tools", "Unable to connect to " + dbName);
        else
            Utils::Log::addError("Tools", "Unable to connect to " + dbName, file, line);
        return false;
    }
    QSqlQuery query(sql, DB);
    if (!query.isActive()) {
        if (file.isEmpty())
            LOG_ERROR_FOR("Tools", "Query Error : " + sql + " // " + query.lastError().text());
        else
            Utils::Log::addError("Tools", "Query Error : " + sql + " // " + query.lastError().text(), file, line);
        return false;
    }
    return true;
}

DrugsDB::Internal::DrugBaseEssentials *drugBase()
{
    static DrugsDB::Internal::DrugBaseEssentials *core = 0;
    if (!core)
        core = new DrugsDB::Internal::DrugBaseEssentials;
    return core;
}

/** Return the drugs database full path, for a specific country or for all countries if no \e country is specified. */
QString drugsDatabaseAbsFileName(const QString &country)
{
//    QString path = QString("%1/%2/%3")
//            .arg(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString())
//            .arg(Core::Constants::MASTER_DATABASE_NAME)
//            .arg(country)
//            .arg(Core::Constants::MASTER_DATABASE_FILENAME);
//    return QDir::cleanPath(path);
//    }
    return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::MASTER_DATABASE_FILENAME);
}

QString databaseOutputPath()
{
    return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString());
}

bool connectDatabase(const QString &connection, const QString &fileName)
{
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        LOG_ERROR_FOR("Tools", QString("ERROR : SQLite driver is not available"));
        return false;
    }
    QSqlDatabase DB;
    // test drugs connection
    if (QSqlDatabase::contains(connection)) {
        DB = QSqlDatabase::database(connection);
    }  else {
        DB = QSqlDatabase::addDatabase("QSQLITE" , connection);
        if (QFileInfo(fileName).isRelative())
            DB.setDatabaseName(QDir::cleanPath(qApp->applicationDirPath() + Constants::MACBUNDLE + fileName));
        else
            DB.setDatabaseName(fileName);

        if (!DB.open()) {
            LOG_ERROR_FOR("Tools", QString("ERROR : %1 // %2").arg(DB.lastError().text()).arg(fileName));
            return false;
        } else {
            LOG_FOR("Tools", QString("Connection to database created : %1 %2")
                    .arg(DB.connectionName(), DB.databaseName()));
        }
        if (connection==Constants::MASTER_DATABASE_NAME)
            drugBase()->initialize(databaseOutputPath());
    }
    return true;
}

bool createMasterDrugInteractionDatabase()
{
    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
    if (!db.isOpen() && db.isValid()) {
        if (!connectDatabase(Core::Constants::MASTER_DATABASE_NAME, drugsDatabaseAbsFileName())) {
            LOG_ERROR_FOR("Tools", "Unable to create master database");
            return false;
        }
    }

    if (!db.isValid() || db.tables().count() < DrugsDB::Constants::Table_MaxParam) {
//        executeSqlFile(Core::Constants::MASTER_DATABASE_NAME, masterDatabaseSqlSchema());
        drugBase()->initialize(databaseOutputPath(), true);
        addRoutesToDatabase(Core::Constants::MASTER_DATABASE_NAME, routesCsvAbsFile());
    }
    return true;
}

bool createDrugDatabase(const QString &absPath)
{
    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
    if (!db.isOpen() && db.isValid()) {
        if (!connectDatabase(Core::Constants::MASTER_DATABASE_NAME, absPath)) {
            LOG_ERROR_FOR("Tools", "Unable to create master database");
            return false;
        }
    }

    if (!db.isValid() || db.tables().count() < DrugsDB::Constants::Table_MaxParam) {
//        executeSqlFile(Core::Constants::MASTER_DATABASE_NAME, masterDatabaseSqlSchema());
        drugBase()->initialize(absPath, true);
        addRoutesToDatabase(Core::Constants::MASTER_DATABASE_NAME, routesCsvAbsFile());
    }
    return true;
}

bool signDatabase(const QString &connectionName)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.isOpen()) {
        if (!db.open()) {
            return false;
        }
    }

    QHash<QString, QString> tables;
    foreach(const QString &table, db.tables()) {
        QString req = QString("SELECT count(*) FROM %1;").arg(table);
        QSqlQuery query(db);
        if (query.exec(req)) {
            if (query.next())
                tables.insert(table, query.value(0).toString());
        }
    }
    if (tables.count() != db.tables().count())
        return false;

    QFileInfo info(db.databaseName());
    QString tag = info.fileName() + "(";
    tag += QString::number(info.size()) + "," + info.created().toString(Qt::ISODate) + ")@";
    foreach(const QString &table, tables.keys()) {
        tag += table + ":" + tables.value(table) + "/";
    }
    tag.chop(1);
    tag += "\n";

    QString fileName = QFileInfo(db.databaseName()).absolutePath() + "/check.db";
    QString content = QString(QByteArray::fromBase64(Utils::readTextFile(fileName, Utils::DontWarnUser).toUtf8()));
    QStringList linesToKeep;
    foreach(const QString &line, content.split("\n", QString::SkipEmptyParts)) {
        if (line.startsWith(QFileInfo(db.databaseName()).fileName() + "@")) {
            continue;
        }
        linesToKeep << line;
    }
    linesToKeep << tag;
    content.clear();
    content = linesToKeep.join("\n");

    Utils::saveStringToFile(content.toUtf8().toBase64(), fileName, Utils::Overwrite, Utils::DontWarnUser);

    return true;
}

int getSourceId(const QString &connection, const QString &dbUid)
{
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("Tools","Unable to connection database");
            return -1;
        }
    }

    QString req;
    QSqlQuery query(db);

    // Source exists ?
//    req = QString("SELECT SID FROM SOURCES WHERE (DATABASE_UID='%1')").arg(dbUid);
    QHash<int, QString> w;
    w.insert(DrugsDB::Constants::SOURCES_DBUID, QString("='%1'").arg(dbUid));
    req = drugBase()->select(DrugsDB::Constants::Table_SOURCES, DrugsDB::Constants::SOURCES_SID, w);
    if (query.exec(req)) {
        if (query.next()) {
            return query.value(0).toInt();;
        }
    } else {
        LOG_QUERY_ERROR_FOR("Tools", query);
        return -1;
    }
    query.finish();
    return -1;
}

/** \brief Create a new drugs source in the Master database. Return -1 is an error occured, or the SID */
int createNewDrugsSource(const QString &connection, const QString &uid, QMultiHash<QString, QVariant> trLabels)
{
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("Tools","Unable to connection database");
            return -1;
        }
    }

    QString req;
    QSqlQuery query(db);

    // Source exists ?
    int sid = getSourceId(connection, uid);
    if (sid!=-1)
        return sid;

    // insert labels
    int masterLid = addLabels(connection, -1, trLabels);
    if (masterLid == -1) {
        LOG_ERROR_FOR("Tools","Unable to add source");
        return -1;
    }

    // insert source
//    req = QString("INSERT INTO SOURCES (SID, MASTER_LID, DATABASE_UID) VALUES (NULL, %1, '%2')")
//          .arg(masterLid)
//          .arg(uid)
//          ;
    query.prepare(drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_SOURCES));
    for(int i = 0; i < (DrugsDB::Constants::SOURCES_MaxParam); ++i) {
        query.bindValue(i, QVariant());
    }
    query.bindValue(DrugsDB::Constants::SOURCES_MASTERLID, masterLid);
    query.bindValue(DrugsDB::Constants::SOURCES_DBUID, uid);
    if (query.exec()) {
        return query.lastInsertId().toInt();
    } else {
        LOG_QUERY_ERROR_FOR("Tools", query);
    }

    return -1;
}

/** \brief Return the used MASTER_LID or -1 in case of an error */
int addLabels(const QString &connection, const int masterLid, QMultiHash<QString, QVariant> trLabels)
{
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("Tools","Unable to connection database");
            return -1;
        }
    }
    QSqlQuery query(db);
    QString req;
    int mid = masterLid;
    if (mid == -1) {
        // get new master_lid
//        req = "SELECT max(MASTER_LID) FROM `LABELS_LINK`;";
        mid = drugBase()->max(DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_MASTERLID).toInt();
        ++mid;
//        if (query.exec(req)) {
//            if (query.next())
//                mid = query.value(0).toInt();
//            ++mid;
//        } else {
//            LOG_QUERY_ERROR_FOR("Tools", query);
//            return -1;
//        }
    }

    // insert all translated labels
    foreach(const QString &lang, trLabels.uniqueKeys()) {
        foreach(const QVariant &value, trLabels.values(lang)) {
            QString t = value.toString();

            // Check is couple already exists
            // TODO: code here: code buggy */
//            req = QString("SELECT LID FROM LABELS WHERE (LANG='%1' AND LABEL=\"%2\")")
//                  .arg(lang)
//                  .arg(t);
//            if (query.exec(req)) {
//                if (query.next()) {
//                    int lid = query.value(0).toInt();
//                    query.finish();

//                    req = QString("INSERT INTO `LABELS_LINK` (MASTER_LID, LID) VALUES "
//                                  "(%1  ,%2)")
//                            .arg(mid)
//                            .arg(lid)
//                            ;
//                    if (!query.exec(req)) {
//                        LOG_QUERY_ERROR_FRO("Drugs", query);
//                        return false;
//                    }
//                    query.finish();
//                    return mid;
//                }
//            } else {
//                LOG_QUERY_ERROR_FOR("Drugs", query);
//                return -1;
//            }
//            query.finish();

//            req = QString("INSERT INTO `LABELS` (LID,LANG,LABEL) VALUES (NULL,'%1','%2')")
//                    .arg(lang).arg(t.replace("'","''"));
            query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_LABELS));
            query.bindValue(DrugsDB::Constants::LABELS_LID, QVariant());
            query.bindValue(DrugsDB::Constants::LABELS_LANG, lang);
            query.bindValue(DrugsDB::Constants::LABELS_LABEL, t.replace("'","''"));
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR("Tools", query);
                return -1;
            }
            int id = query.lastInsertId().toInt();
            query.finish();

//            req = QString("INSERT INTO `LABELS_LINK` (MASTER_LID, LID) VALUES "
//                          "(%1  ,%2)")
//                    .arg(mid)
//                    .arg(id)
//                    ;
            query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_LABELSLINK));
            query.bindValue(DrugsDB::Constants::LABELSLINK_LID, id);
            query.bindValue(DrugsDB::Constants::LABELSLINK_MASTERLID, mid);
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR("Drugs", query);
                return -1;
            }
            query.finish();
        }
    }
    return mid;
}

bool recreateRoutes()
{
    if (!connectDatabase(Core::Constants::MASTER_DATABASE_NAME, drugsDatabaseAbsFileName())) {
        LOG_ERROR_FOR("Tools", "Unable to create master database");
        return false;
    }
    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
    Tools::executeSqlQuery(drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_ROUTES), Core::Constants::MASTER_DATABASE_NAME);
    return addRoutesToDatabase(Core::Constants::MASTER_DATABASE_NAME, routesCsvAbsFile());
}

bool addRoutesToDatabase(const QString &connection, const QString &absFileName)
{
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("Tools","Unable to connection database");
            return false;
        }
    }
    QString content = Utils::readTextFile(absFileName);
    if (content.isEmpty()) {
        LOG_ERROR_FOR("Tools","Routes file does not exist.\n" + absFileName);
        return false;
    }
    LOG_FOR("Tools", "Adding routes to database from " + absFileName);
    QSqlQuery query(db);

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
        int masterLid = Tools::addLabels(connection, -1, trLabels);
        if (masterLid == -1) {
            LOG_ERROR_FOR("Tools", "Route not integrated");
            continue;
        }
//        QString req = QString("INSERT INTO ROUTES (RID, MASTER_LID, SYSTEMIC_STATUS) VALUES (NULL, %1, '%2')")
//                .arg(masterLid)
//                .arg(systemic)
//                ;
        query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_ROUTES));
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

QHash<int, QString> generateMids(const QStringList &molnames, const int sid, const QString &connection)
{
    QHash<int, QString> mids;
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen())
        return mids;

    QString req;
    db.transaction();
    QSqlQuery query(db);

    foreach(const QString &name, molnames) {

        // Ask for an existing MID
//        req = QString("SELECT MID FROM MOLS WHERE NAME=\"%1\" AND SID=\"%2\";").arg(name).arg(sid);
        QHash<int, QString> w;
        w.insert(DrugsDB::Constants::MOLS_NAME, QString("=\"%1\"").arg(name));
        w.insert(DrugsDB::Constants::MOLS_SID, QString("='%1'").arg(sid));
        req = Core::Tools::drugBase()->select(DrugsDB::Constants::Table_MOLS, DrugsDB::Constants::MOLS_MID, w);
        if (query.exec(req)) {
            if (query.next()) {
                // is already in the table MOLS
                mids.insert(query.value(0).toInt(), name);
                continue;
            }
        } else {
            LOG_QUERY_ERROR_FOR("Tools", query);
            continue;
        }
        query.finish();

        // create a new MID
//        req = QString("INSERT INTO MOLS (MID, SID, NAME) VALUES (NULL,%1,\"%2\");")
//              .arg(sid)
//              .arg(name);
        query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_MOLS));
        query.bindValue(DrugsDB::Constants::MOLS_MID, QVariant());
        query.bindValue(DrugsDB::Constants::MOLS_NAME, name);
        query.bindValue(DrugsDB::Constants::MOLS_SID, sid);
        query.bindValue(DrugsDB::Constants::MOLS_WWW, QVariant());
        if (query.exec()) {
            mids.insert(query.lastInsertId().toInt(), name);
            continue;
        } else {
            LOG_QUERY_ERROR_FOR("Tools", query);
            continue;
        }
        query.finish();
    }
    db.commit();
    return mids;
}

bool createAtc(const QString &connection, const QString &code, const QMultiHash<QString, QVariant> &trLabels, const int forceAtcId, const bool warnDuplicates)
{
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen())
        return false;
    db.transaction();
    QSqlQuery query(db);
    int id = 0;
    QString req;
    query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_ATC));
    query.bindValue(DrugsDB::Constants::ATC_ID, QVariant());
    query.bindValue(DrugsDB::Constants::ATC_CODE, code);
    query.bindValue(DrugsDB::Constants::ATC_WARNDUPLICATES, warnDuplicates);

    if (forceAtcId==-1) {
//        req = QString("INSERT INTO ATC  (ATC_ID, CODE, WARNDUPLICATES) "
//                  "VALUES (NULL, '%1', %2) ").arg(code).arg(warnDuplicates);
    } else {
//        req = QString("INSERT INTO ATC  (ATC_ID, CODE, WARNDUPLICATES) "
//                  "VALUES (%1, '%2', %3) ").arg(forceAtcId).arg(code).arg(warnDuplicates);
        query.bindValue(DrugsDB::Constants::ATC_ID, forceAtcId);
    }
    if (query.exec()) {
        id = query.lastInsertId().toInt();
        if (forceAtcId!=-1 && forceAtcId!=id) {
            LOG_ERROR_FOR("Tools", QString("Wrong ATC_ID Db=%1 / Asked=%2").arg(id).arg(forceAtcId));
            db.rollback();
            return false;
        }
    } else {
        LOG_QUERY_ERROR_FOR("Tools", query);
    }
    query.finish();

    // Create labels
    int masterLid = Core::Tools::addLabels(Core::Constants::MASTER_DATABASE_NAME, -1, trLabels);
    if (masterLid == -1) {
        LOG_ERROR_FOR("Tools", "No MasterLid");
        db.rollback();
        return false;
    }

    // Create ATC_LABELS link
//    req = QString("INSERT INTO ATC_LABELS (ATC_ID, MASTER_LID) VALUES (%1, %2) ").arg(id).arg(masterLid);
    query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_ATC_LABELS));
    query.bindValue(DrugsDB::Constants::ATC_LABELS_ATCID, id);
    query.bindValue(DrugsDB::Constants::ATC_LABELS_MASTERLID, masterLid);
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Tools", query);
        db.rollback();
        return false;
    }
    query.finish();

    db.commit();
    return true;
}

bool addInteraction(const QString &connection, const QStringList &atc1, const QStringList &atc2, const QString &type, const QMultiHash<QString, QVariant> &risk, const QMultiHash<QString, QVariant> &management)
{
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen())
        return false;
    QSqlQuery query(db);
    QString req;
    int iak_id = -1;
    QList<int> ia_ids;

    foreach(const QString &a1, atc1) {
        foreach(const QString &a2, atc2) {
//            req = QString("INSERT INTO INTERACTIONS (ATC_ID1, ATC_ID2) VALUES (%1, %2);")
//                    .arg(QString("(SELECT ATC_ID FROM ATC WHERE CODE=\"%1\")").arg(a1))
//                    .arg(QString("(SELECT ATC_ID FROM ATC WHERE CODE=\"%1\")").arg(a2));
            int atcId1 = -1;
            int atcId2 = -1;
            QHash<int, QString> w;
            w.insert(DrugsDB::Constants::ATC_CODE, QString("='%1'").arg(a1));
            req = Core::Tools::drugBase()->select(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID, w);
            if (query.exec(req)) {
                atcId1 = query.lastInsertId().toInt();
            } else {
                LOG_QUERY_ERROR_FOR("Tools", query);
                return false;
            }
            w.clear();
            w.insert(DrugsDB::Constants::ATC_CODE, QString("='%1'").arg(a2));
            req = Core::Tools::drugBase()->select(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID, w);
            if (query.exec(req)) {
                atcId2 = query.lastInsertId().toInt();
            } else {
                LOG_QUERY_ERROR_FOR("Tools", query);
                return false;
            }

            query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_INTERACTIONS));
            query.bindValue(DrugsDB::Constants::INTERACTIONS_IAID, QVariant());
            query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID1, atcId1);
            query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID2, atcId2);
            if (query.exec()) {
                ia_ids << query.lastInsertId().toInt();
            } else {
                LOG_QUERY_ERROR_FOR("Tools", query);
                return false;
            }
            query.finish();
        }
    }
    // Check errors
    if (!ia_ids.count()) {
        LOG_ERROR_FOR("Tools", QString("Interaction not added : %1   //  %2").arg(atc1.join(",")).arg(atc2.join(",")));
        return false;
    }
    // Add labels
    int riskMasterLid = addLabels(Core::Constants::MASTER_DATABASE_NAME, -1, risk);
    int manMasterLid = addLabels(Core::Constants::MASTER_DATABASE_NAME, -1, management);
    if (riskMasterLid==-1 || manMasterLid==-1)
        return false;

    // Add IAK
    // TODO: add bibliography */
//    req = QString("INSERT INTO IAKNOWLEDGE (IAKID,TYPE,RISK_MASTER_LID,MAN_MASTER_LID) VALUES "
//                  "(NULL, \"%1\", %2, %3)")
//            .arg(type)
//            .arg(riskMasterLid)
//            .arg(manMasterLid);
    query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_IAKNOWLEDGE));
    query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_IAKID, QVariant());
    query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_TYPE, type);
    query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_RISK_MASTERLID, riskMasterLid);
    query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_BIB_MASTERID, QVariant());
    query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_MANAGEMENT_MASTERLID, manMasterLid);
    query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_WWW, QVariant());
    if (query.exec()) {
        iak_id = query.lastInsertId().toInt();
    } else {
        LOG_QUERY_ERROR_FOR("Tools", query);
        return false;
    }
    query.finish();
    if (iak_id==-1)
        return false;

    // Add to IA_IAK
    foreach(const int ia, ia_ids) {
//        req = QString("INSERT INTO IA_IAK (IAID,IAKID) VALUES (%1,%2)")
//              .arg(ia)
//              .arg(iak_id)
//              ;
        query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_IA_IAK));
        query.bindValue(DrugsDB::Constants::IA_IAK_IAID, ia);
        query.bindValue(DrugsDB::Constants::IA_IAK_IAKID, iak_id);
        if (!query.exec()) {
            LOG_QUERY_ERROR_FOR("Tools", query);
            return false;
        }
        query.finish();
    }

    return true;
}

int addBibliography(const QString &connection, const QString &type, const QString &link, const QString &reference, const QString &abstract, const QString &explain, const QString &xml)
{
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen()) {
        if (!db.open()) {
            return false;
        }
    }
    QString req;
    db.transaction();
    QSqlQuery query(db);

    // Bibliography exists ?
    int bib_id = -1;
//    req = QString("SELECT BIB_ID FROM BIBLIOGRAPHY WHERE LINK=\"%1\"").arg(link);
    QHash<int, QString> w;
    w.insert(DrugsDB::Constants::BIB_LINK, QString("=%1'").arg(link));
    req = drugBase()->select(DrugsDB::Constants::Table_BIB, DrugsDB::Constants::BIB_BIBID, w);
    if (query.exec(req)) {
        if (query.next()) {
            bib_id = query.value(0).toInt();
        } else {
            // Create the bib and retreive the bib_id
//            QString t = type;
//            QString l = link;
//            QString r = reference;
//            QString a = abstract;
//            QString e = explain;
//            req = QString("INSERT INTO BIBLIOGRAPHY "
//                          "(BIB_ID,TYPE,LINK,TEXTUAL_REFERENCE,ABSTRACT,EXPLANATION) VALUES"
//                          "(NULL, '%1', '%2', '%3', '%4', '%5')")
//                    .arg(t.replace("'","''"))
//                    .arg(l.replace("'","''"))
//                    .arg(r.replace("'","''"))
//                    .arg(a.replace("'","''"))
//                    .arg(e.replace("'","''"));
            query.finish();
            // TODO: add bibliography XML here ? */
            query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_BIB));
            query.bindValue(DrugsDB::Constants::BIB_BIBID, QVariant());
            query.bindValue(DrugsDB::Constants::BIB_TYPE, type);
            query.bindValue(DrugsDB::Constants::BIB_LINK, link);
            query.bindValue(DrugsDB::Constants::BIB_TEXTREF, reference);
            query.bindValue(DrugsDB::Constants::BIB_ABSTRACT, abstract);
            query.bindValue(DrugsDB::Constants::BIB_EXPLAIN, explain);
            query.bindValue(DrugsDB::Constants::BIB_XML, xml);
            if (query.exec()) {
                bib_id = query.lastInsertId().toInt();
            } else {
                LOG_QUERY_ERROR_FOR("Tools", query);
                db.rollback();
                return false;
            }
            query.finish();
        }
    } else {
        LOG_QUERY_ERROR_FOR("Tools", query);
        db.rollback();
        return false;
    }
    query.finish();
    db.commit();
    return bib_id;
}

bool addComponentAtcLinks(const QString &connection, const QMultiHash<int, int> &mol_atc, const int sid)
{
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen()) {
        if (!db.open()) {
            return false;
        }
    }

    // Clear existing links
    QHash<int, QString> w;
    w.insert(DrugsDB::Constants::LK_ATC_SID, QString("='%1'").arg(sid));
    Core::Tools::executeSqlQuery(drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_LK_MOL_ATC, w), Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);

    // Save to links to drugs database
    db.transaction();
    QSqlQuery query(db);
    foreach(int mol, mol_atc.uniqueKeys()) {
        QList<int> atcCodesSaved;
        foreach(int atc, mol_atc.values(mol)) {
            if (atcCodesSaved.contains(atc))
                continue;
            atcCodesSaved.append(atc);
//            QString req = QString("INSERT INTO LK_MOL_ATC (MID,ATC_ID,SID) VALUES (%1, %2, %3)")
//                          .arg(mol).arg(atc).arg(sid);
            query.prepare(Core::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_LK_MOL_ATC));
            query.bindValue(DrugsDB::Constants::LK_MID, mol);
            query.bindValue(DrugsDB::Constants::LK_ATC_ID, atc);
            query.bindValue(DrugsDB::Constants::LK_ATC_SID, sid);
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR("Tools", query);
                db.rollback();
                return false;
            }
            query.finish();
        }
    }
    db.commit();
    return true;
}

QVector<int> getAtcIdsFromLabel(const QString &connection, const QString &label)
{
    QVector<int> ret;
    QString req;
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen()) {
        if (!db.open()) {
            return ret;
        }
    }

//    req = QString("SELECT DISTINCT ATC.ATC_ID FROM ATC "
//                  "JOIN ATC_LABELS ON ATC_LABELS.ATC_ID=ATC.ATC_ID "
//                  "JOIN LABELS_LINK ON LABELS_LINK.MASTER_LID=ATC_LABELS.MASTER_LID "
//                  "JOIN LABELS ON LABELS_LINK.LID=LABELS.LID "
//                  "WHERE LABELS.LABEL like \"%1\";").arg(label);
    Utils::FieldList get;
    get << Utils::Field(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID);
    Utils::JoinList joins;
    joins << Utils::Join(DrugsDB::Constants::Table_ATC_LABELS, DrugsDB::Constants::ATC_LABELS_ATCID, DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID)
          << Utils::Join(DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_MASTERLID, DrugsDB::Constants::Table_ATC_LABELS, DrugsDB::Constants::ATC_LABELS_MASTERLID)
          << Utils::Join(DrugsDB::Constants::Table_LABELS, DrugsDB::Constants::LABELS_LID, DrugsDB::Constants::Table_LABELSLINK, DrugsDB::Constants::LABELSLINK_LID);
    Utils::FieldList cond;
    cond << Utils::Field(DrugsDB::Constants::Table_LABELS, DrugsDB::Constants::LABELS_LABEL, QString("='%1'").arg(label));
    QSqlQuery query(db);

//    qWarning() << "LKKLKLKLK\n" << drugBase()->select(get,joins,cond);

    if (query.exec(drugBase()->select(get,joins,cond))) {
        while (query.next()) {
            ret << query.value(0).toInt();
        }
    } else {
        LOG_QUERY_ERROR_FOR("Tools", query);
    }
    return ret;
}

QVector<int> getAtcIdsFromCode(const QString &connection, const QString &code)
{
    QVector<int> ret;
    QString req;
    QSqlDatabase db = QSqlDatabase::database(connection);
    if (!db.isOpen()) {
        if (!db.open()) {
            return ret;
        }
    }
    QSqlQuery query(db);
//    req = QString("SELECT DISTINCT ATC.ATC_ID FROM ATC "
//                  "WHERE ATC.CODE like \"%1\";").arg(code);
    QHash<int, QString> w;
    w.insert(DrugsDB::Constants::ATC_CODE, QString("LIKE '%1'").arg(code));
    req = Core::Tools::drugBase()->select(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID, w);
    if (query.exec(req)) {
        while (query.next()) {
            ret << query.value(0).toInt();
        }
    } else {
        LOG_QUERY_ERROR_FOR("Tools", query);
    }
    return ret;
}

bool saveDrugDatabaseDescription(const QString &fileName, const int completion)
{
    QDomDocument doc;
    int line = 0;
    int col = 0;
    QString error;
    if (!doc.setContent(Utils::readTextFile(fileName, Utils::DontWarnUser), &error, &line, &col)) {
        LOG_ERROR_FOR("Tools", QString("Wrong XML: (%1;%2): %3 in file %4").arg(line).arg(col).arg(error).arg(fileName));
        return false;
    }
    Core::DrugDatabaseDescription descr;
    QDomElement root = doc.firstChildElement("DrugDbDescriptor");
    root = root.firstChildElement("description");
    descr.fromDomElement(root);
    QSqlDatabase db = QSqlDatabase::database(Constants::MASTER_DATABASE_NAME);
    if (!db.isOpen()) {
        if (!db.open()) {
            return false;
        }
    }
    QSqlQuery query(db);
    QHash<int, QString> where;
    where.insert(DrugsDB::Constants::SOURCES_DBUID, QString("='%1'").arg(descr.data(Core::DrugDatabaseDescription::Uuid).toString()));
    query.prepare(Core::Tools::drugBase()->prepareUpdateQuery(DrugsDB::Constants::Table_SOURCES,
                                                              QList<int>()
                                                              << DrugsDB::Constants::SOURCES_LANG
                                                              << DrugsDB::Constants::SOURCES_WEB
                                                              << DrugsDB::Constants::SOURCES_COPYRIGHT
                                                              << DrugsDB::Constants::SOURCES_LICENSE
                                                              << DrugsDB::Constants::SOURCES_DATE
                                                              << DrugsDB::Constants::SOURCES_DRUGS_VERSION
                                                              << DrugsDB::Constants::SOURCES_AUTHORS
                                                              << DrugsDB::Constants::SOURCES_VERSION
                                                              << DrugsDB::Constants::SOURCES_PROVIDER
                                                              << DrugsDB::Constants::SOURCES_WEBLINK
                                                              << DrugsDB::Constants::SOURCES_DRUGUID_NAME
                                                              << DrugsDB::Constants::SOURCES_ATC // 10
                                                              << DrugsDB::Constants::SOURCES_INTERACTIONS
                                                              << DrugsDB::Constants::SOURCES_COMPL_WEBSITE
                                                              << DrugsDB::Constants::SOURCES_PACKUID_NAME
                                                              << DrugsDB::Constants::SOURCES_COMPLETION
                                                              << DrugsDB::Constants::SOURCES_AUTHOR_COMMENTS  // 15
                                                              << DrugsDB::Constants::SOURCES_DRUGNAMECONSTRUCTOR
                                                              << DrugsDB::Constants::SOURCES_FMFCOMPAT
                                                              << DrugsDB::Constants::SOURCES_OPENREACT_COMPAT // 18
                                                              , where));
    int i=0;
    query.bindValue(i, descr.data(Core::DrugDatabaseDescription::Language));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::WebLink));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::Copyright));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::LicenseName));
    query.bindValue(++i, QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::Version));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::Author));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::FreeMedFormsCompatVersion));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::Vendor));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::WebLink));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::DrugUidName));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::IsAtcValid));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::IsDDIValid));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::ComplementaryWebLink));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::PackUidName));
    query.bindValue(++i, completion);
    query.bindValue(++i, QVariant());
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::DrugNameConstructor));
    query.bindValue(++i, descr.data(Core::DrugDatabaseDescription::FreeMedFormsCompatVersion));
    query.bindValue(++i, QVariant());
    if (!query.exec()) {
        LOG_QUERY_ERROR_FOR("Tools", query);
        return false;
    }

    return true;
}

} // end namespace Tools
} // end namespace DrugsDbCreator
