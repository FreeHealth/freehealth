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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "interactionstep.h"
#include "afssapsintegrator.h"
#include "interactionsdatabasepage.h"
#include "drugdruginteraction.h"
#include "drugdruginteractioncore.h"
#include "druginteractor.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>

#include <biblio/bibliocore.h>

#include <drugsbaseplugin/drugbaseessentials.h>
#include <drugsbaseplugin/constants_databaseschema.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpdownloader.h>
#include <utils/pubmeddownloader.h>
#include <translationutils/constanttranslations.h>
#include <translationutils/googletranslator.h>

#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <QFile>
#include <QApplication>
#include <QDir>
#include <QProgressDialog>

using namespace IAMDb;
using namespace Trans::ConstantTranslations;

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline IAMDb::DrugDrugInteractionCore *core() {return IAMDb::DrugDrugInteractionCore::instance();}

static inline QString workingPath()         {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/Interactions/") + QDir::separator();}
static inline QString databaseAbsPath()  {return QString();}//DrugsDB::Tools::drugsDatabaseAbsFileName();}
static inline QString databaseOutputPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString());}

static inline QString translationsCorrectionsFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::INTERACTIONS_ENGLISHCORRECTIONS_FILENAME);}
static inline QString afssapsIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString atcCsvFile()          {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + Core::Constants::ATC_FILENAME);}


InteractionStep::InteractionStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    m_UseProgressDialog(false), m_Downloader(0)
{
    setObjectName("InteractionStep");
}

bool InteractionStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        LOG_ERROR("Unable to create interactions Working Path:" + workingPath());
    else
        LOG("Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath))
            LOG_ERROR("Unable to create interactions database output path:" + dbpath);
        else
            LOG("Drugs database output dir created");
    }
    return true;
}

bool InteractionStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool InteractionStep::downloadFiles(QProgressBar *bar)
{
    Q_UNUSED(bar);
    Q_EMIT downloadFinished();
    return true;
}

struct Source
{
    Source() {}
    bool isNull() {return (m_TreeClass.isEmpty() && m_Inn.isEmpty() && m_Link.isEmpty());}

    QString m_TreeClass, m_Inn, m_Link, m_TypeOfLink, m_Abstract, m_TextualReference, m_Explanation;
};

bool InteractionStep::process()
{
    // Erase old database
//    QFileInfo info(databaseAbsPath());
//    if (info.exists()) {
//        QFile f(info.absoluteFilePath());
//        f.copy(info.absolutePath() + QDir::separator() + "bkup" + QDateTime::currentDateTime().toString("yyMMddHHmmss") + info.fileName());
//        f.remove();
//    }
//    DrugsDB::Tools::drugBase()->initialize(databaseOutputPath(), true);
//    return computeModelsAndPopulateDatabase();
    Q_EMIT processFinished();
    return true;
}

bool InteractionStep::computeModelsAndPopulateDatabase()
{
    // connect db
    if (!DrugsDB::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

//    if (!DrugsDB::Tools::createMasterDrugInteractionDatabase())
//        return false;

//    DrugsDB::Tools::recreateRoutes();

    QSqlDatabase iam = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);

    Q_EMIT progressLabelChanged(tr("Creating interactions database"));
    Q_EMIT progressRangeChanged(0, 7);
    Q_EMIT progress(0);

    QList<DrugInteractor *> interactors = core()->getDrugInteractors();
    QList<DrugDrugInteraction *> ddis   = core()->getDrugDrugInteractions();

    // Save ATC + FreeMedForms specific codes
    saveAtcClassification(interactors);

    // Recreate interacting classes tree
//    QString req = DrugsDB::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE);//"DELETE FROM ATC_CLASS_TREE";
//    DrugsDB::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);
//    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
//    db.transaction();
//    foreach(DrugInteractor *interactor, interactors) {
//        if (interactor->isClass()) {
//            if (!saveClassDrugInteractor(interactor, interactors, db, 0)) {
//                db.rollback();
//                return false;
//            }
//        }
//    }
//    db.commit();
    Q_EMIT progress(2);

    // Save DDIs
    saveDrugDrugInteractions(interactors, ddis);

    // Save Bibliographic references
    saveBibliographicReferences();


    QMultiHash<QString, Source> class_sources;
    // Add interacting classes tree
    Q_EMIT progressLabelChanged(tr("Creating interactions database (create DDI.Classes tree)"));
    Q_EMIT progress(5);
    qWarning() << "sources=" << class_sources.count() << class_sources.uniqueKeys().count();

    // Computation
    Q_EMIT progressLabelChanged(tr("Creating interactions database (save DDI.Classes tree)"));
    Q_EMIT progress(7);

    iam.commit();

    // refresh the innToAtc content (reload ATC codes because we added some new ATC)


    // drugs databases needs to be relinked with the new ATC codes


    Q_EMIT processFinished();

    return true;
}

void InteractionStep::downloadNextSource()
{
//    static int maxDownloads = 0;
//    WARN_FUNC << m_ActiveDownloadId;
//    if (!m_Downloader) {
//        Q_EMIT progressLabelChanged(tr("Downloading bibliographic data"));
//        Q_EMIT progressRangeChanged(0, 1);
//        Q_EMIT progress(0);
//        m_Downloader = new Utils::PubMedDownloader;
//        connect(m_Downloader, SIGNAL(downloadFinished()), this, SLOT(downloadNextSource()));
//    }

//    // connect db
//    if (!DrugsDB::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
//        return ;

//    // If first call --> get all sources to download
//    if (m_ActiveDownloadId == -1) {
//        LOG("Gettings bibliographies to download");
//        m_SourceToDownload.clear();
//        QString req = "SELECT `BIB_ID` FROM `BIBLIOGRAPHY` WHERE (`BIBLIOGRAPHY`.`XML` IS NULL);";
//        QSqlQuery query(QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME));
//        if (query.exec(req)) {
//            while (query.next()) {
//                m_SourceToDownload << query.value(0).toInt();
//            }
//        } else {
//            LOG_QUERY_ERROR(query);
//        }
//        LOG(QString("Got %1 bibliographies to download").arg(m_SourceToDownload.count()));
//        if (m_SourceToDownload.isEmpty()) {
//            delete m_Downloader;
//            m_Downloader = 0;
//            Q_EMIT postProcessDownloadFinished();
//            return;
//        }
//        maxDownloads = m_SourceToDownload.count();
//        Q_EMIT progressRangeChanged(0, maxDownloads);
//        m_ActiveDownloadId = m_SourceToDownload.first();
//    } else {
//        // Source retrieved
//        QString req = QString("UPDATE `BIBLIOGRAPHY` SET "
//                              "`TEXTUAL_REFERENCE`='%1', "
//                              "`ABSTRACT`='%2', "
//                              "`XML`='%3' "
//                              "WHERE `BIB_ID`=%4;")
//                .arg(m_Downloader->reference().replace("'","''"))
//                .arg(m_Downloader->abstract().replace("'","''"))
//                .arg(m_Downloader->xmlEncoded().replace("'","''"))
//                .arg(m_ActiveDownloadId)
//                ;
//        DrugsDB::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);

//        if (m_SourceToDownload.contains(m_ActiveDownloadId))
//            m_SourceToDownload.remove(0);

//        if (m_SourceToDownload.count() == 0) {
//            delete m_Downloader;
//            m_Downloader = 0;
//            Q_EMIT postProcessDownloadFinished();
//            return;
//        }

//        Q_EMIT progressRangeChanged(0, 1);
//        Q_EMIT progressRangeChanged(0, maxDownloads - m_SourceToDownload.count());

//        m_ActiveDownloadId = m_SourceToDownload.first();
//    }

//    if (m_ActiveDownloadId == -1)
//        return;

//    // Get link
//    QString link;
//    QString req = QString("SELECT `LINK` FROM `BIBLIOGRAPHY` WHERE (`BIB_ID`=%1 AND `LINK` NOT NULL) LIMIT 1;").arg(m_ActiveDownloadId);
//    QSqlQuery query(QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME));
//    if (query.exec(req)) {
//        if (query.next()) {
//            link = query.value(0).toString();
//        }
//    }

//    if (link.isEmpty()) {
//        delete m_Downloader;
//        m_Downloader = 0;
//        Q_EMIT postProcessDownloadFinished();
//        return;
//    }

//    query.finish();

//    // Start pubmed downloader
//    if (m_Downloader->setFullLink(link)) {
//        LOG(QString("Downloading (id:%1) link: %2").arg(m_ActiveDownloadId).arg(link));
//        m_Downloader->startDownload();
//    } else {
//        LOG_ERROR("Unable to download pubmed link " + link);
//    }
}

bool InteractionStep::saveAtcClassification(const QList<DrugInteractor *> &interactors)
{
    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("InteractionStep", tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
            return false;
        }
    }
    Q_EMIT progressLabelChanged(tr("Creating interactions database (refresh ATC table)"));
    // Clean ATC table from old values
    QString req;
//    req = DrugsDB::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_ATC);
//    DrugsDB::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);
//    req = DrugsDB::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_ATC_LABELS);
//    DrugsDB::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);

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
//            if (!DrugsDB::Tools::createAtc(Core::Constants::MASTER_DATABASE_NAME, values[0].remove("\""), labels)) {
//                return false;
//            }
        }
    }
    Q_EMIT progress(1);

    // add FreeDiams ATC specific codes
    Q_EMIT progressLabelChanged(tr("Creating interactions database (add specific ATC codes)"));
    db.transaction();

    // 100 000 < ID < 199 999  == Interacting molecules without ATC code
    // 200 000 < ID < 299 999  == Interactings classes
    int molId = 100000;
    int classId = 200000;
    foreach(DrugInteractor *di, interactors) {
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
//                if (!DrugsDB::Tools::createAtc(Core::Constants::MASTER_DATABASE_NAME, "ZXX" + n, labels, classId, !di->data(DrugInteractor::DoNotWarnDuplicated).toBool()))
//                    return false;
                di->setData(CLASS_OR_MOL_ID, classId);
                di->setData(FREEMEDFORMS_ATC_CODE, "ZXX" + n);
            } else {
                ++molId;
                QString n = QString::number(molId-100000);
                n = n.rightJustified(2, '0');
//                if (!DrugsDB::Tools::createAtc(Core::Constants::MASTER_DATABASE_NAME, "Z01AA" + n, labels, molId, !di->data(DrugInteractor::DoNotWarnDuplicated).toBool()))
//                    return false;
                di->setData(CLASS_OR_MOL_ID, molId);
                di->setData(FREEMEDFORMS_ATC_CODE, "Z01AA" + n);
            }
        }
    }
    db.commit();
    return true;
}

bool InteractionStep::saveClassDrugInteractor(DrugInteractor *interactor, const QList<DrugInteractor *> &completeList, QSqlDatabase &db, DrugInteractor *parent)
{
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("InteractionStep", tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
            return false;
        }
    }

    // save interactor
    QSqlQuery query(db);
    int id = -1;
    // save using all associated ATC codes
    const QStringList &atcCodes = interactor->data(DrugInteractor::ATCCodeStringList).toStringList();
    if (atcCodes.isEmpty() && !interactor->isClass() && parent && parent->isClass()) {
        //        QString req = QString("INSERT INTO ATC_CLASS_TREE (ID_TREE, ID_CLASS, ID_ATC) VALUES "
        //                              "(NULL, %1,%2);")
        //                .arg(parent->data(CLASS_OR_MOL_ID).toString())
        //                .arg(interactor->data(CLASS_OR_MOL_ID).toString());
//        query.prepare(DrugsDB::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE));
//        query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID, QVariant());
//        query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_ATC, interactor->data(CLASS_OR_MOL_ID).toString());
//        query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_CLASS, parent->data(CLASS_OR_MOL_ID).toString());
//        query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_BIBMASTERID, QVariant());

//        if (!query.exec()) {
//            LOG_QUERY_ERROR_FOR("InteractionStep", query);
//        } else {
//            id = query.lastInsertId().toInt();
//        }
        query.finish();
    } else if (!atcCodes.isEmpty() && !interactor->isClass() && parent && parent->isClass()) {
        foreach(const QString &atc, atcCodes) {
            //            QString req = QString("INSERT INTO ATC_CLASS_TREE (ID_TREE, ID_CLASS, ID_ATC) VALUES "
            //                                  "(NULL, %1, (SELECT ATC_ID FROM ATC WHERE CODE=\"%2\"));")
            //                    .arg(parent->data(CLASS_OR_MOL_ID).toString()).arg(atc);

            QString atcId;
            QHash<int, QString> w;
            w.insert(DrugsDB::Constants::ATC_CODE, QString("='%1'").arg(atc));
//            QString req = DrugsDB::Tools::drugBase()->select(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID, w);
//            if (query.exec(req)) {
//                if (query.next())
//                    atcId = query.value(0).toString();
//            } else {
//                LOG_QUERY_ERROR_FOR("InteractionStep", query);
//                db.rollback();
//                return false;
//            }
            query.finish();

//            query.prepare(DrugsDB::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE));
//            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID, QVariant());
//            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_ATC, atcId);
//            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_ID_CLASS, parent->data(CLASS_OR_MOL_ID).toString());
//            query.bindValue(DrugsDB::Constants::ATC_CLASS_TREE_BIBMASTERID, QVariant());

//            if (!query.exec()) {
//                LOG_QUERY_ERROR_FOR("InteractionStep", query);
//            } else {
//                id = query.lastInsertId().toInt();
//            }
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
                if (!saveClassDrugInteractor(child, completeList, db, interactor))
                    return false;
        }
    }
    return true;
}

bool InteractionStep::saveDrugDrugInteractions(const QList<DrugInteractor *> &interactors, const QList<DrugDrugInteraction *> ddis)
{
    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
    if (!db.isOpen()) {
        if (!db.open()) {
            LOG_ERROR_FOR("InteractionStep", tkTr(Trans::Constants::ERROR_1_FROM_DATABASE_2).arg(db.lastError().text()).arg(db.connectionName()));
            return false;
        }
    }
    // Clear database first
//    QString req;// = "DELETE FROM INTERACTIONS";
//    req = DrugsDB::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_INTERACTIONS);
//    DrugsDB::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);
//    //    req = "DELETE FROM IAKNOWLEDGE";
//    req = DrugsDB::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_IAKNOWLEDGE);
//    DrugsDB::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);
//    //    req = "DELETE FROM IA_IAK";
//    req = DrugsDB::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_IA_IAK);
//    DrugsDB::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);

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
//                req = DrugsDB::Tools::drugBase()->select(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID, w);
//                if (query.exec(req)) {
//                    if (query.next())
//                        atcId1 = query.value(0).toString();
//                } else {
//                    LOG_QUERY_ERROR_FOR("InteractionStep", query);
//                    db.rollback();
//                    return false;
//                }
                query.finish();

                QString atcId2;
                w.clear();
                w.insert(DrugsDB::Constants::ATC_CODE, QString("='%1'").arg(a2));
//                req = DrugsDB::Tools::drugBase()->select(DrugsDB::Constants::Table_ATC, DrugsDB::Constants::ATC_ID, w);
//                if (query.exec(req)) {
//                    if (query.next())
//                        atcId2 = query.value(0).toString();
//                } else {
//                    LOG_QUERY_ERROR_FOR("InteractionStep", query);
//                    db.rollback();
//                    return false;
//                }
                query.finish();

                //                req = QString("INSERT INTO INTERACTIONS (ATC_ID1, ATC_ID2) VALUES (%1, %2);")
                //                        .arg(QString("(SELECT ATC_ID FROM ATC WHERE CODE=\"%1\")").arg(a1))
                //                        .arg(QString("(SELECT ATC_ID FROM ATC WHERE CODE=\"%1\")").arg(a2));
//                query.prepare(DrugsDB::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_INTERACTIONS));
//                query.bindValue(DrugsDB::Constants::INTERACTIONS_IAID, QVariant());
//                query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID1, atcId1);
//                query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID2, atcId2);
//                if (!query.exec()) {
//                    LOG_QUERY_ERROR(query);
//                    LOG_ERROR(QString("*** Interactors not found: \n  %1 - %2 (%3)")
//                              .arg(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())
//                              .arg(ddi->data(DrugDrugInteraction::SecondInteractorName).toString())
//                              .arg(ddi->data(DrugDrugInteraction::LevelName).toString()));
//                    db.rollback();
//                    return false;
//                } else {
//                    ia_ids << query.lastInsertId().toInt();
//                }
                query.finish();

                // mirror DDI
                //                req = QString("INSERT INTO INTERACTIONS (ATC_ID2, ATC_ID1) VALUES (%1, %2);")
                //                        .arg(QString("(SELECT ATC_ID FROM ATC WHERE CODE=\"%1\")").arg(a1))
                //                        .arg(QString("(SELECT ATC_ID FROM ATC WHERE CODE=\"%1\")").arg(a2));
//                query.prepare(DrugsDB::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_INTERACTIONS));
//                query.bindValue(DrugsDB::Constants::INTERACTIONS_IAID, QVariant());
//                query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID1, atcId2);
//                query.bindValue(DrugsDB::Constants::INTERACTIONS_ATC_ID2, atcId1);
//                if (!query.exec()) {
//                    LOG_QUERY_ERROR(query);
//                    LOG_ERROR(QString("*** Interactors not found: \n  %1 - %2 (%3)")
//                              .arg(ddi->data(DrugDrugInteraction::FirstInteractorName).toString())
//                              .arg(ddi->data(DrugDrugInteraction::SecondInteractorName).toString())
//                              .arg(ddi->data(DrugDrugInteraction::LevelName).toString()));
//                    db.rollback();
//                    return false;
//                } else {
//                    ia_ids << query.lastInsertId().toInt();
//                }
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
//        int riskMasterLid = DrugsDB::Tools::addLabels(Core::Constants::MASTER_DATABASE_NAME, -1, risk);
//        int manMasterLid = DrugsDB::Tools::addLabels(Core::Constants::MASTER_DATABASE_NAME, -1, management);
//        if (riskMasterLid==-1 || manMasterLid==-1)
//            return false;

        // Add IAK
        //        req = QString("INSERT INTO IAKNOWLEDGE (IAKID, TYPE, RISK_MASTER_LID, MAN_MASTER_LID) VALUES "
        //                      "(NULL, \"%1\", %2, %3)")
        //                .arg(ddi->data(DrugDrugInteraction::LevelCode).toString())
        //                .arg(riskMasterLid)
        //                .arg(manMasterLid);

//        query.prepare(DrugsDB::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_IAKNOWLEDGE));
//        query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_IAKID, QVariant());
//        query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_TYPE, ddi->data(DrugDrugInteraction::LevelCode).toString());
//        query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_RISK_MASTERLID, riskMasterLid);
//        query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_BIB_MASTERID, QVariant());
//        query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_MANAGEMENT_MASTERLID, manMasterLid);
//        query.bindValue(DrugsDB::Constants::IAKNOWLEDGE_WWW, QVariant());
//        if (query.exec()) {
//            // keep trace of bibliographic references
//            iak_id = query.lastInsertId().toInt();
//            foreach(const QString &pmid, ddi->data(DrugDrugInteraction::PMIDsStringList).toStringList())
//                m_ddiPmids.insertMulti(iak_id, pmid);
//        } else {
//            LOG_QUERY_ERROR(query);
//            db.rollback();
//            return false;
//        }
        query.finish();

        // Add to IA_IAK link table
        foreach(const int ia, ia_ids) {
            //            req = QString("INSERT INTO IA_IAK (IAID, IAKID) VALUES (%1,%2)")
            //                    .arg(ia)
            //                    .arg(iak_id)
            //                    ;
//            query.prepare(DrugsDB::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_IA_IAK));
//            query.bindValue(DrugsDB::Constants::IA_IAK_IAID, ia);
//            query.bindValue(DrugsDB::Constants::IA_IAK_IAKID, iak_id);
//            if (!query.exec()) {
//                LOG_QUERY_ERROR(query);
//                db.rollback();
//                return false;
//            }
            query.finish();
        }
    }

    db.commit();

    return true;
}

bool InteractionStep::saveBibliographicReferences()
{
    qWarning() << "SAVE BIBLIO";
    // TODO: Ensure all PMIDs are available */
    QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
    // Clear database first
//    QString req = DrugsDB::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_BIB);
//    DrugsDB::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);
//    req = DrugsDB::Tools::drugBase()->prepareDeleteQuery(DrugsDB::Constants::Table_BIB_LINK);
//    DrugsDB::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);

    // Save all pmids
    QHash<QString, int> pmidsBibliographyId;
    QStringList pmids;
    pmids << m_iamTreePmids.values();
    pmids << m_ddiPmids.values();
    pmids.removeAll("");
    pmids.removeDuplicates();
    QSqlQuery query(db);
    foreach(const QString &pmid, pmids) {
        //        req = QString("INSERT INTO BIBLIOGRAPHY (TYPE, LINK, XML) "
        //                      "VALUES ('pubmed', 'http://www.ncbi.nlm.nih.gov/pubmed/%1?dopt=Abstract&format=text', '%2')")
        //                .arg(pmid)
        //                .arg(Biblio::BiblioCore::instance()->xml(pmid).replace("'","''"));
//        query.prepare(DrugsDB::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_BIB));
//        query.bindValue(DrugsDB::Constants::BIB_BIBID, QVariant());
//        query.bindValue(DrugsDB::Constants::BIB_TYPE, "pubmed");
//        query.bindValue(DrugsDB::Constants::BIB_LINK, QString("http://www.ncbi.nlm.nih.gov/pubmed/%1?dopt=Abstract&format=text").arg(pmid));
//        query.bindValue(DrugsDB::Constants::BIB_TEXTREF, QVariant());
//        query.bindValue(DrugsDB::Constants::BIB_ABSTRACT, QVariant());
//        query.bindValue(DrugsDB::Constants::BIB_EXPLAIN, QVariant());
//        query.bindValue(DrugsDB::Constants::BIB_XML, Biblio::BiblioCore::instance()->xml(pmid).replace("'","''"));
//        if (!query.exec()) {
//            LOG_QUERY_ERROR(query);
//            return false;
//        } else {
//            pmidsBibliographyId.insert(pmid, query.lastInsertId().toInt());
//        }
        query.finish();
    }

    // Save all tree references
    int bibMasterId = 0;
    foreach(int key, m_iamTreePmids.uniqueKeys()) {
        const QStringList &pmids = m_iamTreePmids.values(key);
        ++bibMasterId;
        //        req = QString("UPDATE ATC_CLASS_TREE SET BIB_MASTER_ID=%1 WHERE ID_TREE=%2")
        //                .arg(bibMasterId).arg(key);
        QHash<int, QString> w;
        w.insert(DrugsDB::Constants::ATC_CLASS_TREE_ID, QString("='%1'").arg(key));
//        query.prepare(DrugsDB::Tools::drugBase()->prepareUpdateQuery(DrugsDB::Constants::Table_ATC_CLASS_TREE,
//                                                                     DrugsDB::Constants::ATC_CLASS_TREE_BIBMASTERID,
//                                                                     w));
//        query.bindValue(0, bibMasterId);
//        if (!query.exec()) {
//            LOG_QUERY_ERROR(query);
//            return false;
//        }
        query.finish();
        foreach(const QString &pmid, pmids) {
            // create the master_id for this pmid
            //            req = QString("INSERT INTO BIBLIOGRAPHY_LINKS (BIB_MASTER_ID, BIB_ID) VALUES (%1, %2)")
            //                    .arg(bibMasterId).arg(pmidsBibliographyId.value(pmid));
//            query.prepare(DrugsDB::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_BIB_LINK));
//            query.bindValue(DrugsDB::Constants::BIB_LINK_BIBID, pmidsBibliographyId.value(pmid));
//            query.bindValue(DrugsDB::Constants::BIB_LINK_MASTERID, bibMasterId);
//            if (!query.exec()) {
//                LOG_QUERY_ERROR(query);
//                return false;
//            }
            query.finish();
        }
    }

    // Save DDI references
    foreach(int key, m_ddiPmids.uniqueKeys()) {
        const QStringList &pmids = m_ddiPmids.values(key);
        ++bibMasterId;
        //        req = QString("UPDATE IAKNOWLEDGE SET BIB_MASTER_ID=%1 WHERE IAKID=%2")
        //                .arg(bibMasterId).arg(key);
        QHash<int, QString> w;
        w.insert(DrugsDB::Constants::IAKNOWLEDGE_IAKID, QString("='%1'").arg(key));
//        query.prepare(DrugsDB::Tools::drugBase()->prepareUpdateQuery(DrugsDB::Constants::Table_IAKNOWLEDGE,
//                                                                     DrugsDB::Constants::IAKNOWLEDGE_BIB_MASTERID,
//                                                                     w));
//        query.bindValue(0, bibMasterId);
//        if (!query.exec()) {
//            LOG_QUERY_ERROR(query);
//            return false;
//        }
        query.finish();
        foreach(const QString &pmid, pmids) {
            // create the master_id for this pmid
            //            req = QString("INSERT INTO BIBLIOGRAPHY_LINKS (BIB_MASTER_ID, BIB_ID) VALUES (%1, %2)")
            //                    .arg(bibMasterId).arg(pmidsBibliographyId.value(pmid));
//            query.prepare(DrugsDB::Tools::drugBase()->prepareInsertQuery(DrugsDB::Constants::Table_BIB_LINK));
//            query.bindValue(DrugsDB::Constants::BIB_LINK_BIBID, pmidsBibliographyId.value(pmid));
//            query.bindValue(DrugsDB::Constants::BIB_LINK_MASTERID, bibMasterId);
//            if (!query.exec()) {
//                LOG_QUERY_ERROR(query);
//                return false;
//            }
            query.finish();
        }
    }
    qWarning() << "END SAVE BIBLIO";
    return true;
}
