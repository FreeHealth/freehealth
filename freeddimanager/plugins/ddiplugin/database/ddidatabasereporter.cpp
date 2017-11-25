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
 *   Main developers: Eric MAEKER, <eric.maeker@gmail.com>                 *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class DDI::Internal::DDIDatabaseReporter
 * Create reports on the ATC/Interactors/DDI using a direct access to the database.
 */

#include "ddidatabasereporter.h"
#include <ddiplugin/constants.h>
#include <ddiplugin/ddicore.h>
#include <ddiplugin/database/ddidatabase.h>
#include <ddiplugin/ddi/drugdruginteraction.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>

#include <QSqlDatabase>

#include <QDebug>

using namespace DDI;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DDI::DDICore *ddiCore()  { return DDI::DDICore::instance(); }
static inline DDI::Internal::DDIDatabase &ddiBase()  { return DDI::DDICore::instance()->database(); }

namespace {
const int columnSize = 60;

static inline bool connectDatabase(QSqlDatabase &DB, const QString &file, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("DDIDatabaseReporte", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 file, line);
            return false;
        }
    }
    return true;
}
} // anonymous namespace

namespace DDI {
namespace Internal {
class DDIDatabaseReporterPrivate
{
public:
    DDIDatabaseReporterPrivate(DDIDatabaseReporter *parent) :
        q(parent)
    {
        Q_UNUSED(q);
    }
    
    ~DDIDatabaseReporterPrivate()
    {
    }
    
    // Returns a basic human readable DDI information using DDI::Constants::DDI_UID field
    QString ddiInformationForReport(const QVariant &uid) const
    {
        // Check database
        QSqlDatabase DB = QSqlDatabase::database(ddiBase().connectionName());
        if (!connectDatabase(DB, __FILE__, __LINE__))
            return "Error";

        QHash<int, QString> where;
        where.insert(Constants::DDI_UID, QString("='%1'").arg(uid.toString()));
        QSqlQuery query(DB);
        QString req = ddiBase().select(Constants::Table_DDI, QList<int>()
                             << Constants::DDI_FIRSTINTERACTORUID
                             << Constants::DDI_SECONDINTERACTORUID
                             << Constants::DDI_LEVELCODE
                             , where);
        if (query.exec(req)) {
            if (query.next()) {
                return QString("%1 - %2 (%3)")
                        .arg(query.value(0).toString())
                        .arg(query.value(1).toString())
                        .arg(query.value(2).toString());
            }
        } else {
            LOG_QUERY_ERROR_FOR("DDIDatabase", query);
        }
        return "Error";
    }

public:
    // Put your data here
    
private:
    DDIDatabaseReporter *q;
};
} // namespace Internal
} // end namespace DDI


/*! Constructor of the DDI::Internal::DDIDatabaseReporter class */
DDIDatabaseReporter::DDIDatabaseReporter(QObject *parent) :
    QObject(parent),
    d(new DDIDatabaseReporterPrivate(this))
{
}

/*! Destructor of the DDI::Internal::DDIDatabaseReporter class */
DDIDatabaseReporter::~DDIDatabaseReporter()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DDIDatabaseReporter::initialize()
{
    return true;
}

/** Returns a full plain text report about the drug-drug interaction database */
QString DDIDatabaseReporter::plainTextFullReport() const
{
    QString report;
    report += plainTextDatabaseUpdate();
    report += "\n\n";
    report += plainTextAtcReport();
    report += "\n\n";
    report += plainTextInteractorsReport();
    report += "\n\n";
    report += plainTextDrugDrugInteractionsReport();
    report += "\n\n";
    report += plainTextDrugDrugInteractionsStatistics();
    return report;
}

/**
 * Check the database version and user updates
 * and return a formatted plain text report.
 */
QString DDIDatabaseReporter::plainTextDatabaseUpdate() const
{
    // Check database
    QSqlDatabase DB = QSqlDatabase::database(ddiBase().connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString::null;
    DB.transaction();

    QStringList report, header;
    QHash<int, QString> where;
    header << Utils::centerString("  Database report  ", '*', 90);
    QFileInfo file(ddiBase().database().databaseName());
    header << Utils::centerString(file.fileName(), ' ', 90);
    header << QString("%1 %2")
              .arg(QString("File size").leftJustified(columnSize, '.'))
              .arg(Utils::humanReadableFileSize(file.size()));
    header << QString("%1 %2")
              .arg(QString("File date of creation").leftJustified(columnSize, '.'))
              .arg(QLocale().toString(file.created()));
    header << QString("%1 %2")
              .arg(QString("File last modification").leftJustified(columnSize, '.'))
              .arg(QLocale().toString(file.lastModified()));
    header << QString("%1 %2")
              .arg(QString("File last access").leftJustified(columnSize, '.'))
              .arg(QLocale().toString(file.lastRead()));
    header << QString("%1 %2")
              .arg(QString("Database connection name").leftJustified(columnSize, '.'))
              .arg(ddiBase().database().connectionName());
    header << QString("%1 %2")
              .arg(QString("Database opened").leftJustified(columnSize, '.'))
              .arg(ddiBase().database().isOpen()?"true":"false");

    header << QString("%1 %2 (%3)")
              .arg(QString("Current database version").leftJustified(columnSize, '.'))
              .arg(ddiBase().version())
              .arg(ddiBase().checkDatabaseVersion()?"valid":"**invalid**");

    // Get latest server update (last time the user sent his data to the FreeMedForms server)
    QDateTime lastUpdate = ddiBase().dateOfLastServerUpdate();
    header << QString("%1 %2")
              .arg(QString("Last time the user sent his data to FreeMedForms server").leftJustified(columnSize, '.'))
              .arg(lastUpdate.isValid()?QLocale().toString(lastUpdate):"Never");

    if (!lastUpdate.isValid())
        lastUpdate = ddiBase().dateOfRelease();

//    if (lastUpdate.isValid()) {
        // Get all ATC codes changes since the last update
        QString req;
        QSqlQuery query(DB);
        where.insert(Constants::ATC_ISVALID, "=1");
        where.insert(Constants::ATC_DATEUPDATE, Constants::SQL_ISNOTNULL);
        where.insert(Constants::ATC_DATEUPDATE, QString("> '%1'").arg(lastUpdate.toString(Qt::ISODate)));
        int n = ddiBase().count(Constants::Table_ATC, Constants::ATC_ID, ddiBase().getWhereClause(Constants::Table_ATC, where));
        header << QString("%1 %2")
                  .arg(QString("Updated ATC codes").leftJustified(columnSize, '.'))
                  .arg(n);
        if (n > 0) {
            report << Utils::centerString(QString("  Updated ATC codes (%1)  ").arg(n), '-', 90);
            req = ddiBase().selectDistinct(Constants::Table_ATC, QList<int>()
                                           << Constants::ATC_CODE
                                           << Constants::ATC_FR
                                           << Constants::ATC_EN
                                           << Constants::ATC_DE
                                           , where);
            if (query.exec(req)) {
                while (query.next()) {
                    // FIXME: use tkTr(Trans::Constants::_1_COLON_2)
                    report << QString("     %1: %2 / %3 / %4")
                              .arg(query.value(0).toString())
                              .arg(query.value(1).toString())
                              .arg(query.value(2).toString())
                              .arg(query.value(3).toString());
                }
            }
            query.finish();
        }

        // Get all interactor changes since the last update
        where.clear();
        where.insert(Constants::INTERACTOR_ISVALID, "=1");
        where.insert(Constants::INTERACTOR_DATEUPDATE, QString("> '%1'").arg(lastUpdate.toString(Qt::ISODate)));
        n = ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_DATEUPDATE, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where));
        header << QString("%1 %2")
                  .arg(QString("Updated Interactors").leftJustified(columnSize, '.'))
                  .arg(n);
        if (n > 0) {
            report << Utils::centerString(QString("  Updated Interactors (%1)  ").arg(n), '-', 90);
            req = ddiBase().selectDistinct(Constants::Table_INTERACTORS, QList<int>()
                                           << Constants::INTERACTOR_UID
                                           << Constants::INTERACTOR_FR
                                           << Constants::INTERACTOR_EN
                                           << Constants::INTERACTOR_DE
                                           , where);
            if (query.exec(req)) {
                while (query.next()) {
                    // FIXME: use tkTr(Trans::Constants::_1_COLON_2)
                    report << QString("     %1: %2 / %3 / %4")
                              .arg(query.value(0).toString())
                              .arg(query.value(1).toString())
                              .arg(query.value(2).toString())
                              .arg(query.value(3).toString());
                }
            }
            query.finish();
        }

        // Get all drug-drug interactions changes since the last update
        where.clear();
        where.insert(Constants::DDI_ISVALID, "=1");
        where.insert(Constants::DDI_DATEUPDATE, QString("> '%1'").arg(lastUpdate.toString(Qt::ISODate)));
        n = ddiBase().count(Constants::Table_DDI, Constants::DDI_DATEUPDATE, ddiBase().getWhereClause(Constants::Table_DDI, where));
        header << QString("%1 %2")
                  .arg(QString("Updated Drug interactions").leftJustified(columnSize, '.'))
                  .arg(n);
        if (n > 0) {
            report << Utils::centerString(QString("  Updated Drug interactions (%1)  ").arg(n), '-', 90);
            req = ddiBase().selectDistinct(Constants::Table_INTERACTORS, QList<int>()
                                           << Constants::DDI_UID
                                           << Constants::DDI_FIRSTINTERACTORUID
                                           << Constants::DDI_SECONDINTERACTORUID
                                           << Constants::DDI_LEVELCODE
                                           , where);
            if (query.exec(req)) {
                while (query.next()) {
                    // FIXME: use tkTr(Trans::Constants::_1_COLON_2)
                    report << QString("     %1: %2 - %3 (%4)")
                              .arg(query.value(0).toString())
                              .arg(query.value(1).toString())
                              .arg(query.value(2).toString())
                              .arg(query.value(3).toString())
                              ;
                }
            }
            query.finish();
        }
//    }

    DB.commit();
    header << report;
    return header.join("\n");
}

/**
 * Check the database for ATC code errors, missing translations, etc...
 * and return a formatted plain text report.
 */
QString DDIDatabaseReporter::plainTextAtcReport() const
{
    // Check database
    QSqlDatabase DB = QSqlDatabase::database(ddiBase().connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString::null;
    DB.transaction();

    QStringList report;
    QHash<int, QString> where;
    where.insert(Constants::ATC_ISVALID, "=1");
    report << Utils::centerString("  ATC Code report  ", '*', 90);

    // Count ATC codes
    report << QString("%1 %2")
              .arg(QString("Number of valid ATC codes:").leftJustified(columnSize, '.'))
              .arg(ddiBase().count(Constants::Table_ATC, Constants::ATC_ID, ddiBase().getWhereClause(Constants::Table_ATC, where)));

    // Get all missing translations
    QList<int> lang;
    lang << Constants::ATC_FR << Constants::ATC_EN << Constants::ATC_DE;
    QSqlQuery query(DB);
    QString langString;
    for(int i=0; i < lang.count(); ++i) {
        where.insert(lang.at(i), Constants::SQL_ISNULL);
        switch (lang.at(i)) {
        case Constants::ATC_FR: langString = QString("French"); break;
        case Constants::ATC_EN: langString = QString("English"); break;
        case Constants::ATC_DE: langString = QString("Deutsch"); break;
        }
        report << Utils::centerString(QString("  Missing %1 translation (%2)  ")
                                      .arg(langString)
                                      .arg(ddiBase().count(Constants::Table_ATC, Constants::ATC_ID, ddiBase().getWhereClause(Constants::Table_ATC, where))), '-', 90);
        QString req = ddiBase().selectDistinct(Constants::Table_ATC, Constants::ATC_CODE, where);
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(query.value(0).toString(), 5);
            }
        }
        query.finish();
    }
    DB.commit();
    return report.join("\n");
}

/**
 * Check the database for interactors errors, missing translations, missing ATC linking, etc...
 * and return a formatted plain text report.
 */
QString DDIDatabaseReporter::plainTextInteractorsReport() const
{
    // Check database
    QSqlDatabase DB = QSqlDatabase::database(ddiBase().connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString::null;
    DB.transaction();

    QStringList report, header;
    header << Utils::centerString("  Drug interactors report  ", '*', 90);
    QHash<int, QString> where;
    where.insert(Constants::INTERACTOR_ISVALID, "=1");

    // Count Interactors
    where.insert(Constants::INTERACTOR_ISCLASS, "=1");
    header << QString("%1 %2")
              .arg(QString("Number of valid interacting classes").leftJustified(columnSize, '.'))
              .arg(ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where)));
    where.insert(Constants::INTERACTOR_ISCLASS, "=0");
    header << QString("%1 %2")
              .arg(QString("Number of valid interacting molecules").leftJustified(columnSize, '.'))
              .arg(ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where)));

    // Get interacting classes without children
    QString req;
    QSqlQuery query(DB);
    where.clear();
    where.insert(Constants::INTERACTOR_ISVALID, "=1");
    where.insert(Constants::INTERACTOR_ISCLASS, "=1");
    where.insert(Constants::INTERACTOR_CHILDREN, "IS NULL"); // Constants::SQL_ISNULL cause an error
    int n = ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_UID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where));
    header << QString("%1 %2")
              .arg(QString("Empty interacting class").leftJustified(columnSize, '.'))
              .arg(ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where)));
    if (n > 0) {
        report << Utils::centerString(QString("  Empty interacting class (%1)  ")
                                      .arg(n), '-', 90);
        req = ddiBase().selectDistinct(Constants::Table_INTERACTORS, QList<int>()
                                       << Constants::INTERACTOR_UID
                                       << Constants::INTERACTOR_FR
                                       << Constants::INTERACTOR_EN
                                       << Constants::INTERACTOR_DE
                                       << Constants::INTERACTOR_COMMENT
                                       , where);
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(QString("%1\n    fr: %2\n    en: %3")
                          .arg(query.value(0).toString())
                          .arg(query.value(1).toString())
                          .arg(query.value(2).toString()), 10);
                if (!query.value(4).toString().isEmpty())
                    report << Utils::indentString(query.value(4).toString(), 10);
            }
        }
        query.finish();
    }

    // Get interacting molecules without ATC linking
    where.clear();
    where.insert(Constants::INTERACTOR_ISVALID, "=1");
    where.insert(Constants::INTERACTOR_ISCLASS, "=0");
    where.insert(Constants::INTERACTOR_ATC, "IS NULL"); // Constants::SQL_ISNULL cause an error
    n = ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_UID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where));
    header << QString("%1 %2")
              .arg(QString("Interacting molecules without ATC link").leftJustified(columnSize, '.'))
              .arg(ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where)));
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_INTERACTORS, QList<int>()
                                       << Constants::INTERACTOR_UID
                                       << Constants::INTERACTOR_FR
                                       << Constants::INTERACTOR_EN
                                       << Constants::INTERACTOR_DE
                                       << Constants::INTERACTOR_COMMENT, where);
        report << Utils::centerString(QString("  Interacting molecules without ATC link (%1)  ")
                                      .arg(n), '-', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(QString("%1\n    fr: %2\n    en: %3")
                          .arg(query.value(0).toString())
                          .arg(query.value(1).toString())
                          .arg(query.value(2).toString()), 10);
                if (!query.value(4).toString().isEmpty())
                    report << Utils::indentString(query.value(4).toString(), 10);
            }
        }
        query.finish();
    }
    DB.commit();
    header << report;
    return header.join("\n");
}

/**
 * Check the database for drug-drug interaction errors,
 * missing translations, missing ATC linking, etc...
 * and return a formatted plain text report.
 */
QString DDIDatabaseReporter::plainTextDrugDrugInteractionsReport() const
{
    // Check database
    QSqlDatabase DB = QSqlDatabase::database(ddiBase().connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString::null;
    DB.transaction();

    QStringList report;
    QHash<int, QString> where;
    where.insert(Constants::DDI_ISVALID, "=1");

    // Count DDIs
    QStringList header;
    header << Utils::centerString("  Drug-Drug Interactions report  ", '*', 90);
    int n = 0;
    header << QString("%1 %2")
              .arg(QString("Number of valid DDIs").leftJustified(columnSize, '.'))
              .arg(ddiBase().count(Constants::Table_DDI, Constants::DDI_ID, ddiBase().getWhereClause(Constants::Table_DDI, where)));

    // Count unreviewed
    where.insert(Constants::DDI_ISREVIEWED, "=1");
    header << QString("%1 %2")
              .arg(QString("Number of unreviewed DDIs").leftJustified(columnSize, '.'))
              .arg(ddiBase().count(Constants::Table_DDI, Constants::DDI_ID, ddiBase().getWhereClause(Constants::Table_DDI, where)));

    QString req;
    QSqlQuery query(DB);

    // Get 'no risk' DDIs
    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_RISKFR, Constants::SQL_ISNULL);
    where.insert(Constants::DDI_RISKEN, Constants::SQL_ISNULL);
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("No textual risk defined").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  No textual risk defined (%1)  ")
                                      .arg(n), '-', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(d->ddiInformationForReport(query.value(0)), 5);
            }
        }
        query.finish();
    }

    // Get missing translations
    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_RISKFR, Constants::SQL_ISNULL);
    where.insert(Constants::DDI_RISKEN, Constants::SQL_ISNOTNULL);
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Missing french risk translation").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Missing french risk translation (%1)  ")
                                      .arg(n), '-', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(d->ddiInformationForReport(query.value(0)), 5);
            }
        }
        query.finish();
    }
    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_MANAGEMENTFR, Constants::SQL_ISNULL);
    where.insert(Constants::DDI_MANAGEMENTEN, Constants::SQL_ISNOTNULL);
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Missing french management translation").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Missing french management translation (%1)  ")
                                      .arg(n), '-', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(d->ddiInformationForReport(query.value(0)), 5);
            }
        }
        query.finish();
    }

    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_RISKEN, Constants::SQL_ISNULL);
    where.insert(Constants::DDI_RISKFR, Constants::SQL_ISNOTNULL);
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Missing english risk translation").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Missing english risk translation (%1)  ")
                                      .arg(n), '-', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(d->ddiInformationForReport(query.value(0)), 5);
            }
        }
        query.finish();
    }
    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_MANAGEMENTEN, Constants::SQL_ISNULL);
    where.insert(Constants::DDI_MANAGEMENTFR, Constants::SQL_ISNOTNULL);
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Missing english management translation").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Missing english management translation (%1)  ")
                                      .arg(n), '-', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(d->ddiInformationForReport(query.value(0)), 5);
            }
        }
        query.finish();
    }

    // Get multi-level DDI (450 or one char)
    // WARNING: this can evolve with future version of the database
    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_LEVELCODE, "<>\"450\"");
    where.insert(-1, QString("LENGTH(%1) > 1").arg(ddiBase().fieldName(Constants::Table_DDI, Constants::DDI_LEVELCODE)));
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Multi-level interactions").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Multi-level interactions (%1)  ")
                                      .arg(n), '-', 90);
        // report << req;
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(d->ddiInformationForReport(query.value(0)), 5);
            }
        }
        query.finish();
    }

    // Get interactors uid error
    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_FIRSTINTERACTORUID, QString("NOT IN (%1)").arg(ddiBase().selectDistinct(Constants::Table_INTERACTORS, Constants::INTERACTOR_UID)));
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Unknown first interactor").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Unknown first interactor (%1)  ")
                                      .arg(n), '-', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(d->ddiInformationForReport(query.value(0)), 5);
            }
        }
        query.finish();
    }
    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_SECONDINTERACTORUID, QString("NOT IN (%1)").arg(ddiBase().selectDistinct(Constants::Table_INTERACTORS, Constants::INTERACTOR_UID)));
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Unknown second interactor").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Unknown second interactor (%1)  ")
                                      .arg(n), '-', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << Utils::indentString(d->ddiInformationForReport(query.value(0)), 5);
            }
        }
        query.finish();
    }

    DB.commit();
    header << report;
    return header.join("\n");
}

QString DDIDatabaseReporter::plainTextDrugDrugInteractionsStatistics() const
{
    // Check database
    QSqlDatabase DB = QSqlDatabase::database(ddiBase().connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString::null;
    DB.transaction();

    QStringList report;
    QHash<QString, int> stats;
    int total = 0;
    QHash<int, QString> where;
    where.insert(Constants::DDI_ISVALID, "=1");

    QString req;
    QSqlQuery query(DB);
    req = ddiBase().selectDistinct(Constants::Table_DDI, QList<int>()
                                   << Constants::DDI_UID
                                   << Constants::DDI_FIRSTINTERACTORUID
                                   << Constants::DDI_SECONDINTERACTORUID
                                   << Constants::DDI_LEVELCODE
                                   , where);
    report << Utils::centerString(QString("  Drug-drug interactions statistics  "), '*', 90);
    if (query.exec(req)) {
        while (query.next()) {
            int firstChildren = 0;
            int secondChildren = 0;
            int n = 0;
            const QString &firstUid = query.value(1).toString();
            const QString &secondUid = query.value(2).toString();
            const QString &level = query.value(3).toString();

            where.clear();
            where.insert(Constants::INTERACTOR_ISVALID, "=1");
            where.insert(Constants::INTERACTOR_UID, QString("='%1'").arg(firstUid));
            req = ddiBase().select(Constants::Table_INTERACTORS, Constants::INTERACTOR_CHILDREN, where);
            QSqlQuery query2(DB);
            if (query2.exec(req)) {
                if (query2.next()) {
                    if (!query2.value(0).toString().isEmpty())
                        firstChildren = query2.value(0).toString().count(";") + 1;
                }
            } else {
                LOG_QUERY_ERROR_FOR("DDIDatabaseReporter", query2);
            }

            where.clear();
            where.insert(Constants::INTERACTOR_ISVALID, "=1");
            where.insert(Constants::INTERACTOR_UID, QString("='%1'").arg(secondUid));
            req = ddiBase().select(Constants::Table_INTERACTORS, Constants::INTERACTOR_CHILDREN, where);
            if (query2.exec(req)) {
                if (query2.next()) {
                    if (!query2.value(0).toString().isEmpty())
                        secondChildren = query2.value(0).toString().count(";") + 1;
                }
            } else {
                LOG_QUERY_ERROR_FOR("DDIDatabaseReporter", query2);
            }

            // Both interactors are classes
            if (firstChildren && secondChildren)
                n = firstChildren * secondChildren;
            // Only one is a class
            else if (firstChildren || secondChildren)
                n = firstChildren + secondChildren;
            // Both are molecules
            else n = 1;

            // We suppose here that none the DDI are multi-level
            stats.insert(level, stats.value(level, 0) + n);
            total += n;
        }
    }
    query.finish();
    DB.commit();

    QStringList statsOrder;
    statsOrder.append("C");
    statsOrder.append("D");
    statsOrder.append("450");
    statsOrder.append("Y");
    statsOrder.append("T");
    statsOrder.append("P");
    statsOrder.append("I");
    foreach(const QString &level, statsOrder) {
        int n = stats.value(level);
        stats.remove(level);
        report << QString("     %1 %2 %3%")
                  .arg(DrugDrugInteraction::levelName(level).leftJustified(50, '.'))
                  .arg(QString::number(n).leftJustified(7))
                  .arg(QString::number(n*100/total, 'G', 2));
    }

    QHashIterator<QString, int> it(stats);
    int multi = 0;
    while (it.hasNext()) {
        it.next();
        multi += it.value();
    }
    if (multi)
        report << QString("     %1 %2 %3%")
                  .arg(QString("Multi-level").leftJustified(50, '.'))
                  .arg(QString::number(multi).leftJustified(7))
                  .arg(QString::number(multi*100/total, 'G', 2));

    report << QString("\n     %1 %2 100%")
            .arg(QString("Total DDI per INN").leftJustified(50, '.'))
            .arg(QString::number(total).leftJustified(7));

    return report.join("\n");
}


