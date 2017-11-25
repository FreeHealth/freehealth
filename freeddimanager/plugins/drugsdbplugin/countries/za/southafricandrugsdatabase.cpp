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
/**
 * \class DrugsDb::Internal::FreeSouthAfricanDrugsDatabasePage
 * Option page for the Free South African drugs database.
 * The ctor also creates the DrugsDb::Internal::IDrugDatabaseStep object and
 * registers it in the plugin manager object pool.
 * \n FreeToolBox specific class.
 */

/**
 * \class DrugsDb::Internal::NonFreeSouthAfricanDrugsDatabasePage
 * Option page for the non-free French drugs database.
 * The ctor also create the DrugsDb::Internal::IDrugDatabase object and
 * register it in the plugin manager object pool.
 * \n FreeToolBox specific class.
 */

#include "southafricandrugsdatabase.h"
#include "drug.h"

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/fdm_constants.h>

#include <drugsdbplugin/drugdatabasedescription.h>
#include <drugsdbplugin/tools.h>

#include <drugsbaseplugin/drugbaseessentials.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpmultidownloader.h>
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
#include <QTimer>

#include <QDebug>

using namespace DrugsDb;
using namespace Internal;
using namespace Trans::ConstantTranslations;

// For your tests, you can limit the number of drugs computed and inserted into the database
// using this debugging enum. Set to -1 if you want all drugs to be processed
enum {
    DownloadIndexes = true,             // Do not download html indexes files (you have to make sure that files were already downloaded)
    LimitDrugProcessingTo = -1          // Set to -1 to process all available drugs
};

// get drugs name
// pages http://home.intekom.com/pharm/index/index_T_*.shtml
// html_mask <LI><A HREF=/pharm/lennon/a-l-amox.html>A-LENNON AMOXYCILLIN 250 mg CAPSULES</A><BR>

// get inn/composition
// <FONT SIZE=2 COLOR=ff0000><B>****</B>

namespace {
const char* const  ZA_URL                     = "http://home.intekom.com/pharm/index/index_T_%1.shtml";
const char* const  ZA_DRUGS_DATABASE_NAME     = "SAEPI_ZA";
}

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString uidFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/drugdb/za/za_uids.csv");}

static char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

ZaDrugDatabase::ZaDrugDatabase(QObject *parent) :
    DrugsDb::Internal::IDrugDatabase(parent)
{
    setObjectName("ZaDrugDatabase");
    setDatabaseUid("SAEPI_ZA");
    setDatabaseLanguage("en");
    setCountry(QLocale::SouthAfrica);
    setTempPath(QString("%1/%2")
                .arg(settings()->value(Core::Constants::S_TMP_PATH).toString())
                .arg("/ZaRawSources/"));
    setConnectionName("za_free");
    setDatabaseDescriptionFile(QString("%1/%2/%3")
                               .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                               .arg(Core::Constants::PATH_TO_DRUG_DATABASE_DESCRIPTION_FILES)
                               .arg("za/description.xml"));
    setLicenseType(Free);
    setDownloadUrl("");
    createTemporaryStorage();
}

ZaDrugDatabase::~ZaDrugDatabase()
{
}

void ZaDrugDatabase::setLicenseType(LicenseType type)
{
    IDrugDatabase::setLicenseType(type);
    if (type==NonFree) {
        setDisplayName(tr("Non-free South African drugs database"));
        setConnectionName("za_nonfree");
        setServerOwner(FrenchAssociation);
        setDatapackDescriptionFile(QString("%1/%2/drugs/%3/%4")
                                   .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                   .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                                   .arg("za_ddi")
                                   .arg(DataPack::Constants::PACKDESCRIPTION_FILENAME));
    } else {
        setDisplayName(tr("Free South African drugs database"));
        setConnectionName("za_free");
        setServerOwner(Community);
        setDatapackDescriptionFile(QString("%1/%2/drugs/%3/%4")
                                   .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
                                   .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
                                   .arg("za_noddi")
                                   .arg(DataPack::Constants::PACKDESCRIPTION_FILENAME));
    }
    setOutputPath(QString("%1/%2/%3")
                  .arg(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString())
                  .arg("/drugs/")
                  .arg(connectionName())
                  );
}

bool ZaDrugDatabase::createTemporaryStorage()
{
    QDir().mkpath(tempPath() + "/spc/");
    QDir().mkpath(tempPath() + "/indexes/");
    return DrugsDb::Internal::IDrugDatabase::createTemporaryStorage();
}

bool ZaDrugDatabase::startDownload()
{
    // The extraction of the ZA database works in two download sets.
    // The firstly, we download all html index files from the root server.
    // One index file per alphabetic letter.
    // Then when all these indexes are downloaded, we can extract from their content
    // all the SPC links and drugs name. The second step is represented by a massive download of
    // all locally missing SPC files.
    Q_EMIT progressLabelChanged(tr("South African database extraction: reading indexes"));

    // Create the multi downloader
    Utils::HttpMultiDownloader *_multiDownloader = new Utils::HttpMultiDownloader(this);
    _multiDownloader->setUseUidAsFileNames(true);
    _multiDownloader->setOutputPath(tempPath() + "/indexes/");

    // Create all indexes URL
    QList<QUrl> _urls;
    if (DownloadIndexes) {
        for(int i = 0; i < 26; ++i) {
            _urls << QUrl(QString(ZA_URL).arg(letters[i]));
        }
    } else {
        _multiDownloader->readXmlUrlFileLinks();
    }

    // Start url downloading
    _multiDownloader->setUrls(_urls);
    connect(_multiDownloader, SIGNAL(allDownloadFinished()), this, SLOT(onIndexFilesDownloadFinished()), Qt::UniqueConnection);
    _multiDownloader->startDownload();
    return true;
}

void ZaDrugDatabase::getAllDrugLinksFromIndexesFiles()
{
    m_Drug_Link.clear();
    Utils::HttpMultiDownloader *_multiDownloader = new Utils::HttpMultiDownloader(this);
    _multiDownloader->setUseUidAsFileNames(true);
    _multiDownloader->setOutputPath(tempPath() + "/indexes/");
    _multiDownloader->readXmlUrlFileLinks();

    // Get all drugs name && spc files URL
    for(int i = 0; i < 26; ++i) {
        QUrl url = QUrl(QString(ZA_URL).arg(letters[i]));
        QString content = Utils::readTextFile(_multiDownloader->outputAbsoluteFileName(url), Utils::DontWarnUser);

        if (content.isEmpty()) {
            LOG_ERROR(QString("Index file is empty %1").arg(url.toString()));
            continue;
        }

        // Get drugs pages
        int begin = content.indexOf("<UL>");
        int max = content.indexOf("</UL>");
        while (begin < max) {
            begin = content.indexOf("<LI><A HREF=", begin);
            if (begin==-1)
                break;
            begin += 12;
            // /pharm/aspen-p/a-abac-s.html>ASPEN ABACAVIR 20 mg/mL (oral solution)
            int end = content.indexOf(">", begin);
            QString link = content.mid(begin, end-begin);
            begin = end + 1;
            end = content.indexOf("</A><BR>", begin);
            QString drug = content.mid(begin, end-begin);
            drug.replace("&reg;", "®");
            drug.replace("&#153;", "™");
            drug.replace("&#145;", "");
            drug.replace("&#146;", "");
            drug.replace("&#150;", "–");
            drug.replace("&quot;","‘");
            drug.replace("&amp;","&");
            drug.replace("&#147;","“");
            drug.replace("&#148;","”");
            drug.replace("&#181;","µ");
            drug.replace("&Eacute;","É");
            drug.replace("&Egrave;","È");
            drug.replace("&#196;","Ä");
            drug.replace("&Iuml;","Ï");
            link.prepend("http://home.intekom.com");
            m_Drug_Link.insert(drug, link);
        }
    }
    delete _multiDownloader;
}

bool ZaDrugDatabase::onIndexFilesDownloadFinished()
{
    qWarning() << "-------------------- onIndexFilesDownloadFinished()";
    Utils::HttpMultiDownloader *_multiDownloader = qobject_cast<Utils::HttpMultiDownloader *>(sender());
    if (!_multiDownloader)
        return false;
    _multiDownloader->saveXmlUrlFileLinks();
    disconnect(_multiDownloader, SIGNAL(allDownloadFinished()), this, SLOT(onIndexFilesDownloadFinished()));
    delete _multiDownloader;

    // Here is the second step of the ZA drugs database download: examine all HTML indexes and
    // extract all drugs name & SPC links. Then start the download of the missing files.

    getAllDrugLinksFromIndexesFiles();

    // Download SPC files
    Utils::HttpMultiDownloader *_spcDownloader = new Utils::HttpMultiDownloader(this);
    _spcDownloader->setUseUidAsFileNames(true);
    _spcDownloader->setOutputPath(tempPath() + "/spc/");
    _spcDownloader->readXmlUrlFileLinks();

    // Remove already downloaded files from the queue
    QList<QUrl> urls;
    foreach(const QString &link, m_Drug_Link.values()) {
        QUrl url(link);
        if (!_spcDownloader->urls().contains(url) && !urls.contains(url))
            urls << url;
    }

    // Start the download
    _spcDownloader->setUrls(urls);
    connect(_spcDownloader, SIGNAL(allDownloadFinished()), this, SLOT(onSpcDownloadFinished()), Qt::UniqueConnection);
    return _spcDownloader->startDownload();
}

bool ZaDrugDatabase::onSpcDownloadFinished()
{
    Utils::HttpMultiDownloader *_multiDownloader = qobject_cast<Utils::HttpMultiDownloader *>(sender());
    qWarning() << "-------------------- onSpcDownloadFinished()" << _multiDownloader;
    if (!_multiDownloader)
        return false;
    _multiDownloader->saveXmlUrlFileLinks();
    disconnect(_multiDownloader, SIGNAL(allDownloadFinished()), this, SLOT(onSpcDownloadFinished()));
    delete _multiDownloader;
    Q_EMIT downloadFinished();
    return true;
}

QString ZaDrugDatabase::processMessage() const
{
    if (licenseType() == NonFree)
        return tr("Non-free South African drugs database creation");
    return tr("Free South African drugs database creation");
}

bool ZaDrugDatabase::process()
{
    prepareData();
    createDatabase();
    populateDatabase();
    // linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool ZaDrugDatabase::unzipFiles()
{
    // Nothing to do here
    return true;
}

#include <drugsbaseplugin/constants_databaseschema.h>
bool ZaDrugDatabase::prepareData()
{
    WARN_FUNC;
    Q_EMIT progressLabelChanged(tr("South African database extraction: parsing drugs page"));
    m_Drug_Link.clear();
    getAllDrugLinksFromIndexesFiles();
    return true;
}

class DrugFileParser {
public:
    DrugFileParser(const QString &drugName, const QString &fullContent) :
        name(drugName)
    {
        //        getFormParagraph(fullContent);
        getComposition(fullContent);
        //        getClassificationParagraph(fullContent);
        //        getPresentationParagraph(fullContent);
        //        getRegistrationParagraph(fullContent);
        //        if (!registrationNumberParagraph.isEmpty()) {
        //            getRegistrationNumber();
        //        }
    }

    void getFormParagraph(const QString &)
    {
    }

    void getComposition(const QString &fullContent)
    {
        static QStringList starts;
        static QStringList ends;
        if (starts.isEmpty()) {
            starts << "COMPOSITION" <<"COMPOSITON"<< "PROPRIETARY NAME" << "DESCRIPTION" << "ACTIVE INGREDIENTS"
                   << "Description/Composition" << "NUTRITIONAL INFORMATION" << "Composition";
        }
        if (ends.isEmpty()) {
            ends << "PHARMACOLOGICAL CLASSIFICATION" << "INDICATIONS" << "CLINICAL PHARMACOLOGY"
                 << "Administration" << "DOSAGE AND DIRECTIONS FOR USE:"<< "WHAT IS CALCIUM?";
        }
        compositionParagraph = getBlock(starts, ends, fullContent, "Composition");

        // get COMPOSITION : <FONT SIZE=2 COLOR=ff0000><B>****</B>
        int begin = 0;
        int max = compositionParagraph.length();
        int end = 0;
        //        int nbColoredText = fullContent.count("COLOR=ff0000");
        while (begin < max && begin != -1) {
            begin = compositionParagraph.indexOf("<FONT SIZE=2 COLOR=ff0000><B>", begin);
            if (begin > 0) {
                begin += 29;
                end = compositionParagraph.indexOf("</B>", begin);
                QString tmp = compositionParagraph.mid(begin, end-begin).simplified();
                tmp.remove("<BR>");
                tmp.remove("<I>");
                tmp.remove("</I>");
                tmp.remove("<SUB>");
                tmp.remove("</SUB>");
                if (!tmp.isEmpty())
                    inns.append(tmp.toUpper());
            }
        }
        if (inns.isEmpty()) {
            LOG_FOR("ZADB", QString("No composition for %1").arg(name));
            // qWarning() << fullContent;
        }
    }

    void getClassificationParagraph(const QString &)
    {
    }

    void getPresentationParagraph(const QString &)
    {
    }

    void getRegistrationParagraph(const QString &fullContent)
    {
        // startsWith: REGISTRATION NUMBERS: || REGISTRATION NUMBER: || REGISTRATION NO: || APPLICATION NUMBERS:
        // endsWith: NAME AND BUSINESS ADDRESS OF

        // ALLPYRAL PURE MITE TREATMENT SET --> reg no in PRESENTATION
        static QStringList starts;
        static QStringList ends;
        if (starts.isEmpty()) {
            starts << "REGISTRATION NUMBERS" << "REGISTRATIONS NUMBERS" << "REGISTRATION NUMBER" << "REGISTRATION NO"
                   << "REGISTRATION HUMBER" << "APPLICATION NUMBER" << "REFERENCE NUMBERS"
                   << "REFERENCE NUMBER" << "REFERENCE NO" << "APPLICATION NO"<< "REGISTRATION:";
        }
        if (ends.isEmpty()) {
            ends << "NAME AND BUSINESS ADDRESS" << "NAME AND ADDRESS OF" << "NAME OF APPLICANT"
                 << "NAME OF BUSINESS AND ADDRESS" << "NAME AND BUSINESS OF"
                 << "NAME & BUSINESS ADDRESS" << "<B>APPLICANT";
        }
        registrationNumberParagraph = getBlock(starts, ends, fullContent, "Registration");
    }

    void getRegistrationNumber()
    {
        // Paragraph contains only one reg number
        // ...<BR></B>....<BR>
        int begin = registrationNumberParagraph.indexOf("REGISTRATION NUMBER:") + 19;
        int max = registrationNumberParagraph.indexOf("NAME AND BUSINESS ADDRESS OF", begin);
        if (begin > 19) {
            int end = -1;
            begin = registrationNumberParagraph.indexOf("</B>", begin) + 4;
            end = registrationNumberParagraph.indexOf("<BR>", begin);
            regNumber = registrationNumberParagraph.mid(begin, end-begin);
            regNumber.remove("&nbsp;");
            regNumber = regNumber.trimmed();
        } else {
            begin = registrationNumberParagraph.indexOf("REGISTRATION NUMBERS:") + 20;
            if (begin > 19) {
                qWarning() << "TESTING REGISTRATION NUMBERS:";
                QString tmp = registrationNumberParagraph.mid(begin, max-begin);
                tmp.replace("&nbsp;", " ");

                //                qWarning() << tmp << tmp.contains(drug, Qt::CaseInsensitive);

                QString test;
                // firstly test the entire drug name
                if (tmp.contains(name, Qt::CaseInsensitive)) {
                    int b = tmp.indexOf(name, 0, Qt::CaseInsensitive) + name.length();
                    int e = tmp.indexOf("<BR>", b);
                    QString u = tmp.mid(b, e-b);
                    //                    qWarning() << u;
                    int z = u.indexOf("</");
                    if (z>=0) {
                        z = u.indexOf(">", z) + 1;
                        u = u.mid(z, u.indexOf("<BR>", z)).trimmed();
                        if (!u.contains("<"))
                            regNumber = u;
                        qWarning() << "UID " << u;
                    } else {
                        z=0;
                    }
                    //                    qWarning() << "COMPLEX" << tmp.mid(b, e-b);
                } else {
                    foreach(const QString &word, name.split(" ", QString::SkipEmptyParts)) {
                        test += " " + word;
                        test = test.trimmed();
                        //                        if (tmp.count(test, Qt::CaseInsensitive) == 1) {
                        //                            int b = tmp.indexOf(test) + test.length();
                        //                            int e = tmp.indexOf("<BR>");
                        ////                            qWarning() << "COMPLEX" << tmp << tmp.mid(b, e-b).trimmed();
                        //                        }
                    }
                }

            }
        }
    }

    QString getBlock(const QStringList &starts, const QStringList &ends, const QString &fullContent, const QString &extraWarning)
    {
        int begin = -1;
        int length = 0;
        foreach(const QString &start, starts) {
            begin = fullContent.indexOf(start);
            if (begin != -1) {
                length = start.length();
                break;
            }
        }
        if (begin==-1) {
            qWarning() << "Paragraph not found (begin)" << extraWarning << begin;
            //            qWarning() << fullContent;
            qWarning();
            qWarning();
            return QString();
        }
        begin += length;
        int end = -1;
        // FIXME: this code is buggy
        foreach(const QString &endString, ends) {
            end = fullContent.indexOf(endString);
            if (end != -1) {
                length = endString.length();
                break;
            }
        }
        if (end==-1) {
            qWarning() << "Paragraph not found (end)" << extraWarning << begin << end;
            //            qWarning() << fullContent;
            qWarning();
            qWarning();
            return QString();
        }
        return fullContent.mid(begin, end - begin);
    }

    // UID is not set
    Drug *getDrug() const
    {
        Drug *drug = new Drug;
        drug->setData(Drug::Name, name);
        drug->setData(Drug::Forms, forms);
        drug->setData(Drug::Marketed, 1);
        drug->setData(Drug::Valid, 1);
        int i = 0;
        foreach(const QString &inn, inns) {
            ++i;
            Component *compo = new Component;
            compo->setData(Component::Name, inn);
            compo->setData(Component::Nature, "SA");
            compo->setData(Component::NatureLink, i);
            drug->addComponent(compo);
        }

        return drug;
    }

    QString formParagraph;
    QString compositionParagraph;
    QString classificationParagraph;
    QString presentationParagraph;
    QString registrationNumberParagraph;

private:
    QString name;
    QString regNumber;
    QString forms;
    QString presentation;
    QStringList inns;

};

static int readUids(QHash<QString, int> &drugs_uids)
{
    int lastUid = 20100001;
    QString content = Utils::readTextFile(uidFile());
    if (content.isEmpty())
        LOG_ERROR_FOR("ZaDrugDatatabaseStep", "Unable to read UIDs file");
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
            LOG_ERROR_FOR("ZaDrugDatatabaseStep", QString("Line : %1 , does not contains 2 values").arg(line));
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
            "//  *   eric.maeker@gmail.com                                                 *\n"
            "//  *   All rights reserved.                                                  *\n"
            "//  ***************************************************************************/\n"
            "// /***************************************************************************\n"
            "//  *   Owner :  Eric MAEKER, MD <eric.maeker@gmail.com>                      *\n"
            "//  ***************************************************************************/\n"
            "// /***************************************************************************\n"
            "//  * - Autogenerated file for the South African drugs database               *\n"
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
    if (!Utils::saveStringToFile(content.toUtf8(), uidFile(), Utils::Overwrite, Utils::DontWarnUser)) {
        LOG_ERROR_FOR("ZaDrugDatatabaseStep", "Unable to save UID file");
        return false;
    }
    return true;
}

bool ZaDrugDatabase::populateDatabase()
{
    WARN_FUNC;
    if (!checkDatabase()) {
        if (!createDatabase()) {
            LOG_ERROR("Unable to create drugs database");
            return false;
        }
    }

    // check files
    if (!prepareData()) {
        LOG_ERROR("Unable to prepare data");
        return false;
    }

    // 27 Oct 2012 : 2723 Drugs
    // 14 Aug 2012 : 2723 Drugs ->
    // 24 juin 2010 : 2682 drugs -> 1073 usable drugs

    Q_EMIT progressLabelChanged(tr("South African database creation : reading drugs uids"));
    Q_EMIT progressRangeChanged(0, m_Drug_Link.keys().count() + 1);
    Q_EMIT progress(0);
    QString regError, compoError;

    // Read the UID file from global_resources
    QHash<QString, int> drugs_uids;
    int lastUid = readUids(drugs_uids);

    Q_EMIT progressLabelChanged(tr("South African database creation : reading drugs page"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // Read url <-> file association
    Utils::HttpMultiDownloader *_multiDownloader = new Utils::HttpMultiDownloader(this);
    _multiDownloader->setUseUidAsFileNames(true);
    _multiDownloader->setOutputPath(tempPath() + "/spc/");
    _multiDownloader->readXmlUrlFileLinks();

    int progr = 0;
    QVector<Drug *> drugs;
    foreach(const QString &drugName, m_Drug_Link.keys()) {
        ++progr;
        Q_EMIT progress(progr);
        if (LimitDrugProcessingTo != -1 && progr == LimitDrugProcessingTo)
            break;

        // Get the FreeDiams uid of the drug
        if (!drugs_uids.keys().contains(drugName)) {
            ++lastUid;
            drugs_uids.insert(drugName, lastUid);
            qWarning() << "CREATING UID FOR" << drugName << lastUid;
        }

        // Get the drugs file
        QString spcUrl = m_Drug_Link.value(drugName);
        QString fileName = _multiDownloader->outputAbsoluteFileName(QUrl(spcUrl));
        QString content = Utils::readTextFile(fileName, Utils::DontWarnUser);

        if (content.isEmpty()) {
            LOG_ERROR(QString("Content is empty. URL: %1; FileName: %2").arg(spcUrl).arg(fileName));
            continue;
        }
        if (content.contains("<title>404 Not Found</title>", Qt::CaseInsensitive)) {
            qWarning() << "No HTML file";
            continue;
        }

        // Some correction of the HTML code
        content.replace("&nbsp;", " ");
        content.replace("&reg;", "®");

        DrugFileParser parser(drugName, content);
        Drug *drug = parser.getDrug();
        drug->setData(Drug::Uid1, drugs_uids.value(drugName));
        drug->setData(Drug::OldUid, drugs_uids.value(drugName));
        drug->setData(Drug::Spc, spcUrl);
        drugs << drug;

        // test contents and create an error HTML output
        //        if (dr->registrationNumberParagraph.isEmpty()) {
        //            regError += QString("<li><a href=\"%1\">%2 - %3</a></li>")
        //                        .arg(QFileInfo(f).absoluteFilePath())
        //                        .arg(drug)
        //                        .arg("No Registration paragraph found.");
        //        }
        if (parser.compositionParagraph.isEmpty()) {
            compoError += QString("<li><a href=\"%1\">%2 - %3</a></li>")
                    .arg(QFileInfo(fileName).absoluteFilePath())
                    .arg(drugName)
                    .arg("No Composition paragraph");
        }
    }

    // Save the UIDS
    Q_EMIT progressLabelChanged(tr("South African database creation : processing..."));
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(1);

    // save uids and error HTML output
    saveUids(drugs_uids);
    QString tmp = QString("<html><body><ol>%1</ol><p>&nbsp;</p><ol>%2</ol></body></html>")
            .arg(regError).arg(compoError);
    Utils::saveStringToFile(tmp, tempPath() + "/incomplete.html", Utils::Overwrite, Utils::DontWarnUser);

    // save drugs to db
    saveDrugsIntoDatabase(drugs);
    Q_EMIT progressRangeChanged(0, 3);
    Q_EMIT progress(2);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Run SQL commands one by one
    //    Q_EMIT progressLabelChanged(tr("Running database finalization script"));
    //    if (!DrugsDb::Tools::executeSqlFile(_database, databaseFinalizationScript())) {
    //        LOG_ERROR("Can not create ZA DB.");
    //        return false;
    //    }

    LOG(QString("Database processed"));
    Q_EMIT progress(3);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    qDeleteAll(drugs);
    drugs.clear();
    return true;
}

//bool ZaDrugDatabase::linkMolecules()
//{

//    // 11 Jeb 2014
//    //    NUMBER OF DRUGS         2619
//    //    NUMBER OF MOLECULES     1145
//    //    CORRECTED BY NAME       23
//    //    CORRECTED BY ATC        0
//    //    FOUNDED                 663
//    //    LINKERMODEL (WithATC:122;WithoutATC:2) 124"
//    //    LINKERNATURE            0
//    //    LEFT                    482
//    //    CONFIDENCE INDICE       57.9039

//    WARN_FUNC;
//    // 21 May 2013
//    //    NUMBER OF MOLECULES 1145
//    //    CORRECTED BY NAME 23
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 559 "
//    //    LINKERMODEL (WithATC:122;WithoutATC:2) 124"
//    //    LINKERNATURE 0
//    //    LEFT 584
//    //    CONFIDENCE INDICE 48.9956

//    // 29 Sept 2011
//    //    NUMBER OF MOLECULES 1148
//    //    CORRECTED BY NAME 23
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 658 "
//    //    LINKERMODEL (WithATC:137;WithoutATC:2) 139"
//    //    LINKERNATURE 0
//    //    LEFT 488
//    //    CONFIDENCE INDICE 57

//    // 29 APR 2011
//    //    NUMBER OF MOLECULES 1148
//    //    CORRECTED BY NAME 23
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 658 "
//    //    LINKERMODEL (WithATC:137;WithoutATC:2) 139"
//    //    LINKERNATURE 0
//    //    LEFT 488
//    //    CONFIDENCE INDICE 57


//    // 10 Dec 2010
//    //    NUMBER OF MOLECULES 1148
//    //    CORRECTED BY NAME 23
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 657 "
//    //    LINKERMODEL (WithATC:140;WithoutATC:2) 142"
//    //    LINKERNATURE 0
//    //    LEFT 489
//    //    CONFIDENCE INDICE 57

//    // 13 Nov 2010
//    //    NUMBER OF MOLECULES 1148
//    //    CORRECTED BY NAME 23
//    //    CORRECTED BY ATC 0
//    //    FOUNDED 657
//    //    LINKERMODEL 140
//    //    LINKERNATURE 0
//    //    LEFT 491

//    // 23 Sept 2010
//    //    Number of distinct molecules 1159
//    //    Hand made association: 3
//    //    FOUNDED 654
//    //    LINKERMODEL 126
//    //    LINKERNATURE 0
//    //    LEFT 504

//    // 28 July 2010
//    // Using the new dcDrugsDb::Tools::englishMoleculeLinker()
//    // 1198 distinct mols
//    // Hand association: 27
//    // Found: 568, Left: 631

//    // Connect to databases
//    if (!checkDatabase())
//        return false;

//    Q_EMIT progressLabelChanged(tr("Linking drugs components to ATC codes"));
//    Q_EMIT progressRangeChanged(0, 2);
//    Q_EMIT progress(0);

//    // Associate Mol <-> ATC for drugs with one molecule only
//    MoleculeLinkerModel *model = drugsDbCore()->moleculeLinkerModel();
//    MoleculeLinkData data(drugEssentialDatabase(), sourceId(), ::ZA_DRUGS_DATABASE_NAME, "fr");
//    data.correctedByName.insert("CALCIUM (CALCIUM CARBONATE)", "CALCIUM CARBONATE");
//    data.correctedByName.insert("VITAMIN D3", "vitamine d");
//    data.correctedByName.insert("VITAMIN D3", "COLECALCIFEROL");
//    data.correctedByName.insert("VITAMIN D3 (CHOLECALCIFEROL)", "vitamine d");
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
//    model->addUnreviewedMolecules(::ZA_DRUGS_DATABASE_NAME, data.unfoundMoleculeAssociations);
//    model->saveModel();
//    Q_EMIT progress(1);

//    return true;
//}

bool ZaDrugDatabase::downloadSpcContents()
{
    WARN_FUNC;
    // The ZA database is created directly using the SPC of drugs.
    // So SPC files are already downloaded, we just need to send them to the drugs database.
    Utils::HttpMultiDownloader *_spcDownloader = new Utils::HttpMultiDownloader(this);
    _spcDownloader->setUseUidAsFileNames(true);
    _spcDownloader->setOutputPath(tempPath() + "/spc/");
    _spcDownloader->readXmlUrlFileLinks();

    // Create the SPC content and send them to the drugs database
    foreach(const QUrl &url, _spcDownloader->urls()) {
        SpcContent content;
        content.url = url.toString();
        content.html = Utils::readTextFile(_spcDownloader->outputAbsoluteFileName(url), Utils::DontWarnUser);
        saveDrugSpc(content);
    }

    QTimer::singleShot(1, this, SIGNAL(spcProcessFinished()));
    return true;
}
