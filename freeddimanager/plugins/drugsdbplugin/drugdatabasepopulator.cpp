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
 * \class DrugsDb::Internal::DatabasePopulator
 * Populate a Drug database with DDI information (ATC codes, DDI, PIMs and
 * all other interaction engines data).\n
 * Drugs database are represented by DrugsDB::Internal::DrugBaseEssentials while
 * FreeDDIManager database is represented by DDI::Internal::DDIDatabase.
 *
 * ATC codes:
 * All ATC codes are injected from the WHO ATC data and from the FreeMedForms
 * specific interactors. Explanation: if an interactor does not exist in the
 * WHO ATC data, then a fake ATC code is created for this interactor (eg an
 * interacting class). These fake codes starts with "ZXX" for interacting classes
 * and "Z00" for molecules (like fluindione).
 *
 * Drug-Drug interactions in the final drugs database:
 * A DDI is defined by an interaction between two interactors (ATC codes). \n
 * In the final drugs database (DrugsDB::Internal::DrugBaseEssentials) all DDI
 * are mirrored (A1 interacts with A2, A2 interacts with A1). \n
 * All classes are expanded (if an interactor is a interacting class, all its
 * children are used one by one to define all interactions, and if a child is
 * a class the class is expanded and so one). \n
 *
 * PIMs:
 * \todo
 */

#include "drugdatabasepopulator.h"
#include <drugsdbplugin/tools.h>

#include <ddiplugin/ddicore.h>
#include <ddiplugin/constants.h>
#include <ddiplugin/database/ddidatabase.h>
#include <ddiplugin/atc/atctablemodel.h>
#include <ddiplugin/ddi/drugdruginteraction.h>
#include <ddiplugin/interactors/druginteractor.h>

#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

//#include <biblio/bibliocore.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/fdm_constants.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_database.h>

#include <QHash>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>

using namespace DDI;
using namespace DrugsDb;
using namespace DrugsDb::Internal;
using namespace Trans::ConstantTranslations;

static inline DDI::DDICore *ddiCore() {return DDI::DDICore::instance();}
//static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

namespace  {
const int CLASS_OR_MOL_ID = 65000;
const int FREEMEDFORMS_ATC_CODE = 65001;

static inline bool connectDatabase(QSqlDatabase &DB, const QString &file, const int line)
{
    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError("DrugDatabasePopulator", tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()),
                                 file, line);
            return false;
        }
    }
    return true;
}
}

namespace DrugsDb {
namespace Internal {
class DrugDatabasePopulatorPrivate
{
public:
    DrugDatabasePopulatorPrivate(DrugDatabasePopulator *parent) :
        q(parent)
    {
    }

    ~DrugDatabasePopulatorPrivate()
    {
    }

    // Save ATC data: class interactors tree
    bool saveClassDrugInteractor(DrugInteractor *interactor, const QList<DrugInteractor *> &completeList, DrugsDB::Internal::DrugBaseEssentials *database, DrugInteractor *parent)
    {
        QSqlDatabase db = database->database();
        if (!connectDatabase(db, __FILE__, __LINE__))
            return false;

        // save interactor
        QSqlQuery query(db);
        int id = -1;
        // save using all associated ATC codes
        const QStringList &atcCodes = interactor->data(DrugInteractor::ATCCodeStringList).toString().split(";");
        if (atcCodes.isEmpty() && !interactor->isClass() && parent && parent->isClass()) {
            query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE));
            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID, QVariant());
            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_CLASS, parent->data(CLASS_OR_MOL_ID).toString());
            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_ATC, interactor->data(CLASS_OR_MOL_ID).toString());
            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_BIBMASTERID, QVariant());

            if (interactor->data(CLASS_OR_MOL_ID).toString().isEmpty())
                qWarning() << "saveClassDrugInteractor::1" << interactor->data(DrugInteractor::Uid).toString()
                           << interactor->data(DrugInteractor::IsClass).toBool()
                           << interactor->data(CLASS_OR_MOL_ID).toString()
                           << interactor->data(DrugInteractor::Id).toString()
                           << parent->data(CLASS_OR_MOL_ID).toString();

            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
            } else {
                id = query.lastInsertId().toInt();
            }
            query.finish();
        } else if (!atcCodes.isEmpty() && !interactor->isClass() && parent && parent->isClass()) {
            foreach(const QString &atc, atcCodes) {
                if (atc.isEmpty()) {
                    LOG_ERROR_FOR(q, QString("DrugInteractor is not linked to any ATC code: %1").arg(interactor->data(DrugInteractor::Uid).toString()));
                    continue;
                }

                QString atcId;
                QHash<int, QString> w;
                w.insert(DrugsDB::Constants::ATC_CODE, QString("='%1'").arg(atc));
                QString req = database->select(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID, w);
                if (query.exec(req)) {
                    if (query.next())
                        atcId = query.value(0).toString();
                } else {
                    LOG_QUERY_ERROR_FOR(q, query);
                    db.rollback();
                    return false;
                }
                query.finish();

                if (atcId.isEmpty()) {
                    LOG_ERROR_FOR(q, QString("DrugInteractor: no Id found: %1").arg(interactor->data(DrugInteractor::Uid).toString()));
                    continue;
                }

                query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE));
                query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID, QVariant());
                query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_CLASS, parent->data(CLASS_OR_MOL_ID).toString());
                query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_ATC, atcId);
                query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_BIBMASTERID, QVariant());

                if (atcId.isEmpty())
                    qWarning() << "saveClassDrugInteractor::2"
                               << interactor->data(DrugInteractor::Uid).toString()
                               << interactor->data(DrugInteractor::IsClass).toBool()
                               << interactor->data(CLASS_OR_MOL_ID).toString()
                               << atc << atcId
                               << parent->data(CLASS_OR_MOL_ID).toString();

                if (!query.exec()) {
                    LOG_QUERY_ERROR_FOR(q, query);
                } else {
                    id = query.lastInsertId().toInt();
                }
                query.finish();
            }
        }

        // add pmids references
        if (id>=0) {
            foreach(const QString &pmid, parent->childClassificationPMIDs(interactor->data(DrugInteractor::Uid).toString())) {
                m_iamTreePmids.insertMulti(id, pmid);
            }
        }

        // if class, include all its children recursively
        if (interactor->isClass()) {
            foreach(const QString &childId, interactor->childrenIds()) {
                // find pointer to the child
                DrugInteractor *child = 0;
                for(int j=0; j < completeList.count();++j) {
                    DrugInteractor *testMe = completeList.at(j);
                    if (testMe->data(DrugInteractor::Uid).toString()==childId) {
                        child = testMe;
                        break;
                    }
                }
                if (child)
                    if (!saveClassDrugInteractor(child, completeList, database, interactor))
                        return false;
            }
        }
        return true;
    }

    // Save the DDI data to the drug database
    bool saveDrugDrugInteractions(DrugsDB::Internal::DrugBaseEssentials *database, const QList<DrugInteractor *> &interactors, const QList<DrugDrugInteraction *> &ddis)
    {
        QSqlDatabase db = database->database();
        if (!connectDatabase(db, __FILE__, __LINE__))
            return false;
        LOG_FOR(q, "Saving drug drug interactions");

        // Clear database first
        QString req;
        req = database->prepareDeleteQuery(DrugsDB::Constants::Table_INTERACTIONS);
        database->executeSQL(req, db);
        req = database->prepareDeleteQuery(DrugsDB::Constants::Table_IAKNOWLEDGE);
        database->executeSQL(req, db);
        req = database->prepareDeleteQuery(DrugsDB::Constants::Table_IA_IAK);
        database->executeSQL(req, db);

        // Create the global transaction
        db.transaction();

        for(int i = 0; i < ddis.count(); ++i) {
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            DrugDrugInteraction *ddi = ddis.at(i);

            // find first && second interactors
            bool firstFound = false;
            bool secondFound = false;
            const QString &first = ddi->firstInteractor();
            const QString &second = ddi->secondInteractor();
            DrugInteractor *firstInteractor = 0;
            DrugInteractor *secondInteractor = 0;
            for(int i=0; i < interactors.count(); ++i) {
                const QString &id = interactors.at(i)->data(DrugInteractor::Uid).toString();
                if (!firstFound) {
                    if (id==first) {
                        firstFound = true;
                        firstInteractor = interactors.at(i);
                    }
                }
                if (!secondFound) {
                    if (id==second) {
                        secondFound = true;
                        secondInteractor = interactors.at(i);
                    }
                }
                if (firstFound && secondFound)
                    break;
            }
            bool ok = (firstFound && secondFound);

            if (!ok) {
                LOG_ERROR_FOR(q, QString("*** Interactors not found: \n  %1 - %2 (%3)")
                              .arg(firstFound?ddi->firstInteractor():ddi->firstInteractor()+"*")
                              .arg(secondFound?ddi->secondInteractor():ddi->secondInteractor()+"*")
                              .arg(ddi->levelName()));
                continue;
            }

            QSqlQuery query(db);
            QString req;
            QList<int> ia_ids;
            int iak_id = -1;

            // for all atc of firstInteractor & all atc of secondInteractor -> add an interaction + keep the references of the DDI
            QStringList atc1 = getAllAtcCodesRelatedToOneDrugInteractor(firstInteractor, interactors);
            QStringList atc2 = getAllAtcCodesRelatedToOneDrugInteractor(secondInteractor, interactors);
            atc1.removeAll("");
            atc1.removeDuplicates();
            atc2.removeAll("");
            atc2.removeDuplicates();
            if (atc1.isEmpty())
                atc1 << firstInteractor->data(FREEMEDFORMS_ATC_CODE).toString();
            if (atc2.isEmpty())
                atc2 << secondInteractor->data(FREEMEDFORMS_ATC_CODE).toString();

            foreach(const QString &a1, atc1) {
                foreach(const QString &a2, atc2) {
                    QString atcId1;
                    QHash<int, QString> w;
                    w.insert(DrugsDB::Constants::ATC_CODE, QString("='%1'").arg(a1));
                    req = database->select(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID, w);
                    if (query.exec(req)) {
                        if (query.next())
                            atcId1 = query.value(0).toString();
                    } else {
                        LOG_QUERY_ERROR_FOR(q, query);
                        db.rollback();
                        return false;
                    }
                    query.finish();

                    QString atcId2;
                    w.clear();
                    w.insert(DrugsDB::Constants::ATC_CODE, QString("='%1'").arg(a2));
                    req = database->select(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID, w);
                    if (query.exec(req)) {
                        if (query.next())
                            atcId2 = query.value(0).toString();
                    } else {
                        LOG_QUERY_ERROR_FOR(q, query);
                        db.rollback();
                        return false;
                    }
                    query.finish();

                    query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_INTERACTIONS));
                    query.bindValue(DrugsDB::Constants::INTERACTIONS_IAID, QVariant());
                    query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID1, atcId1);
                    query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID2, atcId2);

                    if (atcId1.isEmpty() || atcId2.isEmpty()) {
                        LOG_ERROR_FOR(q, QString("No ATC: '%1' - '%2'").arg(atcId1).arg(atcId2));
                        qWarning() << firstInteractor << secondInteractor;
                    }

                    if (!query.exec()) {
                        LOG_QUERY_ERROR_FOR(q, query);
                        LOG_ERROR_FOR(q, QString("*** Interactors not found: \n  %1 - %2 (%3)")
                                  .arg(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())
                                  .arg(ddi->data(DrugDrugInteraction::SecondInteractorName).toString())
                                  .arg(ddi->data(DrugDrugInteraction::LevelName).toString()));
                        db.rollback();
                        return false;
                    } else {
                        ia_ids << query.lastInsertId().toInt();
                    }
                    query.finish();

                    // mirror DDI
                    query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_INTERACTIONS));
                    query.bindValue(DrugsDB::Constants::INTERACTIONS_IAID, QVariant());
                    query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID1, atcId2);
                    query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID2, atcId1);
                    if (!query.exec()) {
                        LOG_QUERY_ERROR_FOR(q, query);
                        LOG_ERROR_FOR(q, QString("*** Interactors not found: \n  %1 - %2 (%3)")
                                  .arg(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())
                                  .arg(ddi->data(DrugDrugInteraction::SecondInteractorName).toString())
                                  .arg(ddi->data(DrugDrugInteraction::LevelName).toString()));
                        db.rollback();
                        return false;
                    } else {
                        ia_ids << query.lastInsertId().toInt();
                    }
                    query.finish();
                }
            }

            // Add labels
            QMultiHash<QString, QVariant> risk;
            risk.insert("fr", ddi->risk("fr"));
            risk.insert("en", ddi->risk("en"));
            QMultiHash<QString, QVariant> management;
            management.insert("fr", ddi->management("fr"));
            management.insert("en", ddi->management("en"));
            int riskMasterLid = DrugsDb::Tools::addLabels(database, -1, risk);
            int manMasterLid = DrugsDb::Tools::addLabels(database, -1, management);
            if (riskMasterLid==-1 || manMasterLid==-1)
                return false;

            // Add IAK
            query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_IAKNOWLEDGE));
            query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_IAKID, QVariant());
            query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_TYPE, ddi->data(DrugDrugInteraction::LevelCode).toString());
            query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_RISK_MASTERLID, riskMasterLid);
            query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_BIB_MASTERID, QVariant());
            query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_MANAGEMENT_MASTERLID, manMasterLid);
            query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_WWW, QVariant());
            if (query.exec()) {
                // keep trace of bibliographic references
                iak_id = query.lastInsertId().toInt();
                foreach(const QString &pmid, ddi->data(DrugDrugInteraction::PMIDsStringList).toStringList())
                    m_ddiPmids.insertMulti(iak_id, pmid);
            } else {
                LOG_QUERY_ERROR_FOR(q, query);
                db.rollback();
                return false;
            }
            query.finish();

            // Add to IA_IAK link table
            foreach(const int ia, ia_ids) {
                query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_IA_IAK));
                query.bindValue(DrugsDB::Constants::IA_IAK_IAID, ia);
                query.bindValue(DrugsDB::Constants::IA_IAK_IAKID, iak_id);
                if (!query.exec()) {
                    LOG_QUERY_ERROR_FOR(q, query);
                    db.rollback();
                    return false;
                }
                query.finish();
            }
        }

        // Commit the global transaction
        db.commit();

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        LOG_FOR(q, "Drug drug interactions saved");

        return true;
    }

    // Save all needed bibliographic references to a drug database
    // \note This member does not create transaction, does not commit or rollback.
    bool saveBibliographicReferences(DrugsDB::Internal::DrugBaseEssentials *database)
    {
        LOG_FOR(q, "Saving bibliographic references in: " + database->database().databaseName());
        // TODO: Ensure all PMIDs are available */
        QSqlDatabase db = database->database();
        if (!connectDatabase(db, __FILE__, __LINE__))
            return false;

        // Clear database first
        QString req = database->prepareDeleteQuery(DrugsDB::Constants::Table_BIB);
        database->executeSQL(req, db);
        req = database->prepareDeleteQuery(DrugsDB::Constants::Table_BIB_LINK);
        database->executeSQL(req, db);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        // Save all pmids
        QHash<QString, int> pmidsBibliographyId;
        QStringList pmids;
        pmids << m_iamTreePmids.values();
        pmids << m_ddiPmids.values();
        pmids.removeAll("");
        pmids.removeDuplicates();
        QSqlQuery query(db);
        foreach(const QString &pmid, pmids) {
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_BIB));
            query.bindValue(DrugsDB::Constants::BIB_BIBID, QVariant());
            query.bindValue(DrugsDB::Constants::BIB_TYPE, "pubmed");
            query.bindValue(DrugsDB::Constants::BIB_LINK, QString("http://www.ncbi.nlm.nih.gov/pubmed/%1?dopt=Abstract&format=text").arg(pmid));
            query.bindValue(DrugsDB::Constants::BIB_TEXTREF, QVariant());
            query.bindValue(DrugsDB::Constants::BIB_ABSTRACT, QVariant());
            query.bindValue(DrugsDB::Constants::BIB_EXPLAIN, QVariant());
            query.bindValue(DrugsDB::Constants::BIB_XML, QVariant());
            // TODO: add bibliographic references of DDI: query.bindValue(DrugsDB::Constants::BIB_XML, Biblio::BiblioCore::instance()->xml(pmid).replace("'","''"));
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
                return false;
            } else {
                pmidsBibliographyId.insert(pmid, query.lastInsertId().toInt());
            }
            query.finish();
        }

        // Save all tree references
        int bibMasterId = 0;
        foreach(int key, m_iamTreePmids.uniqueKeys()) {
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            const QStringList &pmids = m_iamTreePmids.values(key);
            ++bibMasterId;
            QHash<int, QString> w;
            w.insert(DrugsDB::Constants::ATC_CLASS_TREE_ID, QString("='%1'").arg(key));
            query.prepare(database->prepareUpdateQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE,
                                                       DrugsDB::Constants::ATC_CLASS_TREE_BIBMASTERID,
                                                       w));
            query.bindValue(0, bibMasterId);
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
                return false;
            }
            query.finish();
            foreach(const QString &pmid, pmids) {
                // create the master_id for this pmid
                query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_BIB_LINK));
                query.bindValue(DrugsDB::Constants::BIB_LINK_BIBID, pmidsBibliographyId.value(pmid));
                query.bindValue(DrugsDB::Constants::BIB_LINK_MASTERID, bibMasterId);
                if (!query.exec()) {
                    LOG_QUERY_ERROR_FOR(q, query);
                    return false;
                }
                query.finish();
            }
        }

        // Save DDI references
        foreach(int key, m_ddiPmids.uniqueKeys()) {
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            const QStringList &pmids = m_ddiPmids.values(key);
            ++bibMasterId;
            QHash<int, QString> w;
            w.insert(DrugsDB::Constants::IAKNOWLEDGE_IAKID, QString("='%1'").arg(key));
            query.prepare(database->prepareUpdateQuery(DrugsDB::Constants::Table_IAKNOWLEDGE,
                                                       DrugsDB::Constants::IAKNOWLEDGE_BIB_MASTERID,
                                                       w));
            query.bindValue(0, bibMasterId);
            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
                return false;
            }
            query.finish();
            foreach(const QString &pmid, pmids) {
                // create the master_id for this pmid
                query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_BIB_LINK));
                query.bindValue(DrugsDB::Constants::BIB_LINK_BIBID, pmidsBibliographyId.value(pmid));
                query.bindValue(DrugsDB::Constants::BIB_LINK_MASTERID, bibMasterId);
                if (!query.exec()) {
                    LOG_QUERY_ERROR_FOR(q, query);
                    return false;
                }
                query.finish();
            }
        }

        LOG_FOR(q, "Bibliographic references saved");
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        return true;
    }

    // Read the FreeDDIManager DDIDatabase and return the list of valid DDI
    // \note creates its transaction
    QList<DrugDrugInteraction *> getDrugDrugInteractions()
    {
        QSqlDatabase db = ddiCore()->database().database();
        QList<DrugDrugInteraction *> list;
        if (!connectDatabase(db, __FILE__, __LINE__))
            return list;
        db.transaction();
        QSqlQuery query(db);
        QHash<int, QString> where;
        where.insert(Constants::DDI_ISVALID, "=1");
        QString req = ddiCore()->database().select(Constants::Table_DDI, where);
        if (query.exec(req)) {
            while (query.next()) {
                DrugDrugInteraction *ddi = new DrugDrugInteraction;
                ddi->setData(DrugDrugInteraction::FirstInteractorName, query.value(Constants::DDI_FIRSTINTERACTORUID));
                ddi->setData(DrugDrugInteraction::SecondInteractorName, query.value(Constants::DDI_SECONDINTERACTORUID));
                ddi->setData(DrugDrugInteraction::FirstInteractorRouteOfAdministrationIds, query.value(Constants::DDI_FIRSTINTERACTORROUTEOFADMINISTRATIONIDS));
                ddi->setData(DrugDrugInteraction::SecondInteractorRouteOfAdministrationIds, query.value(Constants::DDI_SECONDINTERACTORROUTEOFADMINISTRATIONIDS));
                ddi->setData(DrugDrugInteraction::LevelCode, query.value(Constants::DDI_LEVELCODE));
                ddi->setData(DrugDrugInteraction::DateCreation, query.value(Constants::DDI_DATECREATE));
                ddi->setData(DrugDrugInteraction::DateLastUpdate, query.value(Constants::DDI_DATEUPDATE));
                // ddi->setData(DrugDrugInteraction::ListOfUpdates, query.value(Constants::DDI_));
                // ddi->setData(DrugDrugInteraction::IsDuplicated, query.value());
                ddi->setData(DrugDrugInteraction::IsValid, query.value(Constants::DDI_ISVALID));
                ddi->setData(DrugDrugInteraction::IsReviewed, query.value(Constants::DDI_ISREVIEWED));
                ddi->setData(DrugDrugInteraction::ReviewersStringList, query.value(Constants::DDI_REVIEWERSSTRINGLIST));
                ddi->setData(DrugDrugInteraction::Comment, query.value(Constants::DDI_COMMENT));
                ddi->setData(DrugDrugInteraction::PMIDsStringList, query.value(Constants::DDI_PMIDSTRINGLIST));
                ddi->setData(DrugDrugInteraction::InternalUuid, query.value(Constants::DDI_UID));
                ddi->setData(DrugDrugInteraction::Source, query.value(Constants::DDI_SOURCE));
                ddi->setRisk(query.value(Constants::DDI_RISKFR).toString(), "fr");
                ddi->setRisk(query.value(Constants::DDI_RISKEN).toString(), "en");
                ddi->setManagement(query.value(Constants::DDI_MANAGEMENTFR).toString(), "fr");
                ddi->setManagement(query.value(Constants::DDI_MANAGEMENTEN).toString(), "en");
                // TODO: add management of dose
                // Constants::DDI_FIRSTDOSEUSEFROM,
                // Constants::DDI_FIRSTDOSEUSESTO,
                // Constants::DDI_FIRSTDOSEFROMVALUE,
                // Constants::DDI_FIRSTDOSEFROMUNITS,          // RETURN THE ID IN THE DRUGDRUGINTERACTIONMODEL::UNITS() STRINGLIST
                // Constants::DDI_FIRSTDOSEFROMREPARTITION,    // RETURN THE ID IN THE DRUGDRUGINTERACTIONMODEL::REPARTITION() STRINGLIST
                // Constants::DDI_FIRSTDOSETOVALUE,
                // Constants::DDI_FIRSTDOSETOUNITS,            // RETURN THE ID IN THE DRUGDRUGINTERACTIONMODEL::UNITS() STRINGLIST
                // Constants::DDI_FIRSTDOSETOREPARTITION,      // RETURN THE ID IN THE DRUGDRUGINTERACTIONMODEL::REPARTITION() STRINGLIST
                // Constants::DDI_SECONDDOSEUSEFROM,
                // Constants::DDI_SECONDDOSEUSESTO,
                // Constants::DDI_SECONDDOSEFROMVALUE,
                // Constants::DDI_SECONDDOSEFROMUNITS,         // RETURN THE ID IN THE DRUGDRUGINTERACTIONMODEL::UNITS() STRINGLIST
                // Constants::DDI_SECONDDOSEFROMREPARTITION,   // RETURN THE ID IN THE DRUGDRUGINTERACTIONMODEL::REPARTITION() STRINGLIST
                // Constants::DDI_SECONDDOSETOVALUE,
                // Constants::DDI_SECONDDOSETOUNITS,           // RETURN THE ID IN THE DRUGDRUGINTERACTIONMODEL::UNITS() STRINGLIST
                // Constants::DDI_SECONDDOSETOREPARTITION,     // RETURN THE ID IN THE DRUGDRUGINTERACTIONMODEL::REPARTITION() STRINGLIST
                list << ddi;
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            db.commit();
            return list;
        }
        query.finish();
        db.commit();
        return list;
    }

    // Read the FreeDDIManager DDIDatabase and return the list of valid DrugInteractors
    // \note creates its transaction
    QList<DrugInteractor *> getDrugInteractors()
    {
        QSqlDatabase db = ddiCore()->database().database();
        QList<DrugInteractor *> list;
        if (!connectDatabase(db, __FILE__, __LINE__))
            return list;
        db.transaction();
        QSqlQuery query(db);
        QHash<int, QString> where;
        where.insert(DDI::Constants::INTERACTOR_ISVALID, "=1");
        QString req = ddiCore()->database().select(DDI::Constants::Table_INTERACTORS, where);
        if (query.exec(req)) {
            while (query.next()) {
                DrugInteractor *di = new DrugInteractor;
                di->setData(DrugInteractor::Uid, query.value(DDI::Constants::INTERACTOR_UID));
                di->setData(DrugInteractor::EnLabel, query.value(DDI::Constants::INTERACTOR_EN));
                di->setData(DrugInteractor::FrLabel, query.value(DDI::Constants::INTERACTOR_FR));
                di->setData(DrugInteractor::DeLabel, query.value(DDI::Constants::INTERACTOR_DE));
                // di->setData(DrugInteractor::EsLabel, query.value(DDI::Constants::INTERACTOR_));
                di->setData(DrugInteractor::IsValid, query.value(DDI::Constants::INTERACTOR_ISVALID));
                di->setData(DrugInteractor::IsClass, query.value(DDI::Constants::INTERACTOR_ISCLASS));
                di->setData(DrugInteractor::ClassInformationFr, query.value(DDI::Constants::INTERACTOR_INFO_FR));
                di->setData(DrugInteractor::ClassInformationEn, query.value(DDI::Constants::INTERACTOR_INFO_EN));
                di->setData(DrugInteractor::ClassInformationDe, query.value(DDI::Constants::INTERACTOR_INFO_DE));
                di->setData(DrugInteractor::DoNotWarnDuplicated, query.value(DDI::Constants::INTERACTOR_WARNDUPLICATES));
                di->setData(DrugInteractor::Reference, query.value(DDI::Constants::INTERACTOR_REF));
                di->setData(DrugInteractor::PMIDsStringList, query.value(DDI::Constants::INTERACTOR_PMIDS));
                di->setData(DrugInteractor::Comment, query.value(DDI::Constants::INTERACTOR_COMMENT));
                di->setData(DrugInteractor::IsReviewed, query.value(DDI::Constants::INTERACTOR_ISREVIEWED));
                // TODO : di->setData(DrugInteractor::ReviewersStringList, query.value(DDI::Constants::INTERACTOR_));
                di->setData(DrugInteractor::DateOfReview, query.value(DDI::Constants::INTERACTOR_DATEREVIEW));
                di->setData(DrugInteractor::DateOfCreation, query.value(DDI::Constants::INTERACTOR_DATECREATE));
                di->setData(DrugInteractor::DateLastUpdate, query.value(DDI::Constants::INTERACTOR_DATEUPDATE));
                di->setData(DrugInteractor::IsAutoFound, query.value(DDI::Constants::INTERACTOR_ISAUTOFOUND));
                di->setData(DrugInteractor::ChildrenInteractorsUidStringList, query.value(DDI::Constants::INTERACTOR_CHILDREN).toStringList());
                QStringList atcCodes;
                atcCodes << query.value(DDI::Constants::INTERACTOR_ATC).toStringList();
                atcCodes.removeAll("");
                // if (atcCodes.isEmpty())
                //     qWarning() << di->data(DDI::DrugInteractor::Uid).toString() << _freeDiamsOnFlyAtcCodeToInteractorUid.key(di->data(DDI::DrugInteractor::Uid).toString());
                atcCodes << _freeDiamsOnFlyAtcCodeToInteractorUid.key(di->data(DDI::DrugInteractor::Uid).toString());
                di->setData(DrugInteractor::ATCCodeStringList, atcCodes);
                list << di;
            }
        } else {
            LOG_QUERY_ERROR_FOR(q, query);
            query.finish();
            db.commit();
            return list;
        }
        query.finish();
        db.commit();
        return list;
    }

    QStringList getAllAtcCodesRelatedToOneDrugInteractor(DDI::DrugInteractor *di, const QList<DDI::DrugInteractor *> interactors)
    {
        QStringList list;
        // If is a mol -> return the recorded ATC codes
        if (di->data(DDI::DrugInteractor::ATCCodeStringList).toString().contains("ZXX")) {
            list << di->data(DDI::DrugInteractor::ATCCodeStringList).toString().split(";");
        } else if (!di->isClass()) {
            return di->data(DDI::DrugInteractor::ATCCodeStringList).toString().split(";");
        }

        // When we have a class we need to include all children ATC codes
        // Some children are class too so recall this member with each child
        foreach(const QString &child, di->childrenIds()) {
            foreach(DDI::DrugInteractor *interactor, interactors) {
                if (interactor->id() == child) {
                    list << getAllAtcCodesRelatedToOneDrugInteractor(interactor, interactors);
                }
            }
        }
        return list;
    }

public:
    QMultiMap<int, QString> m_iamTreePmids; //K=IAM_ID  ;  V=PMIDs
    QMultiMap<int, QString> m_ddiPmids;     //K=IAK_ID  ;  V=PMIDs

    // FIXME: volatile data (loosing FreeDiams specific codes Uid to ATC Code on DrugInteractor)
    // In saveAtcClassification, for FreeDiams specific interactors, we create a Z..... ATC code
    // but this Z-code is created on the fly when we populate drugs database
    // ATC tables, we keep this information later when we save all the interaction data.
    // You guess it, you must (re)create the ATC tables to get the corrected link.
    // We need to add a table in the drugs database to keep this information.
    QHash<QString, QString> _freeDiamsOnFlyAtcCodeToInteractorUid; // K=ATCcode V=DrugInteractor::Uid

private:
    DrugDatabasePopulator *q;
};
}  // namespace Internal
} // namespace DDI

/*! Constructor of the DrugsDB::Internal::DatabasePopulator class */
DrugDatabasePopulator::DrugDatabasePopulator(QObject *parent) :
    QObject(parent),
    d(new DrugDatabasePopulatorPrivate(this))
{
    setObjectName("DrugDatabasePopulator");
}

/*! Destructor of the DrugsDB::Internal::DatabasePopulator class */
DrugDatabasePopulator::~DrugDatabasePopulator()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DrugDatabasePopulator::initialize()
{
    return true;
}

/**
 * Save the ATC data to a drug database. Read the XML resource file
 * \note This member does not create transaction, does not commit or rollback.
 */
bool DrugDatabasePopulator::saveAtcClassification(DrugsDB::Internal::DrugBaseEssentials *database)
{
    QSqlDatabase db = database->database();
    if (!connectDatabase(db, __FILE__, __LINE__))
        return false;
    LOG("Saving ATC");
    QTime chrono;
    chrono.start();

    // Clean ATC table from old values
    QString req;
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_ATC);
    if (!database->executeSQL(req, db))
        return false;
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_ATC_LABELS);
    if (!database->executeSQL(req, db))
        return false;
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Import ATC codes to database
    DDI::AtcTableModel *model = ddiCore()->atcTableModel();
    int n = model->rowCount();
    for(int i = 0; i < n; ++i) {
        QModelIndex code = model->index(i, AtcTableModel::Code);
        QModelIndex fr = model->index(i, AtcTableModel::LabelFr);
        QModelIndex en = model->index(i, AtcTableModel::LabelEn);
        QModelIndex de = model->index(i, AtcTableModel::LabelDe);
        QModelIndex sp = model->index(i, AtcTableModel::LabelSp);
        QHash<QString, QVariant> labels; // lang, label
        labels.insert("fr", fr.data());
        labels.insert("en", en.data());
        labels.insert("de", de.data());
        labels.insert("sp", sp.data());
        if (!DrugsDb::Tools::createAtc(database, code.data().toString(), labels)) {
            db.rollback();
            return false;
        }
        if (i % 100 == 0) {
            qWarning() << i << chrono.elapsed() << "ms";
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            // TODO: add a Q_EMIT for QProgress...
        }
    }
    Utils::Log::logTimeElapsed(chrono, "DrugDatabasePopulator", "Saving WHO ATC codes to Drugs database");
    // 53686ms MacOs
    // TODO: inserting ATC data under Ubuntu 14.04 is TOOOOOOOOO S.L.O.W. then under macos!!!
    // Samle computer (MacBook 5,1), ubuntu 14.04
    //    0 218 ms
    //    100 18618 ms
    //    200 37501 ms
    //    300 57891 ms
    //    400 78921 ms
    //    500 99291 ms
    //    600 120078 ms
    //    700 141213 ms
    //    800 162379 ms
    //    900 183524 ms
    //    1000 204805 ms
    //    1100 226137 ms
    //    1200 247346 ms
    //    1300 268549 ms
    //    1400 290353 ms
    //    1500 311501 ms
    //    1600 332986 ms
    //    1700 354254 ms
    //    1800 375646 ms
    //    1900 398155 ms
    //    2000 419714 ms
    //    2100 441354 ms
    //    2200 462111 ms
    //    2300 483106 ms
    //    2400 503139 ms
    //    2500 524241 ms
    //    2600 545288 ms
    //    2700 566284 ms
    //    2800 587401 ms
    //    2900 608846 ms
    //    3000 630108 ms
    //    3100 651477 ms
    //    3200 673558 ms
    //    3300 694816 ms
    //    3400 715565 ms
    //    3500 736508 ms
    //    3600 757644 ms
    //    3700 778660 ms
    //    3800 799796 ms
    //    3900 820876 ms
    //    4000 841911 ms
    //    4100 863609 ms
    //    4200 884695 ms
    //    4300 905806 ms
    //    4400 926953 ms
    //    4500 948139 ms
    //    4600 969287 ms
    //    4700 990333 ms
    //    4800 1011520 ms
    //    4900 1032800 ms
    //    5000 1054554 ms
    //    5100 1075739 ms
    //    5200 1097208 ms
    //    5300 1118174 ms
    //    5400 1138894 ms
    //    5500 1159588 ms
    //    5600 1180772 ms
    //    5700 1201376 ms

    // add FreeDiams ATC specific codes
    // 100 000 < ID < 199 999  == Interacting molecules without ATC code
    // 200 000 < ID < 299 999  == Interactings classes
    int molId = 100000;
    int classId = 200000;
    QList<DDI::DrugInteractor *> interactors = d->getDrugInteractors();
    foreach(DDI::DrugInteractor *di, interactors) {
        if (!di->hasAtcLink()) {
            // Create new ATC code for mols and/or interacting classes
            QMultiHash<QString, QVariant> labels;
            labels.insert("fr", di->data(DrugInteractor::FrLabel));
            if (!di->data(DrugInteractor::EnLabel).isNull())
                labels.insert("en", di->data(DrugInteractor::EnLabel));
            else
                labels.insert("en", di->data(DrugInteractor::FrLabel));
            if (!di->data(DrugInteractor::DeLabel).isNull())
                labels.insert("de", di->data(DrugInteractor::DeLabel));
            else
                labels.insert("de", di->data(DrugInteractor::FrLabel));

            QString onFlyAtcCode;
            int atcCodeId;
            if (di->isClass()) {
                ++classId;
                QString n = QString::number(classId-200000);
                n = n.rightJustified(4, '0');
                onFlyAtcCode = "ZXX" + n;
                atcCodeId = classId;
            } else {
                ++molId;
                QString n = QString::number(molId-100000);
                n = n.rightJustified(4, '0');
                onFlyAtcCode = "Z01" + n;
                atcCodeId = molId;
            }
            if (!DrugsDb::Tools::createAtc(database, onFlyAtcCode, labels, atcCodeId, !di->data(DrugInteractor::DoNotWarnDuplicated).toBool()))
                    return false;

            di->setData(CLASS_OR_MOL_ID, atcCodeId);
            di->setData(FREEMEDFORMS_ATC_CODE, onFlyAtcCode);

            d->_freeDiamsOnFlyAtcCodeToInteractorUid.insert(onFlyAtcCode, di->data(DDI::DrugInteractor::Uid).toString());
            ++n;
        }
        // TODO: add a Q_EMIT for QProgress...
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    // Recreate interacting classes tree
    LOG("Saving interactors");
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE);
    database->executeSQL(req, db);
    foreach(DDI::DrugInteractor *interactor, interactors) {
        if (interactor->isClass()) {
            if (!d->saveClassDrugInteractor(interactor, interactors, database, 0)) {
                db.rollback();
                return false;
            }
        }
    }

    Utils::Log::logTimeElapsed(chrono, "DrugDatabasePopulator", "Saving FreeMedForms ATC codes to Drugs database");
    LOG(tr("%1 ATC code saved").arg(n));
    return true;
}

/** Populate a drug database with:
 * - the ATC data
 * - DDI data
 * \sa DrugsDB::DrugsDbCore::addInteractionData()
 */
bool DrugDatabasePopulator::saveDrugDrugInteractions(DrugsDB::Internal::DrugBaseEssentials *database)
{
    QSqlDatabase db = database->database();
    if (!connectDatabase(db, __FILE__, __LINE__))
        return false;

    LOG("Starting DDI saving");
    // Save ATC + FreeMedForms specific codes
    if (!database->isAtcAvailable())
        saveAtcClassification(database);

    QList<DDI::DrugInteractor *> interactors = d->getDrugInteractors();
    QList<DDI::DrugDrugInteraction *> ddis = d->getDrugDrugInteractions();

    LOG(tr("Extracted %1 drug interactors from the %2 database")
        .arg(interactors.count())
        .arg(qApp->applicationName()));

    LOG(tr("Extracted %1 drug-drug interactions from the %2 database")
        .arg(ddis.count())
        .arg(qApp->applicationName()));

    // Save DDIs
    d->saveDrugDrugInteractions(database, interactors, ddis);

    // Save Bibliographic references
    d->saveBibliographicReferences(database);

    // refresh the innToAtc content (reload ATC codes because we added some new ATC)

    // drugs databases needs to be relinked with the new ATC codes

    return true;
}

/**
 * Save all ATC <-> drug component links to the country drug database.
 * \sa IDrugDatabase::linkDrugsComponentsAndDrugInteractors()
 * \note This member does not create transaction, does not commit or rollback.
 */
bool DrugDatabasePopulator::saveComponentAtcLinks(DrugsDB::Internal::DrugBaseEssentials *database, const QMultiHash<int, int> &componentIdToAtcId, const int sid)
{
    QSqlDatabase db = database->database();
    if (!connectDatabase(db, __FILE__, __LINE__))
        return false;

    LOG("Start saving component to ATC links");
    if (!database->isAtcAvailable()) {
        LOG_ERROR("No ATC data found in the database");
        return false;
    }

    const QList<int> &uniqueKeys = componentIdToAtcId.uniqueKeys();
    LOG(QString("Saving %1 component to ATC links (%2 associations)")
        .arg(uniqueKeys.count())
        .arg(componentIdToAtcId.count()));
    QString req = database->prepareDeleteQuery(DrugsDB::Constants::Table_LK_MOL_ATC);
    database->executeSQL(req, db);
    QSqlQuery query(db);
    for(int i = 0; i < uniqueKeys.count(); ++i) {
        QList<int> atcCodesSaved;
        int compo = uniqueKeys.at(i);
        const QList<int> &values = componentIdToAtcId.values(compo);
        for(int j = 0; j < values.count(); ++j) {
            int atc = values.at(j);
            // avoir duplicate
            if (atcCodesSaved.contains(atc))
                continue;
            atcCodesSaved.append(atc);
            query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_LK_MOL_ATC));
            query.bindValue(DrugsDB::Constants::LK_MID, compo);
            query.bindValue(DrugsDB::Constants::LK_ATC_ID, atc);
            query.bindValue(DrugsDB::Constants::LK_ATC_SID, sid);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                query.finish();
                return false;
            }
            query.finish();
        }
    }
    return true;
}
