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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pimdatabasecreator.h"
#include "pim_constants.h"
#include <drugsdb/ddi/afssapsintegrator.h>

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>
#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <icd10db/icdgrouplinker.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpdownloader.h>
#include <utils/pubmeddownloader.h>
#include <translationutils/constanttranslations.h>
#include <translationutils/googletranslator.h>

#include <extensionsystem/pluginmanager.h>

#include <QDir>
#include <QDomDocument>

#include "ui_pimdatabasecreator.h"

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString treeXmlFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::PIMS_FILENAME);}

PimDatabaseCreator::PimDatabaseCreator(QObject *parent) :
    QObject(parent),
    m_UseProgressDialog(false),
    m_ActiveDownloadId(-1),
    m_Downloader(0)
{
    setObjectName("PimDatabaseCreator");
}

bool PimDatabaseCreator::initialize()
{
    return true;
}

bool PimDatabaseCreator::populateDatabaseWithPims(DrugsDB::Internal::DrugBaseEssentials *database)
{
    qWarning() << Q_FUNC_INFO;
    QSqlDatabase db = database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR(tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
            return false;
        }
    }

    // Clean database
    QString req;
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_PIM_SOURCES);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_PIM_TYPES);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_PIMS);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_PIMS_RELATED_ATC);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);
    req = database->prepareDeleteQuery(DrugsDB::Constants::Table_PIMS_RELATED_ICD);
    DrugsDB::Tools::executeSqlQuery(req, database->connectionName(), __FILE__, __LINE__);

    QDomDocument doc;
    int line, col;
    QString error;
    if (!doc.setContent(Utils::readTextFile(treeXmlFile()), &error, &line, &col)) {
        LOG_ERROR(QString("Unable to read PIM XML file: (%1;%2) %3").arg(line).arg(col).arg(error));
        return false;
    }

    db.transaction();
    QSqlQuery query(db);

    // add sources to database
    QHash<QString, int> sourcesIds;
    QDomElement root = doc.firstChildElement(Constants::XML_TAG_ROOT);
    QDomElement element = root.firstChildElement(Constants::XML_TAG_SOURCE_ROOT);
    element = element.firstChildElement(Constants::XML_TAG_SOURCE);
    while (!element.isNull()) {
        query.prepare(database->prepareInsertQuery(Constants::Table_PIM_SOURCES));
        query.bindValue(Constants::PIM_SOURCES_SID, QVariant());
        query.bindValue(Constants::PIM_SOURCES_UID, element.attribute(Constants::XML_ATTRIB_SOURCE_ID));
        query.bindValue(Constants::PIM_SOURCES_NAME, element.attribute(Constants::XML_ATTRIB_SOURCE_NAME));
        query.bindValue(Constants::PIM_SOURCES_PMID, element.attribute(Constants::XML_ATTRIB_SOURCE_PMID));
        query.bindValue(Constants::PIM_SOURCES_COUNTRY, element.attribute(Constants::XML_ATTRIB_SOURCE_COUNTRY));
        query.bindValue(Constants::PIM_SOURCES_WWW, element.attribute(Constants::XML_ATTRIB_SOURCE_WWW));
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            db.rollback();
            return false;
        }
        sourcesIds.insert(element.attribute(Constants::XML_ATTRIB_SOURCE_ID), query.lastInsertId().toInt());
        query.finish();
        element = element.nextSiblingElement(Constants::XML_TAG_SOURCE);
    }

    // add types to database
    QHash<QString, int> typeIds;
    element = root.firstChildElement(Constants::XML_TAG_TYPE_ROOT);
    element = element.firstChildElement(Constants::XML_TAG_TYPE);
    while (!element.isNull()) {
        // create labels
        QHash<QString, QVariant> labels;
        labels.insert("fr", element.attribute(Constants::XML_ATTRIB_TYPE_FR).toUpper());
        labels.insert("en", element.attribute(Constants::XML_ATTRIB_TYPE_EN).toUpper());
        labels.insert("de", element.attribute(Constants::XML_ATTRIB_TYPE_DE).toUpper());
        labels.insert("es", element.attribute(Constants::XML_ATTRIB_TYPE_ES).toUpper());
        int masterLid = DrugsDB::Tools::addLabels(database, -1, labels);
        // create type
        query.prepare(database->prepareInsertQuery(Constants::Table_PIM_TYPES));
        query.bindValue(Constants::PIM_TYPES_TID, QVariant());
        query.bindValue(Constants::PIM_TYPES_UID, element.attribute(Constants::XML_ATTRIB_TYPE_ID));
        query.bindValue(Constants::PIM_TYPES_MASTER_LID, masterLid);
        if (!query.exec()) {
            LOG_QUERY_ERROR(query);
            query.finish();
            db.rollback();
            return false;
        }
        typeIds.insert(element.attribute(Constants::XML_ATTRIB_TYPE_ID), query.lastInsertId().toInt());
        query.finish();
        element = element.nextSiblingElement(Constants::XML_TAG_TYPE);
    }

    query.finish();
    db.commit();

    // Create PIMS
    // getting mols to ATC correspondance using the AfssapsLinkerModel
    QMultiHash<QString, QString> molNameToAtcCode;
    QStringList interactingClasses;
    DrugsDB::AfssapsLinkerModel *molLinkModel = DrugsDB::AfssapsLinkerModel::instance();
    while (molLinkModel->canFetchMore(QModelIndex()))
        molLinkModel->fetchMore(QModelIndex());
    int nb = molLinkModel->rowCount();
    for(int i = 0; i < nb; ++i) {
        // get row informations
        const QString &mol = molLinkModel->index(i, DrugsDB::AfssapsLinkerModel::AfssapsName).data().toString();
        if (mol.isEmpty())
            continue;
        const QString &molEn = molLinkModel->index(i, DrugsDB::AfssapsLinkerModel::En_Label).data().toString();
        const QString &links = molLinkModel->index(i, DrugsDB::AfssapsLinkerModel::AtcCodes).data().toString();
        const QString &type = molLinkModel->index(i, DrugsDB::AfssapsLinkerModel::AffapsCategory).data().toString();

        if (type=="class") {
            interactingClasses << DrugsDB::Tools::noAccent(mol).toUpper();
            interactingClasses << molEn.toUpper();
        } else if (links.isEmpty()) {
            continue;
        } else {
            foreach(const QString &atcCode, links.split(",", QString::SkipEmptyParts)) {
                molNameToAtcCode.insertMulti(mol.toUpper(), atcCode.toUpper());
                if (!molEn.isEmpty()) {
                    molNameToAtcCode.insertMulti(molEn.toUpper(), atcCode.toUpper());
                }
            }
        }
    }

//    <PIMs>
//      <PIM_Source id="beers2003">
//        <PIM_Type id="PimOld" name="POTENTIALLY INNAPROPRIATE MEDICATION USE IN OLD ADULTS: INDEPENDANT TO DIAGNOSES OR CONDITIONS" >
//          <PIM level="low">
//          ...
//          </PIM>
    QDomElement pimRoot = root.firstChildElement(Constants::XML_TAG_PIM_ROOT);
    // get all sources
    QDomElement pimSource = pimRoot.firstChildElement(Constants::XML_TAG_PIM_SOURCE);
    while (!pimSource.isNull()) {
        int sourceId = sourcesIds.value(pimSource.attribute(Constants::XML_ATTRIB_SOURCE_ID));

        // get all type
        QDomElement pimType = pimSource.firstChildElement(Constants::XML_TAG_PIM_TYPE);
        while (!pimType.isNull()) {
            int typeId = typeIds.value(pimType.attribute(Constants::XML_ATTRIB_TYPE_ID));

            // get all pims
            element = pimType.firstChildElement(Constants::XML_TAG_PIM);
            while (!element.isNull()) {
                savePim(database, element, sourceId, typeId, molNameToAtcCode);
                element = element.nextSiblingElement(Constants::XML_TAG_PIM);
            }

            pimType = pimType.nextSiblingElement(Constants::XML_TAG_PIM_TYPE);
        }

        pimSource = pimSource.nextSiblingElement(Constants::XML_TAG_PIM_SOURCE);
    }

    qWarning() << Q_FUNC_INFO;

    return true;
}

/** Save a PIM into the database. Does not handle its own transsaction */
bool PimDatabaseCreator::savePim(DrugsDB::Internal::DrugBaseEssentials *database, const QDomElement &element, const int sourceId, const int typeId, const QMultiHash<QString, QString> &molNameToAtcCode)
{
    // connect db
    QSqlDatabase db = database->database();
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR(tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
            return false;
        }
    }

    QSqlQuery query(db);

    // <PIM level="low">
    //   <Molecules>
    //     <Molecule name="fr or en" maxDailyDose="" />
    //   </Molecules>
    //   <Risk lang="en" value="Offers few analgesic advantages over acetaminophen, yet has the adverse effects of other narcotic drugs."/>
    //   <Diseases>
    //     <Disease icd="ChronicConstipation" />
    //   </Diseases>
    // </PIM>

    // create risk labels
    QHash<QString, QVariant> labels;
    QDomElement risk = element.firstChildElement(Constants::XML_TAG_RISK);
    while (!risk.isNull()) {
        labels.insert(risk.attribute(Constants::XML_ATTRIB_RISK_LANG), risk.attribute(Constants::XML_ATTRIB_RISK_VALUE));
        risk = risk.nextSiblingElement(Constants::XML_TAG_RISK);
    }
    int riskMasterLid = DrugsDB::Tools::addLabels(database, -1, labels);

    // create PIM
    int level = 0;
    const QString &levelString = element.attribute(Constants::XML_ATTRIB_PIM_LEVEL);
    if (levelString.compare("high", Qt::CaseInsensitive)==0) {
        level = 3;
    } else if (levelString.compare("medium", Qt::CaseInsensitive)==0) {
        level = 2;
    } else if (levelString.compare("low", Qt::CaseInsensitive)==0) {
        level = 1;
    }
    if (!level) {
        LOG_ERROR(QString("PIM level is wrong: %1; line: %2").arg(levelString).arg(element.lineNumber()));
        return false;
    }

    int id = -1;
    QString req;
    query.prepare(database->prepareInsertQuery(Constants::Table_PIMS));
    query.bindValue(Constants::PIMS_ID, QVariant());
    query.bindValue(Constants::PIMS_SID, sourceId);
    query.bindValue(Constants::PIMS_TID, typeId);
    query.bindValue(Constants::PIMS_LEVEL, level);
    query.bindValue(Constants::PIMS_RISK_MASTER_LID, riskMasterLid);
    if (!query.exec()) {
        LOG_QUERY_ERROR(query);
        LOG_ERROR(QString("PIM Error line %1").arg(element.lineNumber()));
        query.finish();
        // db.rollback();
        return false;
    } else {
        id = query.lastInsertId().toInt();
    }
    query.finish();

    // add related ATC
    QDomElement mols = element.firstChildElement(Constants::XML_TAG_MOLECULES);
    mols = mols.firstChildElement(Constants::XML_TAG_MOLECULE);
    while (!mols.isNull()) {
        const QString &molName = mols.attribute(Constants::XML_ATTRIB_SOURCE_NAME);
        if (molName.isEmpty())
            continue;

        // get all related ATC to the molecule
        QStringList atcCodes = molNameToAtcCode.values(molName.toUpper());

        // get ATC_ID from the ATC codes
        QVector<int> atcIds;
        if (atcCodes.isEmpty()) {
            // manage interacting classes
            if (molName.isEmpty()) {
                QString c = mols.attribute(Constants::XML_ATTRIB_CLASSNAME);
                c = DrugsDB::Tools::noAccent(c.toUpper());
                if (!c.isEmpty()) {
                    // get ATC_ID from the class's name
                    atcIds = DrugsDB::Tools::getAtcIdsFromLabel(database, c);
                } else {
                    continue;
                }
            } else {
                // get ATC_ID from the molecule's name
                atcIds = DrugsDB::Tools::getAtcIdsFromLabel(database, molName.toUpper());
            }
        } else {
            foreach(const QString &atc, atcCodes) {
                atcIds << DrugsDB::Tools::getAtcIdsFromCode(database, atc);
            }
        }
        atcIds = Utils::removeDuplicates(atcIds);

        if (atcIds.isEmpty()) {
            LOG(QString("ATC without ID: %1").arg(molName.toUpper()));
            qWarning() << QString("    <Label references=\"FreeMedFormsPIMs\" id=\"\" atcCodes=\"\" fr=\"%1\" en=\"%1\" de=\"\" es=\"\" comments=\"\" review=\"false\" reviewer=\"\" category=\"class\" warnDuplication=\"true\" autoFound=\"\" dateofreview=\"\"/>").arg(molName.toUpper());
        }
        qWarning() << "  PIM: actCodes" << atcCodes << atcIds << molName;

        for(int i = 0; i < atcIds.count(); ++i) {
            int atcid = atcIds.at(i);
            query.prepare(database->prepareInsertQuery(Constants::Table_PIMS_RELATED_ATC));
            query.bindValue(Constants::PIMS_RELATC_RMID, QVariant());
            query.bindValue(Constants::PIMS_RELATC_PIM_ID, id);
            query.bindValue(Constants::PIMS_RELATC_ATC_ID, atcid);
            query.bindValue(Constants::PIMS_RELATC_MAXDAYDOSE, mols.attribute(Constants::XML_ATTRIB_MAXDAILYDOSE).toDouble());
            query.bindValue(Constants::PIMS_RELATC_MAXDAYDOSEUNIT, mols.attribute(Constants::XML_ATTRIB_MAXDAILYDOSEUNIT));
            if (!query.exec()) {
                LOG_QUERY_ERROR(query);
                LOG_ERROR(QString("PIM Error line %1").arg(mols.lineNumber()));
                return false;
            }
            query.finish();
        }
        mols = mols.nextSiblingElement(Constants::XML_TAG_MOLECULE);
    }

    // add related ICD10
    Icd10::IcdGroupLinkerModel *icdModel = Icd10::IcdGroupLinkerModel::instance();
    if (icdModel->databaseAvailable()) {
        LOG("Adding ICD related PIMs");
        QDomElement icd = element.firstChildElement(Constants::XML_TAG_DISEASES);
        icd = icd.firstChildElement(Constants::XML_TAG_DISEASE);
        while (!icd.isNull()) {
            const QString &icdGroup = icd.attribute(Constants::XML_ATTRIB_ICDGROUP);
            // get all ICD::SIDs
            QList<QVariant> sids = icdModel->getSidsForGroup(icdGroup);
            foreach(const QVariant &sid, sids) {
                query.prepare(database->prepareInsertQuery(Constants::Table_PIMS_RELATED_ICD));
                query.bindValue(Constants::PIMS_RELICD_RMID, QVariant());
                query.bindValue(Constants::PIMS_RELICD_PIM_ID, id);
                query.bindValue(Constants::PIMS_RELICD_ICD_SID, sid);
                if (!query.exec()) {
                    LOG_QUERY_ERROR(query);
                    LOG_ERROR(QString("PIM Error line %1").arg(icd.lineNumber()));
                    return false;
                }
                query.finish();
            }
            icd = icd.nextSiblingElement(Constants::XML_TAG_DISEASE);
        }
    }
    return true;
}


