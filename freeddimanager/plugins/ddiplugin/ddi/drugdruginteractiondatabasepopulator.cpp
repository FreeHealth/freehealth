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
 * \class DrugsDB::Internal::DrugDrugInteractionDatabasePopulator
 * Populate a Drug database with DDI (ATC codes, DDI).\n
 * This object directly uses XML files to populate the drugs database.
 * \n FreeToolBox specific class.
 */

#include "drugdruginteractiondatabasepopulator.h"
#include "drugdruginteraction.h"
#include "druginteractor.h"
#include "drugdruginteractioncore.h"
#include <drugsdb/drugsdbcore.h>

#include <drugsdb/tools.h>
#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <biblio/bibliocore.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/log.h>
#include <translationutils/constants.h>
#include <translationutils/trans_msgerror.h>

#include <QHash>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>

#include <QDebug>

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline DrugsDB::DrugsDBCore *drugsDbCore() {return DrugsDB::DrugsDBCore::instance();}
static inline DrugsDB::DrugDrugInteractionCore *ddiCore() {return drugsDbCore()->ddiCore();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString afssapsIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString atcCsvFile()          {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::ATC_FILENAME);}

namespace  {
const int CLASS_OR_MOL_ID = 65000;
const int FREEMEDFORMS_ATC_CODE = 65001;
}

namespace DrugsDB {
namespace Internal {
class DrugDrugInteractionDatabasePopulatorPrivate
{
public:
    DrugDrugInteractionDatabasePopulatorPrivate(DrugDrugInteractionDatabasePopulator *parent) :
        q(parent)
    {
    }

    ~DrugDrugInteractionDatabasePopulatorPrivate()
    {
    }

    /** Save ATC data: class interactors */
    bool saveClassDrugInteractor(DrugInteractor *interactor, const QList<DrugInteractor *> &completeList, DrugsDB::Internal::DrugBaseEssentials *database, DrugInteractor *parent)
    {
        QSqlDatabase db = database->database();
        if (!db.isOpen()) {
            if (!db.open()) {
                LOG_ERROR_FOR(q, tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
                return false;
            }
        }

        // save interactor
        QSqlQuery query(db);
        int id = -1;
        // save using all associated ATC codes
        const QStringList &atcCodes = interactor->data(DrugInteractor::ATCCodeStringList).toStringList();
        if (atcCodes.isEmpty() && !interactor->isClass() && parent && parent->isClass()) {
            query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE));
            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID, QVariant());
            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_ATC, interactor->data(CLASS_OR_MOL_ID).toString());
            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_CLASS, parent->data(CLASS_OR_MOL_ID).toString());
            // TODO: manage ClassTreeATC Bibliography
            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_BIBMASTERID, QVariant());

            if (!query.exec()) {
                LOG_QUERY_ERROR_FOR(q, query);
            } else {
                id = query.lastInsertId().toInt();
            }
            query.finish();
        } else if (!atcCodes.isEmpty() && !interactor->isClass() && parent && parent->isClass()) {
            foreach(const QString &atc, atcCodes) {
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

                query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE));
                query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID, QVariant());
                query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_ATC, atcId);
                query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_CLASS, parent->data(CLASS_OR_MOL_ID).toString());
                query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_BIBMASTERID, QVariant());

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

    /** Save the DDI data to the drug database */
    bool saveDrugDrugInteractions(DrugsDB::Internal::DrugBaseEssentials *database, const QList<DrugInteractor *> &interactors, const QList<DrugDrugInteraction *> &ddis)
    {
        QSqlDatabase db = database->database();
        if (!db.isOpen()) {
            if (!db.open()) {
                LOG_ERROR_FOR(q, tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
                return false;
            }
        }
        LOG_FOR(q, "Saving drug drug interactions");

        // Clear database first
        QString req;
        req = database->prepareDeleteQuery(DrugsDB::Constants::Table_INTERACTIONS);
        DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
        req = database->prepareDeleteQuery(DrugsDB::Constants::Table_IAKNOWLEDGE);
        DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
        req = database->prepareDeleteQuery(DrugsDB::Constants::Table_IA_IAK);
        DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);

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
            for(int i=0; i < interactors.count();++i) {
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
                          .arg(ddi->firstInteractor())
                          .arg(ddi->secondInteractor())
                          .arg(ddi->levelName()));
                continue;
            }

            QSqlQuery query(db);
            QString req;
            QList<int> ia_ids;
            int iak_id = -1;

            // for all atc of firstInteractor & all atc of secondInteractor -> add an interaction + keep the references of the DDI
            QStringList atc1 = firstInteractor->data(DrugInteractor::ATCCodeStringList).toStringList();
            QStringList atc2 = secondInteractor->data(DrugInteractor::ATCCodeStringList).toStringList();
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
            int riskMasterLid = DrugsDB::Tools::addLabels(database, -1, risk);
            int manMasterLid = DrugsDB::Tools::addLabels(database, -1, management);
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

        db.commit();
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        LOG_FOR(q, "Drug drug interactions saved");

        return true;
    }

    /** Save all needed bibliographic references to a drug database */
    bool saveBibliographicReferences(DrugsDB::Internal::DrugBaseEssentials *database)
    {
        LOG_FOR(q, "Saving bibliographic references in: " + database->database().databaseName());
        // TODO: Ensure all PMIDs are available */
        QSqlDatabase db = database->database();
        // Clear database first
        QString req = database->prepareDeleteQuery(DrugsDB::Constants::Table_BIB);
        DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
        req = database->prepareDeleteQuery(DrugsDB::Constants::Table_BIB_LINK);
        DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
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
            query.bindValue(DrugsDB::Constants::BIB_XML, Biblio::BiblioCore::instance()->xml(pmid).replace("'","''"));
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

public:
    QMultiMap<int, QString> m_iamTreePmids; //K=IAM_ID  ;  V=PMIDs
    QMultiMap<int, QString> m_ddiPmids;     //K=IAK_ID  ;  V=PMIDs

private:
    DrugDrugInteractionDatabasePopulator *q;
};
}  // namespace Internal
} // end namespace DrugsDB

/*! Constructor of the DrugsDB::Internal::DrugDrugInteractionDatabasePopulator class */
DrugDrugInteractionDatabasePopulator::DrugDrugInteractionDatabasePopulator(QObject *parent) :
    QObject(parent),
    d(new DrugDrugInteractionDatabasePopulatorPrivate(this))
{
    setObjectName("DrugDrugInteractionDatabasePopulator");
}

/*! Destructor of the DrugsDB::Internal::DrugDrugInteractionDatabasePopulator class */
DrugDrugInteractionDatabasePopulator::~DrugDrugInteractionDatabasePopulator()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool DrugDrugInteractionDatabasePopulator::initialize()
{
    return true;
}

bool DrugDrugInteractionDatabasePopulator::isAtcInstalledInDatabase(DrugsDB::Internal::DrugBaseEssentials *database)
{
    QSqlDatabase db = database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR(tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
            return false;
        }
    }
    int max = database->max(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID).toInt();
    return max > 5000;
}

/** Save the ATC data to a drug database. Read the XML resource file */
bool DrugDrugInteractionDatabasePopulator::saveAtcClassification(DrugsDB::Internal::DrugBaseEssentials *database)
{
    QSqlDatabase db = database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR(tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
            return false;
        }
    }
    LOG("Saving ATC");

    // Clean ATC table from old values
    QString req;
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_ATC);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_ATC_LABELS);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Import ATC codes to database
    QFile file(atcCsvFile());
    if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("ERROR : can not open file %1, %2.").arg(file.fileName(), file.errorString()));
    } else {
        QString content = QString::fromUtf8(file.readAll());
        if (content.isEmpty())
            return false;
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        const QStringList &list = content.split("\n", QString::SkipEmptyParts);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        foreach(const QString &s, list) {
            if (s.startsWith("--")) {
                qWarning() << s;
                continue;
            }
            QStringList values = s.split("\";\"");
            QMultiHash<QString, QVariant> labels;
            QString en = values[1].remove("\"").toUpper();
            labels.insert("en", en);
            QString fr = values[2].remove("\"").toUpper();
            if (fr.isEmpty())
                labels.insert("fr", en);
            else
                labels.insert("fr", fr);
            QString de = values[3].remove("\"").toUpper();
            if (de.isEmpty())
                labels.insert("de", en);
            else
                labels.insert("de", de);
            if (!DrugsDB::Tools::createAtc(database, values[0].remove("\""), labels)) {
                return false;
            }
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }
    // add FreeDiams ATC specific codes
    db.transaction();

    // 100 000 < ID < 199 999  == Interacting molecules without ATC code
    // 200 000 < ID < 299 999  == Interactings classes
    int molId = 100000;
    int classId = 200000;
    foreach(DrugInteractor *di, ddiCore()->getDrugInteractors()) {
        if (!di->data(DrugInteractor::ATCCodeStringList).toStringList().count()) {
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

            if (di->isClass()) {
                ++classId;
                QString n = QString::number(classId-200000);
                n = n.rightJustified(4, '0');
                if (!DrugsDB::Tools::createAtc(database, "ZXX" + n, labels, classId, !di->data(DrugInteractor::DoNotWarnDuplicated).toBool()))
                    return false;
                di->setData(CLASS_OR_MOL_ID, classId);
                di->setData(FREEMEDFORMS_ATC_CODE, QString("ZXX" + n));
            } else {
                ++molId;
                QString n = QString::number(molId-100000);
                n = n.rightJustified(2, '0');
                if (!DrugsDB::Tools::createAtc(database, "Z01AA" + n, labels, molId, !di->data(DrugInteractor::DoNotWarnDuplicated).toBool()))
                    return false;
                di->setData(CLASS_OR_MOL_ID, molId);
                di->setData(FREEMEDFORMS_ATC_CODE, QString("Z01AA" + n));
            }
        }
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    db.commit();

    LOG("ATC saved");
    return true;
}

/** Populate a drug database with:
 * - the ATC data
 * - DDI data
 * \sa DrugsDB::DrugsDbCore::addInteractionData()
 */
bool DrugDrugInteractionDatabasePopulator::saveDrugDrugInteractions(DrugsDB::Internal::DrugBaseEssentials *database)
{
    // Save ATC + FreeMedForms specific codes
    if (!isAtcInstalledInDatabase(database))
            saveAtcClassification(database);

    const QList<DrugInteractor *> &interactors = ddiCore()->getDrugInteractors();
    const QList<DrugDrugInteraction *> &ddis   = ddiCore()->getDrugDrugInteractions();

    // Recreate interacting classes tree
    LOG("Saving interactors");
    QString req = database->prepareDeleteQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE);//"DELETE FROM ATC_CLASS_TREE";
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
    QSqlDatabase db = database->database();
    db.transaction();
    foreach(DrugInteractor *interactor, interactors) {
        if (interactor->isClass()) {
            if (!d->saveClassDrugInteractor(interactor, interactors, database, 0)) {
                db.rollback();
                return false;
            }
        }
    }
    db.commit();

    // Save DDIs
    d->saveDrugDrugInteractions(database, interactors, ddis);

    // Save Bibliographic references
    d->saveBibliographicReferences(database);

    // refresh the innToAtc content (reload ATC codes because we added some new ATC)

    // drugs databases needs to be relinked with the new ATC codes

    return true;
}

