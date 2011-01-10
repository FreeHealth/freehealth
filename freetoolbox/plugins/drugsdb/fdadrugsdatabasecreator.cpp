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
#include "fdadrugsdatabasecreator.h"
#include "extramoleculelinkermodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>

#include <utils/log.h>
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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

#include <QDebug>

#include "ui_fdadrugsdatabasewidget.h"

using namespace DrugsDbCreator;

const char* const  FDA_URL                     = "http://www.fda.gov/downloads/Drugs/InformationOnDrugs/ucm054599.zip";
const char* const  FDA_DRUGS_DATABASE_NAME     =  "FDA_US";
const char* const  SEPARATOR                   = "|||";

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/FdaRawSources/") + QDir::separator();}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/drugs/drugs-en_US.db");}
static inline QString iamDatabaseAbsPath()  {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::IAM_DATABASE_FILENAME);}

static inline QString databaseCreationScript()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/usa_db_creator.sql");}
//static inline QString databasePreparationScript()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/usa_db_creator.sql");}
static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/usa_db_finalize.sql");}

static inline QString drugsDatabaseSqlSchema() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + QString(Core::Constants::FILE_DRUGS_DATABASE_SCHEME));}
static inline QString drugsRouteSqlFileName() {return settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + QString(Core::Constants::FILE_DRUGS_ROUTES);}

FdaDrugsDatabasePage::FdaDrugsDatabasePage(QObject *parent) :
        IToolPage(parent)
{
    setObjectName("FdaDrugsDatabaseCreator_IToolPage");
}

QWidget *FdaDrugsDatabasePage::createPage(QWidget *parent)
{
    return new FdaDrugsDatabaseWidget(parent);
}


FdaDrugDatatabaseStep::FdaDrugDatatabaseStep(QObject *parent) :
        m_WithProgress(false)
{
}

FdaDrugDatatabaseStep::~FdaDrugDatatabaseStep()
{
}

bool FdaDrugDatatabaseStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        Utils::Log::addError(this, "Unable to create FDA Working Path :" + workingPath(), __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath)) {
            Utils::Log::addError(this, "Unable to create FDA database output path :" + dbpath, __FILE__, __LINE__);
            m_Errors << tr("Unable to create FDA database output path :") + dbpath;
        } else {
            Utils::Log::addMessage(this, "Drugs database output dir created");
        }
    }
    return true;
}

bool FdaDrugDatatabaseStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool FdaDrugDatatabaseStep::downloadFiles()
{
    Utils::HttpDownloader *dld = new Utils::HttpDownloader(this);
//    dld->setMainWindow(mainwindow());
    dld->setOutputPath(workingPath());
    dld->setUrl(QUrl(FDA_URL));
    dld->startDownload();
    connect(dld, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()));
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    return true;
}

bool FdaDrugDatatabaseStep::process()
{
    unzipFiles();
    prepareDatas();
    createDatabase();
    populateDatabase();
    linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool FdaDrugDatatabaseStep::unzipFiles()
{
    // check file
    QString fileName = workingPath() + QDir::separator() + QFileInfo(FDA_URL).fileName();
    if (!QFile(fileName).exists()) {
        Utils::Log::addError(this, QString("No files founded."), __FILE__, __LINE__);
        Utils::Log::addError(this, QString("Please download files."), __FILE__, __LINE__);
        return false;
    }

    Utils::Log::addMessage(this, QString("Starting unzipping FDA file %1").arg(fileName));

    // unzip files using QProcess
    return Core::Tools::unzipFile(fileName, workingPath());
}

class drug {
public:
    drug(const QString &line)
    {
        // Process line
        QStringList vals = line.split(SEPARATOR);
        if (vals.count() != 9) {
            qWarning() << "Error with line" << line;
        }

        // Get uid
        uid1 = vals.at(0);
        uid2 = vals.at(1);

        // Get form
        QString forms = vals.at(2);
        if (forms.contains(";")) {
            int begin = forms.lastIndexOf(";") + 2;
            route = forms.mid(begin);
            form = forms.left(begin - 2);
        } else {
            form = vals.at(2);
        }

        // Get strength
        globalStrength = vals.at(3);

        // Get composition
        if (globalStrength.contains(";")) {
            QStringList strengths;
            if (globalStrength.contains("(")) {
                QString t = globalStrength;
                int b = globalStrength.indexOf("(");
                int e = globalStrength.indexOf(")", b);
                t.remove(b, e-b);
                strengths = t.split(";");
            } else {
                strengths = globalStrength.split(";");
            }
            QStringList mols = vals.at(8).split(";");
            if (strengths.count() != mols.count()) {

                qWarning() << line;
                // NPLATE

//                if (mols.count() == 1) {
//                    mols_strength.insert(mols.at(0).simplified(), strengths.join(";"));
//                    mols.clear();
//                    strengths.clear();
//                } else {
                    for(int i = 0; i < mols.count(); ++i) {
                        mols_strength.insert(mols.at(i).simplified(), "");
                    }
                    mols.clear();
                    strengths.clear();
//                }
            }
            for(int i = 0; i < mols.count(); ++i) {
                mols_strength.insert(mols.at(i).simplified(), strengths.at(i));
            }
        } else {
            mols_strength.insert(vals.at(8).simplified(), vals.at(3));
        }



        // Get drug brand name
        name = vals.at(7);
    }

    void warn() {
        qWarning() << uid1 << uid2 << name << globalStrength << form << route << mols_strength;
    }

    bool operator<(const drug &other) const
    {
        return this->name < other.name;
    }

    static bool lessThan(const drug *s1, const drug *s2)
     {
         return s1->name < s2->name;
     }

    QString name, uid1, uid2;
    QString globalStrength;
    QString form;
    QString route;
    QHash<QString, QString> mols_strength;
};

bool FdaDrugDatatabaseStep::prepareDatas()
{
    QStringList files = QStringList()
                        << "Product.txt"
                        << "AppDoc.txt"
                        ;

    {
        QProgressDialog *progress = 0;
        if (m_WithProgress) {
            progress = new QProgressDialog("Processing FDA files : preparing files", "Abort", 0, 2, qApp->activeWindow());
            progress->setWindowModality(Qt::WindowModal);
        }

        // check files
        foreach(const QString &file, files) {
            if (!QFile::exists(workingPath() + file)) {
                Utils::Log::addError(this, QString("Missing " + workingPath() + file + " file. prepareDatas()"), __FILE__, __LINE__);
                return false;
            }
        }

        // transform each files
        int i = 0;
        foreach(const QString &file, files) {
            if (m_WithProgress) {
                progress->setValue(i++);
            }
            Utils::Log::addMessage(this, "Processing file :" + file);
            QString tmp;
            {
                QFile f(workingPath() + file);
                if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    Utils::Log::addError(this, QString("ERROR : Enable to open %1 : %2. populateDatabase()").arg(file, f.errorString()), __FILE__, __LINE__);
                    return false;
                }
                Utils::Log::addMessage(this, "Reading file");
                tmp = QString::fromLatin1(f.readAll());
            }

            // Remove first line
            tmp.remove(0, tmp.indexOf("\n")+1);
            // prepare a better separator for the import command
            Utils::Log::addMessage(this, "Replacing separators");
            tmp.replace("\t", SEPARATOR);

            // save file
            {
                QFile f(workingPath() + file);
                if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    Utils::Log::addError(this, QString("ERROR : Enable to open %1 : %2. populateDatabase()").arg(file, f.errorString()), __FILE__, __LINE__);
                    return false;
                }
                Utils::Log::addMessage(this, "Saving file");
                f.write(tmp.toUtf8());
            }
        }
        if (m_WithProgress) {
            progress->close();
            delete progress;
            progress = 0;
        }
    }

    // Now prepare the Products.txt file
    QString content;
    {
        QFile f(workingPath() + "/Product.txt");
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open %1 : %2. populateDatabase()").arg("Product.txt", f.errorString()), __FILE__, __LINE__);
            return false;
        }
        Utils::Log::addMessage(this, "Reading file");
        content = QString::fromLatin1(f.readAll());
    }
    QList<drug *> drugs;
    // Form of the input file
    // "004589","004","INJECTABLE; INJECTION","5ML/100ML;5GM/100ML",1,"AP",1,"ALCOHOL 5% AND DEXTROSE 5%","ALCOHOL; DEXTROSE"
    QStringList lines = content.split("\n");

    QProgressDialog *progress = 0;
    if (m_WithProgress) {
        progress = new QProgressDialog("Processing FDA files : reading files", "Abort", 0, lines.count(), qApp->activeWindow());
        progress->setWindowModality(Qt::WindowModal);
    }

    int i = 0;
    foreach(const QString &line, lines) {
        ++i;
        if (m_WithProgress) {
            if (i % 10 == 0)
                progress->setValue(i);
        }

        if (line.isEmpty())
            continue;

        // get the drug contents
        drug *dr = new drug(line);
        drugs << dr;
    }

    qSort(drugs.begin(), drugs.end(), drug::lessThan);

    // Recreate csv files ready for importation
    // Table DRUGS
//    `UID` int(11) NOT NULL,
//    `NAME` varchar(1000) NOT NULL,
//    `FORM` varchar(500),
//    `ROUTE` varchar(100),
//    `ATC` varchar(7),
//    `GLOBAL_STRENGTH` varchar(40),
//    `TYPE_MP` varchar(1),
//    `AUTHORIZATION` varchar(1),
//    `MARKETED` bool NOT NULL DEFAULT 1,
//    `LINK_SPC` varchar(250)
    /**
      \todo MARKETED, SPC
      \todo Commit to database directly
     */
    QString tableDrugs;
    foreach(drug *dr, drugs) {
        tableDrugs += dr->uid1 + dr->uid2 + SEPARATOR + dr->name + SEPARATOR + dr->form + SEPARATOR +
                      dr->route + SEPARATOR + "" + SEPARATOR + dr->globalStrength + SEPARATOR +
                      "" + SEPARATOR + "" + SEPARATOR + "1" + SEPARATOR + "\n";
    }
    // save file
    {
        QFile f(workingPath() + "FDA_DRUGS.CSV");
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open %1 : %2. populateDatabase()").arg(f.fileName(), f.errorString()), __FILE__, __LINE__);
            return false;
        }
        Utils::Log::addMessage(this, "Saving file");
        f.write(tableDrugs.toUtf8());
    }
    tableDrugs.clear();

    // Table COMPOSITION
    QString tableComposition;
//    `UID` int(10) NOT NULL,
//    `MOLECULE_FORM` varchar(100),
//    `MOLECULE_CODE` int(11) NOT NULL,
//    `MOLECULE_NAME` varchar(200) NOT NULL,
//    `MOLECULE_ATC`
//    `DOSAGE` varchar(100)  NOT NULL,
//    `DOSAGE_REF` varchar(50)  NOT NULL,
//    `NATURE` varchar(2) NOT NULL DEFAULT "SA",
//    `LK_NATURE` int(11) NOT NULL DEFAULT 1

    i = 0;
    // recreate an fictive code_molecule
    QHash<QString, int> codemols;
    foreach(drug *dr, drugs) {
        foreach(const QString &mol, dr->mols_strength.keys()) {
            if (codemols.keys().contains(mol))
                continue;
            codemols.insert(mol, ++i);
        }
    }

    // now create the CSV
    foreach(drug *dr, drugs) {
        int lknature = 1;
        foreach(const QString &mol, dr->mols_strength.keys()) {
            /** \todo DOSAGE REF can be improved */
            tableComposition += dr->uid1 + dr->uid2 + SEPARATOR + dr->form + SEPARATOR + QString::number(codemols.value(mol)) + SEPARATOR +
                                mol + SEPARATOR + SEPARATOR + dr->mols_strength.value(mol) + SEPARATOR + "" + SEPARATOR + "SA" + SEPARATOR +
                                QString::number(lknature) + "\n";
            ++lknature;
        }
    }
    // save file
    {
        QFile f(workingPath() + "FDA_COMPOSITION.CSV");
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to open %1 : %2. FdaDrugsDatabaseWidget::populateDatabase()").arg(f.fileName(), f.errorString()), __FILE__, __LINE__);
            return false;
        }
        Utils::Log::addMessage(this, "Saving file");
        f.write(tableComposition.toUtf8());
    }
    tableComposition.clear();


    /** \todo PACKAGING */

    if (m_WithProgress) {
        delete progress;
        progress = 0;
    }

    qDeleteAll(drugs);
    drugs.clear();

    return true;

}

bool FdaDrugDatatabaseStep::createDatabase()
{
    if (!Core::Tools::connectDatabase(FDA_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    // create database structure
    if (!Core::Tools::executeSqlFile(FDA_DRUGS_DATABASE_NAME, drugsDatabaseSqlSchema())) {
        Utils::Log::addError(this, "Can not create FDA DB.", __FILE__, __LINE__);
        return false;
    }

    Utils::Log::addMessage(this, QString("Database schema created"));
    return true;
}

bool FdaDrugDatatabaseStep::populateDatabase()
{
    if (!Core::Tools::connectDatabase(FDA_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    // create temporary database schema
    if (!Core::Tools::executeSqlFile(FDA_DRUGS_DATABASE_NAME, databaseCreationScript())) {
        Utils::Log::addError(this, "Can create FDA DB.", __FILE__, __LINE__);
        return false;
    }

    // import files
    QStringList files = QStringList()
                        << "FDA_DRUGS.CSV"
                        << "FDA_COMPOSITION.CSV"
                        ;

    QProgressDialog *progress = 0;
    if (m_WithProgress) {
        progress = new QProgressDialog("Feeding database", "Abort", 0, 3, qApp->activeWindow());
        progress->setWindowModality(Qt::WindowModal);
        progress->setValue(0);
        progress->show();
    }

    if (!Utils::Database::importCsvToDatabase(FDA_DRUGS_DATABASE_NAME, workingPath() + "FDA_DRUGS.CSV", "DRUGS", SEPARATOR)) {
        return false;
    }
    if (m_WithProgress) {
        progress->setValue(1);
    }
    if (!Utils::Database::importCsvToDatabase(FDA_DRUGS_DATABASE_NAME, workingPath() + "FDA_COMPOSITION.CSV", "COMPOSITION", SEPARATOR)) {
        return false;
    }
    if (m_WithProgress) {
        progress->setValue(2);
        progress->setLabelText(tr("Processing SQL script (about 10 minutes)"));
    }

    // Run SQL commands one by one
    if (!Core::Tools::executeSqlFile(FDA_DRUGS_DATABASE_NAME, databaseFinalizationScript())) {
        Utils::Log::addError(this, "Can create FDA DB.", __FILE__, __LINE__);
        return false;
    }
    if (m_WithProgress) {
        progress->setValue(3);
    }

    Utils::Log::addMessage(this, QString("Database processed"));

    if (m_WithProgress) {
        delete progress;
        progress = 0;
    }
    return true;
}

bool FdaDrugDatatabaseStep::linkMolecules()
{
    // 04 Dec 2010
    //    NUMBER OF MOLECULES 1983
    //    CORRECTED BY NAME 11
    //    CORRECTED BY ATC 0
    //    FOUNDED 1343
    //    LINKERMODEL 6
    //    LINKERNATURE 0
    //    LEFT 640

    // 13 Nov 2010
    //    NUMBER OF MOLECULES 1983
    //    CORRECTED BY NAME 11
    //    CORRECTED BY ATC 0
    //    FOUNDED 1337
    //    LINKERMODEL 0
    //    LINKERNATURE 0
    //    LEFT 646


    // 28 July 2010
    // 1960 distinct mols
    // Hand association : 20
    // Found : 1349, Left: 612

    // Connect to databases
    if (!Core::Tools::connectDatabase(Core::Constants::IAM_DATABASE_NAME, iamDatabaseAbsPath()))
        return false;

    if (!Core::Tools::connectDatabase(FDA_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    // get all drugs
    QSqlDatabase us = QSqlDatabase::database(FDA_DRUGS_DATABASE_NAME);
    if (!us.open()) {
        Utils::Log::addError(this, "Can not connect to FDA db : populateDatabase()", __FILE__, __LINE__);
        return false;
    }

    // Associate Mol <-> ATC for drugs with one molecule only
    QHash<QString, QString> corrected;
    corrected.insert("IOTHALAMATE", "SODIUM IOTHALAMATE (125I)");
    corrected.insert("IOTHALAMATE SODIUM I-125" , "SODIUM IOTHALAMATE (125I)");
    corrected.insert("POLYMYXIN" ,"POLYMYXIN B" );
    corrected.insert("POLYMYXIN B SULFATE" ,"POLYMYXIN B" );
    corrected.insert("THIAMINE", "THIAMINE (VIT B1)");
    corrected.insert("GRISEOFULVIN, ULTRAMICROCRYSTALLINE" ,"GRISEOFULVIN");
    corrected.insert("GONADOTROPIN, CHORIONIC" ,"CHORIONIC GONADOTROPIN" );
    corrected.insert("TYROPANOATE SODIUM" ,"TYROPANOIC ACID" );
    corrected.insert("SODIUM NITROPRUSSIDE","NITROPRUSSIDE");

    // imported from Ca db
    corrected.insert("IOXAGLATE SODIUM" ,"IOXAGLIC ACID");
    corrected.insert("IOXAGLATE MEGLUMINE", "IOXAGLIC ACID");

    // Associate Mol <-> ATC for drugs with one molecule only
    QStringList unfound;
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(FDA_DRUGS_DATABASE_NAME, "en", &unfound, corrected, QMultiHash<QString, QString>());
    qWarning() << "unfound" << unfound.count();

    // Save to links to drugs database
    us.transaction();
    Core::Tools::executeSqlQuery("DELETE FROM LK_MOL_ATC;", FDA_DRUGS_DATABASE_NAME);
    foreach(int mol, mol_atc.uniqueKeys()) {
        QList<int> atcCodesSaved;
        foreach(int atc, mol_atc.values(mol)) {
            if (atcCodesSaved.contains(atc))
                continue;
            atcCodesSaved.append(atc);
            QString req = QString("INSERT INTO `LK_MOL_ATC` VALUES (%1, %2)").arg(mol).arg(atc);
            Core::Tools::executeSqlQuery(req, FDA_DRUGS_DATABASE_NAME);
        }
    }
    us.commit();

    // add unfound to extralinkermodel
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(FDA_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    if (!Core::Tools::signDatabase(FDA_DRUGS_DATABASE_NAME))
        Utils::Log::addError(this, "Unable to tag database.", __FILE__, __LINE__);

    Utils::Log::addMessage(this, QString("Database processed"));

    return true;
}



FdaDrugsDatabaseWidget::FdaDrugsDatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FdaDrugsDatabaseWidget),
    m_Step(0)
{
    setObjectName("FdaDrugsDatabaseWidget");
    ui->setupUi(this);
    m_Step = new FdaDrugDatatabaseStep(this);
    pluginManager()->addObject(m_Step);
}

FdaDrugsDatabaseWidget::~FdaDrugsDatabaseWidget()
{
    pluginManager()->removeObject(m_Step);
    delete ui;
}

void FdaDrugsDatabaseWidget::on_startJobs_clicked()
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
    if (ui->linkMols->isChecked()) {
        if (m_Step->linkMolecules())
            ui->linkMols->setText(ui->linkMols->text() + " CORRECTLY DONE");
    }
    Utils::Log::messagesToTreeWidget(ui->messages);
    Utils::Log::errorsToTreeWidget(ui->errors);
}

bool FdaDrugsDatabaseWidget::on_download_clicked()
{
    m_Step->downloadFiles();
    connect(m_Step, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    return true;
}

void FdaDrugsDatabaseWidget::downloadFinished()
{
    ui->download->setEnabled(true);
}

void FdaDrugsDatabaseWidget::changeEvent(QEvent *e)
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

