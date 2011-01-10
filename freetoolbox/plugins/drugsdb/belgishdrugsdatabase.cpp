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
#include "belgishdrugsdatabase.h"
#include "extramoleculelinkermodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/global.h>
#include <utils/log.h>
#include <utils/database.h>

#include <extensionsystem/pluginmanager.h>

#include <QDir>
#include <QFile>

#include "ui_belgishdrugsdatabase.h"

// Download at : http://www.fagg-afmps.be/fr/items-HOME/Bases_de_donnees/index.jsp

using namespace DrugsDbCreator;


const char* const  BE_DRUGS_DATABASE_NAME     = "FAGG_AFMPS_BE";


static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/BeRawSources/");}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/drugs/drugs-fr_BE.db");}
static inline QString iamDatabaseAbsPath()  {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + Core::Constants::IAM_DATABASE_FILENAME);}

static inline QString dumpFileAbsPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/drugdb/be/dump.zip");}

//static inline QString databasePreparationScript()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/canadian_db_preparation.sql");}
//static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/canadian_db_finalize.sql");}

static inline QString drugsDatabaseSqlSchema() {return settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + QString(Core::Constants::FILE_DRUGS_DATABASE_SCHEME);}


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
    if (ui->split->isChecked()) {
        if (m_Step->splitDatabase())
            ui->split->setText(ui->split->text() + " CORRECTLY DONE");
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
        m_WithProgress(false)
{
}

BeDrugDatatabaseStep::~BeDrugDatatabaseStep()
{
}

bool BeDrugDatatabaseStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        Utils::Log::addError(this, "Unable to create Be Working Path :" + workingPath(), __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath)) {
            Utils::Log::addError(this, "Unable to create Be database output path :" + dbpath, __FILE__, __LINE__);
            m_Errors << tr("Unable to create Be database output path :") + dbpath;
        } else {
            Utils::Log::addMessage(this, "Drugs database (Be) output dir created");
        }
    }
    return true;
}

bool BeDrugDatatabaseStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool BeDrugDatatabaseStep::downloadFiles()
{
    Q_EMIT downloadFinished();
    return true;
}

bool BeDrugDatatabaseStep::process()
{
    unzipFiles();
    prepareDatas();
    createDatabase();
    populateDatabase();
    linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool BeDrugDatatabaseStep::unzipFiles()
{
    // check file
    QString fileName = dumpFileAbsPath();
    if (!QFile(fileName).exists()) {
        Utils::Log::addError(this, QString("No files founded."), __FILE__, __LINE__);
        Utils::Log::addError(this, QString("Please download files."), __FILE__, __LINE__);
        Utils::warningMessageBox(tr("This assistant can not dowload and prepare the raw belguish drugs database."),
                                 tr("This step (download) must be done by hand. Please refer to the user manual."));
        return false;
    }

    Utils::Log::addMessage(this, QString("Starting unzipping Belguish dump file %1").arg(fileName));

    // unzip downloaded using QProcess
    if (!Core::Tools::unzipFile(fileName, workingPath()))
        return false;

    return true;
}

bool BeDrugDatatabaseStep::prepareDatas()
{
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

    QTextStream in(&fileIn);
    QTextStream out(&fileOut);

    out << "BEGIN TRANSACTION;\n";

    while (!in.atEnd()) {
        QString line = in.readLine();
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
    }

    out << "COMMIT;\n";

    return true;
}

bool BeDrugDatatabaseStep::createDatabase()
{
    if (!Core::Tools::connectDatabase(BE_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    // create database structure
    if (!Core::Tools::executeSqlFile(BE_DRUGS_DATABASE_NAME, drugsDatabaseSqlSchema())) {
        Utils::Log::addError(this, "Can create Belguish DB.", __FILE__, __LINE__);
        return false;
    }

    Utils::Log::addMessage(this, QString("Database schema created"));
    return true;
}

struct MinCompo {
    MinCompo(int id, const QString &name, const QString &strength, const QString &strength_unit = QString::null, const QString &dosage = QString::null, const QString &dosageunit = QString::null) :
            mol_id(id), mol(name), strength(strength), strength_unit(strength_unit), dosage(dosage), dosage_unit(dosageunit)
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

    QString name, uid, atc, form, aut;
    QVector<int> routes;
    QVector<MinCompo *> compo;
};

bool BeDrugDatatabaseStep::populateDatabase()
{
    if (!Core::Tools::connectDatabase(BE_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

//    if (!Utils::Database::executeSqlFile(BE_DRUGS_DATABASE_NAME, workingPath() + "/dump_sqlite.sql")) {
//        Utils::Log::addError(this, "Can not create BE DB.", __FILE__, __LINE__);
//        return false;
//    }

    // delete existing
    Core::Tools::executeSqlQuery("DELETE FROM DRUGS;", BE_DRUGS_DATABASE_NAME, __FILE__, __LINE__);
    Core::Tools::executeSqlQuery("DELETE FROM COMPOSITION;", BE_DRUGS_DATABASE_NAME, __FILE__, __LINE__);

    QSqlDatabase be = QSqlDatabase::database(BE_DRUGS_DATABASE_NAME);
    be.transaction();

    // Get Molecules
    QString req = "SELECT DISTINCT `ActSubst_Name` FROM Tbl_AMM_det_H;";
    QStringList mols;
    QSqlQuery query(be);
    if (query.exec(req)) {
        while (query.next()) {
            mols.append(query.value(0).toString());
        }
    }
    query.finish();

    // Get drugs
    QString lastUid;
    req = "SELECT `Registratienummer`, `mp_name`, `PharmFormFr`, `GenFr`, `cti` FROM Tbl_AMM_H ORDER BY `mp_name`, `Registratienummer`;";
    if (query.exec(req)) {
        while (query.next()) {
            if (query.value(0).toString()==lastUid)
                continue;
            lastUid = query.value(0).toString();
            MinDrug drug(query.value(1).toString(), query.value(0).toString());
            drug.form = query.value(2).toString();
            drug.aut = query.value(3).toString();

            QString req2 = QString("SELECT `ActSubst_Name`, `unit`, `dosis` FROM Tbl_AMM_det_H "
                                       "WHERE `cti`='%1';").arg(query.value(4).toString());
            QSqlQuery compo(be);
            if (compo.exec(req2)) {
                while (compo.next()) {
                    drug.compo.append(new MinCompo(mols.indexOf(compo.value(0).toString()), compo.value(0).toString(),
                                                   compo.value(2).toString(), compo.value(1).toString()));
                }
            } else {
                Utils::Log::addQueryError(this, compo, __FILE__, __LINE__);
            }
            compo.finish();

            // Insert drug
            req2 = QString("INSERT INTO DRUGS (`UID`,`NAME`,`FORM`,`GLOBAL_STRENGTH`,`AUTHORIZATION`,`MARKETED`) VALUES "
                           "('%1', '%2', '%3', '%4', '%5', 1);")
                    .arg(drug.uid)
                    .arg(drug.name.replace("'", "''"))
                    .arg(drug.form.replace("'", "''"))
                    .arg(drug.globalStrength())
                    .arg(drug.aut.replace("'", "''"));
            if (!compo.exec(req2)) {
                Utils::Log::addQueryError(this, compo, __FILE__, __LINE__);
            }
            compo.finish();

            // Insert drug composition
            foreach(MinCompo *compositon, drug.compo) {
                req2 = QString("INSERT INTO COMPOSITION (`UID`,`MOLECULE_NAME`,`MOLECULE_CODE`,`DOSAGE`) VALUES "
                               "('%1','%2','%3','%4');")
                        .arg(drug.uid)
                        .arg(compositon->mol.replace("'", "''"))
                        .arg(compositon->mol_id)
                        .arg(compositon->sqlDosage().replace("'", "''"));
                if (!compo.exec(req2)) {
                    Utils::Log::addQueryError(this, compo, __FILE__, __LINE__);
                }
                compo.finish();
                req2.clear();
            }
        }
    }

    be.commit();

//    -- route=PackFr
//    -- autoriation de délivrance=DelivFr


    return true;
}

bool BeDrugDatatabaseStep::splitDatabase()
{
    return true;
}

bool BeDrugDatatabaseStep::linkMolecules()
{
    // 10 Jan 2011
    //    NUMBER OF MOLECULES 2146
    //    CORRECTED BY NAME 1
    //    CORRECTED BY ATC 0
    //    FOUNDED 1351 "
    //    LINKERMODEL (WithATC:659;WithoutATC:13) 672"
    //    LINKERNATURE 0
    //    LEFT 782
    //    CONFIDENCE INDICE 63

    if (!Core::Tools::connectDatabase(Core::Constants::IAM_DATABASE_NAME, iamDatabaseAbsPath()))
        return false;

    // get all drugs and ATC codes
    if (!Core::Tools::connectDatabase(BE_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    QSqlDatabase be = QSqlDatabase::database(BE_DRUGS_DATABASE_NAME);
    if (!be.open()) {
        Utils::Log::addError(this, "Can not connect to BE_DB db", __FILE__, __LINE__);
        return false;
    }

    QMultiHash<QString, QString> correctedByAtcCode;
    QHash<QString, QString> corrected;
    corrected.insert("HYPERICUM PERFORATUM (MILLEPERTUIS)", "MILLEPERTUIS");

    // Associate Mol <-> ATC for drugs with one molecule only
    QStringList unfound;
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(BE_DRUGS_DATABASE_NAME, "fr", &unfound, corrected, correctedByAtcCode);
    qWarning() << "unfound" << unfound.count();

    // Save to links to drugs database
    be.transaction();
    Core::Tools::executeSqlQuery("DELETE FROM LK_MOL_ATC;", BE_DRUGS_DATABASE_NAME);
    foreach(int mol, mol_atc.uniqueKeys()) {
        QList<int> atcCodesSaved;
        foreach(int atc, mol_atc.values(mol)) {
            if (atcCodesSaved.contains(atc))
                continue;
            atcCodesSaved.append(atc);
            QString req = QString("INSERT INTO `LK_MOL_ATC` VALUES (%1, %2)").arg(mol).arg(atc);
            Core::Tools::executeSqlQuery(req, BE_DRUGS_DATABASE_NAME);
        }
    }
    be.commit();

    // add unfound to extralinkermodel
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(BE_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    //        if (!Core::Tools::signDatabase(BE_DRUGS_DATABASE_NAME))
    //            Utils::Log::addError(this, "Unable to tag database.", __FILE__, __LINE__);

    Utils::Log::addMessage(this, QString("Database processed"));

    return true;
}

