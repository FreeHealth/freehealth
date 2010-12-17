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
static inline QString iamDatabaseAbsPath()  {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::IAM_DATABASE_FILENAME);}
static inline QString iamDatabaseSqlSchema() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::FILE_IAM_DATABASE_SCHEME);}

static inline QString translationsCorrectionsFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::INTERACTIONS_ENGLISHCORRECTIONS_FILENAME);}
static inline QString afssapsIamXmlFile()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::AFSSAPS_INTERACTIONS_FILENAME);}
static inline QString atcCsvFile()          {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + Core::Constants::ATC_FILENAME);}


InteractionStep::InteractionStep(QObject *parent) :
        m_UseProgressDialog(false)
{
}

bool InteractionStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        Utils::Log::addError(this, "Unable to create interactions Working Path :" + workingPath(), __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(iamDatabaseAbsPath()).absolutePath();
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
    QFile(iamDatabaseAbsPath()).remove();
    return true;
}

bool InteractionStep::downloadFiles()
{
    Q_EMIT downloadFinished();
    return true;
}

static void setClassTreeToDatabase(const QString &iclass,
                                   const QMultiHash<QString, QString> &class_mols,
                                   const QMultiHash<QString, QString> &molsToAtc,
                                   const QStringList &afssapsClass,
                                   const QStringList &molsWithoutAtc,
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
            setClassTreeToDatabase(inn, class_mols, molsToAtc, afssapsClass, molsWithoutAtc, buggyIncludes, insertChildrenIntoClassId);
            qWarning() << "end class within a class";
            continue;
        }
        if (associatedInns.contains(inn, Qt::CaseInsensitive)) {
            foreach(const QString &atc, molsToAtc.values(inn.toUpper())) {
                req = QString("INSERT INTO `IAM_TREE` (`ID_CLASS`, `ID_ATC`) VALUES "
                              "(%1, (SELECT `ID` FROM `ATC` WHERE `CODE`=\"%2\"));")
                        .arg(insertChildrenIntoClassId)
                        .arg(atc);
                Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);
            }
        } else {
            int id = molsWithoutAtc.indexOf(inn.toUpper());
            if (id==-1) {
                req = QString("INSERT INTO `IAM_TREE` (`ID_CLASS`, `ID_ATC`) VALUES "
                              "(%1, (SELECT `ID` FROM `ATC` WHERE `FRENCH` like \"%2\"));")
                        .arg(insertChildrenIntoClassId)
                        .arg(inn);
            } else {
                req = QString("INSERT INTO `IAM_TREE` (`ID_CLASS`, `ID_ATC`) VALUES "
                              "(%1, (SELECT `ID` FROM `ATC` WHERE `CODE`=\"%2\"));")
                        .arg(insertChildrenIntoClassId)
                        .arg("Z01AA" + QString::number(molsWithoutAtc.indexOf(inn.toUpper())+1).rightJustified(2, '0'));
            }

            if (!Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__)) {
                buggyIncludes->insertMulti(iclass, inn);
            }
        }
    }
}

bool InteractionStep::process()
{
    // create db
    if (!QFile(iamDatabaseAbsPath()).exists()) {
        QSqlDatabase iam = QSqlDatabase::addDatabase("QSQLITE", Core::Constants::IAM_DATABASE_NAME);
        iam.setDatabaseName(iamDatabaseAbsPath());
    }

    // connect db
    if (!Core::Tools::connectDatabase(Core::Constants::IAM_DATABASE_NAME, iamDatabaseAbsPath()))
        return false;

    QSqlDatabase iam = QSqlDatabase::database(Core::Constants::IAM_DATABASE_NAME);

    QProgressDialog *progress = 0;
    if (m_UseProgressDialog) {
        progress = new QProgressDialog("Creating interactions database","Abord", 0, 8,qApp->activeWindow());
        progress->setWindowModality(Qt::WindowModal);
        progress->setValue(0);
    }

    // Create database schema
    if (!Core::Tools::executeSqlFile(Core::Constants::IAM_DATABASE_NAME, iamDatabaseSqlSchema())) {
        Utils::Log::addError(this, "Can not create IAM database.", __FILE__, __LINE__);
        return false;
    }
    if (m_UseProgressDialog) {
        progress->setValue(1);
    }

    // refresh ATC table
    {
        // Start transaction
        iam.transaction();

        // Clean ATC table from old values
        QString req;
        req = "DELETE FROM `ATC`"
              "WHERE ID>=0";
        Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);

        // Import ATC codes to database
        QFile file(atcCsvFile());
        if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : can not open file %1, %2.").arg(file.fileName(), file.errorString()), __FILE__, __LINE__);
        } else {
            QString content = QString::fromUtf8(file.readAll());
            int i = 0;
            const QStringList &list = content.split("\n", QString::SkipEmptyParts);
            foreach(const QString &s, list) {
                req = QString("INSERT INTO `ATC`  (`ID`, `CODE`, `ENGLISH`, `FRENCH`, `DEUTSCH`) "
                              "VALUES (%1, %2) ").arg(i).arg(s);
                Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);
                ++i;
            }
        }
        // Commit transaction
        iam.commit();
    }
    if (m_UseProgressDialog) {
        progress->setValue(2);
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
            progress->setValue(3);
        }

        // Add Interacting molecules without ATC code
        // 100 000 < ID < 199 999  == Interacting molecules without ATC code
        for (int i=0; i < molsWithoutAtc.count(); i++) {
            QString n = QString::number(i+1);
            if (i<9)
                n.prepend("0");
            /** \todo add translated class names */
            req = QString("INSERT INTO `ATC` (`ID`, `CODE`, `FRENCH`, `ENGLISH`) VALUES "
                          "(%1, \"%2\", \"%3\", \"%4\");")
                    .arg(i+100000)
                    .arg("Z01AA" + n)
                    .arg(molsWithoutAtc.at(i))
                    .arg(molsWithoutAtc.at(i));
            Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);
        }
        if (m_UseProgressDialog) {
            progress->setValue(4);
        }

        // Add classes
        // 200 000 < ID < 299 999  == Interactings classes
        for (int i=0; i < afssapsClass.count(); i++) {
            QString n = QString::number(i+1);
            n = n.rightJustified(4, '0');
            req = QString("INSERT INTO `ATC` (`ID`, `CODE`, `FRENCH`, `ENGLISH`) VALUES "
                          "(%1, \"%2\", \"%3\", \"%4\");")
                    .arg(i+200000)
                    .arg("ZXX" + n)
                    .arg(afssapsClass.at(i))
                    .arg(afssapsClassEn.at(i));
            Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);
        }
        if (m_UseProgressDialog) {
            progress->setValue(5);
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


    // Add interacting classes tree
    {
        // Prepare computation
        req = "DELETE FROM IAM_TREE WHERE ID_CLASS >= 0";
        Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);

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
                const QString &mol = afssapsTreeModel->index(j, AfssapsClassTreeModel::Name, parent).data().toString();
                class_mols.insertMulti(parent.data().toString(), mol);
                ++j;
            }
        }
        if (m_UseProgressDialog) {
            progress->setValue(6);
        }

        // Computation
        QMultiHash<QString, QString> buggyIncludes;
        foreach(const QString &iclass, afssapsClass) {
            setClassTreeToDatabase(iclass, class_mols, molsToAtc, afssapsClass, molsWithoutAtc, &buggyIncludes);
        }
        if (m_UseProgressDialog) {
            progress->setValue(7);
        }
        afssapsTreeModel->addBuggyInclusions(buggyIncludes);
    }


    // Add interaction knowledges
    {
        // Prepare computation
        req = "DELETE FROM `INTERACTIONS` WHERE ID >= 0";
        Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);

        // Prepare computation
        req = "DELETE FROM `INTERACTION_KNOWLEDGE` WHERE ID >= 0";
        Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);

        InteractionModel *interactionModel = InteractionModel::instance();
        int nb = interactionModel->rowCount();
        int interactionId = 0;
        bool done = false;

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

                foreach(const QString &atc1, mainCodes) {
                    foreach(const QString &atc2, secondCodes) {
                        done = true;
                        req = QString("INSERT INTO `INTERACTIONS`  (`ATC_ID1`,`ATC_ID2`, `INTERACTION_KNOWLEDGE_ID`) VALUES (\n"
                                      "%2, \n%3, \n%4);")
                                .arg(QString("(SELECT `ID` FROM `ATC` WHERE `CODE`=\"%1\")").arg(atc1))
                                .arg(QString("(SELECT `ID` FROM `ATC` WHERE `CODE`=\"%1\")").arg(atc2))
                                .arg(interactionId);
                        Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME, __FILE__, __LINE__);
                    }
                }
                // Check errors
                if (!done)
                    Utils::Log::addError(this, QString("Interaction not added : %1   //  %2").arg(main).arg(child), __FILE__, __LINE__);
                done = false;

                req = QString("INSERT INTO `INTERACTION_KNOWLEDGE`  "
                              "(`ID`,`TYPE`,`RISK_FR`,`MANAGEMENT_FR`, `RISK_EN`, `MANAGEMENT_EN`, `REFERENCES_LINK`) "
                              "VALUES \n"
                              "(%1, \"%2\", \"%3\", \"%4\", \"%5\", \"%6\", \"http://tinyurl.com/24kn96t\")")
                        .arg(interactionId)
                        .arg(interactionModel->getLevel(childItem, "fr"))
                        .arg(interactionModel->getRisk(childItem, "fr"))
                        .arg(interactionModel->getManagement(childItem, "fr"))
                        .arg(interactionModel->getRisk(childItem, "en"))
                        .arg(interactionModel->getManagement(childItem, "en"));
                Core::Tools::executeSqlQuery(req, Core::Constants::IAM_DATABASE_NAME);

                ++j;
                ++interactionId;
            }
        }

        if (m_UseProgressDialog) {
            progress->setValue(8);
        }

    }

    iam.commit();

    // refresh the innToAtc content (reload ATC codes because we added some new ATC)


    // drugs databases needs to be relinked with the new ATC codes

    if (!Core::Tools::signDatabase(Core::Constants::IAM_DATABASE_NAME))
        Utils::Log::addError(this, "Unable to tag database.", __FILE__, __LINE__);

    if (progress) {
        delete progress;
        progress=0;
    }

    return true;
}


