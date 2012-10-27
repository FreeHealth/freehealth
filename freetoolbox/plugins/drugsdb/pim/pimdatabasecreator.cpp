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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "pimdatabasecreator.h"
#include "pim_constants.h"
#include "afssapsintegrator.h"
#include "tools.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

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

using namespace DrugInfos;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()         {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/PIMs/") + QDir::separator();}
static inline QString databaseAbsPath()  {return QString();}//DrugsDB::Tools::drugsDatabaseAbsFileName();}

static inline QString treeXmlFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::PIMS_FILENAME);}


PimStep::PimStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    m_UseProgressDialog(false), m_ActiveDownloadId(-1), m_Downloader(0)
{
    setObjectName("PimStep");
}

bool PimStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        LOG_ERROR("Unable to create interactions Working Path :" + workingPath());
    else
        LOG("Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath))
            LOG_ERROR("Unable to create interactions database output path :" + dbpath);
        else
            LOG("PIMs database output dir created");
    }
    return true;
}

bool PimStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool PimStep::downloadFiles(QProgressBar *bar)
{
    Q_UNUSED(bar);
    Q_EMIT downloadFinished();
    return true;
}

bool PimStep::process()
{
    qWarning() << Q_FUNC_INFO;
    // connect db
    if (!DrugsDB::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

//    if (!DrugsDB::Tools::createMasterDrugInteractionDatabase())
//        return false;

    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);

    DrugsDB::Tools::executeSqlQuery("DELETE FROM PIM_SOURCES;", Core::Constants::MASTER_DATABASE_NAME);
    DrugsDB::Tools::executeSqlQuery("DELETE FROM PIM_TYPES;", Core::Constants::MASTER_DATABASE_NAME);
    DrugsDB::Tools::executeSqlQuery("DELETE FROM PIMS;", Core::Constants::MASTER_DATABASE_NAME);
    DrugsDB::Tools::executeSqlQuery("DELETE FROM PIMS_RELATED_ATC;", Core::Constants::MASTER_DATABASE_NAME);
    DrugsDB::Tools::executeSqlQuery("DELETE FROM PIMS_RELATED_ICD;", Core::Constants::MASTER_DATABASE_NAME);

    QSqlQuery query(db);
    QDomDocument doc;
    doc.setContent(Utils::readTextFile(treeXmlFile()));

    // add sources to database
    QHash<QString, int> sourcesIds;
    QDomElement root = doc.firstChildElement(Constants::XML_TAG_ROOT);
    QDomElement element = root.firstChildElement(Constants::XML_TAG_SOURCE_ROOT);
    element = element.firstChildElement(Constants::XML_TAG_SOURCE);
    while (!element.isNull()) {

        QString req = QString("INSERT INTO PIM_SOURCES (UID, NAME, PMID, COUNTRY, WWW) "
                              "VALUES ('%1', '%2', %3, '%4', '%5');")
                .arg(element.attribute(Constants::XML_ATTRIB_SOURCE_ID))
                .arg(element.attribute(Constants::XML_ATTRIB_SOURCE_NAME))
                .arg(element.attribute(Constants::XML_ATTRIB_SOURCE_PMID, "NULL"))
                .arg(element.attribute(Constants::XML_ATTRIB_SOURCE_COUNTRY, "NULL"))
                .arg(element.attribute(Constants::XML_ATTRIB_SOURCE_WWW, "NULL"))
                ;
        req.replace(", ,", ",NULL,");
        req.replace(", '',", ", NULL,");

        qWarning() << req;

        if (!query.exec(req)) {
            LOG_QUERY_ERROR(query);
            return false;
        }
        sourcesIds.insert(element.attribute(Constants::XML_ATTRIB_SOURCE_ID), query.lastInsertId().toInt());
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
//        int masterLid = DrugsDB::Tools::addLabels(Core::Constants::MASTER_DATABASE_NAME, -1, labels);
        // create type
//        QString req = QString("INSERT INTO PIM_TYPES (PIM_TID, UID, MASTER_LID) "
//                              "VALUES (NULL, '%1', %2);")
//                .arg(element.attribute(Constants::XML_ATTRIB_TYPE_ID))
//                .arg(masterLid);

//        qWarning() << req;

//        if (!query.exec(req)) {
//            LOG_QUERY_ERROR(query);
//            return false;
//        }
        typeIds.insert(element.attribute(Constants::XML_ATTRIB_TYPE_ID), query.lastInsertId().toInt());
        element = element.nextSiblingElement(Constants::XML_TAG_TYPE);
    }

    // Create PIMS
    // getting mols to ATC correspondance using the AfssapsLinkerModel
    QMultiHash<QString, QString> molNameToAtcCode;
    QStringList interactingClasses;
    IAMDb::AfssapsLinkerModel *molLinkModel = IAMDb::AfssapsLinkerModel::instance();
    while (molLinkModel->canFetchMore(QModelIndex()))
        molLinkModel->fetchMore(QModelIndex());
    int nb = molLinkModel->rowCount();
    for(int i = 0; i < nb; ++i) {
        // get row informations
        const QString &mol = molLinkModel->index(i, IAMDb::AfssapsLinkerModel::AfssapsName).data().toString();
        if (mol.isEmpty())
            continue;
        const QString &molEn = molLinkModel->index(i, IAMDb::AfssapsLinkerModel::En_Label).data().toString();
        const QString &links = molLinkModel->index(i, IAMDb::AfssapsLinkerModel::AtcCodes).data().toString();
        const QString &type = molLinkModel->index(i, IAMDb::AfssapsLinkerModel::AffapsCategory).data().toString();

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
                savePim(element, sourceId, typeId, molNameToAtcCode);
                element = element.nextSiblingElement(Constants::XML_TAG_PIM);
            }

            pimType = pimType.nextSiblingElement(Constants::XML_TAG_PIM_TYPE);
        }

        pimSource = pimSource.nextSiblingElement(Constants::XML_TAG_PIM_SOURCE);
    }

    qWarning() << Q_FUNC_INFO;

    return true;
}

void PimStep::savePim(const QDomElement &element, const int sourceId, const int typeId, const QMultiHash<QString, QString> &molNameToAtcCode)
{
    // connect db
    if (!DrugsDB::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return;

    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);

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
//    int riskMasterLid = DrugsDB::Tools::addLabels(Core::Constants::MASTER_DATABASE_NAME, -1, labels);

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
        LOG_ERROR("PIM level is wrong: " + levelString + " line: " + element.lineNumber());
        return;
    }

    int id = -1;
    QString req;
//    QString req = QString("INSERT INTO `PIMS` (`PIM_SID`, `PIM_TID`, `LEVEL`, `RISK_MASTER_LID`) "
//                          "VALUES (%1, %2, %3, %4);")
//            .arg(sourceId)
//            .arg(typeId)
//            .arg(level)
//            .arg(riskMasterLid);

////    qWarning();
////    qWarning() << req;

//    if (!query.exec(req)) {
//        LOG_QUERY_ERROR(query);
//        LOG_ERROR(QString("PIM Error line %1").arg(element.lineNumber()));
//        return;
//    } else {
//        id = query.lastInsertId().toInt();
//    }
    query.finish();

    // add related ATC
//    QDomElement mols = element.firstChildElement(Constants::XML_TAG_MOLECULES);
//    mols = mols.firstChildElement(Constants::XML_TAG_MOLECULE);
//    while (!mols.isNull()) {
//        const QString &molName = mols.attribute(Constants::XML_ATTRIB_SOURCE_NAME);
//        // get all related ATC to the molecule
//        QStringList atcCodes = molNameToAtcCode.values(molName.toUpper());

//        // get ATC_ID from the ATC codes
//        QVector<int> atcIds;
//        if (atcCodes.isEmpty()) {
//            // manage interacting classes
//            if (molName.isEmpty()) {
//                QString c = mols.attribute(Constants::XML_ATTRIB_CLASSNAME);
//                c = DrugsDB::Tools::noAccent(c.toUpper());
//                if (!c.isEmpty()) {
//                    // get ATC_ID from the class's name
//                    atcIds = DrugsDB::Tools::getAtcIdsFromLabel(Core::Constants::MASTER_DATABASE_NAME, c);
//                } else {
//                    continue;
//                }
//            } else {
//                // get ATC_ID from the molecule's name
//                atcIds = DrugsDB::Tools::getAtcIdsFromLabel(Core::Constants::MASTER_DATABASE_NAME, molName.toUpper());
//            }
//        } else {
//            foreach(const QString &atc, atcCodes) {
//                atcIds << DrugsDB::Tools::getAtcIdsFromCode(Core::Constants::MASTER_DATABASE_NAME, atc);
//            }
//        }

//        if (atcIds.isEmpty()) {
//            QString t = QString("    <Label references=\"FreeMedFormsPIMs\" id=\"\" atcCodes=\"\" fr=\"%1\" en=\"%1\" de=\"\" es=\"\" comments=\"\" review=\"false\" reviewer=\"\" category=\"class\" warnDuplication=\"true\" autoFound=\"\" dateofreview=\"\"/>").arg(molName.toUpper());
//            qWarning() << t ;
//        }
////        qWarning() << "actCodes" << atcCodes << atcIds << molName;

//        for(int i = 0; i < atcIds.count(); ++i) {
//            int atcid = atcIds.at(i);
//            req = QString("INSERT INTO `PIMS_RELATED_ATC` (`PIM_ID`, `ATC_ID`, `MAXDAYDOSE`, `MAXDAYDOSEUNIT`) "
//                          "VALUES (%1, %2, %3, '%4');")
//                    .arg(id)
//                    .arg(atcid)
//                    .arg(mols.attribute(Constants::XML_ATTRIB_MAXDAILYDOSE).toDouble())
//                    .arg(mols.attribute(Constants::XML_ATTRIB_MAXDAILYDOSEUNIT));

//            req.replace(", ''", ", NULL");
//            req.replace(", ,", ", NULL,");

////            qWarning() << req;

//            if (!query.exec(req)) {
//                LOG_QUERY_ERROR(query);
//                LOG_ERROR(QString("PIM Error line %1").arg(mols.lineNumber()));
//                return;
//            }
//        }

//        mols = mols.nextSiblingElement(Constants::XML_TAG_MOLECULE);
//    }

//    // add related ICD10
//    QDomElement icd = element.firstChildElement(Constants::XML_TAG_DISEASES);
//    icd = icd.firstChildElement(Constants::XML_TAG_DISEASE);

//    IcdGroupLinkerModel *icdModel = IcdGroupLinkerModel::instance();

//    while (!icd.isNull()) {
//        const QString &icdGroup = icd.attribute(Constants::XML_ATTRIB_ICDGROUP);
//        // get all ICD::SIDs
//        QList<QVariant> sids = icdModel->getSidsForGroup(icdGroup);
//        foreach(const QVariant &sid, sids) {
//            req = QString("INSERT INTO `PIMS_RELATED_ICD` (PIM_RMID, PIM_ID, ICD10_ID) "
//                  "VALUES (NULL, %1, %2);")
//                    .arg(id)
//                    .arg(sid.toString());

//            if (!query.exec(req)) {
//                LOG_QUERY_ERROR(query);
//                LOG_ERROR(QString("PIM Error line %1").arg(icd.lineNumber()));
//                return;
//            }

//        }

//        icd = icd.nextSiblingElement(Constants::XML_TAG_DISEASE);
//    }
}


PimDatabaseCreator::PimDatabaseCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PimDatabaseCreator)
{
    ui->setupUi(this);
    m_Step = new PimStep(this);
    pluginManager()->addObject(m_Step);
    m_Step->createDir();

}

PimDatabaseCreator::~PimDatabaseCreator()
{
    pluginManager()->removeObject(m_Step);
    delete ui; ui=0;
    delete m_Step; m_Step=0;
}

void PimDatabaseCreator::on_createAndSave_clicked()
{
    m_Step->process();
//    if (ui->biblioCheck->isChecked())
//        d->m_Step->postProcessDownload();
}

void PimDatabaseCreator::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString PimDatabasePage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + tr("PIM database");
}
