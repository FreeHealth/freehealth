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
#include "canadiandrugsdatabase.h"
#include "drug.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/fdm_constants.h>

#include <drugsdbplugin/drugdatabasedescription.h>
#include <drugsdbplugin/tools.h>

#include <drugsbaseplugin/drugbaseessentials.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>
#include <translationutils/trans_countries.h>
#include <quazip/global.h>
#include <datapackutils/constants.h>

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
#include <QTextStream>

#include <QDebug>

// For your tests, you can limit the number of drugs computed and inserted into the database
// using this debugging enum. Set to -1 if you want all drugs to be processed
enum { LimitDrugsTo = -1 };

using namespace DrugsDb;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
const char* const  CANADIAN_URL               = "http://www.hc-sc.gc.ca/dhp-mps/alt_formats/zip/prodpharma/databasdon/allfiles.zip";
//const char* const  CANADIAN_URL               = "http://www.hc-sc.gc.ca/dhp-mps/prodpharma/databasdon/txt/allfiles.zip";
}

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

CaDrugDatabase::CaDrugDatabase(QObject *parent) :
    DrugsDb::Internal::IDrugDatabase(parent),
    m_WithProgress(false)
{
    setObjectName("CaDrugDatabase");
    setDatabaseUid("CA_HCDPD");
    setDatabaseLanguage("en");
    setCountry(QLocale::Canada);
    setTempPath(QString("%1/%2")
                .arg(settings()->value(Core::Constants::S_TMP_PATH).toString())
                .arg("/CaRawSources/"));
    setDatabaseDescriptionFile(QString("%1/%2/%3")
                               .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                               .arg(Core::Constants::PATH_TO_DRUG_DATABASE_DESCRIPTION_FILES)
                               .arg("ca/description.xml"));
    setDownloadUrl(::CANADIAN_URL);
    setLicenseType(Free);
    // setSpcHtmlFilesDefaultEncoding("ISO-8859-1");
    createTemporaryStorage();
}

CaDrugDatabase::~CaDrugDatabase()
{
}

void CaDrugDatabase::setLicenseType(LicenseType type)
{
    IDrugDatabase::setLicenseType(type);
    if (type==NonFree) {
        setDisplayName(tr("Non-free Canadian drugs database"));
        setConnectionName("ca_nonfree");
        setDatapackDescriptionFile(QString("%1/%2/drugs/%3/%4")
                                   .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                   .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                                   .arg("ca_ddi")
                                   .arg(DataPack::Constants::PACKDESCRIPTION_FILENAME));
    } else {
        setDisplayName(tr("Free Canadian drugs database"));
        setConnectionName("ca_free");
        setServerOwner(Community);
        setDatapackDescriptionFile(QString("%1/%2/drugs/%3/%4")
                                   .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                   .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                                   .arg("ca_noddi")
                                   .arg(DataPack::Constants::PACKDESCRIPTION_FILENAME));
    }
    setOutputPath(QString("%1/%2/%3")
                  .arg(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString())
                  .arg("/drugs/")
                  .arg(connectionName())
                  );
}

QString CaDrugDatabase::processMessage() const
{
    if (licenseType() == NonFree)
        return tr("Non-free Canadian drugs database creation");
    return tr("Free Canadian drugs database creation");
}

bool CaDrugDatabase::process()
{
    unzipFiles();
    prepareData();
    createDatabase();
    populateDatabase();
    linkDrugsComponentsAndDrugInteractors();
    Q_EMIT processFinished();
    return true;
}

bool CaDrugDatabase::unzipFiles()
{
    Q_EMIT progressLabelChanged(tr("Canadian database creation: unzipping files..."));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // check file
    QString fileName = tempPath() + QDir::separator() + QFileInfo(CANADIAN_URL).fileName();
    if (!QFile(fileName).exists()) {
        LOG_ERROR(QString("No files founded: " + fileName));
        LOG_ERROR(QString("Please download files."));
        return false;
    }

    LOG(QString("Starting unzipping Canadian file %1").arg(fileName));

    // unzip downloaded using QProcess
    if (!QuaZipTools::unzipFile(fileName, tempPath())) {
        LOG_ERROR(QString("Unable to unzip Canadian file %1").arg(fileName));
        return false;
    }

    // unzip all files in the working path
    QFileInfoList list = QDir(tempPath()).entryInfoList(QStringList()<<"*.zip");

    Q_EMIT progressRangeChanged(0, list.count());
    int progr = 0;
    foreach(const QFileInfo &info, list) {
        Q_EMIT progress(progr);
        ++progr;
        if (info.fileName()!="allfiles.zip") {
            if (!QuaZipTools::unzipFile(info.absoluteFilePath(), tempPath())) {
                LOG_ERROR("Unable to unzip " + info.absoluteFilePath());
                return false;
            }
        }
    }
    return true;
}

bool CaDrugDatabase::prepareData()
{
    return true;
}

QMultiHash<int, QString> CaDrugDatabase::extractUidRelatedDatas(const QString &absFileName, const int uidCol, const int dataCol, const QHash<int, QString> &equalityCondition)
{
    // Get forms
    QMultiHash<int, QString> uid_vals;
    QFile csv(absFileName);
    if (!csv.open(QFile::ReadOnly | QFile::Text)) {
        LOG_ERROR("Unable to read file");
        return uid_vals;
    }

    Q_EMIT progressRangeChanged(0, csv.size());
    Q_EMIT progress(0);

    QTextStream in(&csv);
    in.setCodec("ISO 8859-1");
    int pos = 0;
    bool useCondition = equalityCondition.count() >= 1;
    while (!in.atEnd()) {
        QString l = in.readLine();
        pos += l.length();
        QStringList vals = l.split(",");

        if (useCondition) {
            bool goOn = true;
            foreach(const int col, equalityCondition.keys()) {
                QString value = vals[col].remove("\"");
                if (value.compare(equalityCondition.value(col), Qt::CaseInsensitive) != 0) {
                    goOn = false;
                    break;
                }
            }
            if (!goOn)
                continue;
        }

        QString uid = vals[uidCol].remove("\"");
        QString v = vals[dataCol].remove("\"");
        uid_vals.insertMulti(uid.toInt(), v);

        if (uid_vals.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }
    }
    csv.close();
    return uid_vals;
}

bool CaDrugDatabase::populateDatabase()
{
    if (!checkDatabase())
        return false;

    // check files
    if (!prepareData())
        return false;

    // import files
    QHash<int, QString> condition;

    // Get routes
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (routes)"));
    QMultiHash<int, QString> uid_routes = extractUidRelatedDatas(tempPath() + QDir::separator() + "route.txt", 0, 2, condition);

    // Get ther
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (ther)"));
    QMultiHash<int, QString> uid_ther = extractUidRelatedDatas(tempPath() + QDir::separator() + "ther.txt", 0, 1, condition);

    // Get forms
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (forms)"));
    QMultiHash<int, QString> uid_forms = extractUidRelatedDatas(tempPath() + QDir::separator() + "form.txt", 0, 2, condition);

    // Get status
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (status)"));
    condition.insert(1, "Y");
    QMultiHash<int, QString> uid_stats = extractUidRelatedDatas(tempPath() + QDir::separator() + "status.txt", 0, 2, condition);

    // Get drug names
    QHash<int, Drug *> uid_drugs;
    QMultiHash<int, Component *> uid_compos;

    QFile csv(tempPath() + QDir::separator() + "drug.txt");
    if (!csv.open(QFile::ReadOnly | QFile::Text)) {
        LOG_ERROR("Unable to read file");
        return false;
    }

    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (drugs)"));
    Q_EMIT progressRangeChanged(0, csv.size());
    Q_EMIT progress(0);

    QTextStream in(&csv);
    in.setCodec("ISO 8859-1");
    int pos = 0;
    while (!in.atEnd()) {
        QString l = in.readLine();
        pos += l.length();
        QStringList vals;
        foreach(QString val, l.split(",")) {
            vals << val.remove("\"");
        }
        if (vals.count() < 11) {
            qWarning() << vals << l;
            continue;
        }

        if (vals.at(2).compare("Veterinary") == 0)
            continue;

        // "82621","CAT IV","Human","02340631",(4)"DO ... SPF 15",(5)"",(6)"N",(7)"",(8)"4",(9)"14-JUN-2010",(10)"0423152042"
        Drug *drug = new Drug;
        int drugCode = vals.at(0).toInt();
        drug->setData(Drug::Uid1, vals.at(3));
        drug->setData(Drug::OldUid, vals.at(3)); // Since v0.5.4 Ca db uid is the DIN, before is was ProductN;FormId;RouteId
        drug->setData(Drug::Name, vals.at(4));
        QStringList t = uid_forms.values(drugCode);
        drug->setData(Drug::Forms, t);
        t.clear();
        t = uid_routes.values(drugCode);
        drug->setData(Drug::Routes, t);
        t = uid_stats.values(drugCode);
        if (t.count() == 0) {
            drug->setData(Drug::Marketed, 0);
        } else {
            if (t.contains("MARKETED", Qt::CaseInsensitive))
                drug->setData(Drug::Marketed, 1);
            else
                drug->setData(Drug::Marketed, 0);
        }
        drug->setData(Drug::Valid, 1);
        drug->setData(Drug::Strength, 1);
        t.clear();
        t = uid_ther.values(drugCode);
        drug->setData(Drug::AtcCode, t.join(","));

        uid_drugs.insert(drugCode, drug);

        if (uid_drugs.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }
    }
    csv.close();

    // Get components
    csv.setFileName(tempPath() + QDir::separator() + "ingred.txt");
    if (!csv.open(QFile::ReadOnly | QFile::Text)) {
        LOG_ERROR("Unable to read file");
        return false;
    }

    Q_EMIT progressLabelChanged(tr("Reading drugs raw source (ingred)"));
    Q_EMIT progressRangeChanged(0, csv.size());
    Q_EMIT progress(0);

    in.setDevice(&csv);
    in.setCodec("ISO 8859-1");
    pos = 0;
    while (!in.atEnd()) {
        QString l = in.readLine();
        pos += l.length();
        QStringList vals;
        foreach(QString val, l.split("\",\"")) {
            vals << val.remove("\"");
        }

        // (0)"9518",(1)"3182",(2)"OXYQUINOLINE",(3)"I",
        // (4)"0.5",(5)"%",(6)"",(7)"",(8)"N",
        // (9)"",(10)"8-HYDROXYQUINOLINE 0.4% CHG TO 0.5% AS PER VDD 2006.04.05"
        //        "DRUG_CODE" INTEGER(8) NOT NULL ,
        //        "ACTIVE_INGREDIENT_CODE" INTEGER(6),
        //        "INGREDIENT" VARCHAR2(240),
        //        "INGREDIENT_SUPPLIED_IND" VARCHAR2(1),
        //        "STRENGTH" VARCHAR2(20),
        //        "STRENGTH_UNIT" VARCHAR2(40),
        //        "STRENGTH_TYPE" VARCHAR2(40),
        //        "DOSAGE_VALUE" VARCHAR2(20),
        //        "BASE" VARCHAR2(1),
        //        "DOSAGE_UNIT" VARCHAR2(40),
        //        "NOTES" VARCHAR2(2000)

        QString strength = vals.at(4) + vals.at(5);
        if (!vals.at(7).isEmpty() || !vals.at(9).isEmpty()) {
            strength += "/" + vals.at(7) +vals.at(9);
        }

        Component *compo = new Component;
        int drugCode = vals.at(0).toInt();
        compo->setData(Component::Name, vals.at(2));
        compo->setData(Component::Strength, strength);
        compo->setData(Component::Nature, "SA");

        uid_compos.insert(drugCode, compo);

        if (uid_compos.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }
    }
    csv.close();

    Q_EMIT progressLabelChanged(tr("Reading drugs raw source : processing drugs/components"));
    Q_EMIT progressRangeChanged(0, uid_drugs.count());
    Q_EMIT progress(0);

    QVector<Drug *> drugsVector;
    pos = 0;
    foreach(const int uid, uid_drugs.keys()) {
        Drug *drug = uid_drugs[uid];
        QStringList strength;
        int i = 0;
        foreach(Component *c, uid_compos.values(uid)) {
            ++i;
            c->setData(Component::NatureLink, i);
            drug->addComponent(c);
            strength.append(c->data(Component::Strength).toString() + c->data(Component::StrengthUnit).toString());
        }
        drug->setData(Drug::Strength, strength.join(";"));
        drugsVector << drug;
        ++ pos;
        if (pos % 10 == 0)
            Q_EMIT progress(pos);
    }

    Q_EMIT progressLabelChanged(tr("Saving drugs into database"));
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(1);

    saveDrugsIntoDatabase(drugsVector);
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(2);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

//    // Run SQL commands one by one
//    Q_EMIT progressLabelChanged(tr("Running database finalization script"));
//    if (!DrugsDB::Tools::executeSqlFile(_database, databaseFinalizationScript())) {
//        LOG_ERROR("Can create Canadian DB.");
//        return false;
//    }

    LOG(QString("Database processed"));
    Q_EMIT progress(3);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // delete pointers
    qDeleteAll(drugsVector);
    uid_drugs.clear();
    uid_compos.clear();
    drugsVector.clear();
    return true;
}

//bool CaDrugDatabase::linkMolecules()
//{

    //    Mar 2014
    //    Refactoring the code for the FreeDDIManager -> only use ComponentAtcModel
    //    ATC 5665
    //    NUMBER OF MOLECULES           1827
    //    LINKERMODEL                   868
    //    LEFT                          959
    //    CONFIDENCE INDICE             47.5096

    // 18 Oct 2011
    //    NUMBER OF MOLECULES 1845
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 1364 "
    //    LINKERMODEL (WithATC:281;WithoutATC:73) 354"
    //    LINKERNATURE 0
    //    LEFT 408
    //    CONFIDENCE INDICE 77

    // 29 Sept 2011 (ATC 2011 && ATC 2012 + new link in model)
    //    NUMBER OF MOLECULES 1842
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 1308 "
    //    LINKERMODEL (WithATC:215;WithoutATC:72) 287"
    //    LINKERNATURE 0
    //    LEFT 462
    //    CONFIDENCE INDICE 74

    // 29 APR 2011
    //    NUMBER OF MOLECULES 1848
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 1219 "
    //    LINKERMODEL (WithATC:81;WithoutATC:75) 156"
    //    LINKERNATURE 0
    //    LEFT 554
    //    CONFIDENCE INDICE 70

    // 15 Jan 2011
    //    NUMBER OF MOLECULES 1825
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 1213 "
    //    LINKERMODEL (WithATC:87;WithoutATC:75) 162"
    //    LINKERNATURE 0
    //    LEFT 537
    //    CONFIDENCE INDICE 70

    // 11 Dec 2010
    //        NUMBER OF MOLECULES 1825
    //        CORRECTED BY NAME 23
    //        CORRECTED BY ATC 0
    //        FOUNDED 1106 "
    //        LINKERMODEL (WithATC:77;WithoutATC:74) 151"
    //        LINKERNATURE 0
    //        LEFT 548
    //        CONFIDENCE INDICE 69

    // 13 Nov 2010
    //        NUMBER OF MOLECULES 1819
    //        CORRECTED BY NAME 23
    //        CORRECTED BY ATC 0
    //        FOUNDED 1089
    //        LINKERMODEL 73
    //        LINKERNATURE 0
    //        LEFT 634


    // 25 Oct 2010
    // Some corrections :
    //   - correctedByAtc is removed
    //   - one molecule name is associated with multiple molecule codes
    //        NUMBER OF MOLECULES 1819
    //        CORRECTED BY NAME 23
    //        CORRECTED BY ATC 0
    //        FOUNDED 1089
    //        LINKERMODEL 73
    //        LINKERNATURE 0
    //        LEFT 634

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
    // Using the new DrugsDB::Tools::englishMoleculeLinker()
    // 2243 distinct mols
    // Hand association: 1647 (drugs 7-char atc with one molecules + hand made)
    // Found: 1292, Left: 773

    // 26 July 2010
    // 1398 distinct mols
    // Hand association: 86
    // Found: 1071, Left: 327
    // Drugs with one mol and ATC (7-char): 1358

//    if (licenseType() == Free)
//        return true;

//    // Connect to databases
//    if (!checkDatabase())
//        return false;

//    Q_EMIT progressLabelChanged(tr("Linking drugs components to ATC codes"));
//    Q_EMIT progressRangeChanged(0, 2);
//    Q_EMIT progress(0);

//    // Associate Mol <-> ATC for drugs with one molecule only
//    MoleculeLinkerModel *model = drugsDbCore()->moleculeLinkerModel();
//    MoleculeLinkData data(drugEssentialDatabase(), sourceId(), ::CA_DRUGS_DATABASE_NAME, "fr");

//    // TODO: add this code ?
//    // Associate molecule from Drugs == 1 molecule and Drug got 7-char length ATC code
//    // add them to corrected
//    //         SELECT DISTINCT composition.molecule_name, drugs.atc
//    //         FROM composition, drugs
//    //         WHERE drugs.uid=composition.uid AND length(drugs.atc)=7
//    //         GROUP BY composition.uid
//    //         HAVING count(composition.molecule_name) = 1
//    //         LIMIT 100;

////    QString req;
////    LOG("Getting Drugs with ATC and one molecule - Can take some times");
////    QMultiHash<QString, QString> correctedByAtcCode;
////    QSqlQuery drugs(ca);
////    req = "SELECT DISTINCT composition.molecule_name, drugs.atc "
////            "FROM composition, drugs "
////            "WHERE drugs.uid=composition.uid AND length(drugs.atc)=7 "
////            "GROUP BY composition.uid "
////            "HAVING count(composition.molecule_name) = 1;";
////    if (drugs.exec(req)) {
////        while (drugs.next()) {
////            correctedByAtcCode.insertMulti(drugs.value(0).toString(), drugs.value(1).toString());
////        }
////    }
//    data.correctedByName.insert("CALCIUM (CALCIUM CARBONATE)", "CALCIUM CARBONATE");
//    data.correctedByName.insert("VITAMIN D3", "COLECALCIFEROL");
//    data.correctedByName.insert("VITAMIN D3 (CHOLECALCIFEROL)", "COLECALCIFEROL");
//    data.correctedByName.insert("DIATRIZOATE SODIUM", "DIATRIZOIC ACID");
//    data.correctedByName.insert("DIATRIZOATE MEGLUMINE", "DIATRIZOIC ACID");
//    data.correctedByName.insert("IOXAGLATE MEGLUMINE", "IOXAGLIC ACID");
//    data.correctedByName.insert("IOXAGLATE SODIUM", "IOXAGLIC ACID");
//    data.correctedByName.insert("THIAMINE MONONITRATE", "THIAMINE (VIT B1)");
//    data.correctedByName.insert("ETHINYL ESTRADIOL", "ETHINYLESTRADIOL");
//    data.correctedByName.insert("NORGESTREL", "LEVONORGESTREL");
//    data.correctedByName.insert("ALUMINUM CHLOROHYDRATE", "ALUMINIUM CHLOROHYDRATE");
//    data.correctedByName.insert("VITAMIN B6 (PYRIDOXINE HYDROCHLORIDE)", "PYRIDOXINE (VIT B6)");
//    data.correctedByName.insert("VITAMIN B1 (THIAMINE HYDROCHLORIDE)", "THIAMINE (VIT B1)");
//    data.correctedByName.insert("FORMOTEROL FUMARATE DIHYDRATE", "FORMOTEROL");
//    data.correctedByName.insert("FIBRINOGEN (HUMAN)", "FIBRINOGEN, HUMAN");
//    data.correctedByName.insert("FIBRINOGEN (HUMAN)", "HUMAN FIBRINOGEN");
//    data.correctedByName.insert("FACTOR XIII", "COAGULATION FACTOR XIII");
//    data.correctedByName.insert("BETA-CAROTENE", "BETACAROTENE");
//    data.correctedByName.insert("L-ARGININE" , "ARGININE HYDROCHLORIDE" );
//    data.correctedByName.insert("L-LYSINE (L-LYSINE HYDROCHLORIDE)" ,"LYSINE" );
//    data.correctedByName.insert("L-METHIONINE" ,"METHIONINE" );
//    data.correctedByName.insert("GLUTAMIC ACID" ,"GLUTAMIC ACID HYDROCHLORIDE" );
//    data.correctedByName.insert("D-ALPHA TOCOPHEROL", "TOCOPHEROL");
//    data.correctedByName.insert("D-PANTOTHENIC ACID (CALCIUM D-PANTOTHENATE)" ,"CALCIUM PANTOTHENATE" );

//    if (!model->moleculeLinker(&data))
//        return false;

//    Q_EMIT progress(1);

//    Q_EMIT progressLabelChanged(tr("Saving components to ATC links to database"));
//    Q_EMIT progressRangeChanged(0, 1);
//    Q_EMIT progress(0);

//    // Save to links to drugs database
//    Tools::addComponentAtcLinks(drugEssentialDatabase(), data.moleculeIdToAtcId, sourceId());

//    LOG(QString("Database processed"));

//    // add unfound to extralinkermodel
//    Q_EMIT progressLabelChanged(tr("Updating component link XML file"));
//    model->addUnreviewedMolecules(::CA_DRUGS_DATABASE_NAME, data.unfoundMoleculeAssociations);
//    model->saveModel();
//    Q_EMIT progress(1);

//    return true;
//}
