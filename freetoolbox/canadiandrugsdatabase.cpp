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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "canadiandrugsdatabase.h"
#include "globaltools.h"
#include "constants.h"
#include "mainwindow.h"
#include "extramoleculelinkermodel.h"

#include <utils/log.h>
#include <utils/httpdownloader.h>

#include <QDir>
#include <QProgressDialog>
#include <QTimer>

#include "ui_canadiandrugsdatabasewidget.h"


const char* const  CANADIAN_URL               = "http://www.hc-sc.gc.ca/dhp-mps/prodpharma/databasdon/txt/allfiles.zip";
const char* const  CANADIAN_WORKINGPATH       = "/../global_resources/tmp/CanadianRawSources/";

const char* const  CA_DRUGS_DATABASE_NAME     = "CA_HCDPD";
const char* const  CA_DRUGS_DATABASE_FILENAME =  "/../global_resources/databases/drugs/drugs-en_CA.db";

const char* const  FILE_CA_DRUGS_DB_SCRIPT1   = "/../global_resources/sql/canadian_db_preparation.sql";
const char* const  FILE_CA_DRUGS_DB_SCRIPT2   = "/../global_resources/sql/canadian_db_finalize.sql";



CanadianDrugsDatabasePage::CanadianDrugsDatabasePage(QObject *parent) :
        IToolPage(parent)
{
    setObjectName("CanadianDrugsDatabasePage");
}

QWidget *CanadianDrugsDatabasePage::createPage(QWidget *parent)
{
    return new CanadianDrugsDatabaseWidget(parent);
}


CanadianDrugsDatabaseWidget::CanadianDrugsDatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CanadianDrugsDatabaseWidget)
{
    setObjectName("CanadianDrugsDatabaseCreator");
    ui->setupUi(this);
    m_WorkingPath = QDir::cleanPath(qApp->applicationDirPath() + Constants::MACBUNDLE + CANADIAN_WORKINGPATH) + "/";
    if (!QDir().mkpath(m_WorkingPath))
        Utils::Log::addError(this, "Unable to create Canadian Working Path :" + m_WorkingPath, __FILE__, __LINE__);
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

bool CanadianDrugsDatabaseWidget::downloadFiles()
{
//    QString app = "";
//#ifdef Q_OS_MAC
//    app = QString("curl %1 -o \"%2\" -s").arg(CANADIAN_URL).arg(m_WorkingPath + QFileInfo(CANADIAN_URL).fileName());
//#else
//    app = QString("wget %1").arg(CANADIAN_URL);
//#endif
//    if (Tools::executeProcess(app)) {
//        m_FileName = m_WorkingPath + QFileInfo(CANADIAN_URL).fileName();
//        Utils::Log::addMessage(this, QString("Canadian file dowloaded without error %1").arg(m_FileName));
//    } else {
//        m_FileName = "";
//    }

    Utils::HttpDownloader *dld = new Utils::HttpDownloader(this);
    dld->setMainWindow(MainWindow::instance());
    dld->setOutputPath(m_WorkingPath);
    dld->setUrl(QUrl(CANADIAN_URL));
    dld->startDownload();

    return true;
}

bool CanadianDrugsDatabaseWidget::unzipFiles()
{
    // check file
    QString fileName = m_WorkingPath + QDir::separator() + QFileInfo(CANADIAN_URL).fileName();
    if (!QFile(fileName).exists()) {
        Utils::Log::addError(this, QString("No files founded."), __FILE__, __LINE__);
        Utils::Log::addError(this, QString("Please download files."), __FILE__, __LINE__);
        return false;
    }

    Utils::Log::addMessage(this, QString("Starting unzipping Canadian file %1").arg(fileName));

    // unzip downloaded using QProcess
    if (!Tools::unzipFile(fileName, m_WorkingPath))
        return false;

    // unzip all files in the working path
    QFileInfoList list = QDir(m_WorkingPath).entryInfoList(QStringList()<<"*.zip");
    foreach(const QFileInfo &info, list) {
        if (info.fileName()!="allfiles.zip") {
            if (!Tools::unzipFile(info.absoluteFilePath(), m_WorkingPath)) {
                Utils::Log::addError(this, "Unable to unzip " + info.absoluteFilePath(), __FILE__, __LINE__);
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
            Utils::Log::addError(this, QString("Missing " + m_WorkingPath + file + " file. prepareDatas()"), __FILE__, __LINE__);
            return false;
        }
    }

    // transform each files
    QProgressDialog progressDialog(MainWindow::instance());
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setLabelText(tr("Processing files"));
    progressDialog.show();
    progressDialog.setRange(0, files.count());

    foreach(const QString &file, files) {
        Utils::Log::addMessage(this, "Processing file :" + file);
        QString tmp;
        {
            QFile f(m_WorkingPath + file);
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                Utils::Log::addError(this, QString("ERROR : Enable to open %1 : %2. populateDatabase()").arg(file, f.errorString()), __FILE__, __LINE__);
                return false;
            }
            Utils::Log::addMessage(this, "Reading file: " + file);
            tmp = QString::fromLatin1(f.readAll());
        }

        QString out;

        const QStringList &lines = tmp.split("\n", QString::SkipEmptyParts);
        Utils::Log::addMessage(this, tr("Processing %1 lines %2").arg(lines.count()).arg(tmp.count("\n")));

        foreach(QString s, lines) {
            // prepare a better separator for the import command
            s.replace("\",\"", Constants::SEPARATOR);
            s.remove("\"");
            out += s + "\n";
        }

        // save file
        {
            QFile f(m_WorkingPath + QFileInfo(file).baseName() + ".csv");
            if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                Utils::Log::addError(this, QString("ERROR : Enable to open %1 : %2. populateDatabase()").arg(file, f.errorString()), __FILE__, __LINE__);
                return false;
            }
            Utils::Log::addMessage(this, "Saving file: " + f.fileName());
            f.write(out.toUtf8());
        }
        progressDialog.setValue(progressDialog.value()+1);
    }
    return true;
}

bool CanadianDrugsDatabaseWidget::createDatabase()
{
    if (!Tools::connectDatabase(CA_DRUGS_DATABASE_NAME, CA_DRUGS_DATABASE_FILENAME))
        return false;

    // create database structure
    if (!Tools::executeSqlFile(CA_DRUGS_DATABASE_NAME, Constants::MACBUNDLE + Constants::FILE_DRUGS_DATABASE_SCHEME)) {
        Utils::Log::addError(this, "Can create Canadian DB.", __FILE__, __LINE__);
        return false;
    }

    Utils::Log::addMessage(this, QString("Database schema created"));
    return true;
}

struct MinCompo {
    MinCompo(int id, const QString &name, const QString &s, const QString &su, const QString &d, const QString &du) :
            mol_id(id), mol(name), strength(s), strength_unit(su), dosage(d), dosage_unit(du)
    {}

    QString sqlDosage()
    {
        if (dosage.isEmpty()) {
            return strength + strength_unit;
        } else {
            return strength + strength_unit + "/" + dosage + dosage_unit;
        }
        return QString();
    }

    int mol_id;
    QString mol;
    QString strength, strength_unit, dosage, dosage_unit;
};

struct MinDrug {
    MinDrug(const QString &n, const QString &u) :
            name(n), uid(u)
    {}

    ~MinDrug()
    {
        qDeleteAll(compo);
        compo.clear();
    }

    QString globalStrength()
    {
        QString s;
        foreach(MinCompo *c, compo) {
            s += c->strength + c->strength_unit + ";";
        }
        s.chop(1);
        return s;
    }

    QString name, uid, atc;
    QVector<int> forms;
    QVector<int> routes;
    QVector<MinCompo *> compo;
};

bool CanadianDrugsDatabaseWidget::populateDatabase()
{
    if (!Tools::connectDatabase(CA_DRUGS_DATABASE_NAME, CA_DRUGS_DATABASE_FILENAME))
        return false;

    // create temporary database schema
    if (!Tools::executeSqlFile(CA_DRUGS_DATABASE_NAME, Constants::MACBUNDLE + FILE_CA_DRUGS_DB_SCRIPT1)) {
        Utils::Log::addError(this, "Can create Canadian DB.", __FILE__, __LINE__);
        return false;
    }

    // import files
    QStringList files = QStringList()
                        << "drug.csv"
                        << "form.csv"
                        << "route.csv"
                        << "status.csv"
                        << "ingred.csv"
                        << "ther.csv"
                        << "package.csv";

    QProgressDialog progressDialog(MainWindow::instance());
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setLabelText(tr("Processing SQL script (about 20 minutes) : STEP 1"));
    progressDialog.setRange(0, files.count() + 1);
    progressDialog.setValue(1);
    progressDialog.show();
    int count = 0;

    foreach(const QString &f, files) {
        if (!Tools::executeProcess(QString("sqlite3 -separator \"%1\" \"%2\" \".import \"%3\" %4\"")
            .arg(Constants::SEPARATOR, qApp->applicationDirPath() + Constants::MACBUNDLE + CA_DRUGS_DATABASE_FILENAME, m_WorkingPath + f , QFileInfo(f).baseName()))) {
            return false;
        }
        progressDialog.setValue(++count);
        qApp->processEvents();
    }

    // recreate drugs (C++ way)
    progressDialog.setLabelText(tr("Processing SQL script (about 20 minutes) : STEP 2"));
    progressDialog.setValue(1);
    progressDialog.setRange(0, 3);

    // remove "Veterinary" preparations
    if (!Tools::executeSqlQuery("DELETE FROM drug WHERE (CLASS = 'Veterinary');", CA_DRUGS_DATABASE_NAME, __FILE__, __LINE__))
        return false;

    QList<MinDrug *> drugs;
    QString req;
    QSqlDatabase ca = QSqlDatabase::database(CA_DRUGS_DATABASE_NAME);
    ca.transaction();
    QSqlQuery query(ca);
    // get routes
    QHash<int, QString> routes;
    req = QString("SELECT DISTINCT ROUTE_OF_ADMINISTRATION_CODE, ROUTE_OF_ADMINISTRATION FROM route;");
    if (query.exec(req)) {
        while (query.next()) {
            routes.insert(query.value(0).toInt(), query.value(1).toString());
        }
    } else {
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        return false;
    }
    progressDialog.setValue(1);

    // get forms
    QHash<int, QString> forms;
    req = QString("SELECT DISTINCT PHARM_FORM_CODE, PHARMACEUTICAL_FORM FROM form;");
    if (query.exec(req)) {
        while (query.next()) {
            forms.insert(query.value(0).toInt(), query.value(1).toString());
        }
    } else {
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        return false;
    }
    progressDialog.setValue(2);
    query.finish();

    // get drugs
    progressDialog.setLabelText(tr("Processing SQL script (about 20 minutes) : STEP 3"));
    progressDialog.setValue(1);
    req = "SELECT count(DISTINCT DRUG_CODE) FROM drug;";
    if (query.exec(req)) {
        if (query.next()) {
            progressDialog.setRange(0, query.value(0).toInt());
        }
    } else {
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        return false;
    }
    qWarning() << "Retreiving"<<count<<"drugs";
    query.finish();

    req = "SELECT DISTINCT DRUG_CODE, BRAND_NAME FROM drug ORDER BY BRAND_NAME;";
    count = 0;
    if (query.exec(req)) {
        while (query.next()) {
            MinDrug *dr = new MinDrug(query.value(1).toString(), query.value(0).toString());
            // get forms
            req = "SELECT PHARM_FORM_CODE FROM form WHERE DRUG_CODE="+dr->uid;
            QSqlQuery forms_routes(req, ca);
            if (forms_routes.isActive()) {
                while (forms_routes.next()) {
                    dr->forms.append(forms_routes.value(0).toInt());
                }
            } else {
                Utils::Log::addQueryError(this, forms_routes, __FILE__, __LINE__);
                return false;
            }
            forms_routes.finish();
            // get routes
            req = "SELECT ROUTE_OF_ADMINISTRATION_CODE FROM route WHERE DRUG_CODE="+dr->uid;
            if (forms_routes.exec(req)) {
                while (forms_routes.next()) {
                    dr->routes.append(forms_routes.value(0).toInt());
                }
            } else {
                Utils::Log::addQueryError(this, forms_routes, __FILE__, __LINE__);
                return false;
            }
            forms_routes.finish();
            // get ATC
            req = "SELECT TC_ATC_NUMBER FROM ther WHERE DRUG_CODE="+dr->uid;
            if (forms_routes.exec(req)) {
                while (forms_routes.next()) {
                    dr->atc = forms_routes.value(0).toString();
                }
            } else {
                Utils::Log::addQueryError(this, forms_routes, __FILE__, __LINE__);
                return false;
            }
            forms_routes.finish();
            // get Composition
            req = "SELECT ACTIVE_INGREDIENT_CODE, INGREDIENT, STRENGTH, STRENGTH_UNIT, DOSAGE_VALUE, DOSAGE_UNIT FROM ingred WHERE DRUG_CODE="+dr->uid;
            if (forms_routes.exec(req)) {
                while (forms_routes.next()) {
                    MinCompo *compo = new MinCompo(forms_routes.value(0).toInt(),
                                                   forms_routes.value(1).toString(),
                                                   forms_routes.value(2).toString(),
                                                   forms_routes.value(3).toString(),
                                                   forms_routes.value(4).toString(),
                                                   forms_routes.value(5).toString());
                    dr->compo.append(compo);
                }
            } else {
                Utils::Log::addQueryError(this, forms_routes, __FILE__, __LINE__);
                return false;
            }
            forms_routes.finish();

            // add to drugs list
            drugs.append(dr);
            ++count;
            if (count%10 == 0)
                progressDialog.setValue(count);
        }
    } else {
        Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
        return false;
    }
    progressDialog.setValue(count);
    query.finish();


    // recreate drugs
    QStringList commands;
    foreach(MinDrug *drug, drugs) {
        commands.clear();
        for(int i = 0; i < drug->forms.count(); ++i) {
            for(int j = 0; j < drug->routes.count(); ++j) {
                QString newUid = drug->uid + ";" + QString::number(drug->forms.at(i)) + ";" + QString::number(drug->routes.at(j));
                const QString &form = forms.value(drug->forms.at(i));
                const QString &route = routes.value(drug->routes.at(j));
                // insert this drug
                req = "INSERT INTO `DRUGS` (`UID`, `NAME`, `FORM`, `ROUTE`, `GLOBAL_STRENGTH`, `ATC`) VALUES (\n"
                      "  '" + newUid + "', \n" +
                      "  \"" + drug->name + "\", \n" +
                      "  '" + form + "', \n" +
                      "  '" + route + "', \n" +
                      "  '" + drug->globalStrength() + "', \n" +
                      "  '" + drug->atc + "'\n" +
                      ");\n";
                commands << req;

                // insert composition
                foreach(MinCompo *compo, drug->compo) {
                    req = "    INSERT INTO `COMPOSITION` (`UID`, `MOLECULE_FORM`, `MOLECULE_CODE`, `MOLECULE_NAME`, `DOSAGE`) VALUES (\n"
                          "      '" + newUid + "', \n" +
                          "      '" + form + "', \n" +
                          "      '" + QString::number(compo->mol_id) + "', \n" +
                          "      \"" + compo->mol + "\", \n" +
                          "      '" + compo->sqlDosage() + "'\n" +
                          "    );\n";

                    commands << req;
                }
            }
        }
        foreach(const QString &c, commands) {
            if (!query.exec(c)) {
                Utils::Log::addQueryError(this, query, __FILE__, __LINE__);
                return false;
            }
            query.finish();
        }
    }

    ca.commit();

    // Run SQL commands one by one
    if (!Tools::executeSqlFile(CA_DRUGS_DATABASE_NAME, Constants::MACBUNDLE + FILE_CA_DRUGS_DB_SCRIPT2, &progressDialog)) {
        Utils::Log::addError(this, "Can create Canadian DB.", __FILE__, __LINE__);
        return false;
    }

    return true;
}

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
            Utils::Log::addError(this, "Can not connect to CA_DB db : dc_Canadian_DrugsDatabaseCreator::populateDatabase()", __FILE__, __LINE__);
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
        req = "SELECT DISTINCT `MOLECULE_CODE`, `MOLECULE_NAME` FROM `COMPOSITION`;";
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
        QStringList unfound;
        QMultiHash<int, int> mol_atc = Tools::moleculeLinker(CA_DRUGS_DATABASE_NAME, "en", &unfound, mols_id, corrected, correctedByAtcCode);
        qWarning() << "unfound" << unfound.count();

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

        // add unfound to extralinkermodel
        ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(CA_DRUGS_DATABASE_NAME, unfound);
        ExtraMoleculeLinkerModel::instance()->saveModel();

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
