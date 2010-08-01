/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "canadiandrugsdatabasewidget.h"
#include "ui_canadiandrugsdatabasewidget.h"

#include "globaltools.h"
#include "constants.h"

#include <utils/log.h>

#include <QDir>


const char* const  CANADIAN_URL         = "http://www.hc-sc.gc.ca/dhp-mps/prodpharma/databasdon/txt/allfiles.zip";
const char* const  CANADIAN_WORKINGPATH = "/../global_resources/tmp/CanadianRawSources/";
const char* const  CA_DRUGS_DATABASE_NAME  = "drugs_ca";
const char* const  CA_DRUGS_DATABASE_FILENAME =  "/../global_resources/databases/drugs/drugs-en_CA.db";


CanadianDrugsDatabaseWidget::CanadianDrugsDatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CanadianDrugsDatabaseWidget)
{
    setObjectName("CanadianDrugsDatabaseCreator");
    ui->setupUi(this);
    m_WorkingPath = QDir::cleanPath(qApp->applicationDirPath() + Constants::MACBUNDLE + CANADIAN_WORKINGPATH) + "/";
    if (!QDir().mkpath(m_WorkingPath))
        Utils::Log::addError(this, "Unable to create Canadian Working Path :" + m_WorkingPath);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
}

CanadianDrugsDatabaseWidget::~CanadianDrugsDatabaseWidget()
{
    delete ui;
}

void CanadianDrugsDatabaseWidget::on_startJobs_clicked()
{
    if (ui->download->isChecked()) {
        downloadFiles();
    }
    if (ui->unzip->isChecked()) {
        unzipFiles();
    }
    if (ui->prepare->isChecked()) {
        prepareDatas();
    }
    if (ui->createDb->isChecked()) {
        createDatabase();
    }
    if (ui->linkMols->isChecked()) {
        linkMolecules();
    }
    Utils::Log::messagesToTreeWidget(ui->messages);
    Utils::Log::errorsToTreeWidget(ui->errors);
}

bool CanadianDrugsDatabaseWidget::downloadFiles()
{
    QString app = "";
#ifdef Q_OS_MAC
    app = QString("curl %1 -o \"%2\" -s").arg(CANADIAN_URL).arg(m_WorkingPath + QFileInfo(CANADIAN_URL).fileName());
#else
    app = QString("wget %1").arg(CANADIAN_URL);
#endif
    if (Tools::executeProcess(app)) {
        m_FileName = m_WorkingPath + QFileInfo(CANADIAN_URL).fileName();
        Utils::Log::addMessage(this, QString("Canadian file dowloaded without error %1").arg(m_FileName));
    } else {
        m_FileName = "";
    }
    return true;
}

bool CanadianDrugsDatabaseWidget::unzipFiles()
{
    // check  file
    if ((m_FileName.isEmpty()) && (!QFile::exists(QFileInfo(CANADIAN_URL).fileName()))) {
        Utils::Log::addError(this, QString("No Canadian file name."));
        Utils::Log::addError(this, QString("Trying automatic download."));
        downloadFiles();
        if ((m_FileName.isEmpty()) && (!QFile::exists(QFileInfo(CANADIAN_URL).fileName()))) {
            Utils::Log::addError(this, QString("Download failed. Can not unzip Canadian file."));
            return false;
        }
    }
    if ((m_FileName.isEmpty()) && (QFile::exists(QFileInfo(CANADIAN_URL).fileName())))
        m_FileName = QFileInfo(CANADIAN_URL).fileName();
    else if (!QFile::exists(m_FileName)) {
        Utils::Log::addError(this, QString("Canadian zipped file does not exist %1").arg(m_FileName));
        return false;
    }

    Utils::Log::addMessage(this, QString("Starting unzipping Canadian file %1").arg(m_FileName));

    // unzip downloaded using QProcess
    if (!Tools::unzipFile(m_FileName, m_WorkingPath))
        return false;

    // unzip all files in the working path
    QFileInfoList list = QDir(m_WorkingPath).entryInfoList(QStringList()<<"*.zip");
    foreach(const QFileInfo &info, list) {
        if (info.fileName()!="allfiles.zip") {
            if (!Tools::unzipFile(info.absoluteFilePath(), m_WorkingPath)) {
                Utils::Log::addError(this, "Unable to unzip " + info.absoluteFilePath());
                return false;
            }
        }
    }
    return true;
}

bool CanadianDrugsDatabaseWidget::prepareDatas()
{
    QStringList files = QStringList()
                        << "drug.txt"
                        << "form.txt"
                        << "route.txt"
                        << "status.txt"
                        << "ingred.txt"
                        << "ther.txt"
                        << "package.txt";

    // check files
    foreach(const QString &file, files) {
        if (!QFile::exists(m_WorkingPath + file)) {
            Utils::Log::addError(this, QString("Missing " + m_WorkingPath + file + " file. prepareDatas()"));
            return false;
        }
    }

    // transform each files
    foreach(const QString &file, files) {
        Utils::Log::addMessage(this, "Processing file :" + file);
        QString tmp;
        {
            QFile f(m_WorkingPath + file);
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                Utils::Log::addError(this, QString("ERROR : Enable to open %1 : %2. populateDatabase()").arg(file, f.errorString()));
                return false;
            }
            Utils::Log::addMessage(this, "Reading file");
            tmp = QString::fromLatin1(f.readAll());
        }

        // prepare a better separator for the import command
        Utils::Log::addMessage(this, "Replacing separators");
        tmp.replace("\",\"", Constants::SEPARATOR);
        Utils::Log::addMessage(this, "Removing last \"");
        tmp.remove("\"");

        // save file
        {
            QFile f(m_WorkingPath + QFileInfo(file).baseName() + ".csv");
            if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                Utils::Log::addError(this, QString("ERROR : Enable to open %1 : %2. populateDatabase()").arg(file, f.errorString()));
                return false;
            }
            Utils::Log::addMessage(this, "Saving file");
            f.write(tmp.toUtf8());
        }
    }
    return true;
}

bool CanadianDrugsDatabaseWidget::createDatabase()
{
    if (!Tools::connectDatabase(CA_DRUGS_DATABASE_NAME, CA_DRUGS_DATABASE_FILENAME))
        return false;

    // create database structure
    if (!Tools::executeSqlFile(CA_DRUGS_DATABASE_NAME, Constants::MACBUNDLE + Constants::FILE_DRUGS_DATABASE_SCHEME)) {
        Utils::Log::addError(this, "Can create Canadian DB.");
        return false;
    }

    Utils::Log::addMessage(this, QString("Database schema created"));
    return true;
}

//bool CanadianDrugsDatabaseWidget::connectDatabase()
//{
//    // create empty sqlite database
//    // test driver
//    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
//        Utils::Log::addError(this, QString("ERROR : SQLite driver is not available"));
//        return false;
//    }

//    QSqlDatabase DB;

//    // test drugs connection
//    if (QSqlDatabase::contains(CA_DRUGS_DATABASE_NAME)) {
//        DB = QSqlDatabase::database(CA_DRUGS_DATABASE_NAME);
//    }  else {
//        DB = QSqlDatabase::addDatabase("QSQLITE" , CA_DRUGS_DATABASE_NAME);
//        DB.setDatabaseName(qApp->applicationDirPath() + Constants::MACBUNDLE + CA_DRUGS_DATABASE_FILENAME);

//        if (!DB.open()) {
//            Utils::Log::addError(this, QString("ERROR : %1 : in connectDatabase()").arg(DB.lastError().text()));
//            return false;
//        } else {
//            Utils::Log::addMessage(this, QString("Connection to database created : %1 %2")
//                    .arg(DB.connectionName(), DB.databaseName()));
//        }
//    }
//    return true;
//}

struct drug {
    int UID;
    QString atc;
    QHash<int, QString> mols;
    void warn()
    {
        QString tmp = QString("Drug(%1;%2): ").arg(UID).arg(atc);
        foreach(const int id, mols.keys()) {
            tmp += QString("(%1;%2), ").arg(id).arg(mols.value(id));
        }
        tmp.chop(2);
        qWarning() << tmp;
    }
};


bool CanadianDrugsDatabaseWidget::linkMolecules()
{
    {
        // 28 July 2010
        // Using the new Tools::englishMoleculeLinker()
        // 2243 distinct mols
        // Hand association: 1647 (drugs 7-char atc with one molecules + hand made)
        // Found: 1292, Left: 773

        // 26 July 2010
        // 1398 distinct mols
        // Hand association: 86
        // Found: 1071, Left: 327
        // Drugs with one mol and ATC (7-char): 1358

        if (!Tools::connectDatabase(Constants::IAM_DATABASE_NAME, Constants::IAM_DATABASE_FILENAME))
            return false;

        // get all drugs and ATC codes
        if (!Tools::connectDatabase(CA_DRUGS_DATABASE_NAME, CA_DRUGS_DATABASE_FILENAME))
            return false;

        QSqlDatabase ca = QSqlDatabase::database(CA_DRUGS_DATABASE_NAME);
        if (!ca.open()) {
            Utils::Log::addError(this, "Can not connect to CA_DB db : dc_Canadian_DrugsDatabaseCreator::populateDatabase()");
            return false;
        }

        ca.transaction();

        // Associate molecule for Drug == 1 molecule and Drug got 7-char length ATC code
        // add them to corrected
        // SELECT DISTINCT composition.molecule_name, drugs.atc
        // FROM composition, drugs
        // WHERE drugs.uid=composition.uid AND length(drugs.atc)=7
        // GROUP BY composition.uid
        // HAVING count(composition.molecule_name) = 1;

        Utils::Log::addMessage(this, "Getting Drugs with ATC and one molecule - Can take some times");
        QSqlQuery drugs(ca);
        QHash<QString, QString> correctedByAtcCode;
        QString req = "SELECT DISTINCT composition.molecule_name, drugs.atc "
                      "FROM composition, drugs "
                      "WHERE drugs.uid=composition.uid AND length(drugs.atc)=7 "
                      "GROUP BY composition.uid "
                      "HAVING count(composition.molecule_name) = 1;";
        if (drugs.exec(req)) {
            while (drugs.next()) {
                correctedByAtcCode.insert(drugs.value(0).toString(), drugs.value(1).toString());
            }
        }

        Utils::Log::addMessage(this, "Getting Drugs Compostion");
        QHash<QString, int> mols_id;
        req = "SELECT DISTINCT `MOLECULE_CODE`, `MOLECULE_NAME` FROM `COMPOSITION`";
        QSqlQuery mols(ca);
        if (mols.exec(req)) {
            while (mols.next()) {
                mols_id.insert(mols.value(1).toString(), mols.value(0).toInt());
            }
        }
        mols.finish();
        qWarning() << "Number of distinct molecules" << mols_id.count();

        ca.commit();

        QHash<QString, QString> corrected;
        corrected.insert("CALCIUM (CALCIUM CARBONATE)", "CALCIUM CARBONATE");
        corrected.insert("VITAMIN D3", "vitamine d");
        corrected.insert("VITAMIN D3", "COLECALCIFEROL");
        corrected.insert("VITAMIN D3 (CHOLECALCIFEROL)", "vitamine d");
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
        //    corrected.insert(,);
        //    corrected.insert(,);
        //    corrected.insert(,);
        //    corrected.insert(,);
        //    corrected.insert(,);
        //    corrected.insert("INSULIN" ???);
        //    corrected.insert("PSEUDOEPHEDRINE HYDROCHLORIDE", "PSEUDOEPHEDRINE");
    //    corrected.insert("NEOMYCIN (NEOMYCIN SULFATE)", "NEOMYCIN");
    //    corrected.insert("NEOMYCIN SULFATE", "NEOMYCIN");
    //    corrected.insert("ANTITHROMBIN III (HUMAN)", "ANTITHROMBIN III");
    //    corrected.insert("DOPAMINE HYDROCHLORIDE", "DOPAMINE");
    //    corrected.insert("HEPARIN (HEPARIN SODIUM)", "HEPARIN");
    //    corrected.insert("DIPHENOXYLATE HYDROCHLORIDE", "DIPHENOXYLATE");
    //    corrected.insert("PROGUANIL HYDROCHLORIDE", "PROGUANIL");
    //    corrected.insert("EPINEPHRINE BITARTRATE", "EPINEPHRINE");
    //    corrected.insert("DEXAMETHASONE ACETATE", "DEXAMETHASONE");
    //    corrected.insert("POLYMYXIN B (POLYMYXIN B SULFATE)", "POLYMYXIN B");
    //    corrected.insert("ENALAPRIL SODIUM", "ENALAPRIL");
    //    corrected.insert("NALOXONE (NALOXONE HYDROCHLORIDE DIHYDRATE)", "NALOXONE");
    //    corrected.insert("BUPRENORPHINE (BUPRENORPHINE HYDROCHLORIDE)", "BUPRENORPHINE");
    //    corrected.insert("TICARCILLIN (TICARCILLIN DISODIUM)", "TICARCILLIN");
    //    corrected.insert("THROMBIN (HUMAN)", "THROMBIN");
    //    corrected.insert("COAGULATION FACTOR VII (HUMAN)", "COAGULATION FACTOR VII");
    //    corrected.insert("COAGULATION FACTOR IX (HUMAN)", "COAGULATION FACTOR IX");
    //    corrected.insert("CAFFEINE CITRATE" ,"CAFFEINE" );
    //    corrected.insert("TRIPROLIDINE HYDROCHLORIDE" ,"TRIPROLIDINE" );

        // Associate Mol <-> ATC for drugs with one molecule only
        QMultiHash<int, int> mol_atc = Tools::englishMoleculeLinker(mols_id, corrected, correctedByAtcCode);

        // Save to links to drugs database
        ca.transaction();
        Tools::executeSqlQuery("DELETE FROM LK_MOL_ATC;", CA_DRUGS_DATABASE_NAME);
        foreach(int mol, mol_atc.uniqueKeys()) {
            foreach(int atc, mol_atc.values(mol)) {
                QString req = QString("INSERT INTO `LK_MOL_ATC` VALUES (%1, %2)").arg(mol).arg(atc);
                Tools::executeSqlQuery(req, CA_DRUGS_DATABASE_NAME);
            }
        }
        ca.commit();

        Utils::Log::addMessage(this, QString("Database processed"));

        return true;
    }
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
