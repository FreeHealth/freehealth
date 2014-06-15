/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2014 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "belgishdrugsdatabase.h"
#include "moleculelinkermodel.h"
#include "drug.h"
#include "drugsdbcore.h"
#include "idrugdatabasestepwidget.h"
#include "moleculelinkdata.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/isettings.h>

#include <drugsdb/drugdatabasedescription.h>
#include <drugsdb/tools.h>

#include <drugsbaseplugin/drugbaseessentials.h>

#include <utils/global.h>
#include <utils/log.h>
#include <utils/database.h>
#include <quazip/global.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_countries.h>
#include <datapackutils/constants.h>

#include <QDir>
#include <QFile>
#include <QProgressDialog>
#include <QSqlDatabase>
#include <QSqlQuery>

// Download at : http://www.fagg-afmps.be/fr/items-HOME/Bases_de_donnees/index.jsp

using namespace DrugsDB;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
const char* const  BE_DRUGS_DATABASE_NAME     = "FAGG_AFMPS_BE";
}

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}
static inline DrugsDB::DrugsDBCore *drugsDbCore() {return DrugsDB::DrugsDBCore::instance();}

static inline QString dumpFileAbsPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/drugdb/be/dump.zip");}

/**
 * Option page for the Free Belguish drugs database.
 * The ctor also create the DrugsDB::Internal::IDrugDatabaseStep object and
 * register it in the plugin manager object pool.
 */
FreeBeDrugsDatabasePage::FreeBeDrugsDatabasePage(QObject *parent) :
    IToolPage(parent),
    _step(0)
{
    setObjectName("FreeBeDrugsDatabasePage");
    _step = new BeDrugDatabaseStep(this);
    pluginManager()->addObject(_step);
}

FreeBeDrugsDatabasePage::~FreeBeDrugsDatabasePage()
{
    pluginManager()->removeObject(_step);
}

QString FreeBeDrugsDatabasePage::name() const
{
    return tkTr(Trans::Constants::COUNTRY_BELGIUM);
}

QString FreeBeDrugsDatabasePage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_FREEDRUGSDATABASE;
}

QWidget *FreeBeDrugsDatabasePage::createPage(QWidget *parent)
{
    Q_ASSERT(_step);
    IDrugDatabaseStepWidget *widget = new IDrugDatabaseStepWidget(parent);
    widget->initialize(_step);
    return widget;
}

/**
 * Option page for the non-free Belguish drugs database.
 * The ctor also create the DrugsDB::Internal::IDrugDatabaseStep object and
 * register it in the plugin manager object pool.
 */
NonFreeBeDrugsDatabasePage::NonFreeBeDrugsDatabasePage(QObject *parent) :
    IToolPage(parent),
    _step(0)
{
    setObjectName("NonFreeBeDrugsDatabasePage");
    _step = new BeDrugDatabaseStep(this);
    _step->setLicenseType(IDrugDatabaseStep::NonFree);
    pluginManager()->addObject(_step);
}

NonFreeBeDrugsDatabasePage::~NonFreeBeDrugsDatabasePage()
{
    pluginManager()->removeObject(_step);
}

QString NonFreeBeDrugsDatabasePage::name() const
{
    return tkTr(Trans::Constants::COUNTRY_BELGIUM);
}

QString NonFreeBeDrugsDatabasePage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_NONFREEDRUGSDATABASE;
}

QWidget *NonFreeBeDrugsDatabasePage::createPage(QWidget *parent)
{
    Q_ASSERT(_step);
    IDrugDatabaseStepWidget *widget = new IDrugDatabaseStepWidget(parent);
    widget->initialize(_step);
    return widget;
}

/** Ctor of the belguish drug database step */
BeDrugDatabaseStep::BeDrugDatabaseStep(QObject *parent) :
    IDrugDatabaseStep(parent),
    m_WithProgress(false)
{
    setObjectName("BeDrugDatatabaseStep");
    setTempPath(QString("%1/%2")
                .arg(settings()->value(Core::Constants::S_TMP_PATH).toString())
                .arg("/BeRawSources/"));
    setConnectionName("be_free");
    setOutputPath(Tools::databaseOutputPath() + "/drugs/");
    setDatabaseDescriptionFile(QString("%1/%2/%3")
                               .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                               .arg(Core::Constants::PATH_TO_DRUG_DATABASE_DESCRIPTION_FILES)
                               .arg("be/description.xml"));
    setDownloadUrl("");
    setLicenseType(Free);
    createTemporaryStorage();
}

BeDrugDatabaseStep::~BeDrugDatabaseStep()
{
}

void BeDrugDatabaseStep::setLicenseType(LicenseType type)
{
    IDrugDatabaseStep::setLicenseType(type);
    if (type==NonFree) {
        setDisplayName(tr("Non-free Belguish drugs database"));
        setConnectionName("be_nonfree");
        setServerOwner(FrenchAssociation);
        setDatapackDescriptionFile(QString("%1/%2/%3")
                                   .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                   .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                                   .arg(QString("drugs/be_ddi/%1").arg(DataPack::Constants::PACKDESCRIPTION_FILENAME));
    } else {
        setDisplayName(tr("Free Belguish drugs database"));
        setConnectionName("be_free");
        setServerOwner(Community);
        setDatapackDescriptionFile(QString("%1/%2/%3")
                                   .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                   .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                                   .arg(QString("drugs/be_noddi/%1").arg(DataPack::Constants::PACKDESCRIPTION_FILENAME));
    }
}

QString BeDrugDatabaseStep::tmpDatabaseAbsPath()
{
    return QDir::cleanPath(tempPath() + "/drugs-be.db");
}

bool BeDrugDatabaseStep::startDownload()
{
    Q_EMIT downloadFinished();
    return true;
}

QString BeDrugDatabaseStep::processMessage() const
{
    if (licenseType() == NonFree)
        return tr("Non-free Belgium drugs database creation");
    return tr("Free Belgium drugs database creation");
}

bool BeDrugDatabaseStep::process()
{
    unzipFiles();
    prepareData();
    createDatabase();
    populateDatabase();
    linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool BeDrugDatabaseStep::unzipFiles()
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
    if (!QuaZipTools::unzipFile(fileName, tempPath()))
        return false;

    Q_EMIT progress(1);

    return true;
}

bool BeDrugDatabaseStep::prepareData()
{
    Q_EMIT progressLabelChanged(tr("Preparing raw source files"));

    if (!QFile(tempPath() + "/dump.sql").exists()) {
        Utils::warningMessageBox(tr("No dump file found"), tr("Try to execute the unzip step."));
        return false;
    }

    // Dump is MySQL specific --> need some adjustement to feet the SQLite requirements
    QFile fileIn(tempPath() + "/dump.sql");
    if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QFile fileOut(tempPath() + "/dump_sqlite.sql");
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

bool BeDrugDatabaseStep::populateDatabase()
{
    if (!checkDatabase())
        return false;

    // check files
//    if (!prepareData())
//        return false;

    Q_EMIT progressLabelChanged(tr("Reading raw sources..."));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // Create temp database
    const char *const BE_TMP_DB = "TmpBeDb";

    if (!DrugsDB::Tools::connectDatabase(BE_TMP_DB, tmpDatabaseAbsPath()))
        return false;

    QSqlDatabase be = QSqlDatabase::database(BE_TMP_DB);
    if (!be.isOpen()) {
        if (!be.open()) {
            LOG_ERROR("Unable to connect db");
            return false;
        }
    }

    if (!Utils::Database::executeSqlFile(BE_TMP_DB, tempPath() + "/dump_sqlite.sql")) {
        LOG_ERROR("Can not create BE DB.");
        return false;
    }

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
    saveDrugsIntoDatabase(drugs);
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(2);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Run SQL commands one by one
//    if (!DrugsDB::Tools::executeSqlFile(_database, databaseFinalizationScript())) {
//        LOG_ERROR("Can create Belguish DB.");
//        return false;
//    }
    LOG(QString("Database processed"));
    Q_EMIT progress(3);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    qDeleteAll(drugs);
    drugs.clear();

    return true;
}

bool BeDrugDatabaseStep::linkMolecules()
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

    if (licenseType() == Free)
        return true;

    // Connect to databases
    if (!checkDatabase())
        return false;

    Q_EMIT progressLabelChanged(tr("Linking drugs components to ATC codes"));
    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progress(0);

    // Associate Mol <-> ATC for drugs with one molecule only
    MoleculeLinkerModel *model = drugsDbCore()->moleculeLinkerModel();
    MoleculeLinkData data(drugEssentialDatabase(), sourceId(), ::BE_DRUGS_DATABASE_NAME, "fr");
    data.correctedByName.insert("HYPERICUM PERFORATUM (MILLEPERTUIS)", "MILLEPERTUIS");
    if (!model->moleculeLinker(&data))
        return false;

    Q_EMIT progress(1);

    Q_EMIT progressLabelChanged(tr("Saving components to ATC links to database"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // Save to links to drugs database
    Tools::addComponentAtcLinks(drugEssentialDatabase(), data.moleculeIdToAtcId, sourceId());

    LOG(QString("Database processed"));

    // add unfound to extralinkermodel
    Q_EMIT progressLabelChanged(tr("Updating component link XML file"));
    model->addUnreviewedMolecules(::BE_DRUGS_DATABASE_NAME, data.unfoundMoleculeAssociations);
    model->saveModel();
    Q_EMIT progress(1);

    return true;
}

