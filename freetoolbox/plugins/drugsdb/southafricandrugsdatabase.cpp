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
#include "southafricandrugsdatabase.h"
#include "extramoleculelinkermodel.h"
#include "drug.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/global.h>
#include <utils/log.h>
#include <extensionsystem/pluginmanager.h>
#include <translationutils/constants.h>
#include <translationutils/trans_drugs.h>

#include <QApplication>
#include <QFile>
#include <QMultiHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDir>
#include <QDate>
#include <QHash>
#include <QStringList>
#include <QString>
#include <QProgressDialog>

#include <QNetworkReply>

#include <QDebug>

#include "ui_southafricandrugsdatabase.h"

using namespace DrugsDbCreator;
using namespace Trans::ConstantTranslations;

// get drugs name
// pages http://home.intekom.com/pharm/index/index_T_*.shtml
// html_mask <LI><A HREF=/pharm/lennon/a-l-amox.html>A-LENNON AMOXYCILLIN 250 mg CAPSULES</A><BR>

// get inn/composition
// <FONT SIZE=2 COLOR=ff0000><B>****</B>


const char* const  ZA_URL               = "http://home.intekom.com/pharm/index/index_T_%1.shtml";

const char* const  ZA_DRUGS_DATABASE_NAME     = "SAEPI_ZA";


static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline ExtensionSystem::PluginManager *pluginManager() {return ExtensionSystem::PluginManager::instance();}

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/ZARawSources/") + QDir::separator();}
static inline QString databaseAbsPath()  {return Core::Tools::drugsDatabaseAbsFileName();}

static inline QString databaseDescriptionFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/drugdb/za/description.xml");}
static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/drugdb/za/za_db_finalize.sql");}
static inline QString uidFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_SVNFILES_PATH).toString() + "/global_resources/sql/drugdb/za/za_uids.csv");}

SouthAfricanDrugsDatabasePage::SouthAfricanDrugsDatabasePage(QObject *parent) :
    IToolPage(parent)
{
    setObjectName("SouthAfricanDrugsDatabasePage");
}

QString SouthAfricanDrugsDatabasePage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_DRUGSDATABASE;
}

QWidget *SouthAfricanDrugsDatabasePage::createPage(QWidget *parent)
{
    return new SouthAfricanDrugsDatabase(parent);
}

static char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

ZaDrugDatatabaseStep::ZaDrugDatatabaseStep(QObject *parent) :
    Core::IFullReleaseStep(parent),
    m_Progress(0), m_WithProgress(false)
{
    setObjectName("ZaDrugDatatabaseStep");
}

ZaDrugDatatabaseStep::~ZaDrugDatatabaseStep()
{
    if (m_Progress)
        delete m_Progress;
}

bool ZaDrugDatatabaseStep::createDir()
{
    if (!QDir().mkpath(workingPath()))
        LOG_ERROR("Unable to create ZA Working Path :" + workingPath());
    else
        LOG("Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath)) {
            LOG_ERROR("Unable to create Canadian database output path :" + dbpath);
            m_Errors << tr("Unable to create Canadian database output path :") + dbpath;
        } else {
            LOG("Drugs database output dir created");
        }
    }
    return true;
}

bool ZaDrugDatatabaseStep::cleanFiles()
{
    QFile(databaseAbsPath()).remove();
    return true;
}

bool ZaDrugDatatabaseStep::downloadFiles(QProgressBar *bar)
{
    Q_UNUSED(bar);
    // get all tradename html pages from the site
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    Q_EMIT progressLabelChanged(tr("South African database extraction : reading indexes"));
    Q_EMIT progressRangeChanged(0, 26);
    Q_EMIT progress(0);

    m_nbOfDowloads = 26;
    for(int i = 0; i < m_nbOfDowloads; ++i) {
        manager->get(QNetworkRequest(QUrl(QString(ZA_URL).arg(letters[i]))));
    }
    return true;
}

void ZaDrugDatatabaseStep::replyFinished(QNetworkReply *reply)
{
    static int nb = 0;
    qWarning() << "get" << reply->errorString() << reply->isFinished() << reply->isReadable()
            << reply->url();
    QString content = reply->readAll();
    QString fileName = reply->url().toString(QUrl::RemoveScheme|QUrl::RemovePassword|QUrl::RemoveUserInfo);
    fileName.remove("//home.intekom.com/pharm/index/");
    //    delete reply;

    // save file
    {
        QFile file(workingPath() + fileName);
        if (!QDir(workingPath() + fileName).exists()) {
            QDir().mkpath(QFileInfo(workingPath() + fileName).absolutePath());
        }

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            LOG_ERROR(QString("ERROR : Enable to save %1. ZADrugsDB::replyFinished").arg(file.fileName()));
            return;
        }
        file.write(content.toAscii());
    }

    // Get drugs pages
    int begin = content.indexOf("<UL>");
    int max = content.indexOf("</UL>");
    while (begin<max) {
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
        m_Drug_Link.insert(drug, link);
    }

    // If the download list is completed
    ++nb;
    Q_EMIT progress(nb);

    if (nb==m_nbOfDowloads) {
        static bool done = false;
        if (!done) {
            done = true;
            m_nbOfDowloads = m_Drug_Link.count();

            Q_EMIT progressLabelChanged(tr("South African database extraction : reading drugs page"));
            Q_EMIT progressRangeChanged(0, m_nbOfDowloads);
            Q_EMIT progress(0);

            // download the link
            nb=0;
            qWarning() << "Downloading" << m_Drug_Link.count();
            bool downloadStarted = false;
            foreach(const QString &link, m_Drug_Link.values()) {
                if (!QFile(workingPath() + "/home.intekom.com/" + link).exists()) {
                    downloadStarted = true;
                    manager->get(QNetworkRequest(QUrl(QString("http://home.intekom.com%1").arg(link))));
                }
            }
            if (!downloadStarted) {
                Q_EMIT downloadFinished();
            }
        } else {
            Q_EMIT downloadFinished();
        }
    }
}

bool ZaDrugDatatabaseStep::process()
{
    prepareDatas();
    createDatabase();
    populateDatabase();
    linkMolecules();
    Q_EMIT processFinished();
    return true;
}

bool ZaDrugDatatabaseStep::prepareDatas()
{
    m_Drug_Link.clear();

    Q_EMIT progressLabelChanged(tr("South African database extraction : parsing drugs page"));
    Q_EMIT progressRangeChanged(0, 26);
    Q_EMIT progress(0);

    for(int i=0; i<26; ++i) {
        // check files
        QString fileName = QString("index_T_%1.shtml").arg(letters[i]);
        if (!QFile::exists(workingPath() + fileName)) {
            LOG_ERROR(QString("Missing " + workingPath() + fileName + " file. ZADrugsDB::prepareDatas()"));
            continue;
        }

        // read file
        LOG("Processing file :" + fileName);
        QString content = Utils::readTextFile(workingPath() + fileName);
        if (content.isEmpty()) {
            LOG_ERROR("no content");
            return false;
        }

        // get all drugsname
        int begin = content.indexOf("<UL>");
        int max = content.indexOf("</UL>");
        while (begin<max) {
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

            m_Drug_Link.insert(drug, link);
        }

        Q_EMIT progress(i);
    }
    return true;
}

bool ZaDrugDatatabaseStep::createDatabase()
{
    if (!Core::Tools::createMasterDrugInteractionDatabase())
        return false;

    QMultiHash<QString, QVariant> labels;
    labels.insert("fr","Base de données thérapeutique Sud Africaine");
    labels.insert("en","South African therapeutic database");
    labels.insert("de","South African therapeutischen database");

    if (Core::Tools::createNewDrugsSource(Core::Constants::MASTER_DATABASE_NAME, ZA_DRUGS_DATABASE_NAME, labels) == -1) {
        LOG_ERROR("Unable to create the French drugs sources");
        return false;
    }
    Core::Tools::saveDrugDatabaseDescription(databaseDescriptionFile(), 0);
    LOG(QString("Database schema created"));
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
        int end = -1;
        int max = registrationNumberParagraph.indexOf("NAME AND BUSINESS ADDRESS OF", begin);
        if (begin > 19) {
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
               "//  *   (C) 2008-2012 by Eric MAEKER, MD                                      *\n"
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
    if (!Utils::saveStringToFile(content.toUtf8(), uidFile())) {
        LOG_ERROR_FOR("ZaDrugDatatabaseStep", "Unable to save UID file");
        return false;
    }
    return true;
}

bool ZaDrugDatatabaseStep::populateDatabase()
{
    if (!Core::Tools::connectDatabase(ZA_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    // 14 Aug 2012 : 2732 Drugs ->
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

    int progr = 0;
    QVector<Drug *> drugs;
    foreach(const QString &drugName, m_Drug_Link.keys()) {
        ++progr;
        Q_EMIT progress(progr);

        // Get the FreeDiams uid of the drug
        if (!drugs_uids.keys().contains(drugName)) {
            ++lastUid;
            drugs_uids.insert(drugName, lastUid);
            qWarning() << "CREATING UID FOR" << drugName << lastUid;
        }

        // get the drugs file
        QString fileName = workingPath() + "/home.intekom.com/" + m_Drug_Link.value(drugName);
        QString content = Utils::readTextFile(fileName);

        if (content.isEmpty())
            continue;
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
    Utils::saveStringToFile(tmp, workingPath() + "/incomplete.html");

    // save drugs to db
    Drug::saveDrugsIntoDatabase(Core::Constants::MASTER_DATABASE_NAME, drugs, ZA_DRUGS_DATABASE_NAME);
    Q_EMIT progress(2);

    Core::Tools::saveDrugDatabaseDescription(databaseDescriptionFile(), 50);

    // Run SQL commands one by one
//    Q_EMIT progressLabelChanged(tr("Running database finalization script"));
//    if (!Core::Tools::executeSqlFile(Core::Constants::MASTER_DATABASE_NAME, databaseFinalizationScript())) {
//        LOG_ERROR("Can not create ZA DB.");
//        return false;
//    }

    qDeleteAll(drugs);
    return true;
}

bool ZaDrugDatatabaseStep::linkMolecules()
{
    // 29 Sept 2011
    //    NUMBER OF MOLECULES 1148
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 658 "
    //    LINKERMODEL (WithATC:137;WithoutATC:2) 139"
    //    LINKERNATURE 0
    //    LEFT 488
    //    CONFIDENCE INDICE 57

    // 29 APR 2011
    //    NUMBER OF MOLECULES 1148
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 658 "
    //    LINKERMODEL (WithATC:137;WithoutATC:2) 139"
    //    LINKERNATURE 0
    //    LEFT 488
    //    CONFIDENCE INDICE 57


    // 10 Dec 2010
    //    NUMBER OF MOLECULES 1148
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 657 "
    //    LINKERMODEL (WithATC:140;WithoutATC:2) 142"
    //    LINKERNATURE 0
    //    LEFT 489
    //    CONFIDENCE INDICE 57

    // 13 Nov 2010
    //    NUMBER OF MOLECULES 1148
    //    CORRECTED BY NAME 23
    //    CORRECTED BY ATC 0
    //    FOUNDED 657
    //    LINKERMODEL 140
    //    LINKERNATURE 0
    //    LEFT 491

    // 23 Sept 2010
    //    Number of distinct molecules 1159
    //    Hand made association: 3
    //    FOUNDED 654
    //    LINKERMODEL 126
    //    LINKERNATURE 0
    //    LEFT 504

    // 28 July 2010
    // Using the new dcCore::Tools::englishMoleculeLinker()
    // 1198 distinct mols
    // Hand association: 27
    // Found: 568, Left: 631

    // get all drugs and ATC codes
    if (!Core::Tools::connectDatabase(Core::Constants::MASTER_DATABASE_NAME, databaseAbsPath()))
        return false;

    QHash<QString, QString> corrected;
    corrected.insert("CALCIUM (CALCIUM CARBONATE)", "CALCIUM CARBONATE");
    corrected.insert("VITAMIN D3", "vitamine d");
    corrected.insert("VITAMIN D3", "COLECALCIFEROL");
    corrected.insert("VITAMIN D3 (CHOLECALCIFEROL)", "vitamine d");
    corrected.insert("VITAMIN D3 (CHOLECALCIFEROL)", "COLECALCIFEROL");
    corrected.insert("DIATRIZOATE SODIUM", "DIATRIZOIC ACID");
    corrected.insert("DIATRIZOATE MEGLUMINE", "DIATRIZOIC ACID");
    corrected.insert("IOXAGLATE MEGLUMINE", "IOXAGLIC ACID");
    corrected.insert("IOXAGLATE SODIUM", "IOXAGLIC ACID");
    corrected.insert("THIAMINE MONONITRATE", "THIAMINE (VIT B1)");
    corrected.insert("ETHINYL ESTRADIOL", "ETHINYLESTRADIOL");
    corrected.insert("NORGESTREL", "LEVONORGESTREL");
    corrected.insert("ALUMINUM CHLOROHYDRATE", "ALUMINIUM CHLOROHYDRATE");
    corrected.insert("VITAMIN B6 (PYRIDOXINE HYDROCHLORIDE)", "PYRIDOXINE (VIT B6)");
    corrected.insert("VITAMIN B1 (THIAMINE HYDROCHLORIDE)", "THIAMINE (VIT B1)");
    corrected.insert("FORMOTEROL FUMARATE DIHYDRATE", "FORMOTEROL");
    corrected.insert("FIBRINOGEN (HUMAN)", "FIBRINOGEN, HUMAN");
    corrected.insert("FIBRINOGEN (HUMAN)", "HUMAN FIBRINOGEN");
    corrected.insert("FACTOR XIII", "COAGULATION FACTOR XIII");
    corrected.insert("BETA-CAROTENE", "BETACAROTENE");
    corrected.insert("L-ARGININE" , "ARGININE HYDROCHLORIDE" );
    corrected.insert("L-LYSINE (L-LYSINE HYDROCHLORIDE)" ,"LYSINE" );
    corrected.insert("L-METHIONINE" ,"METHIONINE" );
    corrected.insert("GLUTAMIC ACID" ,"GLUTAMIC ACID HYDROCHLORIDE" );
    corrected.insert("D-ALPHA TOCOPHEROL", "TOCOPHEROL");
    corrected.insert("D-PANTOTHENIC ACID (CALCIUM D-PANTOTHENATE)" ,"CALCIUM PANTOTHENATE" );

    int sid = Core::Tools::getSourceId(Core::Constants::MASTER_DATABASE_NAME, ZA_DRUGS_DATABASE_NAME);
    if (sid==-1) {
        LOG_ERROR("NO SID DEFINED");
        return false;
    }

    // Associate Mol <-> ATC for drugs with one molecule only
    QStringList unfound;
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(ZA_DRUGS_DATABASE_NAME, "en", &unfound, corrected, QMultiHash<QString, QString>());
    qWarning() << "unfound" << unfound.count();

    Q_EMIT progressLabelChanged(tr("Saving components to ATC links to database"));
    Q_EMIT progressRangeChanged(0, 1);
    Q_EMIT progress(0);

    // Save to links to drugs database
    Core::Tools::addComponentAtcLinks(Core::Constants::MASTER_DATABASE_NAME, mol_atc, sid);

    // add unfound to extralinkermodel
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(ZA_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    LOG(QString("Database processed"));

    return true;
}

SouthAfricanDrugsDatabase::SouthAfricanDrugsDatabase(QWidget *parent) :
        QWidget(parent)
{
    ui = new Ui::SouthAfricanDrugsDatabase;
    ui->setupUi(this);
    m_Step = new ZaDrugDatatabaseStep(this);
    m_Step->createDir();
    pluginManager()->addObject(m_Step);
}

SouthAfricanDrugsDatabase::~SouthAfricanDrugsDatabase()
{
    pluginManager()->removeObject(m_Step);
    delete ui; ui=0;
}

void SouthAfricanDrugsDatabase::on_startJobs_clicked()
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

bool SouthAfricanDrugsDatabase::on_download_clicked()
{
    ui->download->setEnabled(false);
    m_Step->downloadFiles();
    return true;
}


void SouthAfricanDrugsDatabase::changeEvent(QEvent *e)
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
