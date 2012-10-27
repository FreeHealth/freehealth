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
#include "fdadrugsdatabasecreator.h"
#include "extramoleculelinkermodel.h"
#include "drug.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/isettings.h>

#include <drugsdb/tools.h>

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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QMultiHash>
#include <QHash>

#include <QDebug>

#include "ui_fdadrugsdatabasewidget.h"

using namespace DrugsDbCreator;
using namespace Trans::ConstantTranslations;

const char* const  FDA_URL                     = "http://www.fda.gov/downloads/Drugs/InformationOnDrugs/ucm054599.zip";
const char* const  FDA_DRUGS_DATABASE_NAME     = "FDA_US";

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/FdaRawSources/") + QDir::separator();}
static inline QString databaseAbsPath()  {return DrugsDB::Tools::drugsDatabaseAbsFileName();}
static inline QString freeDatabaseAbsPath()  {return DrugsDB::Tools::drugsDatabaseAbsFileName("free_fda");}
static inline QString fullFdaDatabaseAbsPath()  {return DrugsDB::Tools::drugsDatabaseAbsFileName("full_fda");}

static inline QString databaseDescriptionFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/drugdb/us/description.xml");}


FdaDrugsDatabasePage::FdaDrugsDatabasePage(QObject *parent) :
        IToolPage(parent)
{
    setObjectName("FdaDrugsDatabaseCreator_IToolPage");
}

QString FdaDrugsDatabasePage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_DRUGSDATABASE;
}

QWidget *FdaDrugsDatabasePage::createPage(QWidget *parent)
{
    return new FdaDrugsDatabaseWidget(parent);
}


FdaDrugDatatabaseStep::FdaDrugDatatabaseStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    m_WithProgress(false)
{
    setObjectName("FdaDrugDatatabaseStep");
}

FdaDrugDatatabaseStep::~FdaDrugDatatabaseStep()
{
}

bool FdaDrugDatatabaseStep::createDir()
{
    Utils::checkDir(workingPath(), true, "FdaDrugDatatabaseStep::createDir");
    Utils::checkDir(QFileInfo(databaseAbsPath()).absolutePath(), true, "FdaDrugDatatabaseStep::createDir");
    Utils::checkDir(QFileInfo(freeDatabaseAbsPath()).absolutePath(), true, "FdaDrugDatatabaseStep::createDir");
    Utils::checkDir(QFileInfo(fullFdaDatabaseAbsPath()).absolutePath(), true, "FdaDrugDatatabaseStep::createDir");
    return true;
}

bool FdaDrugDatatabaseStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool FdaDrugDatatabaseStep::downloadFiles(QProgressBar *bar)
{
    Utils::HttpDownloader *dld = new Utils::HttpDownloader(this);
//    dld->setMainWindow(mainwindow());
    dld->setProgressBar(bar);
    dld->setOutputPath(workingPath());
    dld->setUrl(QUrl(FDA_URL));
    dld->startDownload();
    connect(dld, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()));
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    connect(dld, SIGNAL(downloadProgressRange(qint64,qint64)), this, SIGNAL(progressRangeChanged(int,int)));
    connect(dld, SIGNAL(downloadProgressRead(qint64)), this, SIGNAL(progress(int)));
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
    Q_EMIT progressLabelChanged(tr("Unzipping downloaded files"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // check file
    QString fileName = workingPath() + QDir::separator() + QFileInfo(FDA_URL).fileName();
    if (!QFile(fileName).exists()) {
        LOG_ERROR(QString("No files founded."));
        LOG_ERROR(QString("Please download files."));
        return false;
    }

    LOG(QString("Starting unzipping FDA file %1").arg(fileName));

    // unzip files using QProcess
    return QuaZipTools::unzipFile(fileName, workingPath());
}

class Parser {
public:
    Parser(const QString &line)
    {
        // Process line
        QStringList vals = line.split("\t");
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
            route = forms.mid(begin).toLower().simplified();
            form = forms.left(begin - 2).toLower().simplified();
        } else {
            form = vals.at(2);
        }

        // Get strength
        globalStrength = vals.at(3);
        if (globalStrength.contains("*")) {
            int begin = globalStrength.indexOf("*");
            if (begin!=-1) {
                globalStrength = globalStrength.left(begin + 1);
            }
        }

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

    Drug *getDrug()
    {
        Drug *drug = new Drug;
        drug->setData(Drug::Uid1, uid1);
        drug->setData(Drug::Uid2, uid2);
        drug->setData(Drug::OldUid, uid1 + uid2);
        drug->setData(Drug::Name, name);
        drug->setData(Drug::Forms, form);
        drug->setData(Drug::Routes, route.split(","));
        drug->setData(Drug::Strength, globalStrength);
        drug->setData(Drug::Authorization, "");
        drug->setData(Drug::Marketed, "");
        drug->setData(Drug::Spc, "");
        drug->setData(Drug::Valid, 1);
        int i = 0;
        foreach(const QString &mol, mols_strength.keys()) {
            ++i;
            Component *compo = new Component;
            compo->setData(Component::Name, mol);
            compo->setData(Component::Nature, "SA");
            compo->setData(Component::NatureLink, i);
            compo->setData(Component::Strength, mols_strength.value(mol));
            compo->setData(Component::StrengthUnit, "");
            drug->addComponent(compo);
        }
        return drug;
    }

    bool operator<(const Parser &other) const
    {
        return this->name < other.name;
    }

    static bool lessThan(const Parser *s1, const Parser *s2)
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
    return true;
}

bool FdaDrugDatatabaseStep::createDatabase()
{
    if (!DrugsDB::Tools::createMasterDrugInteractionDatabase())
        return false;

    QMultiHash<QString, QVariant> labels;
    labels.insert("fr","Base de données thérapeutique américaine");
    labels.insert("en","USA therapeutic database");
    labels.insert("de","Therapeutische Datenbank USA");

    if (DrugsDB::Tools::createNewDrugsSource(Core::Constants::MASTER_DATABASE_NAME, FDA_DRUGS_DATABASE_NAME, labels) == -1) {
        LOG_ERROR("Unable to create the FDA drugs sources");
        return false;
    }
    DrugsDB::Tools::saveDrugDatabaseDescription(databaseDescriptionFile(), 0);
    LOG(QString("Database schema created"));
    return true;
}

bool FdaDrugDatatabaseStep::populateDatabase()
{
    if (!DrugsDB::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    Q_EMIT progressLabelChanged(tr("Reading downloaded files"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    QStringList files = QStringList()
                        << "Product.txt"
                        << "AppDoc.txt"
                        ;

    // check files
    foreach(const QString &file, files) {
        if (!QFile::exists(workingPath() + file)) {
            LOG_ERROR(QString("Missing " + workingPath() + file + " file. prepareDatas()"));
            return false;
        }
    }

    // Product file
    QVector<Drug *> drugs;

    QFile file(workingPath() + "Product.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("ERROR: Enable to open Product.txt: %1.").arg(file.errorString()));
        return false;
    }
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source"));
    Q_EMIT progressRangeChanged(0, file.size());
    Q_EMIT progress(0);

    QTextStream in(&file);
    in.setCodec("ISO 8859-1");
    int pos = 0;
    while (!in.atEnd()) {
        QString l = in.readLine();
        //ignore first line
        if (pos==0) {
            pos += l.length();
            continue;
        }
        pos += l.length();

        Parser parser(l);
        drugs << parser.getDrug();

        if (drugs.count() % 10 == 0) {
//            break;
            Q_EMIT progress(pos);
        }
    }
    file.close();

    Q_EMIT progressLabelChanged(tr("Saving drugs into database"));
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(1);

    Drug::saveDrugsIntoDatabase(Core::Constants::MASTER_DATABASE_NAME, drugs, FDA_DRUGS_DATABASE_NAME);
    Q_EMIT progress(2);

    qDeleteAll(drugs);
    drugs.clear();

    // Run SQL commands one by one
//    if (!DrugsDB::Tools::executeSqlFile(Core::Constants::MASTER_DATABASE_NAME, databaseFinalizationScript())) {
//        LOG_ERROR("Can create FDA DB.");
//        return false;
//    }

    DrugsDB::Tools::saveDrugDatabaseDescription(databaseDescriptionFile(), 50);

    LOG(QString("Database processed"));
    Q_EMIT progress(3);

    return true;
}

bool FdaDrugDatatabaseStep::linkMolecules()
{
    // 17 Feb 2012
    //    NUMBER OF MOLECULES 2033
    //    CORRECTED BY NAME 11
    //    CORRECTED BY ATC 0
    //    FOUNDED 1574 "
    //    LINKERMODEL (WithATC:250;WithoutATC:1) 251"
    //    LINKERNATURE 0
    //    LEFT 458
    //    CONFIDENCE INDICE 77

    // 28 Sept 2011 (using EN translations of ATC labels + new ATC 2011 && 2012)
    //    NUMBER OF MOLECULES 2014
    //    CORRECTED BY NAME 11
    //    CORRECTED BY ATC 0
    //    FOUNDED 1558 "
    //    LINKERMODEL (WithATC:261;WithoutATC:1) 262"
    //    LINKERNATURE 0
    //    LEFT 455
    //    CONFIDENCE INDICE 77

    // 28 APR 2011
    //    NUMBER OF MOLECULES 2000
    //    CORRECTED BY NAME 11
    //    CORRECTED BY ATC 0
    //    FOUNDED 1207 "
    //    LINKERMODEL (WithATC:259;WithoutATC:1) 260"
    //    LINKERNATURE 0
    //    LEFT 792
    //    CONFIDENCE INDICE 60

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
    if (!DrugsDB::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    // Get SID
    int sid = DrugsDB::Tools::getSourceId(Core::Constants::MASTER_DATABASE_NAME, FDA_DRUGS_DATABASE_NAME);
    if (sid==-1) {
        LOG_ERROR("NO SID DEFINED");
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

    Q_EMIT progressLabelChanged(tr("Linking drugs components to ATC codes"));
    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progress(0);

    // Associate Mol <-> ATC for drugs with one molecule only
    QStringList unfound;
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(FDA_DRUGS_DATABASE_NAME, "en", &unfound, corrected, QMultiHash<QString, QString>());
    qWarning() << "unfound" << unfound.count();

    Q_EMIT progress(1);

    Q_EMIT progressLabelChanged(tr("Saving components to ATC links to database"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // Save to links to drugs database
    DrugsDB::Tools::addComponentAtcLinks(Core::Constants::MASTER_DATABASE_NAME, mol_atc, sid);

    LOG(QString("Database processed"));

    // add unfound to extralinkermodel
    Q_EMIT progressLabelChanged(tr("Updating component link XML file"));
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(FDA_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    return true;
}



FdaDrugsDatabaseWidget::FdaDrugsDatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FdaDrugsDatabaseWidget),
    m_Step(0)
{
    setObjectName("FdaDrugsDatabaseWidget");
    ui->setupUi(this);
    ui->progressBar->hide();
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

bool FdaDrugsDatabaseWidget::on_download_clicked()
{
    ui->progressBar->show();
    m_Step->downloadFiles(ui->progressBar);
    connect(m_Step, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    return true;
}

void FdaDrugsDatabaseWidget::downloadFinished()
{
    ui->progressBar->hide();
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

