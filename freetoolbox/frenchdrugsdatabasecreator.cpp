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
#include "globaltools.h"
#include "constants.h"
#include "mainwindow.h"
#include "extramoleculelinkermodel.h"

#include <utils/log.h>
#include <utils/httpdownloader.h>

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

const char* const  FRENCH_URL                  = "http://afssaps-prd.afssaps.fr/php/ecodex/telecharger/fic_cis_cip.zip";
const char* const  FRENCH_WORKINGPATH          = "/../global_resources/tmp/FrenchRawSources/";
const char* const  FR_DRUGS_DATABASE_NAME      = "AFSSAPS_FR";
const char* const  FR_DRUGS_DATABASE_FILENAME  =  "/../global_resources/databases/drugs/drugs-fr_FR.db";

const char* const  FILE_FR_DRUGS_DB_SCRIPT1    = "/../global_resources/sql/create-fr.sql";
const char* const  FILE_FR_DRUGS_DB_SCRIPT2    = "/../global_resources/sql/create-fr2.sql";


FrenchDrugsDatabasePage::FrenchDrugsDatabasePage(QObject *parent) :
        IToolPage(parent)
{
    setObjectName("FrenchDrugsDatabasePage");
}

QWidget *FrenchDrugsDatabasePage::createPage(QWidget *parent)
{
    return new FrenchDrugsDatabaseWidget(parent);
}


FrenchDrugsDatabaseWidget::FrenchDrugsDatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrenchDrugsDatabaseWidget)
{
    setObjectName("FrenchDrugsDatabaseWidget");
    ui->setupUi(this);
    m_WorkingPath = QDir::cleanPath(qApp->applicationDirPath() + Constants::MACBUNDLE + FRENCH_WORKINGPATH) + QDir::separator();
    if (!QDir().mkpath(m_WorkingPath))
        Utils::Log::addError(this, "Unable to create French Working Path :" + m_WorkingPath, __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
}

FrenchDrugsDatabaseWidget::~FrenchDrugsDatabaseWidget()
{
    delete ui;
}

void FrenchDrugsDatabaseWidget::on_startJobs_clicked()
{
    if (ui->download->isChecked()) {
        downloadFiles();
    }
    if (ui->unzip->isChecked()) {
        if (unzipFiles())
            ui->unzip->setText(ui->unzip->text() + " CORRECTLY DONE");
    }
    if (ui->prepare->isChecked()) {
        if (prepareDatas())
            ui->prepare->setText(ui->prepare->text() + " CORRECTLY DONE");
    }
    if (ui->createDb->isChecked()) {
        if (createDatabase())
            ui->createDb->setText(ui->createDb->text() + " CORRECTLY DONE");
    }
    if (ui->populate->isChecked()) {
        if (populateDatabase())
            ui->populate->setText(ui->populate->text() + " CORRECTLY DONE");
    }
    if (ui->linkMols->isChecked()) {
        if (linkMolecules())
            ui->linkMols->setText(ui->linkMols->text() + " CORRECTLY DONE");
    }
    Utils::Log::messagesToTreeWidget(ui->messages);
    Utils::Log::errorsToTreeWidget(ui->errors);
}

bool FrenchDrugsDatabaseWidget::downloadFiles()
{
    Utils::HttpDownloader *dld = new Utils::HttpDownloader(this);
    dld->setMainWindow(MainWindow::instance());
    dld->setOutputPath(m_WorkingPath);
    dld->setUrl(QUrl(FRENCH_URL));
    dld->startDownload();

    return true;
}

bool FrenchDrugsDatabaseWidget::unzipFiles()
{
    // check file
    QString fileName = m_WorkingPath + QDir::separator() + QFileInfo(FRENCH_URL).fileName();
    if (!QFile(fileName).exists()) {
        Utils::Log::addError(this, QString("No files founded."), __FILE__, __LINE__);
        Utils::Log::addError(this, QString("Please download files."), __FILE__, __LINE__);
        return false;
    }

    // unzip files using QProcess
    Utils::Log::addMessage(this, QString("Starting unzipping afssaps file %1").arg(fileName));

    return Tools::unzipFile(fileName, m_WorkingPath);
}

bool FrenchDrugsDatabaseWidget::prepareDatas()
{
    // check files
    if (!QFile::exists(m_WorkingPath + "CIS.txt")) {
        Utils::Log::addError(this, QString("Missing CIS.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    if (!QFile::exists(m_WorkingPath + "CIS_CIP.txt")) {
        Utils::Log::addError(this, QString("Missing CIS_CIP.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    if (!QFile::exists(m_WorkingPath + "COMPO.txt")) {
        Utils::Log::addError(this, QString("Missing COMPO.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    QString content;
    QStringList lines;

    //////////////////////////////
    // process CIS.txt          //
    //////////////////////////////
    {
        QFile file(m_WorkingPath + "CIS.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open CIS.txt : %1. FrenchDrugsDatabaseWidget::populateDatabase()").arg(file.errorString()), __FILE__, __LINE__);
            return false;
        }
        content = QString::fromLatin1(file.readAll());
    }
    // make some replacements
    // add \t before AMM encoding == ATC code + GLOBAL_STRENGTH
    QMap<QString, QString> replacements;
    replacements.insert(QString::fromUtf8("\tAMM active\t"), "\t\t\tA\t");
    replacements.insert(QString::fromUtf8("\tAMM abrogée\t") , "\t\t\tB\t");
    replacements.insert(QString::fromUtf8("\tAMM retirée\t") , "\t\t\tR\t");
    replacements.insert(QString::fromUtf8("\tAMM suspendue\t") , "\t\t\tS\t");
    replacements.insert(QString::fromUtf8("\tAMM archivée\t") , "\t\t\tZ\t");
    replacements.insert(QString::fromUtf8("\tNon commercialisée\t") , "\t0\t");
    replacements.insert(QString::fromUtf8("\tCommercialisée\t") , "\t1\t");
    replacements.insert(QString::fromUtf8("\tProcédure nationale\t") , "\tN\t");
    replacements.insert(QString::fromUtf8("\tProcédure centralisée\t") , "\tC\t");
    replacements.insert(QString::fromUtf8("\tProcédure de reconnaissance mutuelle\t") , "\tR\t");
    replacements.insert(QString::fromUtf8("\tProcédure décentralisée\t") , "\tD\t");
    replacements.insert(QString::fromUtf8("\tAutorisation d'importation paralléle\t") , "\tI\t");

    {
        QMapIterator<QString, QString> i(replacements);
        while (i.hasNext()) {
            i.next();
            content.replace(i.key() , i.value());
        }
    }

    // prepare a better separator for the import command
    content.replace("\t\n", "\n");
    content.replace("\t", Constants::SEPARATOR);

    // save file
    {
        QFile file(m_WorkingPath + "CIS_processed.txt");
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
        QFile file(m_WorkingPath + "CIS_CIP.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open CIS_CIP.txt : %1. FrenchDrugsDatabaseWidget::populateDatabase()").arg(file.errorString()), __FILE__, __LINE__);
            //refreshViews();
            return false;
        }
        content = QString::fromLatin1(file.readAll());
    }
    // make replacements
    replacements.insert(QString::fromUtf8("\tPrésentation active\t"), "\tA\t");
    replacements.insert(QString::fromUtf8("\tPrésentation abrogée\t"), "\tB\t");
    replacements.insert(QString::fromUtf8("\tDéclaration d'arrêt de commercialisation\t"), "\tA\t") ;
    replacements.insert(QString::fromUtf8("\tDéclaration de commercialisation non communiquée\t"),"\tN\t");
    replacements.insert(QString::fromUtf8("\tDéclaration de commercialisation\t"), "\tC\t");
    replacements.insert(QString::fromUtf8("\tDéclaration de suspension de commercialisation\t"), "\tS\t");
    {
        QMapIterator<QString, QString> i(replacements);
        while (i.hasNext()) {
            i.next();
            content.replace(i.key() , i.value());
        }
    }

    // prepare a better separator for the import command
    content.replace("\t\n", "\n");
    content.replace("\t", Constants::SEPARATOR);

    // TODO : manage date --> SQL Date

    // save file
    {
        QFile file(m_WorkingPath + "CIS_CIP_processed.txt");
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
        QFile file(m_WorkingPath + "COMPO.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open COMPO.txt : %1. FrenchDrugsDatabaseWidget::populateDatabase()").arg(file.errorString()), __FILE__, __LINE__);
            //refreshViews();
            return false;
        }
        content = QString::fromLatin1(file.readAll());
    }

    // prepare a better separator for the import command
    content.replace("\t\n", "\n");
    content.replace("\t", Constants::SEPARATOR);

    // save file
    {
        QFile file(m_WorkingPath + "COMPO_processed.txt");
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

bool FrenchDrugsDatabaseWidget::createDatabase()
{
    if (!Tools::connectDatabase(FR_DRUGS_DATABASE_NAME, FR_DRUGS_DATABASE_FILENAME))
        return false;

    // create database structure
    if (!Tools::executeSqlFile(FR_DRUGS_DATABASE_NAME, Constants::MACBUNDLE + Constants::FILE_DRUGS_DATABASE_SCHEME)) {
        Utils::Log::addError(this, "Can create French DB.", __FILE__, __LINE__);
        return false;
    }

    // Run SQL commands one by one
    if (!Tools::executeSqlFile(FR_DRUGS_DATABASE_NAME, Constants::MACBUNDLE + FILE_FR_DRUGS_DB_SCRIPT1)) {
        Utils::Log::addError(this, "Can create French DB.", __FILE__, __LINE__);
        return false;
    }

    Utils::Log::addMessage(this, QString("Database schema created"));
    return true;
}

bool FrenchDrugsDatabaseWidget::populateDatabase()
{
    if (!Tools::connectDatabase(FR_DRUGS_DATABASE_NAME, Constants::MACBUNDLE + FR_DRUGS_DATABASE_FILENAME))
        return false;

    // check files
    if (!QFile::exists(m_WorkingPath + "CIS_processed.txt")) {
        Utils::Log::addError(this, QString("Missing CIS_processed.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    if (!QFile::exists(m_WorkingPath + "CIS_CIP_processed.txt")) {
        Utils::Log::addError(this, QString("Missing CIS_CIP_processed.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    if (!QFile::exists(m_WorkingPath + "COMPO_processed.txt")) {
        Utils::Log::addError(this, QString("Missing COMPO_processed.txt file. FrenchDrugsDatabaseWidget::populateDatabase()"), __FILE__, __LINE__);
        return false;
    }

    // import to sqlite3 (should work on mac and linux) --> sqlite3 -separator "||" ./drugs.db ".import CIS_processed.txt CIS"
    if (!Tools::executeProcess(QString("sqlite3 -separator \"%1\" \"%2\" \".import \"%3\" %4\"")
        .arg(Constants::SEPARATOR, qApp->applicationDirPath() + Constants::MACBUNDLE + FR_DRUGS_DATABASE_FILENAME, m_WorkingPath + "CIS_processed.txt" , "DRUGS")))
        return false;

    // import to sqlite3
    if (!Tools::executeProcess(QString("sqlite3 -separator \"%1\" \"%2\" \".import \"%3\" %4\"")
        .arg(Constants::SEPARATOR, qApp->applicationDirPath() + Constants::MACBUNDLE + FR_DRUGS_DATABASE_FILENAME, m_WorkingPath + "CIS_CIP_processed.txt" , "PACKAGING")))
        return false;

    // import to sqlite3
   if (!Tools::executeProcess(QString("sqlite3 -separator \"%1\" \"%2\" \".import \"%3\" %4\"")
       .arg(Constants::SEPARATOR, qApp->applicationDirPath() + Constants::MACBUNDLE + FR_DRUGS_DATABASE_FILENAME, m_WorkingPath + "COMPO_processed.txt" , "TMP_COMPOSITION")))
        return false;

   // Run SQL commands one by one
   if (!Tools::executeSqlFile(FR_DRUGS_DATABASE_NAME, Constants::MACBUNDLE + FILE_FR_DRUGS_DB_SCRIPT2)) {
       Utils::Log::addError(this, "Can create French DB.", __FILE__, __LINE__);
       return false;
   }

    return true;
}

bool FrenchDrugsDatabaseWidget::linkMolecules()
{
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
    if (!Tools::connectDatabase(Constants::IAM_DATABASE_NAME, Constants::IAM_DATABASE_FILENAME))
        return false;

    if (!Tools::connectDatabase(FR_DRUGS_DATABASE_NAME, FR_DRUGS_DATABASE_FILENAME))
        return false;

    QSqlDatabase fr = QSqlDatabase::database(FR_DRUGS_DATABASE_NAME);
    if (!fr.open()) {
        Utils::Log::addError(this, "Can not connect to French db", __FILE__, __LINE__);
        return false;
    }

    // get all mols
    QHash<QString, int> mols_id;
    mols_id = Tools::getMoleculesFromDrugsDatabase(FR_DRUGS_DATABASE_NAME);

    // Associate Mol <-> ATC for drugs with one molecule only
    QHash<QString, QString> corrected;

    // Associate Mol <-> ATC for drugs with one molecule only
    QStringList unfound;
    QMultiHash<int, int> mol_atc = Tools::moleculeLinker(FR_DRUGS_DATABASE_NAME, "fr", &unfound, mols_id, corrected);
    qWarning() << "unfound" << unfound.count();

    // Save to links to drugs database
    fr.transaction();
    Tools::executeSqlQuery("DELETE FROM LK_MOL_ATC;", FR_DRUGS_DATABASE_NAME);
    foreach(int mol, mol_atc.uniqueKeys()) {
        foreach(int atc, mol_atc.values(mol)) {
            QString req = QString("INSERT INTO `LK_MOL_ATC` VALUES (%1, %2)").arg(mol).arg(atc);
            Tools::executeSqlQuery(req, FR_DRUGS_DATABASE_NAME);
        }
    }
    fr.commit();
    Utils::Log::addMessage(this, QString("Database processed"));

    // add unfound to extralinkermodel
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(FR_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    return true;
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
