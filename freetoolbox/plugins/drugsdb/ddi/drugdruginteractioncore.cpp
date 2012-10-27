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
#include "drugdruginteractioncore.h"
#include "drugdruginteraction.h"
#include "druginteractor.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>
#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <biblio/bibliocore.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpdownloader.h>
#include <translationutils/constanttranslations.h>
#include <translationutils/googletranslator.h>

#include <QMultiHash>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDir>
#include <QFile>

using namespace IAMDb;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }

static inline QString ddiNewXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::NEW_AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString newInteractorsFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::NEW_INTERACTORS_FILENAME);}

static inline QString afssapsIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString atcCsvFile()          {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::ATC_FILENAME);}

using namespace IAMDb;

namespace  {
const int CLASS_OR_MOL_ID = 65000;
const int FREEMEDFORMS_ATC_CODE = 65001;
}

DrugDrugInteractionCore *DrugDrugInteractionCore::m_Instance = 0;
DrugDrugInteractionCore *DrugDrugInteractionCore::instance()
{
    if (!m_Instance) {
        m_Instance = new DrugDrugInteractionCore;
    }
    return m_Instance;
}

DrugDrugInteractionCore::DrugDrugInteractionCore(QObject *parent) :
    QObject(parent)
{
    setObjectName("DrugDrugInteractionCore");
}

int DrugDrugInteractionCore::createInternalUuid() const
{
    static int uuid = 0;
    return ++uuid;
}

/** Read the DDI XML file if needed and return the list of created DDI. */
QList<DrugDrugInteraction *> DrugDrugInteractionCore::getDrugDrugInteractions() const
{
    if (m_ddisToNode.isEmpty()) {
        QDomDocument domDocument;
        // Read the file
        QFile file(ddiNewXmlFile());
        if (file.open(QIODevice::ReadOnly)) {
            QString error;
            int line, col;
            if (!domDocument.setContent(&file, &error,&line,&col)) {
                LOG_ERROR(tr("Can not read XML file content %1").arg(file.fileName()));
                LOG_ERROR(QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error));
            } else {
                LOG(tr("Reading file: %1").arg(file.fileName()));
            }
            file.close();
        } else {
            LOG_ERROR(tr("Can not open XML file %1").arg(file.fileName()));
        }

        // Create the m_ddisToNode content
        QDomElement root = domDocument.firstChildElement("DrugDrugInteractions");
        QDomElement ddiNode = root.firstChildElement("DDI");
        while (!ddiNode.isNull()) {
            DrugDrugInteraction *ddi = new DrugDrugInteraction(ddiNode);
            ddi->setData(DrugDrugInteraction::InternalUuid, createInternalUuid());
            m_ddisToNode.insert(ddi, ddiNode);
            ddiNode = ddiNode.nextSiblingElement("DDI");
        }
        LOG(tr("Getting %1 non duplicated drug-drug interactions").arg(m_ddisToNode.keys().count()));
    }
    return m_ddisToNode.keys();
}

/** Read the DDI XML file if needed and return the list of created interactors. */
QList<DrugInteractor *> DrugDrugInteractionCore::getDrugInteractors() const
{
    if (m_interactorsToNode.isEmpty()) {
        QDomDocument domDocument;
        // Read the file
        QFile file(newInteractorsFile());
        if (file.open(QIODevice::ReadOnly)) {
            QString error;
            int line, col;
            if (!domDocument.setContent(&file, &error,&line,&col)) {
                LOG_ERROR(tr("Can not read XML file content %1").arg(file.fileName()));
                LOG_ERROR(QString("DOM(%1;%2): %3").arg(line).arg(col).arg(error));
            } else {
                LOG(tr("Reading file: %1").arg(file.fileName()));
            }
            file.close();
        } else {
            LOG_ERROR(tr("Can not open XML file %1").arg(file.fileName()));
        }

        // Create the m_ddisToNode content
        QDomElement root = domDocument.firstChildElement("DDI_Interactors");
        QDomElement iNode = root.firstChildElement("I");
        QHash<QString, DrugInteractor *> initialLabelToDI;
        while (!iNode.isNull()) {
            DrugInteractor *di = new DrugInteractor(iNode);
            di->setData(DrugInteractor::Id, createInternalUuid());
            m_interactorsToNode.insert(di, iNode);
            initialLabelToDI.insert(di->data(DrugInteractor::InitialLabel).toString(), di);
            iNode = iNode.nextSiblingElement("I");
        }

        // reparent items
        for(int i=0;i<m_interactorsToNode.count();++i) {
            DrugInteractor *di = m_interactorsToNode.keys().at(i);
            const QString &parentInitialLabel = di->data(DrugInteractor::InitialLabel).toString();

            foreach(const QString &child, di->childrenIds()) {
                DrugInteractor *childInteractor = initialLabelToDI.value(child, 0);
                if (childInteractor) {
                    childInteractor->addParentId(parentInitialLabel);
                }
            }
        }

        LOG(tr("Getting %1 interactors and interacting classes").arg(m_interactorsToNode.keys().count()));
    }
    return m_interactorsToNode.keys();
}

/** Populate a drug database with:
 * - the ATC data
 * - DDI data
 * \sa DrugsDB::DrugsDbCore::addInteractionData()
 */
bool DrugDrugInteractionCore::populateDrugDatabase(DrugsDB::Internal::DrugBaseEssentials *database)
{
    // Save ATC + FreeMedForms specific codes
    saveAtcClassification(database);

    const QList<DrugInteractor *> &interactors = getDrugInteractors();
    const QList<DrugDrugInteraction *> &ddis   = getDrugDrugInteractions();

    // Recreate interacting classes tree
    QString req = database->prepareDeleteQuery(DrugsDB::Constants::Table_IAM_TREE);//"DELETE FROM IAM_TREE";
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
    QSqlDatabase db = database->database();
    db.transaction();
    foreach(DrugInteractor *interactor, interactors) {
        if (interactor->isClass()) {
            if (!saveClassDrugInteractor(interactor, interactors, database, 0)) {
                db.rollback();
                return false;
            }
        }
    }
    db.commit();

    // Save DDIs
    saveDrugDrugInteractions(database, interactors, ddis);

    // Save Bibliographic references
    saveBibliographicReferences(database);

    // refresh the innToAtc content (reload ATC codes because we added some new ATC)

    // drugs databases needs to be relinked with the new ATC codes

    return true;
}

/** Save the ATC data to a drug database. Read the XML resource file */
bool DrugDrugInteractionCore::saveAtcClassification(DrugsDB::Internal::DrugBaseEssentials *database)
{
    QSqlDatabase db = database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR(tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
            return false;
        }
    }
    // Clean ATC table from old values
    QString req;
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_ATC);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_ATC_LABELS);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);

    // Import ATC codes to database
    QFile file(atcCsvFile());
    if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("ERROR : can not open file %1, %2.").arg(file.fileName(), file.errorString()));
    } else {
        QString content = QString::fromUtf8(file.readAll());
        if (content.isEmpty())
            return false;
        const QStringList &list = content.split("\n", QString::SkipEmptyParts);
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
        }
    }
    // add FreeDiams ATC specific codes
    db.transaction();

    // 100 000 < ID < 199 999  == Interacting molecules without ATC code
    // 200 000 < ID < 299 999  == Interactings classes
    int molId = 100000;
    int classId = 200000;
    foreach(DrugInteractor *di, getDrugInteractors()) {
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
                di->setData(FREEMEDFORMS_ATC_CODE, "ZXX" + n);
            } else {
                ++molId;
                QString n = QString::number(molId-100000);
                n = n.rightJustified(2, '0');
                if (!DrugsDB::Tools::createAtc(database, "Z01AA" + n, labels, molId, !di->data(DrugInteractor::DoNotWarnDuplicated).toBool()))
                    return false;
                di->setData(CLASS_OR_MOL_ID, molId);
                di->setData(FREEMEDFORMS_ATC_CODE, "Z01AA" + n);
            }
        }
    }
    db.commit();
    return true;
}

/** Save ATC data: class interactors */
bool DrugDrugInteractionCore::saveClassDrugInteractor(DrugInteractor *interactor, const QList<DrugInteractor *> &completeList, DrugsDB::Internal::DrugBaseEssentials *database, DrugInteractor *parent)
{
    QSqlDatabase db = database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR(tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
            return false;
        }
    }

    // save interactor
    QSqlQuery query(db);
    int id = -1;
    // save using all associated ATC codes
    const QStringList &atcCodes = interactor->data(DrugInteractor::ATCCodeStringList).toStringList();
    if (atcCodes.isEmpty() && !interactor->isClass() && parent && parent->isClass()) {
        //        QString req = QString("INSERT INTO IAM_TREE (ID_TREE, ID_CLASS, ID_ATC) VALUES "
        //                              "(NULL, %1,%2);")
        //                .arg(parent->data(CLASS_OR_MOL_ID).toString())
        //                .arg(interactor->data(CLASS_OR_MOL_ID).toString());
        query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_IAM_TREE));
        query.bindValue(DrugsDB::Constants::IAM_TREE_ID, QVariant());
        query.bindValue(DrugsDB::Constants::IAM_TREE_ID_ATC, interactor->data(CLASS_OR_MOL_ID).toString());
        query.bindValue(DrugsDB::Constants::IAM_TREE_ID_CLASS, parent->data(CLASS_OR_MOL_ID).toString());
        query.bindValue(DrugsDB::Constants::IAM_TREE_BIBMASTERID, QVariant());

        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
        } else {
            id = query.lastInsertId().toInt();
        }
        query.finish();
    } else if (!atcCodes.isEmpty() && !interactor->isClass() && parent && parent->isClass()) {
        foreach(const QString &atc, atcCodes) {
            //            QString req = QString("INSERT INTO IAM_TREE (ID_TREE, ID_CLASS, ID_ATC) VALUES "
            //                                  "(NULL, %1, (SELECT ATC_ID FROM ATC WHERE CODE=\"%2\"));")
            //                    .arg(parent->data(CLASS_OR_MOL_ID).toString()).arg(atc);

            QString atcId;
            QHash<int, QString> w;
            w.insert(DrugsDB::Constants::ATC_CODE, QString("='%1'").arg(atc));
            QString req = database->select(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID, w);
            if (query.exec(req)) {
                if (query.next())
                    atcId = query.value(0).toString();
            } else {
                LOG_QUERY_ERROR(query);
                db.rollback();
                return false;
            }
            query.finish();

            query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_IAM_TREE));
            query.bindValue(DrugsDB::Constants::IAM_TREE_ID, QVariant());
            query.bindValue(DrugsDB::Constants::IAM_TREE_ID_ATC, atcId);
            query.bindValue(DrugsDB::Constants::IAM_TREE_ID_CLASS, parent->data(CLASS_OR_MOL_ID).toString());
            query.bindValue(DrugsDB::Constants::IAM_TREE_BIBMASTERID, QVariant());

            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
            } else {
                id = query.lastInsertId().toInt();
            }
            query.finish();
        }
    }

    // add pmids references
    if (id>=0) {
        foreach(const QString &pmid, parent->childClassificationPMIDs(interactor->data(DrugInteractor::InitialLabel).toString())) {
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
                if (testMe->data(DrugInteractor::InitialLabel).toString()==childId) {
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
bool DrugDrugInteractionCore::saveDrugDrugInteractions(DrugsDB::Internal::DrugBaseEssentials *database, const QList<DrugInteractor *> &interactors, const QList<DrugDrugInteraction *> &ddis)
{
    QSqlDatabase db = database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR(tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
            return false;
        }
    }
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
        DrugDrugInteraction *ddi = ddis.at(i);

        // find first && second interactors
        bool firstFound = false;
        bool secondFound = false;
        const QString &first = ddi->data(DrugDrugInteraction::FirstInteractorName).toString();
        const QString &second = ddi->data(DrugDrugInteraction::SecondInteractorName).toString();
        DrugInteractor *firstInteractor = 0;
        DrugInteractor *secondInteractor = 0;
        for(int i=0; i < interactors.count();++i) {
            const QString &id = interactors.at(i)->data(DrugInteractor::InitialLabel).toString();
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
            LOG_ERROR(QString("*** Interactors not found: \n  %1 - %2 (%3)")
                      .arg(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())
                      .arg(ddi->data(DrugDrugInteraction::SecondInteractorName).toString())
                      .arg(ddi->data(DrugDrugInteraction::LevelName).toString()));
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
                    LOG_QUERY_ERROR(query);
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
                    LOG_QUERY_ERROR(query);
                    db.rollback();
                    return false;
                }
                query.finish();

                //                req = QString("INSERT INTO INTERACTIONS (ATC_ID1, ATC_ID2) VALUES (%1, %2);")
                //                        .arg(QString("(SELECT ATC_ID FROM ATC WHERE CODE=\"%1\")").arg(a1))
                //                        .arg(QString("(SELECT ATC_ID FROM ATC WHERE CODE=\"%1\")").arg(a2));
                query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_INTERACTIONS));
                query.bindValue(DrugsDB::Constants::INTERACTIONS_IAID, QVariant());
                query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID1, atcId1);
                query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID2, atcId2);
                if (!query.exec()) {
                    LOG_QUERY_ERROR(query);
                    LOG_ERROR(QString("*** Interactors not found: \n  %1 - %2 (%3)")
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
                //                req = QString("INSERT INTO INTERACTIONS (ATC_ID2, ATC_ID1) VALUES (%1, %2);")
                //                        .arg(QString("(SELECT ATC_ID FROM ATC WHERE CODE=\"%1\")").arg(a1))
                //                        .arg(QString("(SELECT ATC_ID FROM ATC WHERE CODE=\"%1\")").arg(a2));
                query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_INTERACTIONS));
                query.bindValue(DrugsDB::Constants::INTERACTIONS_IAID, QVariant());
                query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID1, atcId2);
                query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID2, atcId1);
                if (!query.exec()) {
                    LOG_QUERY_ERROR(query);
                    LOG_ERROR(QString("*** Interactors not found: \n  %1 - %2 (%3)")
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
        //        req = QString("INSERT INTO IAKNOWLEDGE (IAKID, TYPE, RISK_MASTER_LID, MAN_MASTER_LID) VALUES "
        //                      "(NULL, \"%1\", %2, %3)")
        //                .arg(ddi->data(DrugDrugInteraction::LevelCode).toString())
        //                .arg(riskMasterLid)
        //                .arg(manMasterLid);

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
            LOG_QUERY_ERROR(query);
            db.rollback();
            return false;
        }
        query.finish();

        // Add to IA_IAK link table
        foreach(const int ia, ia_ids) {
            //            req = QString("INSERT INTO IA_IAK (IAID, IAKID) VALUES (%1,%2)")
            //                    .arg(ia)
            //                    .arg(iak_id)
            //                    ;
            query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_IA_IAK));
            query.bindValue(DrugsDB::Constants::IA_IAK_IAID, ia);
            query.bindValue(DrugsDB::Constants::IA_IAK_IAKID, iak_id);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                db.rollback();
                return false;
            }
            query.finish();
        }
    }

    db.commit();

    return true;
}

/** Save all needed bibliographic references to a drug database */
bool DrugDrugInteractionCore::saveBibliographicReferences(DrugsDB::Internal::DrugBaseEssentials *database)
{
    LOG("Starting to save the bilbiographic references in: " + database->database().databaseName());
    // TODO: Ensure all PMIDs are available */
    QSqlDatabase db = database->database();
    // Clear database first
    QString req = database->prepareDeleteQuery(DrugsDB::Constants::Table_BIB);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_BIB_LINK);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);

    // Save all pmids
    QHash<QString, int> pmidsBibliographyId;
    QStringList pmids;
    pmids << m_iamTreePmids.values();
    pmids << m_ddiPmids.values();
    pmids.removeAll("");
    pmids.removeDuplicates();
    QSqlQuery query(db);
    foreach(const QString &pmid, pmids) {
        query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_BIB));
        query.bindValue(DrugsDB::Constants::BIB_BIBID, QVariant());
        query.bindValue(DrugsDB::Constants::BIB_TYPE, "pubmed");
        query.bindValue(DrugsDB::Constants::BIB_LINK, QString("http://www.ncbi.nlm.nih.gov/pubmed/%1?dopt=Abstract&format=text").arg(pmid));
        query.bindValue(DrugsDB::Constants::BIB_TEXTREF, QVariant());
        query.bindValue(DrugsDB::Constants::BIB_ABSTRACT, QVariant());
        query.bindValue(DrugsDB::Constants::BIB_EXPLAIN, QVariant());
        query.bindValue(DrugsDB::Constants::BIB_XML, Biblio::BiblioCore::instance()->xml(pmid).replace("'","''"));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        } else {
            pmidsBibliographyId.insert(pmid, query.lastInsertId().toInt());
        }
        query.finish();
    }

    // Save all tree references
    int bibMasterId = 0;
    foreach(int key, m_iamTreePmids.uniqueKeys()) {
        const QStringList &pmids = m_iamTreePmids.values(key);
        ++bibMasterId;
        QHash<int, QString> w;
        w.insert(DrugsDB::Constants::IAM_TREE_ID, QString("='%1'").arg(key));
        query.prepare(database->prepareUpdateQuery(DrugsDB::Constants::Table_IAM_TREE,
                                                   DrugsDB::Constants::IAM_TREE_BIBMASTERID,
                                                   w));
        query.bindValue(0, bibMasterId);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
        foreach(const QString &pmid, pmids) {
            // create the master_id for this pmid
            query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_BIB_LINK));
            query.bindValue(DrugsDB::Constants::BIB_LINK_BIBID, pmidsBibliographyId.value(pmid));
            query.bindValue(DrugsDB::Constants::BIB_LINK_MASTERID, bibMasterId);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }
    }

    // Save DDI references
    foreach(int key, m_ddiPmids.uniqueKeys()) {
        const QStringList &pmids = m_ddiPmids.values(key);
        ++bibMasterId;
        QHash<int, QString> w;
        w.insert(DrugsDB::Constants::IAKNOWLEDGE_IAKID, QString("='%1'").arg(key));
        query.prepare(database->prepareUpdateQuery(DrugsDB::Constants::Table_IAKNOWLEDGE,
                                                   DrugsDB::Constants::IAKNOWLEDGE_BIB_MASTERID,
                                                   w));
        query.bindValue(0, bibMasterId);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        query.finish();
        foreach(const QString &pmid, pmids) {
            // create the master_id for this pmid
            query.prepare(database->prepareInsertQuery(DrugsDB::Constants::Table_BIB_LINK));
            query.bindValue(DrugsDB::Constants::BIB_LINK_BIBID, pmidsBibliographyId.value(pmid));
            query.bindValue(DrugsDB::Constants::BIB_LINK_MASTERID, bibMasterId);
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                return false;
            }
            query.finish();
        }
    }
    LOG("Bibliographic references saved");
    return true;
}

/** Update the XML file for the specified DrugDrugInteraction pointer. This pointer should be extracted from the list created using the getDrugDrugInteractions(). \sa getDrugDrugInteractions() */
void DrugDrugInteractionCore::updateXmlFileForDrugDrugInteraction(DrugDrugInteraction */*ddi*/)
{
    // Populate the DomNode
    // TODO: code here ??? */
}

/** Will overwrite the thesaurus file with a new one created on the basis of the \e ddis. All precedent data will be lost. */
void DrugDrugInteractionCore::saveCompleteList(const QList<DrugDrugInteraction *> &ddis)
{
    QString xml = "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- date format = yyyy-MM-dd -->\n"
            "<!--\n"
            "  Interaction levels :\n"
            "     P: Precaution\n"
            "     C: ContreIndication\n"
            "     D: Deconseille\n"
            "     T: APrendreEnCompte Take into Account\n"
            "     450: P450\n"
            "     I: Information\n"
            "     Y: Glycoprotein P\n"
            "-->\n"
            "<DrugDrugInteractions>\n";
    for(int i=0; i < ddis.count(); ++i) {
        xml += ddis.at(i)->toXml();
    }
    xml += "\n</DrugDrugInteractions>\n";
    Utils::saveStringToFile(xml, ddiNewXmlFile(), Utils::Overwrite, Utils::DontWarnUser);
}

/** Will overwrite the thesaurus file with a new one created on the basis of the \e interactors. All precedent data will be lost. */
void DrugDrugInteractionCore::saveCompleteList(const QList<DrugInteractor *> &interactors)
{
    QString xml = "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<!-- date format = yyyy-MM-dd -->\n"
            "<DDI_Interactors>\n";
    for(int i=0; i < interactors.count(); ++i) {
        xml += interactors.at(i)->toXml();
    }
    xml += "\n</DDI_Interactors>\n";
    Utils::saveStringToFile(xml, newInteractorsFile(), Utils::Overwrite, Utils::DontWarnUser);
}

/** Create a new DrugInteractor with the \e initialLabel as \e isClass. */
DrugInteractor *DrugDrugInteractionCore::createNewInteractor(const QString &initialLabel, const bool isClass)
{
    DrugInteractor *di = new DrugInteractor;
    di->setData(DrugInteractor::IsValid, true);
    di->setData(DrugInteractor::InitialLabel, Utils::removeAccents(initialLabel.toUpper()));
    di->setData(DrugInteractor::FrLabel, initialLabel.toUpper());
    di->setData(DrugInteractor::IsClass, isClass);
    di->setData(DrugInteractor::DateOfCreation, QDate::currentDate());
    di->setData(DrugInteractor::IsDuplicated, false);
    Q_EMIT interactorCreated(di);
    return di;
}

/** Start the download of all needed pubmed references and store to the BiblioPlugin database. */
void DrugDrugInteractionCore::downloadAllPmids()
{
    // get all pmids to download
    QStringList pmids;
    foreach(DrugDrugInteraction *ddi, getDrugDrugInteractions()) {
        pmids.append(ddi->data(DrugDrugInteraction::PMIDsStringList).toStringList());
    }
    foreach(DrugInteractor *di, getDrugInteractors()) {
        pmids.append(di->data(DrugInteractor::PMIDsStringList).toStringList());
        if (di->isClass())
            pmids.append(di->allNeededPMIDs());
    }
    pmids.removeAll("");
    pmids.removeDuplicates();
    Biblio::BiblioCore::instance()->downloadPubMedData(pmids);
}
