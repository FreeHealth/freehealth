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
 *  Main Developer: Eric MAEKER, MD <eric.maeker@gmail.com>                *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "frenchdrugsdatabasecreator.h"
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

// TODO: Mise à jour des bases de données sur un site centralisé
// Voir http://base-donnees-publique.medicaments.gouv.fr/telechargement.php

// For your tests, you can limit the number of drugs computed
// and inserted into the database using this debugging enum.
// Set to -1 if you want all drugs to be processed
enum { LimitDrugsTo = -1 };

using namespace DrugsDb;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
const char* const FRENCH_RPC_LINK = "http://agence-prd.ansm.sante.fr/php/ecodex/rcp/R%1.htm"; // 2+2+3
}

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

FrDrugDatabase::FrDrugDatabase(QObject *parent) :
    DrugsDb::Internal::IDrugDatabase(parent),
    m_WithProgress(false)
{
    setObjectName("FrDrugDatabase");
    setDatabaseUid("FR_AFSSAPS");
    setDatabaseLanguage("fr");
    setCountry(QLocale::France);
    setTempPath(QString("%1/%2")
                .arg(settings()->value(Core::Constants::S_TMP_PATH).toString())
                .arg("/FrenchRawSources/"));
    setDatabaseDescriptionFile(QString("%1/%2/%3")
                               .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                               .arg(Core::Constants::PATH_TO_DRUG_DATABASE_DESCRIPTION_FILES)
                               .arg("fr/description.xml"));
    setDownloadUrl("http://agence-prd.ansm.sante.fr/php/ecodex/telecharger/fic_cis_cip.zip");
    setLicenseType(Free);
    setSpcHtmlFilesDefaultEncoding("ISO-8859-1");
    createTemporaryStorage();
}

FrDrugDatabase::~FrDrugDatabase()
{
}

void FrDrugDatabase::setLicenseType(LicenseType type)
{
    IDrugDatabase::setLicenseType(type);
    if (type==NonFree) {
        setDisplayName(tr("Non-free French drugs database"));
        setConnectionName("fr_nonfree");
        setServerOwner(FrenchAssociation);
        setDatapackDescriptionFile(QString("%1/%2/drugs/%3/%4")
                                   .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                   .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                                   .arg("fr_ddi")
                                   .arg(DataPack::Constants::PACKDESCRIPTION_FILENAME));
    } else {
        setDisplayName(tr("Free French drugs database"));
        setConnectionName("fr_free");
        setServerOwner(Community);
        setDatapackDescriptionFile(QString("%1/%2/drugs/%3/%4")
                                   .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                   .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                                   .arg("fr_noddi")
                                   .arg(DataPack::Constants::PACKDESCRIPTION_FILENAME));
    }
    setOutputPath(QString("%1/%2/%3")
                  .arg(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString())
                  .arg("/drugs/")
                  .arg(connectionName())
                  );
}

QString FrDrugDatabase::processMessage() const
{
    if (licenseType() == NonFree)
        return tr("Non-free French drugs database creation");
    return tr("Free French drugs database creation");
}

bool FrDrugDatabase::process()
{
    clearFinalReport();
    unzipFiles();
    prepareData();
    createDatabase();
    populateDatabase();
    linkDrugsComponentsAndDrugInteractors();
    showReportDialog();
    Q_EMIT processFinished();
    return true;
}

bool FrDrugDatabase::prepareData()
{
    // check files
    QStringList files;
    files << "CIS.txt" << "CIS_CIP.txt" << "COMPO.txt";
    foreach(const QString &f, files) {
        if (!QFile::exists(tempPath() + "/" + f)) {
            LOG_ERROR(QString("Missing %1 file. FrenchDrugsDatabaseWidget::populateDatabase()")
                      .arg(tempPath() + "/" + f));
            return false;
        }
    }
    addFinalReportMessage(tr("French Pharmaceutical Drug Database: raw data processed."));
    return true;
}

/** Read the raw source files and create the drugs database (this does not include the interaction data) */
bool FrDrugDatabase::populateDatabase()
{
    if (!checkDatabase()) {
        if (!createDatabase())
            return false;
    }

    // check files
    if (!prepareData())
        return false;

    QHash<int, Drug *> drugs;
    QMultiHash<int, Component *> compos;

    QFile file(tempPath() + "/CIS.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("ERROR : Enable to open CIS.txt : %1.").arg(file.errorString()));
        return false;
    }
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source"));
    Q_EMIT progressRangeChanged(0, file.size());
    Q_EMIT progress(0);

    QTextStream in(&file);
    in.setCodec("ISO 8859-15");
    int pos = 0;
    while (!in.atEnd()) {
        QString l = in.readLine();
        pos += l.length();
        QStringList line = l.split("\t");
        //68586203	17 B ESTRADIOL BESINS-ISCOVESCO 0,06 POUR CENT, gel pour application cutanée en tube	(2)gel pour application	(3)transdermique	(4)AMM active	(5)Procdure nationale	(6)Non commercialise (7)SPC
        Drug *drug = new Drug;
        drug->setData(Drug::Uid1, line.at(0));
        drug->setData(Drug::OldUid, line.at(0));
        drug->setData(Drug::Name, line.at(1));
        drug->setData(Drug::Forms, line.at(2));
        drug->setData(Drug::Routes, line.at(3).split(";"));
        drug->setData(Drug::Authorization, QString(line.at(4) + " ; " + line.at(5)));
        drug->setData(Drug::Marketed, line.at(6).startsWith("Comm"));
        if (line.at(7).isEmpty())
            drug->setData(Drug::Spc, QString());
        else
            drug->setData(Drug::Spc, QString(FRENCH_RPC_LINK).arg(line.at(7).rightJustified(7,'0')));
        drug->setData(Drug::Valid, 1);
        drug->setData(Drug::SID, 1);
        drugs.insert(line.at(0).toInt(), drug);
        if (drugs.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }

        if (LimitDrugsTo != -1) {
            if (drugs.count() == LimitDrugsTo)
                break;
        }
    }
    file.close();

    file.setFileName(tempPath() + "/COMPO.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("ERROR : Enable to open CIS.txt : %1. FrenchDrugsDatabaseWidget::populateDatabase()").arg(file.errorString()));
        return false;
    }
    in.setDevice(&file);
    in.setCodec("ISO 8859-15");
    Q_EMIT progressLabelChanged(tr("Reading composition raw source"));
    Q_EMIT progressRangeChanged(0, file.size());
    Q_EMIT progress(0);
    pos = 0;
    while (!in.atEnd()) {
        QString l = in.readLine();
        pos += l.length();
        QStringList line = l.split("\t");

        // if (!drugs.keys().contains(line.at(0).toInt()))
        //     continue;

        // 60001288	(1)comprim	(2)00468	(3)CODINE (PHOSPHATE DE) SESQUIHYDRAT	(4)15 mg 	(5)un comprim	(6)SA	(7)1
        Component *compo = new Component;
        compo->setData(Component::Name, line.at(3));
        compo->setData(Component::Strength, line.at(4));
        compo->setData(Component::Dose, line.at(5));
        compo->setData(Component::Nature, line.at(6));
        compo->setData(Component::NatureLink, line.at(7));
        compos.insertMulti(line.at(0).toInt(), compo);
        if (compos.count() % 10 == 0) {
            Q_EMIT progress(pos);
        }
    }
    file.close();

    Q_EMIT progressLabelChanged(tr("Saving drugs into database"));
    Q_EMIT progressRangeChanged(0, drugs.count()*2);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    QVector<Drug *> drugsVector;
    int n = 0;
    foreach(const int uid, drugs.keys()) {
        ++n;
        if (n % 100 == 0) {
            Q_EMIT progress(n);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        Drug *drug = drugs[uid];
        foreach(Component *c, compos.values(uid)) {
            drug->addComponent(c);
        }
        drugsVector << drug;
    }

    saveDrugsIntoDatabase(drugsVector);
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(2);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Run SQL commands one by one
    Q_EMIT progressLabelChanged(tr("Running database finalization script"));
    Q_EMIT progressRangeChanged(0, 2);
    Q_EMIT progress(1);
    //    if (!Tools::executeSqlFile(connectionName(), finalizationScript())) {
    //        LOG_ERROR("Can create French DB.");
    //        return false;
    //    }

    // delete pointers
    qDeleteAll(drugs);
    drugs.clear();
    compos.clear();
    drugsVector.clear();
    Q_EMIT progress(2);
    return true;
}

//bool FrDrugDatabase::linkMolecules()
//{
    //    Mar 2014
    //    Refactoring the code for the FreeDDIManager -> only use ComponentAtcModel
    //    ATC 5665
    //    NUMBER OF MOLECULES           5634
    //    LINKERMODEL                   4746
    //    LEFT                          666
    //    CONFIDENCE INDICE             88.1789

//    // 21 May 2013
//    //    NUMBER OF MOLECULES 5492
//    //    CORRECTED BY NAME 0
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 2877 (links not components, 1 component can have N links)
//    //    LINKERMODEL (WithATC:538;WithoutATC:824) 1362"
//    //    LINKERNATURE 0
//    //    LEFT 1790
//    //    CONFIDENCE INDICE 67.4071

//    // 17 Feb 2012
//    //    NUMBER OF MOLECULES 5340
//    //    CORRECTED BY NAME 0
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 3228 (links not components, 1 component can have N links)
//    //    LINKERMODEL (WithATC:548;WithoutATC:829) 1377"
//    //    LINKERNATURE 405
//    //    LEFT 1282
//    //    CONFIDENCE INDICE 75

//    // 13 Dec 2011: using all length of ATC codes
//    //    NUMBER OF MOLECULES 5230
//    //    CORRECTED BY NAME 0
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 3216 "
//    //    LINKERMODEL (WithATC:550;WithoutATC:829) 1379"
//    //    LINKERNATURE 403
//    //    LEFT 1184
//    //    CONFIDENCE INDICE 77

//    // 18 Oct 2011
//    //    NUMBER OF MOLECULES 5211
//    //    CORRECTED BY NAME 0
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 3114 "
//    //    LINKERMODEL (WithATC:617;WithoutATC:833) 1450"
//    //    LINKERNATURE 470
//    //    LEFT 1263
//    //    CONFIDENCE INDICE 75

//    // 29 Sept 2011 (ATC 2011 && ATC 2012)
//    //    NUMBER OF MOLECULES 5194
//    //    CORRECTED BY NAME 0
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 3036 "
//    //    LINKERMODEL (WithATC:682;WithoutATC:829) 1511"
//    //    LINKERNATURE 558
//    //    LEFT 1328
//    //    CONFIDENCE INDICE 74

//    // 29 Apr 2011
//    //    NUMBER OF MOLECULES 5154
//    //    CORRECTED BY NAME 0
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 2856 "
//    //    LINKERMODEL (WithATC:568;WithoutATC:790) 1358"
//    //    LINKERNATURE 528
//    //    LEFT 1507
//    //    CONFIDENCE INDICE 70

//    // 10 Dec 2010
//    //    NUMBER OF MOLECULES 5112
//    //    CORRECTED BY NAME 0
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 2375 "
//    //    LINKERMODEL (WithATC:366;WithoutATC:790) 1156"
//    //    LINKERNATURE 301
//    //    LEFT 1648
//    //    CONFIDENCE INDICE 61

//    // 13 Nov 2010
//    //    NUMBER OF MOLECULES 5113
//    //    CORRECTED BY NAME 0
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 2304
//    //    LINKERMODEL 283
//    //    LINKERNATURE 302
//    //    LEFT 2511

//    // 20 Sept 2010
//    //    FOUNDED 1825
//    //    LINKERMODEL 279
//    //    LINKERNATURE 412
//    //    LEFT 1203

//    // 08 Sept 2010
//    // Removing french prefix
//    //    Number of marketed drugs 10096
//    //    Number of distinct molecules 2928
//    //    FOUNDED 1764
//    //    LINKERMODEL 199
//    //    LINKERNATURE 392
//    //    LEFT 1262

//    // 05 Sept 2010
//    // Removing non marketed drugs
//    //    Number of marketed drugs 10096
//    //    Number of distinct molecules 2928
//    //    FOUNDED 1702
//    //    LINKERMODEL 128
//    //    LINKERNATURE 387
//    //    LEFT 1321

//    // 28 August 2010
//    // 5472 ATCs
//    // Number of distinct molecules
//    //    Hand 0
//    //    FOUNDED 2176
//    //    LINKERMODEL 97
//    //    LINKERNATURE 299
//    //    LEFT 2613

//    // 27 August 2010
//    // 5472 ATCs
//    // Number of distinct molecules 5069
//    //    Hand 0
//    //    FOUNDED 2128
//    //    LINKERMODEL 34
//    //    LINKERNATURE 304
//    //    LEFT 2647

//    // Old code
//    //   5074 Molecules
//    //   1502 Same name MOL <-> ATC asso
//    //   239 Partial name MOL <-> ATC asso
//    //   2 from LK_NATURE
//    //   Total = 2253 associations

//    if (licenseType() == Free)
//        return true;

//    // Connect to databases
//    if (!checkDatabase())
//        return false;

//    Q_EMIT progressLabelChanged(tr("Linking drugs components to ATC codes"));
//    Q_EMIT progressRangeChanged(0, 2);
//    Q_EMIT progress(0);

////    // Associate Mol <-> ATC for drugs with one molecule only
////    MoleculeLinkerModel *model = drugsDbCore()->moleculeLinkerModel();
////    MoleculeLinkData data(drugEssentialDatabase(), sourceId(), ::FR_DRUGS_DATABASE_NAME, "fr");
////    if (!model->moleculeLinker(&data))
////        return false;

//    Q_EMIT progress(1);

//    Q_EMIT progressLabelChanged(tr("Saving components to ATC links to database"));
//    Q_EMIT progressRangeChanged(0, 1);
//    Q_EMIT progress(0);

//    // Save to links to drugs database
////    Tools::addComponentAtcLinks(drugEssentialDatabase(), data.moleculeIdToAtcId, sourceId());

//    LOG(QString("Database processed"));

//    // add unfound to extralinkermodel
//    Q_EMIT progressLabelChanged(tr("Updating component link XML file"));
////    model->addUnreviewedMolecules(::FR_DRUGS_DATABASE_NAME, data.unfoundMoleculeAssociations);
////    model->saveModel();
//    Q_EMIT progress(1);

//    return true;
//}
