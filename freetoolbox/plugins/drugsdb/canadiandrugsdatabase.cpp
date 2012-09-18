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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "canadiandrugsdatabase.h"
#include "extramoleculelinkermodel.h"
#include "drug.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/log.h>
#include <utils/database.h>
#include <utils/httpdownloader.h>
#include <extensionsystem/pluginmanager.h>
#include <quazip/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>

#include <QDir>
#include <QProgressDialog>
#include <QTimer>

#include "ui_canadiandrugsdatabasewidget.h"

using namespace DrugsDbCreator;
using namespace Trans::ConstantTranslations;

const char* const  CANADIAN_URL               = "http://www.hc-sc.gc.ca/dhp-mps/alt_formats/zip/prodpharma/databasdon/allfiles.zip";
//const char* const  CANADIAN_URL               = "http://www.hc-sc.gc.ca/dhp-mps/prodpharma/databasdon/txt/allfiles.zip";
const char* const  CA_DRUGS_DATABASE_NAME     = "CA_HCDPD";

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/CanadianRawSources")  + QDir::separator();}
static inline QString databaseAbsPath()  {return Core::Tools::drugsDatabaseAbsFileName();}

static inline QString databaseDescriptionFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/drugdb/ca/description.xml");}
static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/drugdb/ca/canadian_db_finalize.sql");}

CanadianDrugsDatabasePage::CanadianDrugsDatabasePage(QObject *parent) :
        Core::IToolPage(parent)
{
    setObjectName("CanadianDrugsDatabasePage");
}

QString CanadianDrugsDatabasePage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_DRUGSDATABASE;
}

QWidget *CanadianDrugsDatabasePage::createPage(QWidget *parent)
{
    return new CanadianDrugsDatabaseWidget(parent);
}


CaDrugDatatabaseStep::CaDrugDatatabaseStep(QObject *parent) :
        m_WithProgress(false)
{
    setObjectName("CaDrugDatatabaseStep");
}

CaDrugDatatabaseStep::~CaDrugDatatabaseStep()
{
}

bool CaDrugDatatabaseStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        LOG_ERROR("Unable to create Ca Working Path :" + workingPath());
    else
        LOG("Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath)) {
            LOG_ERROR("Unable to create Ca database output path :" + dbpath);
            m_Errors << tr("Unable to create Ca database output path :") + dbpath;
        } else {
            LOG("Drugs database output dir created");
        }
    }
    return true;
}

bool CaDrugDatatabaseStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool CaDrugDatatabaseStep::downloadFiles(QProgressBar *bar)
{
    Utils::HttpDownloader *dld = new Utils::HttpDownloader(this);
//    dld->setMainWindow(mainwindow());
    dld->setProgressBar(bar);
    dld->setOutputPath(workingPath());
    dld->setUrl(QUrl(CANADIAN_URL));
    dld->startDownload();
    connect(dld, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()));
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    return true;
}

bool CaDrugDatatabaseStep::process()
{
    unzipFiles();
    prepareDatas();
    createDatabase();
    populateDatabase();
    linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool CaDrugDatatabaseStep::unzipFiles()
{
    Q_EMIT progressLabelChanged(tr("Canadian database creation: unzipping files..."));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // check file
    QString fileName = workingPath() + QDir::separator() + QFileInfo(CANADIAN_URL).fileName();
    if (!QFile(fileName).exists()) {
        LOG_ERROR(QString("No files founded."));
        LOG_ERROR(QString("Please download files."));
        return false;
    }

    LOG(QString("Starting unzipping Canadian file %1").arg(fileName));

    // unzip downloaded using QProcess
    if (!QuaZipTools::unzipFile(fileName, workingPath()))
        return false;

    // unzip all files in the working path
    QFileInfoList list = QDir(workingPath()).entryInfoList(QStringList()<<"*.zip");

    Q_EMIT progressRangeChanged(0, list.count());
    int progr = 0;
    foreach(const QFileInfo &info, list) {
        Q_EMIT progress(progr);
        ++progr;
        if (info.fileName()!="allfiles.zip") {
            if (!QuaZipTools::unzipFile(info.absoluteFilePath(), workingPath())) {
                LOG_ERROR("Unable to unzip " + info.absoluteFilePath());
                return false;
            }
        }
    }
    return true;
}

bool CaDrugDatatabaseStep::prepareDatas()
{
    return true;
}

bool CaDrugDatatabaseStep::createDatabase()
{
    if (!Core::Tools::createMasterDrugInteractionDatabase())
        return false;

    QMultiHash<QString, QVariant> labels;
    labels.insert("fr","Base de données thérapeutique Canadienne");
    labels.insert("en","Canadian therapeutic database");
    labels.insert("de","Kanadierin therapeutischen database");

    if (Core::Tools::createNewDrugsSource(Core::Constants::MASTER_DATABASE_NAME, CA_DRUGS_DATABASE_NAME, labels) == -1) {
        LOG_ERROR("Unable to create the Canadian drugs sources");
        return false;
    }
    Core::Tools::saveDrugDatabaseDescription(databaseDescriptionFile(), 0);
    LOG(QString("Database schema created"));
    return true;
}

QMultiHash<int, QString> CaDrugDatatabaseStep::extractUidRelatedDatas(const QString &absFileName, const int uidCol, const int dataCol, const QHash<int, QString> &equalityCondition)
{
    // Get forms
    QMultiHash<int, QString> uid_vals;
    QFile csv(absFileName);
    if (!csv.open(QFile::ReadOnly | QFile::Text)) {
        LOG_ERROR("Unable to read file");
        return uid_vals;
    }

    Q_EMIT progressRangeChanged(0, csv.size());
    Q_EMIT progress(0);

    QTextStream in(&csv);
    in.setCodec("ISO 8859-1");
    int pos = 0;
    bool useCondition = equalityCondition.count() >= 1;
    while (!in.atEnd()) {
        QString l = in.readLine();
        pos += l.length();
        QStringList vals = l.split(",");

        if (useCondition) {
            bool goOn = true;
            foreach(const int col, equalityCondition.keys()) {
                QString value = vals[col].remove("\"");
                if (value.compare(equalityCondition.value(col), Qt::CaseInsensitive) != 0) {
                    goOn = false;
                    break;
                }
            }
            if (!goOn)
                continue;
        }

        QString uid = vals[uidCol].remove("\"");
        QString v = vals[dataCol].remove("\"");
        uid_vals.insertMulti(uid.toInt(), v);

        if (uid_vals.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }
    }
    csv.close();
    return uid_vals;
}

bool CaDrugDatatabaseStep::populateDatabase()
{
    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    // import files
    QHash<int, QString> condition;

    // Get routes
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (routes)"));
    QMultiHash<int, QString> uid_routes = extractUidRelatedDatas(workingPath() + "route.txt", 0, 2, condition);

    // Get ther
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (ther)"));
    QMultiHash<int, QString> uid_ther = extractUidRelatedDatas(workingPath() + "ther.txt", 0, 1, condition);

    // Get forms
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (forms)"));
    QMultiHash<int, QString> uid_forms = extractUidRelatedDatas(workingPath() + "form.txt", 0, 2, condition);

    // Get status
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (status)"));
    condition.insert(1, "Y");
    QMultiHash<int, QString> uid_stats = extractUidRelatedDatas(workingPath() + "status.txt", 0, 2, condition);

    // Get drug names
    QHash<int, Drug *> uid_drugs;
    QMultiHash<int, Component *> uid_compos;

    QFile csv(workingPath() + "drug.txt");
    if (!csv.open(QFile::ReadOnly | QFile::Text)) {
        LOG_ERROR("Unable to read file");
        return false;
    }

    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (drugs)"));
    Q_EMIT progressRangeChanged(0, csv.size());
    Q_EMIT progress(0);

    QTextStream in(&csv);
    in.setCodec("ISO 8859-1");
    int pos = 0;
    while (!in.atEnd()) {
        QString l = in.readLine();
        pos += l.length();
        QStringList vals;
        foreach(QString val, l.split(",")) {
            vals << val.remove("\"");
        }
        if (vals.count() < 11) {
            qWarning() << vals << l;
            continue;
        }

        if (vals.at(2).compare("Veterinary") == 0)
            continue;

        // "82621","CAT IV","Human","02340631",(4)"DO ... SPF 15",(5)"",(6)"N",(7)"",(8)"4",(9)"14-JUN-2010",(10)"0423152042"
        Drug *drug = new Drug;
        int drugCode = vals.at(0).toInt();
        drug->setData(Drug::Uid1, vals.at(3));
        drug->setData(Drug::OldUid, vals.at(3)); // Since v0.5.4 Ca db uid is the DIN, before is was ProductN;FormId;RouteId
        drug->setData(Drug::Name, vals.at(4));
        QStringList t = uid_forms.values(drugCode);
        drug->setData(Drug::Forms, t);
        t.clear();
        t = uid_routes.values(drugCode);
        drug->setData(Drug::Routes, t);
        t = uid_stats.values(drugCode);
        if (t.count() == 0) {
            drug->setData(Drug::Marketed, 0);
        } else {
            if (t.contains("MARKETED", Qt::CaseInsensitive))
                drug->setData(Drug::Marketed, 1);
            else
                drug->setData(Drug::Marketed, 0);
        }
        drug->setData(Drug::Valid, 1);
        drug->setData(Drug::Strength, 1);
        t.clear();
        t = uid_ther.values(drugCode);
        drug->setData(Drug::AtcCode, t.join(","));

        uid_drugs.insert(drugCode, drug);

        if (uid_drugs.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }
    }
    csv.close();

    // Get components
    csv.setFileName(workingPath() + "ingred.txt");
    if (!csv.open(QFile::ReadOnly | QFile::Text)) {
        LOG_ERROR("Unable to read file");
        return false;
    }

    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (ingred)"));
    Q_EMIT progressRangeChanged(0, csv.size());
    Q_EMIT progress(0);

    in.setDevice(&csv);
    in.setCodec("ISO 8859-1");
    pos = 0;
    while (!in.atEnd()) {
        QString l = in.readLine();
        pos += l.length();
        QStringList vals;
        foreach(QString val, l.split("\",\"")) {
            vals << val.remove("\"");
        }

        // (0)"9518",(1)"3182",(2)"OXYQUINOLINE",(3)"I",
        // (4)"0.5",(5)"%",(6)"",(7)"",(8)"N",
        // (9)"",(10)"8-HYDROXYQUINOLINE 0.4% CHG TO 0.5% AS PER VDD 2006.04.05"
        //        "DRUG_CODE" INTEGER(8) NOT NULL ,
        //        "ACTIVE_INGREDIENT_CODE" INTEGER(6),
        //        "INGREDIENT" VARCHAR2(240),
        //        "INGREDIENT_SUPPLIED_IND" VARCHAR2(1),
        //        "STRENGTH" VARCHAR2(20),
        //        "STRENGTH_UNIT" VARCHAR2(40),
        //        "STRENGTH_TYPE" VARCHAR2(40),
        //        "DOSAGE_VALUE" VARCHAR2(20),
        //        "BASE" VARCHAR2(1),
        //        "DOSAGE_UNIT" VARCHAR2(40),
        //        "NOTES" VARCHAR2(2000)

        QString strength = vals.at(4) + vals.at(5);
        if (!vals.at(7).isEmpty() || !vals.at(9).isEmpty()) {
            strength += "/" + vals.at(7) +vals.at(9);
        }

        Component *compo = new Component;
        int drugCode = vals.at(0).toInt();
        compo->setData(Component::Name, vals.at(2));
        compo->setData(Component::Strength, strength);
        compo->setData(Component::Nature, "SA");

        uid_compos.insert(drugCode, compo);

        if (uid_compos.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }
    }
    csv.close();

    Q_EMIT progressLabelChanged(tr("Reading drugs raw source : processing drugs/components"));
    Q_EMIT progressRangeChanged(0, uid_drugs.count());
    Q_EMIT progress(0);

    QVector<Drug *> drugsVector;
    pos = 0;
    foreach(const int uid, uid_drugs.keys()) {
        Drug *drug = uid_drugs[uid];
        QStringList strength;
        int i = 0;
        foreach(Component *c, uid_compos.values(uid)) {
            ++i;
            c->setData(Component::NatureLink, i);
            drug->addComponent(c);
            strength.append(c->data(Component::Strength).toString() + c->data(Component::StrengthUnit).toString());
        }
        drug->setData(Drug::Strength, strength.join(";"));
        drugsVector << drug;
        ++ pos;
        if (pos % 10 == 0)
            Q_EMIT progress(pos);
    }

    Q_EMIT progressLabelChanged(tr("Saving drugs into database"));
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(1);
    Drug::saveDrugsIntoDatabase(Core::Constants::MASTER_DATABASE_NAME, drugsVector, CA_DRUGS_DATABASE_NAME);
    Q_EMIT progress(2);

    Core::Tools::saveDrugDatabaseDescription(databaseDescriptionFile(), 50);

//    // Run SQL commands one by one
//    Q_EMIT progressLabelChanged(tr("Running database finalization script"));
//    if (!Core::Tools::executeSqlFile(Core::Constants::MASTER_DATABASE_NAME, databaseFinalizationScript())) {
//        LOG_ERROR("Can create Canadian DB.");
//        return false;
//    }
    Q_EMIT progress(3);

    // delete pointers
    qDeleteAll(drugsVector);
    uid_drugs.clear();
    uid_compos.clear();
    drugsVector.clear();
    return true;
}

bool CaDrugDatatabaseStep::linkMolecules()
{
    // 18 Oct 2011
    //    NUMBER OF MOLECULES 1845
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 1364 "
    //    LINKERMODEL (WithATC:281;WithoutATC:73) 354"
    //    LINKERNATURE 0
    //    LEFT 408
    //    CONFIDENCE INDICE 77

    // 29 Sept 2011 (ATC 2011 && ATC 2012 + new link in model)
    //    NUMBER OF MOLECULES 1842
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 1308 "
    //    LINKERMODEL (WithATC:215;WithoutATC:72) 287"
    //    LINKERNATURE 0
    //    LEFT 462
    //    CONFIDENCE INDICE 74

    // 29 APR 2011
    //    NUMBER OF MOLECULES 1848
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 1219 "
    //    LINKERMODEL (WithATC:81;WithoutATC:75) 156"
    //    LINKERNATURE 0
    //    LEFT 554
    //    CONFIDENCE INDICE 70

    // 15 Jan 2011
    //    NUMBER OF MOLECULES 1825
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 1213 "
    //    LINKERMODEL (WithATC:87;WithoutATC:75) 162"
    //    LINKERNATURE 0
    //    LEFT 537
    //    CONFIDENCE INDICE 70

    // 11 Dec 2010
    //        NUMBER OF MOLECULES 1825
    //        CORRECTED BY NAME 23
    //        CORRECTED BY ATC 0
    //        FOUNDED 1106 "
    //        LINKERMODEL (WithATC:77;WithoutATC:74) 151"
    //        LINKERNATURE 0
    //        LEFT 548
    //        CONFIDENCE INDICE 69

    // 13 Nov 2010
    //        NUMBER OF MOLECULES 1819
    //        CORRECTED BY NAME 23
    //        CORRECTED BY ATC 0
    //        FOUNDED 1089
    //        LINKERMODEL 73
    //        LINKERNATURE 0
    //        LEFT 634


    // 25 Oct 2010
    // Some corrections :
    //   - correctedByAtc is removed
    //   - one molecule name is associated with multiple molecule codes
    //        NUMBER OF MOLECULES 1819
    //        CORRECTED BY NAME 23
    //        CORRECTED BY ATC 0
    //        FOUNDED 1089
    //        LINKERMODEL 73
    //        LINKERNATURE 0
    //        LEFT 634

    // 21 Sept 2010
    // Removed Veterinary drugs
    // Number of distinct molecules 1823
    // Hand made association: 1875 (drugs with one mol and an ATC code)
    //        FOUNDED 1291
    //        LINKERMODEL 55
    //        LINKERNATURE 0
    //        LEFT 405

    // 28 Aug 2010
    // Number of distinct molecules 2250
    //  Hand made association: 1648 (drugs with one mol and an ATC code)
    //  FOUNDED 1345
    //  LINKERMODEL 61
    //  LINKERNATURE 0
    //  LEFT 724

    // 28 July 2010
    // Using the new Core::Tools::englishMoleculeLinker()
    // 2243 distinct mols
    // Hand association: 1647 (drugs 7-char atc with one molecules + hand made)
    // Found: 1292, Left: 773

    // 26 July 2010
    // 1398 distinct mols
    // Hand association: 86
    // Found: 1071, Left: 327
    // Drugs with one mol and ATC (7-char): 1358

    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

//    QSqlDatabase ca = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
//    if (!ca.open()) {
//        LOG_ERROR("Can not connect to CA_DB db : dc_Canadian_DrugsDatabaseCreator::populateDatabase()");
//        return false;
//    }

    // Associate molecule from Drugs == 1 molecule and Drug got 7-char length ATC code
    // add them to corrected
    //         SELECT DISTINCT composition.molecule_name, drugs.atc
    //         FROM composition, drugs
    //         WHERE drugs.uid=composition.uid AND length(drugs.atc)=7
    //         GROUP BY composition.uid
    //         HAVING count(composition.molecule_name) = 1
    //         LIMIT 100;

    //        QString req;
    //        LOG("Getting Drugs with ATC and one molecule - Can take some times");
    QMultiHash<QString, QString> correctedByAtcCode;
    //        QSqlQuery drugs(ca);
    //        req = "SELECT DISTINCT composition.molecule_name, drugs.atc "
    //              "FROM composition, drugs "
    //              "WHERE drugs.uid=composition.uid AND length(drugs.atc)=7 "
    //              "GROUP BY composition.uid "
    //              "HAVING count(composition.molecule_name) = 1;";
    //        if (drugs.exec(req)) {
    //            while (drugs.next()) {
    //                correctedByAtcCode.insertMulti(drugs.value(0).toString(), drugs.value(1).toString());
    //            }
    //        }

    QHash<QString, QString> corrected;
    corrected.insert("CALCIUM (CALCIUM CARBONATE)", "CALCIUM CARBONATE");
    corrected.insert("VITAMIN D3", "COLECALCIFEROL");
    corrected.insert("VITAMIN D3 (CHOLECALCIFEROL)", "COLECALCIFEROL");
    corrected.insert("DIATRIZOATE SODIUM", "DIATRIZOIC ACID");
    corrected.insert("DIATRIZOATE MEGLUMINE", "DIATRIZOIC ACID");
    corrected.insert("IOXAGLATE MEGLUMINE", "IOXAGLIC ACID");
    corrected.insert("IOXAGLATE SODIUM", "IOXAGLIC ACID");
    corrected.insert("THIAMINE MONONITRATE", "THIAMINE (VIT B1)");
    corrected.insert("ETHINYL ESTRADIOL", "ETHINYLESTRADIOL");
    corrected.insert("NORGESTREL", "LEVONORGESTREL");
    corrected.insert("ALUMINUM CHLOROHYDRATE", "ALUMINIUM CHLOROHYDRATE");
    corrected.insert("VITAMIN B6 (PYRIDOXINE HYDROCHLORIDE)", "PYRIDOXINE (VIT B6)");
    corrected.insert("VITAMIN B1 (THIAMINE HYDROCHLORIDE)", "THIAMINE (VIT B1)");
    corrected.insert("FORMOTEROL FUMARATE DIHYDRATE", "FORMOTEROL");
    corrected.insert("FIBRINOGEN (HUMAN)", "FIBRINOGEN, HUMAN");
    corrected.insert("FIBRINOGEN (HUMAN)", "HUMAN FIBRINOGEN");
    corrected.insert("FACTOR XIII", "COAGULATION FACTOR XIII");
    corrected.insert("BETA-CAROTENE", "BETACAROTENE");
    corrected.insert("L-ARGININE" , "ARGININE HYDROCHLORIDE" );
    corrected.insert("L-LYSINE (L-LYSINE HYDROCHLORIDE)" ,"LYSINE" );
    corrected.insert("L-METHIONINE" ,"METHIONINE" );
    corrected.insert("GLUTAMIC ACID" ,"GLUTAMIC ACID HYDROCHLORIDE" );
    corrected.insert("D-ALPHA TOCOPHEROL", "TOCOPHEROL");
    corrected.insert("D-PANTOTHENIC ACID (CALCIUM D-PANTOTHENATE)" ,"CALCIUM PANTOTHENATE" );

    // Get SID
    int sid = Core::Tools::getSourceId(Core::Constants::MASTER_DATABASE_NAME, CA_DRUGS_DATABASE_NAME);
    if (sid==-1) {
        LOG_ERROR("NO SID DEFINED");
        return false;
    }

    // Associate Mol <-> ATC for drugs with one molecule only
    QStringList unfound;
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(CA_DRUGS_DATABASE_NAME, "en", &unfound, corrected, correctedByAtcCode);
    qWarning() << "unfound" << unfound.count();

    // Save to links to drugs database
    Core::Tools::addComponentAtcLinks(Core::Constants::MASTER_DATABASE_NAME, mol_atc, sid);

    // add unfound to extralinkermodel
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(CA_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    LOG(QString("Database processed"));

    return true;
}





CanadianDrugsDatabaseWidget::CanadianDrugsDatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CanadianDrugsDatabaseWidget),
    m_Step(0)
{
    setObjectName("CanadianDrugsDatabaseCreator");
    ui->setupUi(this);
    ui->progressBar->hide();
    m_Step = new CaDrugDatatabaseStep(this);
    m_Step->createDir();
    pluginManager()->addObject(m_Step);
}

CanadianDrugsDatabaseWidget::~CanadianDrugsDatabaseWidget()
{
    pluginManager()->removeObject(m_Step);
    delete ui;
}

void CanadianDrugsDatabaseWidget::on_startJobs_clicked()
{
    QProgressDialog progressDialog(mainwindow());
    progressDialog.setLabelText(tr("Starting jobs"));
    progressDialog.setRange(0, 1);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setValue(0);
    progressDialog.show();
    connect(m_Step, SIGNAL(progressRangeChanged(int,int)), &progressDialog, SLOT(setRange(int, int)));
    connect(m_Step, SIGNAL(progress(int)), &progressDialog, SLOT(setValue(int)));
    connect(m_Step, SIGNAL(progressLabelChanged(QString)), &progressDialog, SLOT(setLabelText(QString)));
    m_Step->createDir();
    if (ui->unzip->isChecked()) {
        if (m_Step->unzipFiles())
            ui->unzip->setText(ui->unzip->text() + " CORRECTLY DONE");
    }
    if (ui->prepare->isChecked()) {
        if (m_Step->prepareDatas())
            ui->prepare->setText(ui->prepare->text() + " CORRECTLY DONE");
    }
    if (ui->createDb->isChecked()) {
        if (m_Step->createDatabase())
            ui->createDb->setText(ui->createDb->text() + " CORRECTLY DONE");
    }
    if (ui->populate->isChecked()) {
        if (m_Step->populateDatabase())
            ui->populate->setText(ui->populate->text() + " CORRECTLY DONE");
    }
    if (ui->linkMols->isChecked()) {
        if (m_Step->linkMolecules())
            ui->linkMols->setText(ui->linkMols->text() + " CORRECTLY DONE");
    }
    Utils::Log::messagesToTreeWidget(ui->messages);
    Utils::Log::errorsToTreeWidget(ui->errors);
}

bool CanadianDrugsDatabaseWidget::on_download_clicked()
{
    m_Step->downloadFiles(ui->progressBar);
    ui->progressBar->show();
    connect(m_Step, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    return true;
}

void CanadianDrugsDatabaseWidget::downloadFinished()
{
    ui->progressBar->show();
    ui->download->setEnabled(true);
}

void CanadianDrugsDatabaseWidget::changeEvent(QEvent *e)
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
