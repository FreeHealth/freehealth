/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2016 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  along with this program.                                               *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "portuguesedrugsdatabase.h"
#include "extramoleculelinkermodel.h"
#include "drug.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>

#include <utils/global.h>
#include <utils/log.h>
#include <utils/database.h>
#include <utils/httpdownloader.h>

#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>

#include <QDir>
#include <QFile>
#include <QProgressDialog>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "ui_portuguesedrugsdatabase.h"

using namespace DrugsDbCreator;
using namespace Trans::ConstantTranslations;

const char* const  PT_DRUGS_DATABASE_NAME     = "INFARMED_PT";
const char* const  PT_URL = "http://www.infarmed.pt/infomed/lista_excel_public.php?nome_comer=%";

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/PtRawSources/");}
static inline QString databaseAbsPath()  {return DrugsDB::Tools::drugsDatabaseAbsFileName();}
static inline QString rawCsvAbsFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/drugdb/pt/lista_infomed.csv.xls");}

static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/drugdb/pt/pt_db_finalize.sql");}
static inline QString uidFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/drugdb/pt/pt_uids.csv");}


QWidget *PtDrugsDatabasePage::createPage(QWidget *parent)
{
    return new PortugueseDrugsDatabase(parent);
}

QString PtDrugsDatabasePage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_DRUGSDATABASE;
}

PortugueseDrugsDatabase::PortugueseDrugsDatabase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PortugueseDrugsDatabase),
    m_Step(0)
{
    ui->setupUi(this);
    m_Step = new PtDrugDatatabaseStep(this);
    m_Step->createDir();
    pluginManager()->addObject(m_Step);
}

PortugueseDrugsDatabase::~PortugueseDrugsDatabase()
{
    pluginManager()->removeObject(m_Step);
    delete ui;
}

void PortugueseDrugsDatabase::on_startJobs_clicked()
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
            if (m_Step->prepareDatas())
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

bool PortugueseDrugsDatabase::on_download_clicked()
{
    m_Step->downloadFiles();
    connect(m_Step, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    return true;
}

void PortugueseDrugsDatabase::downloadFinished()
{
    ui->download->setEnabled(true);
}

void PortugueseDrugsDatabase::changeEvent(QEvent *e)
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



PtDrugDatatabaseStep::PtDrugDatatabaseStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    m_WithProgress(false)
{
}

PtDrugDatatabaseStep::~PtDrugDatatabaseStep()
{
}

bool PtDrugDatatabaseStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        LOG_ERROR("Unable to create Pt Working Path :" + workingPath());
    else
        LOG("Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath)) {
            LOG_ERROR("Unable to create Pt database output path :" + dbpath);
            m_Errors << tr("Unable to create Pt database output path :") + dbpath;
        } else {
            LOG("Drugs database (Pt) output dir created");
        }
    }
    return true;
}

bool PtDrugDatatabaseStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool PtDrugDatatabaseStep::downloadFiles(QProgressBar *bar)
{
    Q_UNUSED(bar);
    // TODO: add progress download */
    Utils::HttpDownloader *dld = new Utils::HttpDownloader(this);
    //    dld->setMainWindow(mainwindow());
    dld->setOutputPath(workingPath());
    dld->setUrl(QUrl(PT_URL));
    dld->startDownload();
    connect(dld, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()));
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    connect(dld, SIGNAL(downloadProgressRange(qint64,qint64)), this, SIGNAL(progressRangeChanged(int,int)));
    connect(dld, SIGNAL(downloadProgressValue(qint64)), this, SIGNAL(progress(int)));
    return true;
}

bool PtDrugDatatabaseStep::process()
{
    unzipFiles();
    prepareDatas();
    createDatabase();
    populateDatabase();
    linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool PtDrugDatatabaseStep::unzipFiles()
{
    return true;
}

bool PtDrugDatatabaseStep::prepareDatas()
{
    return true;
}

bool PtDrugDatatabaseStep::createDatabase()
{
    Q_EMIT progressLabelChanged(tr("Preparing raw source files"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    if (!DrugsDB::Tools::createMasterDrugInteractionDatabase())
        return false;

    QMultiHash<QString, QVariant> labels;
    labels.insert("fr","Base de données thérapeutique du portugaise");
    labels.insert("en","Portuguese therapeutic database");
    labels.insert("de","Portugiesische Therapeutische Datenbank");

    if (DrugsDB::Tools::createNewDrugsSource(Core::Constants::MASTER_DATABASE_NAME, PT_DRUGS_DATABASE_NAME, labels) == -1) {
        LOG_ERROR("Unable to create the PT drugs sources");
        return false;
    }

    LOG(QString("Database schema created"));
    Q_EMIT progress(1);
    return true;
}

static int readUids(QHash<QString, int> &drugs_uids)
{
    int lastUid = 20110001;
    QString content = Utils::readTextFile(uidFile());
    if (content.isEmpty())
        LOG_ERROR_FOR("PtDrugDatatabaseStep", "Unable to read UIDs file");
    foreach(const QString &line, content.split("\n", QString::SkipEmptyParts)) {
        if (line.startsWith("//"))
            continue;
        int separator = line.lastIndexOf(";");
        QString drugname = line.left(separator);
        int uid = line.right(8).toInt();
        if (!drugname.isEmpty() && uid>0) {
            if (lastUid < uid)
                lastUid = uid;
            drugs_uids.insert(drugname, uid);
        } else {
            LOG_ERROR_FOR("PtDrugDatatabaseStep", QString("Line : %1 , does not contains 2 values").arg(line));
        }
    }
    return lastUid;
}

static bool saveUids(const QHash<QString, int> &drugs_uids)
{
    QString content;
    content += "// /***************************************************************************\n"
               "//  *   FreeMedicalForms                                                      *\n"
               "//  *   (C) 2008-2016 by Eric MAEKER, MD                                      *\n"
               "//  *   eric.maeker@gmail.com                                                   *\n"
               "//  *   All rights reserved.                                                  *\n"
               "//  ***************************************************************************/\n"
               "// /***************************************************************************\n"
               "//  *   Owner :  Eric MAEKER, MD <eric.maeker@gmail.com>                      *\n"
               "//  ***************************************************************************/\n"
               "// /***************************************************************************\n"
               "//  * - Autogenerated file for the Portuguese drugs database                  *\n"
               "//  *    This file presents all known drugs and their                         *\n"
               "//  *    FreeDiams autogenerated UID                                          *\n"
               "//  *    !!!! The content MUST NOT be changed by hand !!!!                    *\n"
               "//  ***************************************************************************/\n"
               "// \n";
    QStringList names = drugs_uids.keys();
    qSort(names);
    foreach(const QString &drug, names) {
        content += drug + ";" + QString::number(drugs_uids.value(drug)) + "\n";
    }
    if (!Utils::saveStringToFile(content.toUtf8(), uidFile())) {
        LOG_ERROR_FOR("PtDrugDatatabaseStep", "Unable to save UID file");
        return false;
    }
    return true;
}

bool PtDrugDatatabaseStep::populateDatabase()
{
    if (!DrugsDB::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    Q_EMIT progressLabelChanged(tr("Reading raw sources..."));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // Read the UID file from global_resources
    QHash<QString, int> drugs_uids;
    int lastUid = readUids(drugs_uids);

    QFile csv(rawCsvAbsFile());
    if (!csv.open(QFile::ReadOnly | QFile::Text)) {
        LOG_ERROR("Unable to read file");
        return false;
    }

    Q_EMIT progressLabelChanged(tr("Reading drugs raw source"));
    Q_EMIT progressRangeChanged(0, csv.size());
    Q_EMIT progress(0);
    QVector<Drug *> drugs;

    QTextStream in(&csv);
    in.setCodec("ISO 8859-1");
    int pos = 0;
    while (!in.atEnd()) {
        QString l = in.readLine();

        // Ignore first line
        if (pos==0) {
            pos += l.length();
            continue;
        }
        pos += l.length();

        // 0"DCI / Nome Genérico";1"Nome do Medicamento";
        // 2"Forma Farmacêutica";3"Dosagem";4"Estado de Autorização";5"Genérico";
        //"Abacavir + Lamivudina + Zidovudina";"Trizivir";"Comprimido revestido por película";"300 mg + 150 mg + 300 mg";"Autorizado";"N";
        QStringList vals;
        foreach(QString val, l.split("\";")) {
            vals << val.remove("\"");
        }
        if (vals.count() < 7) {
            qWarning() << vals << l;
            continue;
        }

        // Get the FreeDiams uid of the drug (name//form//strength)
        QString uidDrugName = vals.at(1).trimmed() + "//" + vals.at(2).trimmed() + "//" + vals.at(3).trimmed();
        if (!drugs_uids.keys().contains(uidDrugName)) {
            ++lastUid;
            drugs_uids.insert(uidDrugName, lastUid);
            qWarning() << "CREATING UID FOR" << uidDrugName << lastUid;
        }


        Drug *drug = new Drug;
        drug->setData(Drug::Uid1, drugs_uids.value(uidDrugName));
        drug->setData(Drug::Name, vals.at(1).trimmed());
        drug->setData(Drug::Forms, vals.at(2).trimmed());
        if (vals.at(4).compare("Autorizado",Qt::CaseInsensitive)==0)
            drug->setData(Drug::Marketed, 1);
        else
            drug->setData(Drug::Marketed, 0);

        // Composition
        QStringList inns = vals.at(0).split("+");
        QStringList doses = vals.at(3).split("+");
        bool innsAndDosesOk = inns.count() == doses.count();
//        qWarning() << vals.at(1).trimmed() << inns << doses;
        for(int i = 0; i < inns.count(); ++i) {
            Component *compo = new Component;
            QString inn = inns.at(i);
            compo->setData(Component::Name, inn.trimmed().toUpper());
            if (innsAndDosesOk)
                compo->setData(Component::Dose, doses.at(i).trimmed());
            compo->setData(Component::Nature, "SA");
            compo->setData(Component::NatureLink, i);
            drug->addComponent(compo);
        }

        drug->setData(Drug::Valid, 1);
        drug->setData(Drug::Strength, doses.join(";").remove(" "));

        drugs << drug;

        if (drugs.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }

//        if (drugs.count() == 100)
//            break;
    }
    csv.close();

    // save uids
    saveUids(drugs_uids);

    Q_EMIT progressLabelChanged(tr("Saving drugs into database"));
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(1);

    // Save into database
    Drug::saveDrugsIntoDatabase(Core::Constants::MASTER_DATABASE_NAME, drugs, PT_DRUGS_DATABASE_NAME);
    Q_EMIT progress(2);

    qDeleteAll(drugs);
    drugs.clear();

    // Run SQL commands one by one
    if (!DrugsDB::Tools::executeSqlFile(Core::Constants::MASTER_DATABASE_NAME, databaseFinalizationScript())) {
        LOG_ERROR("Can create FDA DB.");
        return false;
    }
    Q_EMIT progress(3);

    LOG(QString("Database processed"));

    return true;
}

bool PtDrugDatatabaseStep::linkMolecules()
{
    if (!DrugsDB::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    // Get SID
    int sid = DrugsDB::Tools::getSourceId(Core::Constants::MASTER_DATABASE_NAME, PT_DRUGS_DATABASE_NAME);
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
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(PT_DRUGS_DATABASE_NAME, "es", &unfound, corrected, QMultiHash<QString, QString>());
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
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(PT_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    return true;
}

