/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@free.fr>               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "frenchdrugsdatabasecreator.h"
#include "extramoleculelinkermodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <utils/httpdownloader.h>
#include <extensionsystem/pluginmanager.h>


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

#include <QDebug>

#include "ui_frenchdrugsdatabasewidget.h"

using namespace DrugsDbCreator;

const char* const  FRENCH_URL                  = "http://afssaps-prd.afssaps.fr/php/ecodex/telecharger/fic_cis_cip.zip";
const char* const  FR_DRUGS_DATABASE_NAME      = "AFSSAPS_FR";


static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/FrenchRawSources/") + QDir::separator();}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/drugs/drugs-fr_FR.db");}
static inline QString iamDatabaseAbsPath()  {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::IAM_DATABASE_FILENAME);}

static inline QString databasePreparationScript()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/create-fr.sql");}
static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/create-fr2.sql");}

static inline QString drugsDatabaseSqlSchema() {return settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + QString(Core::Constants::FILE_DRUGS_DATABASE_SCHEME);}
static inline QString drugsRouteSqlFileName() {return settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + QString(Core::Constants::FILE_DRUGS_ROUTES);}


FrenchDrugsDatabasePage::FrenchDrugsDatabasePage(QObject *parent) :
        IToolPage(parent)
{
    setObjectName("FrenchDrugsDatabasePage");
}

QWidget *FrenchDrugsDatabasePage::createPage(QWidget *parent)
{
    return new FrenchDrugsDatabaseWidget(parent);
}



FrDrugDatatabaseStep::FrDrugDatatabaseStep(QObject *parent) :
        m_WithProgress(false)
{
}

FrDrugDatatabaseStep::~FrDrugDatatabaseStep()
{
}

bool FrDrugDatatabaseStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        Utils::Log::addError(this, "Unable to create French Working Path :" + workingPath(), __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath)) {
            Utils::Log::addError(this, "Unable to create French database output path :" + dbpath, __FILE__, __LINE__);
            m_Errors << tr("Unable to create French database output path :") + dbpath;
        } else {
            Utils::Log::addMessage(this, "Drugs database output dir created");
        }
    }
    return true;
}

bool FrDrugDatatabaseStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool FrDrugDatatabaseStep::downloadFiles()
{
    Utils::HttpDownloader *dld = new Utils::HttpDownloader;
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
    linkDrugsRoutes();
    linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool FrDrugDatatabaseStep::unzipFiles()
{
    // check file
    QString fileName = workingPath() + QDir::separator() + QFileInfo(FRENCH_URL).fileName();
    if (!QFile(fileName).exists()) {
        Utils::Log::addError(this, QString("No files founded."), __FILE__, __LINE__);
        Utils::Log::addError(this, QString("Please download files."), __FILE__, __LINE__);
        return false;
    }

    // unzip files using QProcess
    Utils::Log::addMessage(this, QString("Starting unzipping afssaps file %1").arg(fileName));

    return Core::Tools::unzipFile(fileName, workingPath());
}

bool FrDrugDatatabaseStep::prepareDatas()
{
    // check files
    if (!QFile::exists(workingPath() + "CIS.txt")) {
        Utils::Log::addError(this, QString("Missing CIS.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    if (!QFile::exists(workingPath() + "CIS_CIP.txt")) {
        Utils::Log::addError(this, QString("Missing CIS_CIP.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    if (!QFile::exists(workingPath() + "COMPO.txt")) {
        Utils::Log::addError(this, QString("Missing COMPO.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    QString content;
    QStringList lines;

    //////////////////////////////
    // process CIS.txt          //
    //////////////////////////////
    {
        QFile file(workingPath() + "CIS.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open CIS.txt : %1. FrenchDrugsDatabaseWidget::populateDatabase()").arg(file.errorString()), __FILE__, __LINE__);
            return false;
        }
        content = QString::fromLatin1(file.readAll());
    }
    // make some replacements
    // add \t before AMM encoding == ATC code + GLOBAL_STRENGTH
    QMap<QString, QString> replacements;
//    replacements.insert(QString::fromUtf8("\tAMM active\t"), "\t\t\tA\t");
//    replacements.insert(QString::fromUtf8("\tAMM abrogée\t") , "\t\t\tB\t");
//    replacements.insert(QString::fromUtf8("\tAMM retirée\t") , "\t\t\tR\t");
//    replacements.insert(QString::fromUtf8("\tAMM suspendue\t") , "\t\t\tS\t");
//    replacements.insert(QString::fromUtf8("\tAMM archivée\t") , "\t\t\tZ\t");
//    replacements.insert(QString::fromUtf8("\tNon commercialisée\t") , "\t0\t");
//    replacements.insert(QString::fromUtf8("\tCommercialisée\t") , "\t1\t");
//    replacements.insert(QString::fromUtf8("\tProcédure nationale\t") , "\tN\t");
//    replacements.insert(QString::fromUtf8("\tProcédure centralisée\t") , "\tC\t");
//    replacements.insert(QString::fromUtf8("\tProcédure de reconnaissance mutuelle\t") , "\tR\t");
//    replacements.insert(QString::fromUtf8("\tProcédure décentralisée\t") , "\tD\t");
//    replacements.insert(QString::fromUtf8("\tAutorisation d'importation paralléle\t") , "\tI\t");

    {
        QMapIterator<QString, QString> i(replacements);
        while (i.hasNext()) {
            i.next();
            content.replace(i.key() , i.value());
        }
    }

    // prepare a better separator for the import command
    content.replace("\t\n", "\n");
    content.replace("\t", Core::Constants::SEPARATOR);

    // save file
    {
        QFile file(workingPath() + "CIS_processed.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open CIS.txt : %1. FrenchDrugsDatabaseWidget::populateDatabase()").arg(file.errorString()), __FILE__, __LINE__);
            return false;
        }
        file.write(content.toUtf8());
    }

    replacements.clear();


    //////////////////////////////
    // process CIS_CIP.txt      //
    //////////////////////////////
    {
        QFile file(workingPath() + "CIS_CIP.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open CIS_CIP.txt : %1. FrenchDrugsDatabaseWidget::populateDatabase()").arg(file.errorString()), __FILE__, __LINE__);
            //refreshViews();
            return false;
        }
        content = QString::fromLatin1(file.readAll());
    }
    // make replacements
//    replacements.insert(QString::fromUtf8("\tPrésentation active\t"), "\tA\t");
//    replacements.insert(QString::fromUtf8("\tPrésentation abrogée\t"), "\tB\t");
//    replacements.insert(QString::fromUtf8("\tDéclaration d'arrêt de commercialisation\t"), "\tA\t") ;
//    replacements.insert(QString::fromUtf8("\tDéclaration de commercialisation non communiquée\t"),"\tN\t");
//    replacements.insert(QString::fromUtf8("\tDéclaration de commercialisation\t"), "\tC\t");
//    replacements.insert(QString::fromUtf8("\tDéclaration de suspension de commercialisation\t"), "\tS\t");
    {
        QMapIterator<QString, QString> i(replacements);
        while (i.hasNext()) {
            i.next();
            content.replace(i.key() , i.value());
        }
    }

    // prepare a better separator for the import command
    content.replace("\t\n", "\n");
    content.replace("\t", Core::Constants::SEPARATOR);

    // TODO : manage date --> SQL Date

    // save file
    {
        QFile file(workingPath() + "CIS_CIP_processed.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open CIS_CIP_processed.txt : %1").arg(file.errorString()), __FILE__, __LINE__);
            //refreshViews();
            return false;
        }
        file.write(content.toUtf8());
    }

    replacements.clear();


    // process COMPO.txt
    {
        QFile file(workingPath() + "COMPO.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open COMPO.txt : %1. FrenchDrugsDatabaseWidget::populateDatabase()").arg(file.errorString()), __FILE__, __LINE__);
            //refreshViews();
            return false;
        }
        content = QString::fromLatin1(file.readAll());
    }

    // prepare a better separator for the import command
    content.replace("\t\n", "\n");
    content.replace("\t", Core::Constants::SEPARATOR);

    // save file
    {
        QFile file(workingPath() + "COMPO_processed.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open COMPO_processed.txt : %1. FrenchDrugsDatabaseWidget::populateDatabase()").arg(file.errorString()), __FILE__, __LINE__);
            //refreshViews();
            return false;
        }
        file.write(content.toUtf8());
    }

    replacements.clear();

    return true;

}

bool FrDrugDatatabaseStep::createDatabase()
{
    if (!Core::Tools::connectDatabase(FR_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    // create database structure
    if (!Core::Tools::executeSqlFile(FR_DRUGS_DATABASE_NAME, drugsDatabaseSqlSchema())) {
        Utils::Log::addError(this, "Can not create French DB.", __FILE__, __LINE__);
        return false;
    }

    if (!Core::Tools::executeSqlFile(FR_DRUGS_DATABASE_NAME, drugsRouteSqlFileName())) {
        Utils::Log::addError(this, "Can not create French DB.", __FILE__, __LINE__);
        return false;
    }

    // Run SQL commands one by one
    if (!Core::Tools::executeSqlFile(FR_DRUGS_DATABASE_NAME, databasePreparationScript())) {
        Utils::Log::addError(this, "Can not create French DB.", __FILE__, __LINE__);
        return false;
    }

    Utils::Log::addMessage(this, QString("Database schema created"));
    return true;
}

bool FrDrugDatatabaseStep::populateDatabase()
{
    if (!Core::Tools::connectDatabase(FR_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    // check files
    if (!QFile::exists(workingPath() + "CIS_processed.txt")) {
        Utils::Log::addError(this, QString("Missing CIS_processed.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    if (!QFile::exists(workingPath() + "CIS_CIP_processed.txt")) {
        Utils::Log::addError(this, QString("Missing CIS_CIP_processed.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    if (!QFile::exists(workingPath() + "COMPO_processed.txt")) {
        Utils::Log::addError(this, QString("Missing COMPO_processed.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    QProgressDialog *progressDialog = 0;
    if (m_WithProgress) {
        progressDialog = new QProgressDialog(mainwindow());
        progressDialog->setLabelText(tr("Feeding database"));
        progressDialog->setRange(0, 4);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setValue(0);
        progressDialog->show();
    }

    if (!Utils::Database::importCsvToDatabase(FR_DRUGS_DATABASE_NAME, workingPath() + "CIS_processed.txt", "DRUGS", Core::Constants::SEPARATOR)) {
        return false;
    }
    if (m_WithProgress) {
        progressDialog->setValue(progressDialog->value() + 1);
    }

//    if (!Utils::Database::importCsvToDatabase(FR_DRUGS_DATABASE_NAME, workingPath() + "CIS_CIP_processed.txt", "PACKAGING", Core::Constants::SEPARATOR)) {
//        return false;
//    }
    if (m_WithProgress) {
        progressDialog->setValue(progressDialog->value() + 1);
    }

    if (!Utils::Database::importCsvToDatabase(FR_DRUGS_DATABASE_NAME, workingPath() + "COMPO_processed.txt", "TMP_COMPOSITION", Core::Constants::SEPARATOR)) {
        return false;
    }
    if (m_WithProgress) {
        progressDialog->setValue(progressDialog->value() + 1);
    }

   // Run SQL commands one by one
   if (!Core::Tools::executeSqlFile(FR_DRUGS_DATABASE_NAME, databaseFinalizationScript())) {
       Utils::Log::addError(this, "Can create French DB.", __FILE__, __LINE__);
       return false;
   }
   if (m_WithProgress) {
       delete progressDialog;
       progressDialog = 0;
   }

   linkDrugsRoutes();

   return true;
}

bool FrDrugDatatabaseStep::linkDrugsRoutes()
{
    if (!Core::Tools::connectDatabase(FR_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    // Link drugs and routes
    QString req;
    QSqlDatabase fr = QSqlDatabase::database(FR_DRUGS_DATABASE_NAME);
    QSqlQuery query(fr);
    QHash<int, QString> routes;

    // Get all routes
    req = "SELECT ID, FR FROM ROUTES;";
    if (query.exec(req)) {
        while (query.next()) {
            routes.insert(query.value(0).toInt(), query.value(1).toString());
        }
    }
    query.finish();

    // Associate drugs routes with routes
    req = "SELECT UID, ROUTE FROM DRUGS;";
    QStringList commands;
    if (query.exec(req)) {
        while (query.next()) {
            foreach(const QString &r, query.value(1).toString().split(";", QString::SkipEmptyParts)) {
                commands << QString("INSERT INTO `DRUG_ROUTES` VALUES (%1, %2);").arg(query.value(0).toString()).arg(routes.key(r));
                if (routes.key(r)==0)
                    qWarning() << r << query.value(1).toString();
            }
        }
    }

//    foreach(const QString &command, commands)
//        Core::Tools::executeSqlQuery(command, FR_DRUGS_DATABASE_NAME, __FILE__, __LINE__);

    query.finish();

    return true;
}

bool FrDrugDatatabaseStep::linkMolecules()
{
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
    if (!Core::Tools::connectDatabase(Core::Constants::IAM_DATABASE_NAME, iamDatabaseAbsPath()))
        return false;

    if (!Core::Tools::connectDatabase(FR_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    QSqlDatabase fr = QSqlDatabase::database(FR_DRUGS_DATABASE_NAME);
    if (!fr.open()) {
        Utils::Log::addError(this, "Can not connect to French db", __FILE__, __LINE__);
        return false;
    }

    // Associate Mol <-> ATC for drugs with one molecule only
    QHash<QString, QString> corrected;

    // Associate Mol <-> ATC for drugs with one molecule only
    QStringList unfound;
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(FR_DRUGS_DATABASE_NAME, "fr", &unfound, corrected, QMultiHash<QString, QString>());
    qWarning() << "unfound" << unfound.count();

    // Save to links to drugs database
    fr.transaction();
    Core::Tools::executeSqlQuery("DELETE FROM LK_MOL_ATC;", FR_DRUGS_DATABASE_NAME);
    foreach(int mol, mol_atc.uniqueKeys()) {
        QList<int> atcCodesSaved;
        foreach(int atc, mol_atc.values(mol)) {
            if (atcCodesSaved.contains(atc))
                continue;
            atcCodesSaved.append(atc);
            QString req = QString("INSERT INTO `LK_MOL_ATC` VALUES (%1, %2)").arg(mol).arg(atc);
            Core::Tools::executeSqlQuery(req, FR_DRUGS_DATABASE_NAME);
        }
    }
    fr.commit();
    Utils::Log::addMessage(this, QString("Database processed"));

    // add unfound to extralinkermodel
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(FR_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    if (!Core::Tools::signDatabase(FR_DRUGS_DATABASE_NAME))
        Utils::Log::addError(this, "Unable to tag database.", __FILE__, __LINE__);

    return true;
}




FrenchDrugsDatabaseWidget::FrenchDrugsDatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrenchDrugsDatabaseWidget),
    m_Step(0)
{
    setObjectName("FrenchDrugsDatabaseWidget");
    ui->setupUi(this);
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
    if (ui->routes->isChecked()) {
        if (m_Step->linkDrugsRoutes())
            ui->routes->setText(ui->routes->text() + " CORRECTLY DONE");
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
    m_Step->downloadFiles();
    connect(m_Step, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    return true;
}

void FrenchDrugsDatabaseWidget::downloadFinished()
{
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
