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
#include "fdadrugsdatabase.h"
#include <drugsdbplugin/drug.h>

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/fdm_constants.h>
#include <coreplugin/isettings.h>

#include <drugsdbplugin/drugdatabasedescription.h>
#include <drugsdbplugin/tools.h>

#include <drugsbaseplugin/drugbaseessentials.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <utils/httpdownloader.h>
#include <extensionsystem/pluginmanager.h>
#include <quazip/global.h>
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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QMultiHash>
#include <QHash>

#include <QDebug>

// For your tests, you can limit the number of drugs computed and inserted into the database
// using this debugging enum. Set to -1 if you want all drugs to be processed
enum { LimitDrugsTo = -1 };

using namespace DrugsDb;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

FdaDrugDatabase::FdaDrugDatabase(QObject *parent) :
    IDrugDatabase(parent),
    m_WithProgress(false)
{
    setObjectName("FdaDrugDatabase");
    setDatabaseUid("FDA_US");
    setDatabaseLanguage("en");
    setCountry(QLocale::UnitedStates);
    setTempPath(QString("%1/%2")
                .arg(settings()->value(Core::Constants::S_TMP_PATH).toString())
                .arg("FdaRawSources"));
    setConnectionName("fda_free");
    setDatabaseDescriptionFile(QString("%1/%2/%3")
                               .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                               .arg(Core::Constants::PATH_TO_DRUG_DATABASE_DESCRIPTION_FILES)
                               .arg("us/description.xml"));
    setDownloadUrl("http://www.fda.gov/downloads/Drugs/InformationOnDrugs/UCM054599.zip");
    setLicenseType(Free);
    createTemporaryStorage();
}

FdaDrugDatabase::~FdaDrugDatabase()
{
}

void FdaDrugDatabase::setLicenseType(LicenseType type)
{
    IDrugDatabase::setLicenseType(type);
    if (type==NonFree) {
        setDisplayName(tr("Non-free FDA drugs database"));
        setConnectionName("fda_nonfree");
        setServerOwner(FrenchAssociation);
        setDatapackDescriptionFile(QString("%1/%2/drugs/%3/%4")
                                   .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                   .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                                   .arg("fda_ddi")
                                   .arg(DataPack::Constants::PACKDESCRIPTION_FILENAME));
    } else {
        setDisplayName(tr("Free FDA drugs database"));
        setConnectionName("fda_free");
        setServerOwner(Community);
        setDatapackDescriptionFile(QString("%1/%2/drugs/%3/%4")
                                   .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                   .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                                   .arg("fda_noddi")
                                   .arg(DataPack::Constants::PACKDESCRIPTION_FILENAME));
    }
    setOutputPath(QString("%1/%2/%3")
                  .arg(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString())
                  .arg("/drugs/")
                  .arg(connectionName())
                  );
}

QString FdaDrugDatabase::processMessage() const
{
    if (licenseType() == NonFree)
        return tr("Non-free FDA drugs database creation");
    return tr("Free FDA drugs database creation");
}

bool FdaDrugDatabase::process()
{
    unzipFiles();
    prepareData();
    createDatabase();
    populateDatabase();
    // linkMolecules();
    Q_EMIT processFinished();
    return true;
}

class Parser {
public:
    enum Fields {
        ApplNo = 0,
        ProductNo,
        FormRoute,
        Dosage,
        ProductMktStatus,
        TECode,
        ReferenceDrug,
        DrugName,
        ActiveIngred,
        TotalFields
    };

    Parser(const QString &line)
    {
        // ApplNo	ProductNo	Form	Dosage	ProductMktStatus	TECode	ReferenceDrug	drugname	activeingred
        // 000004	004	SOLUTION/DROPS;OPHTHALMIC	1%	3		0	PAREDRINE	HYDROXYAMPHETAMINE HYDROBROMIDE
        // 000159	001	TABLET;ORAL	500MG	3		0	SULFAPYRIDINE	SULFAPYRIDINE

        // Process line
        QStringList vals = line.split("\t");
        if (vals.count() != TotalFields) {
            qWarning() << "Error with line" << line;
        }

        // Get uid
        uid1 = vals.at(ApplNo);
        uid2 = vals.at(ProductNo);

        // Get form
        QString forms = vals.at(FormRoute);

        if (forms.contains(";")) {
            int begin = forms.lastIndexOf(";") + 1;
            route = forms.mid(begin).toLower().simplified();
            form = forms.left(begin - 1).toLower().simplified();
        } else {
            form = vals.at(FormRoute);
        }

        // Get strength
        globalStrength = vals.at(Dosage);
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
            QStringList mols = vals.at(ActiveIngred).split(";");
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
            mols_strength.insert(vals.at(ActiveIngred).simplified(), vals.at(Dosage));
        }

        // Get drug brand name
        name = vals.at(DrugName);
    }

    Drug *getDrug()
    {
        Drug *drug = new Drug;
        drug->setData(Drug::Uid1, uid1);
        drug->setData(Drug::Uid2, uid2);
        drug->setData(Drug::OldUid, QString(uid1 + uid2));
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

bool FdaDrugDatabase::prepareData()
{
    return true;
}

bool FdaDrugDatabase::populateDatabase()
{
    if (!checkDatabase())
        return false;

    // check files
    if (!prepareData())
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
        if (!QFile::exists(tempPath() + QDir::separator() + file)) {
            LOG_ERROR(QString("Missing " + tempPath() + QDir::separator() + file + " file. prepareDatas()"));
            return false;
        }
    }

    // Product file
    QVector<Drug *> drugs;

    QFile file(tempPath() + QDir::separator() + "Product.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("ERROR: Enable to open Product.txt: %1.").arg(file.errorString()));
        Q_EMIT progress(1);
        return false;
    }
    Q_EMIT progressLabelChanged(tr("Reading drugs raw source"));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    Q_EMIT progressRangeChanged(0, file.size());
    Q_EMIT progress(0);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    QTextStream in(&file);
    in.setCodec("ISO 8859-1");
    int pos = 0;
    while (!in.atEnd()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
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
            if (LimitDrugsTo != -1) {
                if (drugs.count() == LimitDrugsTo)
                    break;
            }
            Q_EMIT progress(pos);
        }
    }
    file.close();

    Q_EMIT progressLabelChanged(tr("Saving drugs into database"));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(1);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    saveDrugsIntoDatabase(drugs);
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(2);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Run SQL commands one by one
//    if (!Tools::executeSqlFile(connectionName(), finalizationScript())) {
//        LOG_ERROR("Can create FDA DB.");
//        Q_EMIT progress(3);
//        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
//        return false;
//    }

    LOG(QString("Database processed"));
    Q_EMIT progress(3);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    qDeleteAll(drugs);
    drugs.clear();

    return true;
}

//bool FdaDrugDatabase::linkMolecules()
//{
    // 21 Apr 2014
    // Added 2122 components to database.
    // Added 30987 drugs to database.
    // Added ATC data to database.
    // *** Starting drug component to ATC linkage ***
    //   . 6092 ATC codes retrieved from database
    //   . 2122 drug components retrieved from database
    //   . Links processed.
    //     . Suggestion found (number of components): 171.
    // Updating confidence indice to 8.
    //   . Links saved to database.

//    // 17 Feb 2012
//    //    NUMBER OF MOLECULES 2033
//    //    CORRECTED BY NAME 11
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 1574 "
//    //    LINKERMODEL (WithATC:250;WithoutATC:1) 251"
//    //    LINKERNATURE 0
//    //    LEFT 458
//    //    CONFIDENCE INDICE 77

//    // 28 Sept 2011 (using EN translations of ATC labels + new ATC 2011 && 2012)
//    //    NUMBER OF MOLECULES 2014
//    //    CORRECTED BY NAME 11
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 1558 "
//    //    LINKERMODEL (WithATC:261;WithoutATC:1) 262"
//    //    LINKERNATURE 0
//    //    LEFT 455
//    //    CONFIDENCE INDICE 77

//    // 28 APR 2011
//    //    NUMBER OF MOLECULES 2000
//    //    CORRECTED BY NAME 11
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 1207 "
//    //    LINKERMODEL (WithATC:259;WithoutATC:1) 260"
//    //    LINKERNATURE 0
//    //    LEFT 792
//    //    CONFIDENCE INDICE 60

//    // 04 Dec 2010
//    //    NUMBER OF MOLECULES 1983
//    //    CORRECTED BY NAME 11
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 1343
//    //    LINKERMODEL 6
//    //    LINKERNATURE 0
//    //    LEFT 640

//    // 13 Nov 2010
//    //    NUMBER OF MOLECULES 1983
//    //    CORRECTED BY NAME 11
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 1337
//    //    LINKERMODEL 0
//    //    LINKERNATURE 0
//    //    LEFT 646


//    // 28 July 2010
//    // 1960 distinct mols
//    // Hand association : 20
//    // Found : 1349, Left: 612


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
//    MoleculeLinkData data(drugEssentialDatabase(), sourceId(), ::FDA_DRUGS_DATABASE_NAME, "fr");
//    // Associate Mol <-> ATC for drugs with one molecule only
//    data.correctedByName.insert("IOTHALAMATE", "SODIUM IOTHALAMATE (125I)");
//    data.correctedByName.insert("IOTHALAMATE SODIUM I-125" , "SODIUM IOTHALAMATE (125I)");
//    data.correctedByName.insert("POLYMYXIN" ,"POLYMYXIN B" );
//    data.correctedByName.insert("POLYMYXIN B SULFATE" ,"POLYMYXIN B" );
//    data.correctedByName.insert("THIAMINE", "THIAMINE (VIT B1)");
//    data.correctedByName.insert("GRISEOFULVIN, ULTRAMICROCRYSTALLINE" ,"GRISEOFULVIN");
//    data.correctedByName.insert("GONADOTROPIN, CHORIONIC" ,"CHORIONIC GONADOTROPIN" );
//    data.correctedByName.insert("TYROPANOATE SODIUM" ,"TYROPANOIC ACID" );
//    data.correctedByName.insert("SODIUM NITROPRUSSIDE","NITROPRUSSIDE");
//    data.correctedByName.insert("IOXAGLATE SODIUM" ,"IOXAGLIC ACID");
//    data.correctedByName.insert("IOXAGLATE MEGLUMINE", "IOXAGLIC ACID");
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
//    model->addUnreviewedMolecules(::FDA_DRUGS_DATABASE_NAME, data.unfoundMoleculeAssociations);
//    model->saveModel();
//    Q_EMIT progress(1);

//    return true;
//}

