#include "interactionstep.h"
#include "afssapsintegrator.h"
#include "interactionsdatabasepage.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/itheme.h>
#include <coreplugin/constants_icons.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

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

static inline QString workingPath()         {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/Interactions/") + QDir::separator();}
static inline QString databaseAbsPath()  {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::MASTER_DATABASE_FILENAME);}

static inline QString translationsCorrectionsFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::INTERACTIONS_ENGLISHCORRECTIONS_FILENAME);}
static inline QString afssapsIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString atcCsvFile()          {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::ATC_FILENAME);}


InteractionStep::InteractionStep(QObject *parent) :
        m_UseProgressDialog(false), m_ActiveDownloadId(-1), m_Downloader(0)
{
}

bool InteractionStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        Utils::Log::addError(this, "Unable to create interactions Working Path :" + workingPath(), __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath))
            Utils::Log::addError(this, "Unable to create interactions database output path :" + dbpath, __FILE__, __LINE__);
        else
            Utils::Log::addMessage(this, "Drugs database output dir created");
    }
    return true;
}

bool InteractionStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool InteractionStep::downloadFiles()
{
    Q_EMIT downloadFinished();
    return true;
}

struct Source
{
    Source() {}
    bool isNull() {return (m_TreeClass.isEmpty() && m_Inn.isEmpty() && m_Link.isEmpty());}

    QString m_TreeClass, m_Inn, m_Link, m_TypeOfLink, m_Abstract, m_TextualReference, m_Explanation;
};


static bool setClassTreeToDatabase(const QString &iclass,
                                   const QMultiHash<QString, QString> &class_mols,
                                   const QMultiHash<QString, QString> &molsToAtc,
                                   const QStringList &afssapsClass,
                                   const QStringList &molsWithoutAtc,
                                   const QMultiHash<QString, Source> &class_sources,
                                   QMultiHash<QString, QString> *buggyIncludes,
                                   int insertChildrenIntoClassId = -1)
{
    const QStringList &associatedInns = molsToAtc.uniqueKeys();

    if (insertChildrenIntoClassId == -1) {
        insertChildrenIntoClassId = afssapsClass.indexOf(iclass)+200000;
    }

    // Take all included inns
    QString req;
    foreach(const QString &inn, class_mols.values(iclass)) {
        req.clear();
        if (afssapsClass.contains(inn)) {
            // Avoid inclusion of self class
            if (iclass==inn) {
                qWarning() << "error: CLASS==INN"<< iclass << inn;
                continue;
            }
            qWarning() << "class within a class" << iclass << inn;
            setClassTreeToDatabase(inn, class_mols, molsToAtc, afssapsClass, molsWithoutAtc, class_sources, buggyIncludes, insertChildrenIntoClassId);
            qWarning() << "end class within a class";
            continue;
        }

        // Find source for this couple class/inn
        QVector<Source> sources;
        foreach(const Source &s, class_sources.values(iclass)) {
            if (s.m_Inn==inn.toUpper()) {
                sources << s;
            }
        }

        int bibMasterId = -1;
        QVector<int> bib_ids;
        if (sources.count()) {
            // Insert all sources
            foreach(const Source &s, sources) {
                int lastId = Core::Tools::addBibliography(Core::Constants::MASTER_DATABASE_NAME, s.m_TypeOfLink, s.m_Link, "","");
                if (lastId==-1)
                    return false;
                bib_ids << lastId;
            }
        }
        QSqlDatabase db = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
        db.transaction();
        QSqlQuery query(db);
        req = QString("SELECT MAX(BIB_MASTER_ID) FROM BIBLIOGRAPHY_LINKS");
        if (query.exec(req)) {
            if (query.next()) {
                bibMasterId = query.value(0).toInt() + 1;
            }
        } else {
            Utils::Log::addQueryError("InteractionStep", query, __FILE__, __LINE__);
            db.rollback();
            return false;
        }
        query.finish();
        if (bibMasterId==-1) {
            Utils::Log::addError("InteractionStep", "NO BIB MASTER", __FILE__, __LINE__);
            db.rollback();
            return false;
        }

        foreach(int id, bib_ids) {
            req = QString("INSERT INTO BIBLIOGRAPHY_LINKS (BIB_MASTER_ID, BIB_ID) VALUES (%1,%2)")
                  .arg(bibMasterId)
                  .arg(id);
            if (!query.exec(req)) {
                Utils::Log::addQueryError("InteractionStep", query, __FILE__, __LINE__);
                db.rollback();
                return false;
            }
            query.finish();
        }

        // Insert IAM Tree + Bib link
        if (associatedInns.contains(inn, Qt::CaseInsensitive)) {
            foreach(const QString &atc, molsToAtc.values(inn.toUpper())) {
                req = QString("INSERT INTO IAM_TREE (ID_CLASS, ID_ATC, BIB_MASTER_ID) VALUES "
                              "(%1, (SELECT ATC_ID FROM ATC WHERE CODE=\"%2\"), %3);")
                        .arg(insertChildrenIntoClassId)
                        .arg(atc)
                        .arg(bibMasterId);
                if (!query.exec(req)) {
                    Utils::Log::addQueryError("InteractionStep", query, __FILE__, __LINE__);
                    db.rollback();
                    return false;
                }
                query.finish();
            }
        } else {
            int id = molsWithoutAtc.indexOf(inn.toUpper());
            if (id==-1) {
                QString i__ = inn;
                req = QString("INSERT INTO IAM_TREE (ID_CLASS, ID_ATC, BIB_MASTER_ID) VALUES "
                              "(%1, ("
                              "SELECT ATC.ATC_ID "
                              "FROM ATC "
                              "JOIN ATC_LABELS ON ATC_LABELS.ATC_ID=ATC.ATC_ID "
                              "JOIN LABELS_LINK ON LABELS_LINK.MASTER_LID=ATC_LABELS.MASTER_LID "
                              "JOIN LABELS ON LABELS_LINK.LID=LABELS.LID "
                              "WHERE LABELS.LANG='fr' AND LABELS.LABEL='%2'), %3);")
                        .arg(insertChildrenIntoClassId)
                        .arg(i__.replace("'","''"))
                        .arg(bibMasterId);
            } else {
                req = QString("INSERT INTO IAM_TREE (ID_CLASS, ID_ATC, BIB_MASTER_ID) VALUES "
                              "(%1, (SELECT ATC_ID FROM ATC WHERE CODE=\"%2\"), %3);")
                        .arg(insertChildrenIntoClassId)
                        .arg("Z01AA" + QString::number(molsWithoutAtc.indexOf(inn.toUpper())+1).rightJustified(2, '0'))
                        .arg(bibMasterId);
            }

            if (!query.exec(req)) {
                buggyIncludes->insertMulti(iclass, inn);
                Utils::Log::addQueryError("InteractionStep", query, __FILE__, __LINE__);
                db.rollback();
                return false;
            }
            query.finish();
            db.commit();
        }
    }
    return true;
}

bool InteractionStep::process()
{
    return computeModelsAndPopulateDatabase();
}

bool InteractionStep::computeModelsAndPopulateDatabase()
{
    // connect db
    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    if (!Core::Tools::createMasterDrugInteractionDatabase())
        return false;

    QSqlDatabase iam = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);

    QProgressDialog *progress = 0;
    if (m_UseProgressDialog) {
        progress = new QProgressDialog("Creating interactions database","Abord", 0, 7, qApp->activeWindow());
        progress->setWindowModality(Qt::WindowModal);
        progress->setValue(0);
    }

    // refresh ATC table
    {
        // Clean ATC table from old values
        QString req;
        req = "DELETE FROM ATC";
        Core::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);
        req = "DELETE FROM ATC_LABELS";
        Core::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);
        req = "DELETE FROM IAM_TREE";
        Core::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);

        // Import ATC codes to database
        QFile file(atcCsvFile());
        if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : can not open file %1, %2.").arg(file.fileName(), file.errorString()), __FILE__, __LINE__);
        } else {
            QString content = QString::fromUtf8(file.readAll());
            if (content.isEmpty())
                return false;
            const QStringList &list = content.split("\n", QString::SkipEmptyParts);
            foreach(const QString &s, list) {
                QStringList values = s.split(";");
                QMultiHash<QString, QVariant> labels;
                labels.insert("en", values[1].remove("\""));
                labels.insert("fr", values[2].remove("\""));
                labels.insert("de", values[3].remove("\""));
                if (!Core::Tools::createAtc(Core::Constants::MASTER_DATABASE_NAME, values[0].remove("\""), labels)) {
                    return false;
                }
            }
        }
    }
    if (m_UseProgressDialog) {
        progress->setValue(1);
    }

    // add FreeDiams ATC specific codes
    iam.transaction();
    QStringList molsWithoutAtc;
    QStringList afssapsClass, afssapsClassEn;
    QMultiHash<QString, QString> molsToAtc;
    QMultiHash<QString, QString> class_mols;
    QString req;

    {
        // getting datas from models
        AfssapsLinkerModel *molLinkModel = AfssapsLinkerModel::instance();
        while (molLinkModel->canFetchMore(QModelIndex()))
            molLinkModel->fetchMore(QModelIndex());
        int nb = molLinkModel->rowCount();
        for(int i = 0; i < nb; ++i) {
            const QString &mol = molLinkModel->index(i, AfssapsLinkerModel::AfssapsName).data().toString();
            const QString &molEn = molLinkModel->index(i, AfssapsLinkerModel::En_Label).data().toString();
            const QString &links = molLinkModel->index(i, AfssapsLinkerModel::AtcCodes).data().toString();
            const QString &type = molLinkModel->index(i, AfssapsLinkerModel::AffapsCategory).data().toString();
            if (mol.isEmpty())
                continue;
            if (type=="class") {
                 afssapsClass << Core::Tools::noAccent(mol).toUpper();
                 afssapsClassEn << molEn;
             } else if (links.isEmpty()) {
                 molsWithoutAtc << mol.toUpper();
             } else {
                 foreach(const QString &atcCode, links.split(",", QString::SkipEmptyParts))
                     molsToAtc.insertMulti(mol.toUpper(), atcCode.toUpper());
             }
         }
         if (m_UseProgressDialog) {
             progress->setValue(2);
         }

        // Add Interacting molecules without ATC code
        // 100 000 < ID < 199 999  == Interacting molecules without ATC code
        for (int i=0; i < molsWithoutAtc.count(); i++) {
            QString n = QString::number(i+1);
            if (i<9)
                n.prepend("0");
            QMultiHash<QString, QVariant> labels;
            labels.insert("fr", molsWithoutAtc.at(i));
            labels.insert("en", molsWithoutAtc.at(i));
            labels.insert("de", molsWithoutAtc.at(i));
            if (!Core::Tools::createAtc(Core::Constants::MASTER_DATABASE_NAME, "Z01AA" + n, labels, i+100000))
                return false;
        }
        if (m_UseProgressDialog) {
            progress->setValue(3);
        }

        // Add classes
        // 200 000 < ID < 299 999  == Interactings classes
        for (int i=0; i < afssapsClass.count(); i++) {
            QString n = QString::number(i+1);
            n = n.rightJustified(4, '0');
            QMultiHash<QString, QVariant> labels;
            labels.insert("fr", afssapsClass.at(i));
            labels.insert("en", afssapsClassEn.at(i));
            labels.insert("de", afssapsClassEn.at(i));
            if (!Core::Tools::createAtc(Core::Constants::MASTER_DATABASE_NAME, "ZXX" + n, labels, i+200000))
                return false;
        }
        if (m_UseProgressDialog) {
            progress->setValue(4);
        }

        // Warn AFSSAPS molecules with multiples ATC
        //        if (WarnTests) {
        //            foreach(const QString &inn, innToAtc.uniqueKeys()) {
        //                const QStringList &atc = innToAtc.values(inn);
        //                if (atc.count() <= 1)
        //                    continue;
        //                qWarning() << inn << atc;
        //            }
        //        }
    }


    QMultiHash<QString, Source> class_sources;
    // Add interacting classes tree
    {
        // Prepare computation
        req = "DELETE FROM IAM_TREE;";
        Core::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);

        // retreive AFSSAPS class tree from model
        AfssapsClassTreeModel *afssapsTreeModel = AfssapsClassTreeModel::instance();
        while (afssapsTreeModel->canFetchMore(QModelIndex()))
            afssapsTreeModel->fetchMore(QModelIndex());
        int nb = afssapsTreeModel->rowCount();
        for(int i = 0; i < nb; ++i) {
            int j = 0;
            // Get class name
            QModelIndex parent = afssapsTreeModel->index(i, AfssapsClassTreeModel::Name);
            // Get mols
            while (afssapsTreeModel->hasIndex(j, 0, parent)) {
                QModelIndex molIndex = afssapsTreeModel->index(j, AfssapsClassTreeModel::Name, parent);
                const QString &mol = molIndex.data().toString();
                class_mols.insertMulti(parent.data().toString(), mol);
                ++j;
                // catch source
                if (afssapsTreeModel->hasIndex(0, 0, molIndex)) {
                    for(int k = 0; k < afssapsTreeModel->rowCount(molIndex); ++k) {
                        Source s;
                        s.m_Link = afssapsTreeModel->index(k, AfssapsClassTreeModel::Name, molIndex).data().toString();
                        s.m_TreeClass = Core::Tools::noAccent(parent.data().toString()).toUpper();
                        s.m_Inn = Core::Tools::noAccent(mol).toUpper();
                        if (s.m_Link.startsWith("http://www.ncbi.nlm.nih.gov/pubmed/")) {
                            s.m_TypeOfLink = "pubmed";
                        } else {
                            s.m_TypeOfLink = "web";
                        }
                        class_sources.insertMulti(s.m_TreeClass, s);
                    }
                }
            }
        }
        if (m_UseProgressDialog) {
            progress->setValue(5);
        }

        qWarning() << "sources=" << class_sources.count() << class_sources.uniqueKeys().count();

        // Computation
        QMultiHash<QString, QString> buggyIncludes;
        foreach(const QString &iclass, afssapsClass) {
            setClassTreeToDatabase(iclass, class_mols, molsToAtc, afssapsClass, molsWithoutAtc, class_sources, &buggyIncludes);
        }
        if (m_UseProgressDialog) {
            progress->setValue(6);
        }
        afssapsTreeModel->addBuggyInclusions(buggyIncludes);
    }


    // Add interaction knowledges
    {
        // Prepare computation
        req = "DELETE FROM INTERACTIONS";
        Core::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);
        req = "DELETE FROM IAKNOWLEDGE";
        Core::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);
        req = "DELETE FROM IA_IAK";
        Core::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);

        InteractionModel *interactionModel = InteractionModel::instance();
        int nb = interactionModel->rowCount();
        int interactionId = 0;

        for(int i = 0; i < nb; ++i) {
            // get interactions row by row
            int j = 1;
            QModelIndex parent = interactionModel->index(i, InteractionModel::Name);
            // get all ATC related to interactors
            const QString &main = parent.data().toString();
            QStringList mainCodes;
            if (!molsToAtc.uniqueKeys().contains(main)) {
                if (!afssapsClass.contains(main)) {
                    if (!molsWithoutAtc.contains(main)) {
                        Utils::Log::addError(this, tr("Main Interactor not found: %1").arg(main), __FILE__, __LINE__);
                    } else {
                        int id = molsWithoutAtc.indexOf(main);
                        QString n = QString::number(id+1);
                        if (id<9)
                            n.prepend("0");
                        mainCodes.append("Z01AA" + n);
                    }
                } else {
                    QString n = QString::number(afssapsClass.indexOf(main)+1);
                    n = n.rightJustified(4, '0');
                    mainCodes.append("ZXX"+n);
                }
            } else {
                mainCodes.append(molsToAtc.values(main));
            }

            //            qWarning() << "MAIN" << parent.data().toString();

            while (interactionModel->hasIndex(j, 0, parent)) {
                QModelIndex childItem = interactionModel->index(j, InteractionModel::Name, parent);
                const QString &child = childItem.data().toString();
                //                qWarning() << "      " << child;

                QStringList secondCodes;
                if (!molsToAtc.uniqueKeys().contains(child)) {
                    if (!afssapsClass.contains(child)) {
                        if (!molsWithoutAtc.contains(child)) {
                            Utils::Log::addError(this, tr("Child Interactor not found: %1").arg(child), __FILE__, __LINE__);
                        } else {
                            int id = molsWithoutAtc.indexOf(child);
                            QString n = QString::number(id+1);
                            if (id<9)
                                n.prepend("0");
                            secondCodes.append("Z01AA" + n);
                        }
                    } else {
                        QString n = QString::number(afssapsClass.indexOf(child)+1);
                        n = n.rightJustified(4, '0');
                        secondCodes.append("ZXX"+n);
                    }
                } else {
                    secondCodes.append(molsToAtc.values(child));
                }

                // Add IA to database
                QMultiHash<QString, QVariant> risk;
                QMultiHash<QString, QVariant> man;
                risk.insert("fr", interactionModel->getRisk(childItem, "fr"));
                risk.insert("en", interactionModel->getRisk(childItem, "en"));
                man.insert("fr", interactionModel->getManagement(childItem, "fr"));
                man.insert("en", interactionModel->getManagement(childItem, "en"));
                Core::Tools::addInteraction(Core::Constants::MASTER_DATABASE_NAME, mainCodes, secondCodes, interactionModel->getLevel(childItem, "fr"), risk, man);

                ++j;
                ++interactionId;
            }
        }

        if (m_UseProgressDialog) {
            progress->setValue(7);
        }

    }

    iam.commit();

    // refresh the innToAtc content (reload ATC codes because we added some new ATC)


    // drugs databases needs to be relinked with the new ATC codes

    if (progress) {
        delete progress;
        progress=0;
    }

    Q_EMIT processFinished();

    return true;
}

void InteractionStep::downloadNextSource()
{
    qWarning() << "download" << m_ActiveDownloadId;
    if (!m_Downloader) {
        m_Downloader = new Utils::PubMedDownloader;
        connect(m_Downloader, SIGNAL(downloadFinished()), this, SLOT(downloadNextSource()));
    }

    // connect db
    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return ;

    // If first call --> get all sources to download
    if (m_ActiveDownloadId == -1) {
        m_SourceToDownload.clear();
        QString req = "SELECT `ID` FROM `SOURCES` WHERE (`TEXTUAL_REFERENCE` IS NULL AND `ABSTRACT` IS NULL);";
        QSqlQuery query(QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME));
        if (query.exec(req)) {
            while (query.next()) {
                m_SourceToDownload << query.value(0).toInt();
            }
        }
        if (m_SourceToDownload.isEmpty()) {
            delete m_Downloader;
            m_Downloader = 0;
            Q_EMIT postProcessDownloadFinished();
            return;
        }
        m_ActiveDownloadId = m_SourceToDownload.first();
    } else {
        // Source retrieved
        QString req = QString("UPDATE `SOURCES` SET "
                              "`TEXTUAL_REFERENCE`=\"%1\", "
                              "`ABSTRACT`=\"%2\" "
                              "WHERE `ID`=%3;")
                .arg(m_Downloader->reference().replace("\"","'"))
                .arg(m_Downloader->abstract().replace("\"","'"))
                .arg(m_ActiveDownloadId)
                ;
        Core::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);

        if (m_SourceToDownload.contains(m_ActiveDownloadId))
            m_SourceToDownload.remove(0);

        if (m_SourceToDownload.count() == 0) {
            delete m_Downloader;
            m_Downloader = 0;
            Q_EMIT postProcessDownloadFinished();
            return;
        }

        m_ActiveDownloadId = m_SourceToDownload.first();
    }

    if (m_ActiveDownloadId == -1)
        return;

    // Get link
    QString link;
    QString req = QString("SELECT `LINK` FROM `SOURCES` WHERE (`ID`=%1 AND `LINK` NOT NULL) LIMIT 1;").arg(m_ActiveDownloadId);
    QSqlQuery query(QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME));
    if (query.exec(req)) {
        if (query.next()) {
            link = query.value(0).toString();
        }
    }

    if (link.isEmpty()) {
        delete m_Downloader;
        m_Downloader = 0;
        Q_EMIT postProcessDownloadFinished();
        return;
    }

    query.finish();

    // Start pubmed downloader
    if (m_Downloader->setFullLink(link)) {
        m_Downloader->startDownload();
    } else {
        Utils::Log::addError(this, "Unable to download pubmed link " + link);
    }
}
