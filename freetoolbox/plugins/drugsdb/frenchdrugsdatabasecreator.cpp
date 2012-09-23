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
#include "frenchdrugsdatabasecreator.h"
#include "extramoleculelinkermodel.h"
#include "drug.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/drugdatabasedescription.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <utils/httpdownloader.h>
#include <extensionsystem/pluginmanager.h>
#include <quazip/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>

#include <QFile>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDir>
#include <QDate>
#include <QProgressDialog>
#include <QHash>
#include <QStringList>
#include <QString>
#include <QTextStream>

#include <QDebug>

#include "ui_frenchdrugsdatabasewidget.h"

using namespace DrugsDbCreator;
using namespace Trans::ConstantTranslations;

const char* const  FRENCH_URL                  = "http://afssaps-prd.afssaps.fr/php/ecodex/telecharger/fic_cis_cip.zip";
const char* const  FR_DRUGS_DATABASE_NAME      = "FR_AFSSAPS";

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/FrenchRawSources/") + QDir::separator();}
static inline QString databaseAbsPath()  {return Core::Tools::drugsDatabaseAbsFileName();}

static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/drugdb/fr/fr_db_finalize.sql");}
static inline QString databaseDescriptionFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/drugdb/fr/description.xml");}


FrenchDrugsDatabasePage::FrenchDrugsDatabasePage(QObject *parent) :
        IToolPage(parent)
{
    setObjectName("FrenchDrugsDatabasePage");
}

QString FrenchDrugsDatabasePage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_DRUGSDATABASE;
}

QWidget *FrenchDrugsDatabasePage::createPage(QWidget *parent)
{
    return new FrenchDrugsDatabaseWidget(parent);
}



FrDrugDatatabaseStep::FrDrugDatatabaseStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    m_WithProgress(false)
{
    setObjectName("FrDrugDatatabaseStep");
}

FrDrugDatatabaseStep::~FrDrugDatatabaseStep()
{
}

bool FrDrugDatatabaseStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        LOG_ERROR("Unable to create French Working Path :" + workingPath());
    else
        LOG("Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath)) {
            LOG_ERROR("Unable to create French database output path :" + dbpath);
            m_Errors << tr("Unable to create French database output path :") + dbpath;
        } else {
            LOG("Drugs database output dir created");
        }
    }
    return true;
}

bool FrDrugDatatabaseStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool FrDrugDatatabaseStep::downloadFiles(QProgressBar *bar)
{
    Utils::HttpDownloader *dld = new Utils::HttpDownloader;
    dld->setProgressBar(bar);
//    dld->setMainWindow(mainwindow());
    dld->setOutputPath(workingPath());
    dld->setUrl(QUrl(FRENCH_URL));
    dld->startDownload();
    connect(dld, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()));
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    return true;
}

bool FrDrugDatatabaseStep::process()
{
    unzipFiles();
    prepareDatas();
    createDatabase();
    populateDatabase();
    linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool FrDrugDatatabaseStep::unzipFiles()
{
    // check file
    QString fileName = workingPath() + QDir::separator() + QFileInfo(FRENCH_URL).fileName();
    if (!QFile(fileName).exists()) {
        LOG_ERROR(QString("No files founded."));
        LOG_ERROR(QString("Please download files."));
        return false;
    }

    // unzip files using QProcess
    LOG(QString("Starting unzipping afssaps file %1").arg(fileName));

    return QuaZipTools::unzipFile(fileName, workingPath());
}

bool FrDrugDatatabaseStep::prepareDatas()
{
    // check files
    if (!QFile::exists(workingPath() + "CIS.txt")) {
        LOG_ERROR(QString("Missing CIS.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"));
        return false;
    }

    if (!QFile::exists(workingPath() + "CIS_CIP.txt")) {
        LOG_ERROR(QString("Missing CIS_CIP.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"));
        return false;
    }

    if (!QFile::exists(workingPath() + "COMPO.txt")) {
        LOG_ERROR(QString("Missing COMPO.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"));
        return false;
    }

    return true;
}

bool FrDrugDatatabaseStep::createDatabase()
{
    if (!Core::Tools::createMasterDrugInteractionDatabase())
        return false;

    QMultiHash<QString, QVariant> labels;
    labels.insert("fr","Base de données thérapeutique française");
    labels.insert("en","French therapeutic database");
    labels.insert("de","Französische Therapeutische Datenbank");

    if (Core::Tools::createNewDrugsSource(Core::Constants::MASTER_DATABASE_NAME, FR_DRUGS_DATABASE_NAME, labels) == -1) {
        LOG_ERROR("Unable to create the French drugs sources");
        return false;
    }
    Core::Tools::saveDrugDatabaseDescription(databaseDescriptionFile(), 0);
    LOG(QString("Database schema created"));
    return true;
}

bool FrDrugDatatabaseStep::populateDatabase()
{
    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    // check files
    if (!QFile::exists(workingPath() + "CIS.txt")) {
        LOG_ERROR(QString("Missing CIS.txt file."));
        return false;
    }

//    if (!QFile::exists(workingPath() + "CIS_CIP.txt")) {
//        LOG_ERROR(QString("Missing CIS_CIP.txt file."));
//        return false;
//    }

    if (!QFile::exists(workingPath() + "COMPO.txt")) {
        LOG_ERROR(QString("Missing COMPO.txt file."));
        return false;
    }

    QHash<int, Drug *> drugs;
    QMultiHash<int, Component *> compos;

    QFile file(workingPath() + "CIS.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("ERROR : Enable to open CIS.txt : %1.").arg(file.errorString()));
        return false;
    }
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source"));
    Q_EMIT progressRangeChanged(0, file.size());
    Q_EMIT progress(0);

    QTextStream in(&file);
    in.setCodec("ISO 8859-15");
    int pos = 0;
    while (!in.atEnd()) {
        QString l = in.readLine();
        pos += l.length();
        QStringList line = l.split("\t");
        //68586203	17 B ESTRADIOL BESINS-ISCOVESCO 0,06 POUR CENT, gel pour application cutanée en tube	(2)gel pour application	(3)transdermique	(4)AMM active	(5)Procdure nationale	(6)Non commercialise (7)SPC
        Drug *drug = new Drug;
        drug->setData(Drug::Uid1, line.at(0));
        drug->setData(Drug::OldUid, line.at(0));
        drug->setData(Drug::Name, line.at(1));
        drug->setData(Drug::Forms, line.at(2));
        drug->setData(Drug::Routes, line.at(3).split(";"));
        drug->setData(Drug::Authorization, line.at(4) + " ; " + line.at(5));
        drug->setData(Drug::Marketed, line.at(6).startsWith("Comm"));
        drug->setData(Drug::Spc, line.at(7));
        drug->setData(Drug::Valid, 1);
        drug->setData(Drug::SID, 1);
        drugs.insert(line.at(0).toInt(), drug);
        if (drugs.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }
    }
    file.close();

    file.setFileName(workingPath() + "COMPO.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("ERROR : Enable to open CIS.txt : %1. FrenchDrugsDatabaseWidget::populateDatabase()").arg(file.errorString()));
        return false;
    }
    in.setDevice(&file);
    in.setCodec("ISO 8859-15");
    Q_EMIT progressLabelChanged(tr("Reading composition raw source"));
    Q_EMIT progressRangeChanged(0, file.size());
    Q_EMIT progress(0);
    pos = 0;
    while (!in.atEnd()) {
        QString l = in.readLine();
        pos += l.length();
        QStringList line = l.split("\t");

//        if (!drugs.keys().contains(line.at(0).toInt()))
//            continue;

        //60001288	(1)comprim	(2)00468	(3)CODINE (PHOSPHATE DE) SESQUIHYDRAT	(4)15 mg 	(5)un comprim	(6)SA	(7)1
        Component *compo = new Component;
        compo->setData(Component::Name, line.at(3));
        compo->setData(Component::Strength, line.at(4));
        compo->setData(Component::Dose, line.at(5));
        compo->setData(Component::Nature, line.at(6));
        compo->setData(Component::NatureLink, line.at(7));
        compos.insertMulti(line.at(0).toInt(), compo);
        if (compos.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }
    }
    file.close();

    Q_EMIT progressLabelChanged(tr("Saving drugs into database"));
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(1);

    QVector<Drug *> drugsVector;
    foreach(const int uid, drugs.keys()) {
        Drug *drug = drugs[uid];
        foreach(Component *c, compos.values(uid)) {
            drug->addComponent(c);
        }
        drugsVector << drug;
    }

    Drug::saveDrugsIntoDatabase(Core::Constants::MASTER_DATABASE_NAME, drugsVector, FR_DRUGS_DATABASE_NAME);
    Q_EMIT progress(2);

    // Run SQL commands one by one
    Q_EMIT progressLabelChanged(tr("Running database finalization script"));
//    if (!Core::Tools::executeSqlFile(Core::Constants::MASTER_DATABASE_NAME, databaseFinalizationScript())) {
//        LOG_ERROR("Can create French DB.");
//        return false;
//    }

    Core::Tools::saveDrugDatabaseDescription(databaseDescriptionFile(), 50);

    // delete pointers
    qDeleteAll(drugs);
    drugs.clear();
    compos.clear();
    drugsVector.clear();
    return true;
}

bool FrDrugDatatabaseStep::linkMolecules()
{
    // 17 Feb 2012
    //    NUMBER OF MOLECULES 5340
    //    CORRECTED BY NAME 0
    //    CORRECTED BY ATC 0
    //    FOUNDED 3228 "
    //    LINKERMODEL (WithATC:548;WithoutATC:829) 1377"
    //    LINKERNATURE 405
    //    LEFT 1282
    //    CONFIDENCE INDICE 75

    // 13 Dec 2011: using all length of ATC codes
    //    NUMBER OF MOLECULES 5230
    //    CORRECTED BY NAME 0
    //    CORRECTED BY ATC 0
    //    FOUNDED 3216 "
    //    LINKERMODEL (WithATC:550;WithoutATC:829) 1379"
    //    LINKERNATURE 403
    //    LEFT 1184
    //    CONFIDENCE INDICE 77


    // 18 Oct 2011
    //    NUMBER OF MOLECULES 5211
    //    CORRECTED BY NAME 0
    //    CORRECTED BY ATC 0
    //    FOUNDED 3114 "
    //    LINKERMODEL (WithATC:617;WithoutATC:833) 1450"
    //    LINKERNATURE 470
    //    LEFT 1263
    //    CONFIDENCE INDICE 75

    // 29 Sept 2011 (ATC 2011 && ATC 2012)
    //    NUMBER OF MOLECULES 5194
    //    CORRECTED BY NAME 0
    //    CORRECTED BY ATC 0
    //    FOUNDED 3036 "
    //    LINKERMODEL (WithATC:682;WithoutATC:829) 1511"
    //    LINKERNATURE 558
    //    LEFT 1328
    //    CONFIDENCE INDICE 74

    // 29 Apr 2011
    //    NUMBER OF MOLECULES 5154
    //    CORRECTED BY NAME 0
    //    CORRECTED BY ATC 0
    //    FOUNDED 2856 "
    //    LINKERMODEL (WithATC:568;WithoutATC:790) 1358"
    //    LINKERNATURE 528
    //    LEFT 1507
    //    CONFIDENCE INDICE 70

    // 10 Dec 2010
    //    NUMBER OF MOLECULES 5112
    //    CORRECTED BY NAME 0
    //    CORRECTED BY ATC 0
    //    FOUNDED 2375 "
    //    LINKERMODEL (WithATC:366;WithoutATC:790) 1156"
    //    LINKERNATURE 301
    //    LEFT 1648
    //    CONFIDENCE INDICE 61

    // 13 Nov 2010
    //    NUMBER OF MOLECULES 5113
    //    CORRECTED BY NAME 0
    //    CORRECTED BY ATC 0
    //    FOUNDED 2304
    //    LINKERMODEL 283
    //    LINKERNATURE 302
    //    LEFT 2511

    // 20 Sept 2010
    //    FOUNDED 1825
    //    LINKERMODEL 279
    //    LINKERNATURE 412
    //    LEFT 1203

    // 08 Sept 2010
    // Removing french prefix
    //    Number of marketed drugs 10096
    //    Number of distinct molecules 2928
    //    FOUNDED 1764
    //    LINKERMODEL 199
    //    LINKERNATURE 392
    //    LEFT 1262

    // 05 Sept 2010
    // Removing non marketed drugs
    //    Number of marketed drugs 10096
    //    Number of distinct molecules 2928
    //    FOUNDED 1702
    //    LINKERMODEL 128
    //    LINKERNATURE 387
    //    LEFT 1321

    // 28 August 2010
    // 5472 ATCs
    // Number of distinct molecules
    //    Hand 0
    //    FOUNDED 2176
    //    LINKERMODEL 97
    //    LINKERNATURE 299
    //    LEFT 2613

    // 27 August 2010
    // 5472 ATCs
    // Number of distinct molecules 5069
    //    Hand 0
    //    FOUNDED 2128
    //    LINKERMODEL 34
    //    LINKERNATURE 304
    //    LEFT 2647

    // Old code
    //   5074 Molecules
    //   1502 Same name MOL <-> ATC asso
    //   239 Partial name MOL <-> ATC asso
    //   2 from LK_NATURE
    //   Total = 2253 associations


    // Connect to databases
    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    QSqlDatabase fr = QSqlDatabase::database(Core::Constants::MASTER_DATABASE_NAME);
    if (!fr.isOpen()) {
        LOG_ERROR("Can not connect to French db");
        return false;
    }

    // Get SID
    int sid = Core::Tools::getSourceId(Core::Constants::MASTER_DATABASE_NAME, FR_DRUGS_DATABASE_NAME);
    if (sid==-1) {
        LOG_ERROR("NO SID DEFINED");
        return false;
    }

    // Associate Mol <-> ATC for drugs with one molecule only
    QHash<QString, QString> corrected;

    Q_EMIT progressLabelChanged(tr("Linking drugs components to ATC codes"));
    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progress(0);

    // Associate Mol <-> ATC for drugs with one molecule only
    QStringList unfound;
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(FR_DRUGS_DATABASE_NAME, "fr", &unfound, corrected, QMultiHash<QString, QString>());
    qWarning() << "unfound" << unfound.count();

    Q_EMIT progress(1);

    Q_EMIT progressLabelChanged(tr("Saving components to ATC links to database"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // Save to links to drugs database
    Core::Tools::addComponentAtcLinks(Core::Constants::MASTER_DATABASE_NAME, mol_atc, sid);

    LOG(QString("Database processed"));

    // add unfound to extralinkermodel
    Q_EMIT progressLabelChanged(tr("Updating component link XML file"));
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(FR_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    return true;
}




FrenchDrugsDatabaseWidget::FrenchDrugsDatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrenchDrugsDatabaseWidget),
    m_Step(0)
{
    setObjectName("FrenchDrugsDatabaseWidget");
    ui->setupUi(this);
    ui->progressBar->hide();
    m_Step = new FrDrugDatatabaseStep(this);
    m_Step->createDir();
    pluginManager()->addObject(m_Step);
}

FrenchDrugsDatabaseWidget::~FrenchDrugsDatabaseWidget()
{
    pluginManager()->removeObject(m_Step);
    delete ui;
}

void FrenchDrugsDatabaseWidget::on_startJobs_clicked()
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

bool FrenchDrugsDatabaseWidget::on_download_clicked()
{
    ui->download->setEnabled(false);
    ui->progressBar->show();
    m_Step->downloadFiles(ui->progressBar);
    connect(m_Step, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    return true;
}

void FrenchDrugsDatabaseWidget::downloadFinished()
{
    ui->progressBar->hide();
    ui->download->setEnabled(true);
}

void FrenchDrugsDatabaseWidget::changeEvent(QEvent *e)
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
