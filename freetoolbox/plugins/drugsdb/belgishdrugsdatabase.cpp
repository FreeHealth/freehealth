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
#include "belgishdrugsdatabase.h"
#include "extramoleculelinkermodel.h"
#include "drug.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/global.h>
#include <utils/log.h>
#include <utils/database.h>
#include <quazip/global.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>

#include <QDir>
#include <QFile>
#include <QProgressDialog>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "ui_belgishdrugsdatabase.h"

// Download at : http://www.fagg-afmps.be/fr/items-HOME/Bases_de_donnees/index.jsp

using namespace DrugsDbCreator;
using namespace Trans::ConstantTranslations;

const char* const  BE_DRUGS_DATABASE_NAME     = "FAGG_AFMPS_BE";


static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/BeRawSources/");}
static inline QString databaseAbsPath()  {return Core::Tools::drugsDatabaseAbsFileName();}
static inline QString tmpDatabaseAbsPath() {return QDir::cleanPath(workingPath() + "/drugs-be.db");}

static inline QString dumpFileAbsPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/drugdb/be/dump.zip");}

static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/drugdb/be/be_db_finalize.sql");}

QString BeDrugsDatabasePage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_DRUGSDATABASE;
}

QWidget *BeDrugsDatabasePage::createPage(QWidget *parent)
{
    return new BelgishDrugsDatabase(parent);
}

BelgishDrugsDatabase::BelgishDrugsDatabase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BelgishDrugsDatabase),
    m_Step(0)
{
    ui->setupUi(this);
    m_Step = new BeDrugDatatabaseStep(this);
    m_Step->createDir();
    pluginManager()->addObject(m_Step);
}

BelgishDrugsDatabase::~BelgishDrugsDatabase()
{
    pluginManager()->removeObject(m_Step);
    delete ui;
}

void BelgishDrugsDatabase::on_startJobs_clicked()
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
            if (m_Step->prepareData())
                ui->unzip->setText(ui->unzip->text() + " CORRECTLY DONE");
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
//    Utils::Log::messagesToTreeWidget(ui->messages);
//    Utils::Log::errorsToTreeWidget(ui->errors);
}

void BelgishDrugsDatabase::changeEvent(QEvent *e)
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



BeDrugDatatabaseStep::BeDrugDatatabaseStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    m_WithProgress(false)
{
    setObjectName("BeDrugDatatabaseStep");
}

BeDrugDatatabaseStep::~BeDrugDatatabaseStep()
{
}

bool BeDrugDatatabaseStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        LOG_ERROR("Unable to create Be Working Path :" + workingPath());
    else
        LOG("Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath)) {
            LOG_ERROR("Unable to create Be database output path :" + dbpath);
            m_Errors << tr("Unable to create Be database output path :") + dbpath;
        } else {
            LOG("Drugs database (Be) output dir created");
        }
    }
    return true;
}

bool BeDrugDatatabaseStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool BeDrugDatatabaseStep::downloadFiles(QProgressBar *bar)
{
    Q_UNUSED(bar);
    Q_EMIT downloadFinished();
    return true;
}

bool BeDrugDatatabaseStep::process()
{
    unzipFiles();
    prepareData();
    createDatabase();
    populateDatabase();
    linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool BeDrugDatatabaseStep::unzipFiles()
{
    Q_EMIT progressLabelChanged(tr("Unzipping raw source files"));
    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progress(0);

    // check file
    QString fileName = dumpFileAbsPath();
    if (!QFile(fileName).exists()) {
        LOG_ERROR(QString("No files founded."));
        LOG_ERROR(QString("Please download files."));
        Utils::warningMessageBox(tr("This assistant can not dowload and prepare the raw belguish drugs database."),
                                 tr("This step (download) must be done by hand. Please refer to the user manual."));
        return false;
    }

    LOG(QString("Starting unzipping Belguish dump file %1").arg(fileName));

    // unzip downloaded using QProcess
    if (!QuaZipTools::unzipFile(fileName, workingPath()))
        return false;

    Q_EMIT progress(1);

    return true;
}

bool BeDrugDatatabaseStep::prepareData()
{
    Q_EMIT progressLabelChanged(tr("Preparing raw source files"));

    if (!QFile(workingPath() + "/dump.sql").exists()) {
        Utils::warningMessageBox(tr("No dump file found"), tr("Try to execute the unzip step."));
        return false;
    }

    // Dump is MySQL specific --> need some adjustement to feet the SQLite requirements
    QFile fileIn(workingPath() + "/dump.sql");
    if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QFile fileOut(workingPath() + "/dump_sqlite.sql");
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    Q_EMIT progressRangeChanged(0, fileIn.size());
    Q_EMIT progress(0);

    QTextStream in(&fileIn);
    QTextStream out(&fileOut);

    out << "BEGIN TRANSACTION;\n";

    int pos = 0;
    int i = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        pos += line.length();
        ++i;

        // line start with '#' --> '--'
        if (line.startsWith('#'))
            continue;

        // line start with SET --> '--'
        if (line.startsWith("SET"))
            continue;

        if (line.startsWith("DROP DATABASE"))
            continue;
        if (line.startsWith("USE "))
            continue;
        if (line.startsWith("CREATE DATABASE"))
            continue;

        // line contains \' --> ''
        line = line.replace("\\'","''");

        // ENGINE=myisam DEFAULT CHARSET=utf8  --> remove
        line = line.replace("ENGINE=myisam DEFAULT CHARSET=utf8","");

        out << line << "\n";

        if (i % 10 == 0)
            Q_EMIT progress(pos);
    }

    out << "COMMIT;\n";

    return true;
}

bool BeDrugDatatabaseStep::createDatabase()
{
    Q_EMIT progressLabelChanged(tr("Preparing raw source files"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    if (!Core::Tools::createMasterDrugInteractionDatabase())
        return false;

    QMultiHash<QString, QVariant> labels;
    labels.insert("fr","Base de données thérapeutique belge");
    labels.insert("en","Belgium therapeutic database");
    labels.insert("de","Belgische Therapeutische Datenbank");

    if (Core::Tools::createNewDrugsSource(Core::Constants::MASTER_DATABASE_NAME, BE_DRUGS_DATABASE_NAME, labels) == -1) {
        LOG_ERROR("Unable to create the BE drugs sources");
        return false;
    }

    LOG(QString("Database schema created"));
    Q_EMIT progress(1);
    return true;
}

bool BeDrugDatatabaseStep::populateDatabase()
{
    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    Q_EMIT progressLabelChanged(tr("Reading raw sources..."));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // Create temp database
    const char *const BE_TMP_DB = "TmpBeDb";

    if (!Core::Tools::connectDatabase(BE_TMP_DB, tmpDatabaseAbsPath()))
        return false;

    QSqlDatabase be = QSqlDatabase::database(BE_TMP_DB);
    if (!be.isOpen()) {
        if (!be.open()) {
            LOG_ERROR("Unable to connect db");
            return false;
        }
    }

//    if (!Utils::Database::executeSqlFile(BE_TMP_DB, workingPath() + "/dump_sqlite.sql")) {
//        LOG_ERROR("Can not create BE DB.");
//        return false;
//    }

    be.transaction();
    QSqlQuery query(be);
    QString req;

    // Get drugs
    QString lastUid;
    QVector<Drug *> drugs;
    int nb = 0;
    req = "SELECT COUNT(*) FROM Tbl_AMM_H;";
    if (query.exec(req)) {
        if (query.next()) {
            nb = query.value(0).toInt();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }
    query.finish();

    Q_EMIT progressLabelChanged(tr("Creating %1 drugs").arg(nb));

    req = "SELECT `Registratienummer`, `mp_name`, "
          "`PharmFormFr`, `GenFr`, "
          "`PharmFormEn`, `GenEn`, "
          "`PharmFormDe`, `GenDe`, "
          "`PharmFormNl`, `GenNl`, "
          "`cti` "
          "FROM Tbl_AMM_H ORDER BY `mp_name`, `Registratienummer` "
          //          "LIMIT 100;"
          ;
    if (query.exec(req)) {
        Q_EMIT progressRangeChanged(0, nb);
        Q_EMIT progress(0);

        int pos = 0;
        while (query.next()) {
            pos ++;
            if (pos % 50 == 0)
                Q_EMIT progress(pos);

            if (query.value(0).toString()==lastUid) {
                continue;
            }
            lastUid = query.value(0).toString();

            Drug *drug = new Drug;
            drug->setData(Drug::Name, query.value(1).toString());
            drug->setData(Drug::Uid1, query.value(0).toString());
            drug->setData(Drug::Marketed, 1);
            drug->setData(Drug::Valid, 1);

            drug->setData(Drug::Forms, query.value(2).toString(), "fr");
            drug->setData(Drug::Authorization, query.value(3).toString(), "fr");

            drug->setData(Drug::Forms, query.value(4).toString(), "en");
            drug->setData(Drug::Authorization, query.value(5).toString(), "en");

            drug->setData(Drug::Forms, query.value(6).toString(), "de");
            drug->setData(Drug::Authorization, query.value(7).toString(), "de");

            drug->setData(Drug::Forms, query.value(8).toString(), "nl");
            drug->setData(Drug::Authorization, query.value(9).toString(), "nl");

            drugs << drug;

            QString req2 = QString("SELECT `ActSubst_Name`, `unit`, `dosis` FROM Tbl_AMM_det_H "
                                       "WHERE `cti`='%1';").arg(query.value(10).toString());
            QSqlQuery compoQuery(be);
            int natureLink = 0;
            if (compoQuery.exec(req2)) {
                while (compoQuery.next()) {
                    ++natureLink;
                    Component *compo = new Component;
                    compo->setData(Component::Name, compoQuery.value(0).toString());
                    compo->setData(Component::Strength, compoQuery.value(1).toString());
                    compo->setData(Component::Nature, "SA");
                    compo->setData(Component::NatureLink, natureLink);
                    drug->addComponent(compo);
                }
            } else {
                LOG_QUERY_ERROR(compoQuery);
            }
            compoQuery.finish();
        }
    } else {
        LOG_QUERY_ERROR(query);
    }

    be.commit();

//    -- route=PackFr
//    -- autoriation de délivrance=DelivFr

    Q_EMIT progressLabelChanged(tr("Saving drugs into database"));
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(1);

    // Save into database
    Drug::saveDrugsIntoDatabase(Core::Constants::MASTER_DATABASE_NAME, drugs, BE_DRUGS_DATABASE_NAME);
    Q_EMIT progress(2);

    qDeleteAll(drugs);
    drugs.clear();

    // Run SQL commands one by one
    if (!Core::Tools::executeSqlFile(Core::Constants::MASTER_DATABASE_NAME, databaseFinalizationScript())) {
        LOG_ERROR("Can create FDA DB.");
        return false;
    }
    Q_EMIT progress(3);

    LOG(QString("Database processed"));

    return true;
}

bool BeDrugDatatabaseStep::linkMolecules()
{
    // 29 Sept 2011
    //    NUMBER OF MOLECULES 2147
    //    CORRECTED BY NAME 1
    //    CORRECTED BY ATC 0
    //    FOUNDED 1394 "
    //    LINKERMODEL (WithATC:404;WithoutATC:13) 417"
    //    LINKERNATURE 0
    //    LEFT 740
    //    CONFIDENCE INDICE 65

    // 29 APR 2011
    //    NUMBER OF MOLECULES 2147
    //    CORRECTED BY NAME 1
    //    CORRECTED BY ATC 0
    //    FOUNDED 1353 "
    //    LINKERMODEL (WithATC:659;WithoutATC:13) 672"
    //    LINKERNATURE 0
    //    LEFT 781
    //    CONFIDENCE INDICE 63

    // 10 Jan 2011
    //    NUMBER OF MOLECULES 2146
    //    CORRECTED BY NAME 1
    //    CORRECTED BY ATC 0
    //    FOUNDED 1351 "
    //    LINKERMODEL (WithATC:659;WithoutATC:13) 672"
    //    LINKERNATURE 0
    //    LEFT 782
    //    CONFIDENCE INDICE 63

    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    // Get SID
    int sid = Core::Tools::getSourceId(Core::Constants::MASTER_DATABASE_NAME, BE_DRUGS_DATABASE_NAME);
    if (sid==-1) {
        LOG_ERROR("NO SID DEFINED");
        return false;
    }

    // Associate Mol <-> ATC for drugs with one molecule only
    QHash<QString, QString> corrected;
    corrected.insert("HYPERICUM PERFORATUM (MILLEPERTUIS)", "MILLEPERTUIS");

    Q_EMIT progressLabelChanged(tr("Linking drugs components to ATC codes"));
    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progress(0);

    // Associate Mol <-> ATC for drugs with one molecule only
    QStringList unfound;
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(BE_DRUGS_DATABASE_NAME, "fr", &unfound, corrected, QMultiHash<QString, QString>());
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
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(BE_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    return true;
}

