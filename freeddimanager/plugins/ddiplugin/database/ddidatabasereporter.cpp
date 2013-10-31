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
 * \class DDI::Internal::DDIDatabaseReporter
 * Create reports on the ATC/Interactors/DDI using a direct access to the database.
 */

#include "ddidatabasereporter.h"
#include <ddiplugin/constants.h>
#include <ddiplugin/ddicore.h>
#include <ddiplugin/database/ddidatabase.h>

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
const char * const SQL_ISNULL = "=''";
const char * const SQL_ISNOTNULL = "<>''";
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

QString DDIDatabaseReporter::plainTextDatabaseUpdate() const
{
    // Check database
    QSqlDatabase DB = QSqlDatabase::database(ddiBase().connectionName());
    if (!connectDatabase(DB, __FILE__, __LINE__))
        return QString::null;

    QStringList report;
    QHash<int, QString> where;
    report << Utils::centerString("  Database update report  ", '*', 90);

    report << QString("%1 %2 (%3)")
              .arg(QString("Current database version").leftJustified(columnSize, '.'))
              .arg(ddiBase().version())
              .arg(ddiBase().checkDatabaseVersion()?"valid":"**invalid**");

    // Get latest updated ATC
//    QString req;
//    QSqlQuery query(DB);
//    where.insert(Constants::ATC_DATEUPDATE, QString("> '%1'").arg());
//    req = ddiBase().selectDistinct(Constants::Table_INTERACTORS, Constants::INTERACTOR_UID, where);
//    report << Utils::centerString(QString("  Empty interacting class (%1)  ")
//                                  .arg(ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_UID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where))), '.', 90);
//    if (query.exec(req)) {
//        while (query.next()) {
//            report << query.value(0).toString();
//        }
//    }
//    query.finish();

    return report.join("\n");
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
        where.insert(lang.at(i), ::SQL_ISNULL);
        switch (lang.at(i)) {
        case Constants::ATC_FR: langString = QString("French"); break;
        case Constants::ATC_EN: langString = QString("English"); break;
        case Constants::ATC_DE: langString = QString("Deutsch"); break;
        }
        report << Utils::centerString(QString("  Missing %1 translation (%2)  ")
                                      .arg(langString)
                                      .arg(ddiBase().count(Constants::Table_ATC, Constants::ATC_ID, ddiBase().getWhereClause(Constants::Table_ATC, where))), '.', 90);
        QString req = ddiBase().selectDistinct(Constants::Table_ATC, Constants::ATC_CODE, where);
        if (query.exec(req)) {
            while (query.next()) {
                report << query.value(0).toString();
            }
        }
        query.finish();
    }
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

    QStringList report;
    report << Utils::centerString("  Drug interactors report  ", '*', 90);
    QHash<int, QString> where;
    where.insert(Constants::INTERACTOR_ISVALID, "=1");

    // Count Interactors
    where.insert(Constants::INTERACTOR_ISCLASS, "=1");
    report << QString("%1 %2")
              .arg(QString("Number of valid interacting classes").leftJustified(columnSize, '.'))
              .arg(ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where)));
    where.insert(Constants::INTERACTOR_ISCLASS, "=0");
    report << QString("%1 %2")
              .arg(QString("Number of valid interacting classes").leftJustified(columnSize, '.'))
              .arg(ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_ID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where)));

    // Get interacting classes without children
    QString req;
    QSqlQuery query(DB);
    where.insert(Constants::INTERACTOR_ISCLASS, "=1");
    where.insert(Constants::INTERACTOR_CHILDREN, "IS NULL");
    req = ddiBase().selectDistinct(Constants::Table_INTERACTORS, Constants::INTERACTOR_UID, where);
    report << Utils::centerString(QString("  Empty interacting class (%1)  ")
                                  .arg(ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_UID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where))), '.', 90);
    if (query.exec(req)) {
        while (query.next()) {
            report << query.value(0).toString();
        }
    }
    query.finish();

    // Get interacting molecules without ATC linking
    where.insert(Constants::INTERACTOR_ISCLASS, "=1");
    where.insert(Constants::INTERACTOR_ATC, "IS NULL");
    req = ddiBase().selectDistinct(Constants::Table_INTERACTORS, Constants::INTERACTOR_UID, where);
    report << Utils::centerString(QString("  Interacting without ATC link (%1)  ")
                                  .arg(ddiBase().count(Constants::Table_INTERACTORS, Constants::INTERACTOR_UID, ddiBase().getWhereClause(Constants::Table_INTERACTORS, where))), '.', 90);
    if (query.exec(req)) {
        while (query.next()) {
            report << query.value(0).toString();
        }
    }
    query.finish();

    return report.join("\n");
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
    where.insert(Constants::DDI_RISKFR, ::SQL_ISNULL);
    where.insert(Constants::DDI_RISKEN, ::SQL_ISNULL);
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("No textual risk defined").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  No textual risk defined (%1)  ")
                                      .arg(n), '.', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << d->ddiInformationForReport(query.value(0));
            }
        }
        query.finish();
    }

    // Get missing translations
    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_RISKFR, ::SQL_ISNULL);
    where.insert(Constants::DDI_RISKEN, ::SQL_ISNOTNULL);
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Missing french risk translation").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Missing french risk translation (%1)  ")
                                      .arg(n), '.', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << d->ddiInformationForReport(query.value(0));
            }
        }
        query.finish();
    }
    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_MANAGEMENTFR, ::SQL_ISNULL);
    where.insert(Constants::DDI_MANAGEMENTEN, ::SQL_ISNOTNULL);
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Missing french management translation").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Missing french management translation (%1)  ")
                                      .arg(n), '.', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << d->ddiInformationForReport(query.value(0));
            }
        }
        query.finish();
    }

    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_RISKEN, ::SQL_ISNULL);
    where.insert(Constants::DDI_RISKFR, ::SQL_ISNOTNULL);
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Missing english risk translation").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Missing english risk translation (%1)  ")
                                      .arg(n), '.', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << d->ddiInformationForReport(query.value(0));
            }
        }
        query.finish();
    }
    where.clear();
    where.insert(Constants::DDI_ISVALID, "=1");
    where.insert(Constants::DDI_MANAGEMENTEN, ::SQL_ISNULL);
    where.insert(Constants::DDI_MANAGEMENTFR, ::SQL_ISNOTNULL);
    n = ddiBase().count(Constants::Table_DDI, Constants::DDI_UID, ddiBase().getWhereClause(Constants::Table_DDI, where));
    header << QString("%1 %2")
              .arg(QString("Missing english management translation").leftJustified(columnSize, '.'))
              .arg(n);
    if (n > 0) {
        req = ddiBase().selectDistinct(Constants::Table_DDI, Constants::DDI_UID, where);
        report << Utils::centerString(QString("  Missing english management translation (%1)  ")
                                      .arg(n), '.', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << d->ddiInformationForReport(query.value(0));
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
                                      .arg(n), '.', 90);
        report << req;
        if (query.exec(req)) {
            while (query.next()) {
                report << d->ddiInformationForReport(query.value(0));
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
                                      .arg(n), '.', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << d->ddiInformationForReport(query.value(0));
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
                                      .arg(n), '.', 90);
        if (query.exec(req)) {
            while (query.next()) {
                report << d->ddiInformationForReport(query.value(0));
            }
        }
        query.finish();
    }

    // TODO: add user updates (using ddi date update & version date)
    // TODO: add DDI statistics

    // When all DDI are loaded, read all DDI level and store the stat into the
    // m_levelStatistics (K=level name  ; V=number of ddi recorded)
//    void globalLevelStatistics(const QList<DrugDrugInteraction *> &ddis)
//    {
//        int total = 0;
//        for(int i=0; i < ddis.count(); ++i) {
//            DrugDrugInteraction *ddi = ddis.at(i);
//            int n = 0;
//            DrugInteractor *interactor1 = getInteractor(ddi->firstInteractor(), m_interactors);
//            DrugInteractor *interactor2 = getInteractor(ddi->secondInteractor(), m_interactors);
//            if (!interactor1 || !interactor2) {
//                LOG_ERROR_FOR(q, "Interactor not found? " + ddi->firstInteractor() + " - " + ddi->secondInteractor());
//                continue;
//            }
//            if (interactor1->isClass() && interactor2->isClass()) {
//                n = interactor1->childrenCount() * interactor2->childrenCount();
//            } else if (interactor1->isClass() && !interactor2->isClass()) {
//                n = interactor1->childrenCount();
//            } else if (!interactor1->isClass() && interactor2->isClass()) {
//                n = interactor2->childrenCount();
//            } else {
//                n = 1;
//            }

//            foreach(QString name, ddi->levelName().split(", ")) {
//                if (name.isEmpty()) {
//                    name = ddi->levelCode();
//                    qWarning() << "EMPTY LEVEL" << ddi->firstInteractor() << ddi->secondInteractor();
//                }
//                m_levelStatistics.insert(name, m_levelStatistics.value(name, 0) + n);
//                total += n;
//            }
//        }

    header << report;
    return header.join("\n");
}


