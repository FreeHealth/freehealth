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
 *   Main Developper : Eric MAEKER, MD <eric.maeker@free.fr>               *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "southafricandrugsdatabase.h"
#include "extramoleculelinkermodel.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/global.h>
#include <utils/log.h>

#include <QApplication>
#include <QFile>
#include <QMultiHash>
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
#include <QProgressDialog>

#include <QNetworkReply>

#include <QDebug>

#include "ui_southafricandrugsdatabase.h"

using namespace DrugsDbCreator;

// get drugs name
// pages http://home.intekom.com/pharm/index/index_T_*.shtml
// html_mask <LI><A HREF=/pharm/lennon/a-l-amox.html>A-LENNON AMOXYCILLIN 250 mg CAPSULES</A><BR>

// get inn/composition
// <FONT SIZE=2 COLOR=ff0000><B>****</B>


const char* const  ZA_URL               = "http://home.intekom.com/pharm/index/index_T_%1.shtml";

const char* const  ZA_DRUGS_DATABASE_NAME     = "SAEPI_ZA";


static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/ZARawSources/") + QDir::separator();}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/drugs/drugs-en_ZA.db");}

static inline QString databasePreparationScript()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + "/za_db_preparation.sql");}
static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + "/za_db_finalize.sql");}
static inline QString uidFile() {return QDir::cleanPath(settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + "/south_african_uids.csv");}

static inline QString drugsDatabaseSqlSchema() {return settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + QString(Core::Constants::FILE_DRUGS_DATABASE_SCHEME);}


QWidget *SouthAfricanDrugsDatabasePage::createPage(QWidget *parent)
{
    return new SouthAfricanDrugsDatabase(parent);
}

static char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


SouthAfricanDrugsDatabase::SouthAfricanDrugsDatabase(QWidget *parent) :
        QWidget(parent)
{
    ui = new Ui::SouthAfricanDrugsDatabase;
    ui->setupUi(this);
    m_WorkingPath = workingPath();
    if (!QDir().mkpath(m_WorkingPath))
        Utils::Log::addError(this, "Unable to create ZA Working Path :" + m_WorkingPath, __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath))
            Utils::Log::addError(this, "Unable to create Canadian database output path :" + dbpath, __FILE__, __LINE__);
        else
            Utils::Log::addMessage(this, "Drugs database output dir created");
    }
}

SouthAfricanDrugsDatabase::~SouthAfricanDrugsDatabase()
{
    delete ui; ui=0;
}

void SouthAfricanDrugsDatabase::on_startJobs_clicked()
{
    if (ui->prepare->isChecked()) {
        if (prepareDatas())
            ui->prepare->setText(ui->prepare->text() + " CORRECTLY DONE");
    }
    if (ui->createDb->isChecked()) {
        if (createDatabase())
            ui->createDb->setText(ui->createDb->text() + " CORRECTLY DONE");
    }
    if (ui->populate->isChecked()) {
        if (populateDatabase())
            ui->populate->setText(ui->populate->text() + " CORRECTLY DONE");
    }
    if (ui->linkMols->isChecked()) {
        if (linkMolecules())
            ui->linkMols->setText(ui->linkMols->text() + " CORRECTLY DONE");
    }
    Utils::Log::messagesToTreeWidget(ui->messages);
    Utils::Log::errorsToTreeWidget(ui->errors);
}

bool SouthAfricanDrugsDatabase::on_download_clicked()
{
    ui->download->setEnabled(false);
    /** \todo First : download A..Z files; then download drugs files */
    // get all tradename html pages from the site
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    m_Progress = new QProgressDialog(this);
    m_Progress->setLabelText(tr("Downloading South African drugs database"));
    m_Progress->setCancelButtonText(tr("Cancel"));
    m_Progress->setRange(0, 26);
    m_Progress->setWindowModality(Qt::WindowModal);
    m_Progress->setValue(0);

    m_nbOfDowloads = 26;
    for(int i = 0; i < m_nbOfDowloads; ++i) {
        manager->get(QNetworkRequest(QUrl(QString(ZA_URL).arg(letters[i]))));
    }
    return true;
}

void SouthAfricanDrugsDatabase::replyFinished(QNetworkReply *reply)
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
        QFile file(m_WorkingPath + fileName);
        if (!QDir(m_WorkingPath + fileName).exists()) {
            QDir().mkpath(QFileInfo(m_WorkingPath + fileName).absolutePath());
        }

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            Utils::Log::addError(this, QString("ERROR : Enable to save %1. ZADrugsDB::replyFinished").arg(file.fileName()));
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
    m_Progress->setValue(nb);
    if (nb==m_nbOfDowloads) {
        static bool done = false;
        if (!done) {
            done = true;
            // download the link
            m_nbOfDowloads = m_Drug_Link.count();
            nb=0;
            m_Progress->setRange(0, m_nbOfDowloads);
            m_Progress->setValue(0);
            qWarning() << "Downloading" << m_Drug_Link.count();
            foreach(const QString &link, m_Drug_Link.values())
                manager->get(QNetworkRequest(QUrl(QString("http://home.intekom.com%1").arg(link))));
        } else {
            Q_EMIT downloadFinished();
        }
    }
}

bool SouthAfricanDrugsDatabase::prepareDatas()
{
    m_Drug_Link.clear();
    for(int i=0; i<26; ++i) {
        // check files
        QString fileName = QString("index_T_%1.shtml").arg(letters[i]);
        if (!QFile::exists(m_WorkingPath + fileName)) {
            Utils::Log::addError(this, QString("Missing " + m_WorkingPath + fileName + " file. ZADrugsDB::prepareDatas()"));
            continue;
        }

        // read file
        Utils::Log::addMessage(this, "Processing file :" + fileName);
        QString content;
        {
            QFile f(m_WorkingPath + fileName);
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                Utils::Log::addError(this, QString("ERROR : Enable to open %1 : %2. SouthAfricanDrugsDB::populateDatabase()").arg(f.fileName(), f.errorString()));
                return false;
            }
            Utils::Log::addMessage(this, "Reading file");
            content = f.readAll();
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
            m_Drug_Link.insert(drug, link);
        }
    }
    return true;
}

bool SouthAfricanDrugsDatabase::createDatabase()
{
    if (!Core::Tools::connectDatabase(ZA_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    // create database structure
    if (!Core::Tools::executeSqlFile(ZA_DRUGS_DATABASE_NAME, drugsDatabaseSqlSchema())) {
        Utils::Log::addError(this, "Can not create ZA DB.", __FILE__, __LINE__);
        return false;
    }

    Utils::Log::addMessage(this, QString("Database schema created"));
    return true;
}

class Drug {
public:
    Drug(const QString &drugName, const QString &fullContent) :
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

    void getFormParagraph(const QString &fullContent)
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

    void getClassificationParagraph(const QString &fullContent)
    {
    }

    void getPresentationParagraph(const QString &fullContent)
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
                        if (tmp.count(test, Qt::CaseInsensitive) == 1) {
                            int b = tmp.indexOf(test) + test.length();
                            int e = tmp.indexOf("<BR>");
//                            qWarning() << "COMPLEX" << tmp << tmp.mid(b, e-b).trimmed();
                        }
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

    QString name;
    QString regNumber;
    QString forms;
    QString presentation;
    QStringList inns;

    QString formParagraph;
    QString compositionParagraph;
    QString classificationParagraph;
    QString presentationParagraph;
    QString registrationNumberParagraph;
};

bool SouthAfricanDrugsDatabase::populateDatabase()
{
    if (!Core::Tools::connectDatabase(ZA_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    qWarning() << "SouthAfricanDrugsDB::populateDatabase()" << m_Drug_Link.count();

    // 24 juin 2010 : 2682 drugs -> 1073 usable drugs

    QString regError, compoError;
    QList<Drug *> drugs;

    // Read the UID file from global_resources
    QHash<QString, int> drugs_uids;
    int lastUid = 20100001;
    drugs_uids.insert("UIDS STARTS FROM", lastUid);
    {
        QFile uids(uidFile());
        if (uids.open(QFile::ReadOnly | QFile::Text)) {
            QString content = uids.readAll();
            foreach(const QString &line, content.split("\n", QString::SkipEmptyParts)) {
                if (line.startsWith("//"))
                    continue;
                QStringList vals = line.split(";", QString::SkipEmptyParts);
                if (vals.count()==2) {
                    if (lastUid < vals.at(1).toInt())
                        lastUid = vals.at(1).toInt();
                    drugs_uids.insert(vals.at(0), vals.at(1).toInt());
                }
            }
        }
    }

    foreach(const QString &drug, m_Drug_Link.keys()) {
        // Get the FreeDiams uid of the drug
        if (drugs_uids.value(drug, -1) == -1) {
            ++lastUid;
            drugs_uids.insert(drug, lastUid);
        }

        // get the drugs file
//        Utils::Log::addMessage(this, drug + QString::number(drugs_uids.value(drug)));
        QString fileName = m_WorkingPath + "/home.intekom.com/" + m_Drug_Link.value(drug);
        QFile f(fileName);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            Utils::Log::addError(this, "Unable to read " + f.fileName() + " " + f.errorString());
            // +++ add drugName to the database ? problem is that we do not have a drug's UID +++
            continue;
        }

        QString content = f.readAll();

        if (content.isEmpty())
            continue;
        if (content.contains("<title>404 Not Found</title>", Qt::CaseInsensitive)) {
            qWarning() << "No HTML file";
            continue;
        }

        // Some correction of the HTML code
        content.replace("&nbsp;", " ");
        content.replace("&reg;", "®");

        Drug *dr = new Drug(drug, content);
        drugs << dr;

        // test contents and create an error HTML output
//        if (dr->registrationNumberParagraph.isEmpty()) {
//            regError += QString("<li><a href=\"%1\">%2 - %3</a></li>")
//                        .arg(QFileInfo(f).absoluteFilePath())
//                        .arg(drug)
//                        .arg("No Registration paragraph found.");
//        }
        if (dr->compositionParagraph.isEmpty()) {
            compoError += QString("<li><a href=\"%1\">%2 - %3</a></li>")
                        .arg(QFileInfo(f).absoluteFilePath())
                        .arg(drug)
                        .arg("No Composition paragraph");
        }
    }

    // Save the UIDS
    {
        QFile uids(uidFile());
        if (uids.open(QFile::WriteOnly | QFile::Text)) {
            QString content;
            content += "// /***************************************************************************\n"
                       "// *   FreeMedicalForms                                                      *\n"
                       "// *   (C) 2008-2010 by Eric MAEKER, MD                                      *\n"
                       "// *   eric.maeker@free.fr                                                   *\n"
                       "// *   All rights reserved.                                                  *\n"
                       "// *                                                                         *\n"
                       "// *   This program is a free and open source software.                      *\n"
                       "// *   It is released under the terms of the new BSD License.                *\n"
                       "// *                                                                         *\n"
                       "// *   Redistribution and use in source and binary forms, with or without    *\n"
                       "// *   modification, are permitted provided that the following conditions    *\n"
                       "// *   are met:                                                              *\n"
                       "// *   - Redistributions of source code must retain the above copyright      *\n"
                       "// *   notice, this list of conditions and the following disclaimer.         *\n"
                       "// *   - Redistributions in binary form must reproduce the above copyright   *\n"
                       "// *   notice, this list of conditions and the following disclaimer in the   *\n"
                       "// *   documentation and/or other materials provided with the distribution.  *\n"
                       "// *   - Neither the name of the FreeMedForms' organization nor the names of *\n"
                       "// *   its contributors may be used to endorse or promote products derived   *\n"
                       "// *   from this software without specific prior written permission.         *\n"
                       "// *                                                                         *\n"
                       "// *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *\n"
                       "// *   \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *\n"
                       "// *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *\n"
                       "// *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *\n"
                       "// *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *\n"
                       "// *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *\n"
                       "// *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *\n"
                       "// *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *\n"
                       "// *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *\n"
                       "// *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *\n"
                       "// *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *\n"
                       "// *   POSSIBILITY OF SUCH DAMAGE.                                           *\n"
                       "// ***************************************************************************/\n"
                       "// /***************************************************************************\n"
                       "//  *   Main Developper :  Eric MAEKER, MD <eric.maeker@gmail.com>            *\n"
                       "//  *   Contributors :                                                        *\n"
                       "//  *       NAME <MAIL@ADRESS>                                                *\n"
                       "//  ***************************************************************************/\n"
                       "//\n"
                       "// /***************************************************************************\n"
                       "//  * - Autogenerated file for the South African drugs database               *\n"
                       "//  *    This file presents all known drugs and their UID                     *\n"
                       "//  *    The content MUST NOT be changed by hand                              *\n"
                       "//  ***************************************************************************/\n"
                       "// \n";
            QStringList names = drugs_uids.keys();
            qSort(names);
            foreach(const QString &drug, names) {
                content += drug + ";" + QString::number(drugs_uids.value(drug)) + "\n";
            }
            uids.write(content.toUtf8());
        }
    }

    // save the error HTML output
    QFile save(m_WorkingPath + "/incomplete.html");
    if (!save.open(QFile::WriteOnly | QFile::Text)) {
        Utils::Log::addError(this, "Can open file " + save.fileName());
    } else {
        QString tmp = QString("<html><body><ol>%1</ol><p>&nbsp;</p><ol>%2</ol></body></html>")
                      .arg(regError).arg(compoError);
        save.write(tmp.toUtf8());
    }

    // Create the INN list to create a molecule_code
    QStringList uniqueInns;
    foreach(Drug *dr, drugs) {
        foreach(const QString &s, dr->inns) {
            if (uniqueInns.contains(s))
                continue;
            uniqueInns << s;
        }
    }

    // Send datas to the database
    QSqlDatabase db = QSqlDatabase::database(ZA_DRUGS_DATABASE_NAME);
    db.transaction();
    QString req;
    foreach(Drug *dr, drugs) {
        // Insert the drug
        QString tmp = dr->name;
        req = QString("INSERT INTO `DRUGS` (`UID`, `NAME`, `MARKETED`, `LINK_SPC`) \n"
                      "VALUES (%1, '%2', %3, '%4')")
              .arg(drugs_uids.value(dr->name))
              .arg(tmp.replace("'", "''"))
              .arg(1)
              .arg("http://home.intekom.com" + m_Drug_Link.value(dr->name))
              ;
        Core::Tools::executeSqlQuery(req, ZA_DRUGS_DATABASE_NAME);
        int lknature = 1;
        foreach(const QString &mol, dr->inns) {
            req = QString("INSERT INTO `COMPOSITION` (`UID`, `MOLECULE_NAME`, `MOLECULE_CODE`, `LK_NATURE`) \n"
                          "VALUES (%1, '%2', %3, %4)")
                  .arg(drugs_uids.value(dr->name))
                  .arg(mol)
                  .arg(uniqueInns.indexOf(mol) + 1)
                  .arg(lknature)
                  ;
            ++lknature;
            Core::Tools::executeSqlQuery(req, ZA_DRUGS_DATABASE_NAME);
        }
    }

//    Core::Tools::executeSqlFile(ZA_DRUGS_DATABASE_NAME, databasePreparationScript());
    Core::Tools::executeSqlFile(ZA_DRUGS_DATABASE_NAME, databaseFinalizationScript());

    db.commit();

    return true;
}

bool SouthAfricanDrugsDatabase::linkMolecules()
{
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

    if (!Core::Tools::connectDatabase(Core::Constants::IAM_DATABASE_NAME, Core::Constants::IAM_DATABASE_FILENAME))
        return false;

    // get all drugs and ATC codes
    if (!Core::Tools::connectDatabase(ZA_DRUGS_DATABASE_NAME, databaseAbsPath()))
        return false;

    QSqlDatabase za = QSqlDatabase::database(ZA_DRUGS_DATABASE_NAME);
    if (!za.open()) {
        Utils::Log::addError(this, "Can not connect to ZA_DB db : SouthAfricanDrugsDB::linkMolecules()", __FILE__, __LINE__);
        return false;
    }

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


    // Associate Mol <-> ATC for drugs with one molecule only
    QStringList unfound;
    QMultiHash<int, int> mol_atc = ExtraMoleculeLinkerModel::instance()->moleculeLinker(ZA_DRUGS_DATABASE_NAME, "en", &unfound, corrected, QMultiHash<QString, QString>());
    qWarning() << "unfound" << unfound.count();

    // Save to links to drugs database
    za.transaction();
    Core::Tools::executeSqlQuery("DELETE FROM LK_MOL_ATC;", ZA_DRUGS_DATABASE_NAME);
    foreach(int mol, mol_atc.uniqueKeys()) {
        QList<int> atcCodesSaved;
        foreach(int atc, mol_atc.values(mol)) {
            if (atcCodesSaved.contains(atc))
                continue;
            atcCodesSaved.append(atc);
            QString req = QString("INSERT INTO `LK_MOL_ATC` VALUES (%1, %2)").arg(mol).arg(atc);
            Core::Tools::executeSqlQuery(req, ZA_DRUGS_DATABASE_NAME);
        }
    }
    za.commit();

    // add unfound to extralinkermodel
    ExtraMoleculeLinkerModel::instance()->addUnreviewedMolecules(ZA_DRUGS_DATABASE_NAME, unfound);
    ExtraMoleculeLinkerModel::instance()->saveModel();

    Utils::Log::addMessage(this, QString("Database processed"));

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
