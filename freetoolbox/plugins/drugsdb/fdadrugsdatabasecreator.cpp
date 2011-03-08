/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
#include "drug.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>

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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QMultiHash>
#include <QHash>

#include <QDebug>

#include "ui_fdadrugsdatabasewidget.h"

using namespace DrugsDbCreator;

const char* const  FDA_URL                     = "http://www.fda.gov/downloads/Drugs/InformationOnDrugs/ucm054599.zip";
const char* const  FDA_DRUGS_DATABASE_NAME     = "FDA_US";

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/FdaRawSources/") + QDir::separator();}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::MASTER_DATABASE_FILENAME);}

static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/drugdb/us/us_db_finalize.sql");}



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
        Utils::Log::addError(this, QString("No files founded."), __FILE__, __LINE__);
        Utils::Log::addError(this, QString("Please download files."), __FILE__, __LINE__);
        return false;
    }

    Utils::Log::addMessage(this, QString("Starting unzipping FDA file %1").arg(fileName));

    // unzip files using QProcess
    return Core::Tools::unzipFile(fileName, workingPath());
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

    Drug *getDrug()
    {
        Drug *drug = new Drug;
        drug->setData(Drug::Uid1, uid1);
        drug->setData(Drug::Uid2, uid2);
        drug->setData(Drug::OldUid, uid1 + uid2);
        drug->setData(Drug::Name, name);
        drug->setData(Drug::Forms, form);
        drug->setData(Drug::Routes, route);
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

    void warn() {
        qWarning() << uid1 << uid2 << name << globalStrength << form << route << mols_strength;
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
    if (!Core::Tools::createMasterDrugInteractionDatabase())
        return false;

    QMultiHash<QString, QVariant> labels;
    labels.insert("fr","Base de données thérapeutiques américaine");
    labels.insert("en","USA therapeutic database");
    labels.insert("de","USA therapeutischen database");

    if (Core::Tools::createNewDrugsSource(Core::Constants::MASTER_DATABASE_NAME, FDA_DRUGS_DATABASE_NAME, labels) == -1) {
        Utils::Log::addError(this, "Unable to create the FDA drugs sources");
        return false;
    }

    Utils::Log::addMessage(this, QString("Database schema created"));
    return true;
}

bool FdaDrugDatatabaseStep::populateDatabase()
{
    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
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
            Utils::Log::addError(this, QString("Missing " + workingPath() + file + " file. prepareDatas()"), __FILE__, __LINE__);
            return false;
        }
    }

    // Product file
    QVector<Drug *> drugs;

    QFile file(workingPath() + "Product.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Utils::Log::addError(this, QString("ERROR : Enable to open Product.txt : %1.").arg(file.errorString()), __FILE__, __LINE__);
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
    if (!Core::Tools::executeSqlFile(Core::Constants::MASTER_DATABASE_NAME, databaseFinalizationScript())) {
        Utils::Log::addError(this, "Can create FDA DB.", __FILE__, __LINE__);
        return false;
    }
    Utils::Log::addMessage(this, QString("Database processed"));
    Q_EMIT progress(3);

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
    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    // Get SID
    int sid = Core::Tools::getSourceId(Core::Constants::MASTER_DATABASE_NAME, FDA_DRUGS_DATABASE_NAME);
    if (sid==-1) {
        Utils::Log::addError(this, "NO SID DEFINED", __FILE__, __LINE__);
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
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(FDA_DRUGS_DATABASE_NAME, "fr", &unfound, corrected, QMultiHash<QString, QString>());
    qWarning() << "unfound" << unfound.count();

    Q_EMIT progress(1);
    // Clear existing links
    QString req = QString("DELETE FROM LK_MOL_ATC WHERE SID=%1;").arg(sid);
    Core::Tools::executeSqlQuery(req, Core::Constants::MASTER_DATABASE_NAME, __FILE__, __LINE__);

    Q_EMIT progressLabelChanged(tr("Saving components to ATC links to database"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // Save to links to drugs database
    Core::Tools::addComponentAtcLinks(Core::Constants::MASTER_DATABASE_NAME, mol_atc, sid);

    Utils::Log::addMessage(this, QString("Database processed"));

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

