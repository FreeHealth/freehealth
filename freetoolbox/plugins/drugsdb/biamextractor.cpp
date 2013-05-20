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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "biamextractor.h"
#include "ui_biamextractor.h"

#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <drugsdb/tools.h>

#include <utils/global.h>
#include <utils/log.h>
#include <translationutils/constanttranslations.h>

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


#define TESTS_LIMIT_NUMBER_OF_PROCESSED_FILES_TO  10

enum { TestOnly = true };


const char* const  BIAM_URL               = "http://www.biam2.org/www/I_sub.html";
const char* const  BIAM_PATH_URL          = "http://www.biam2.org/www/";

const char* const  DATABASE_NAME          = "BIAM2";


static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/BIAM/") + QDir::separator();}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/drugs/druginfos-fr_FR.db");}

static inline QString drugInfosDatabaseSqlSchema() {return settings()->value(Core::Constants::S_GITFILES_PATH).toString() + QString(Core::Constants::FILE_DRUGINFOS_DATABASE_SCHEME);}

using namespace DrugInfos;
using namespace Trans::ConstantTranslations;

namespace DrugInfos {
namespace Internal {

    struct Info {
        QString label, complement, note;
    };

    class SubstanceInfos {
    public:
        SubstanceInfos() {}
        ~SubstanceInfos()
        {
            qDeleteAll(m_Properties);
            qDeleteAll(m_Effects);
            qDeleteAll(m_Indications);
            qDeleteAll(m_Adverses);
            qDeleteAll(m_Pregnancy);
            qDeleteAll(m_Contraindications);
            qDeleteAll(m_Dependency);
            m_Properties.clear();
            m_Effects.clear();
            m_Indications.clear();
            m_Adverses.clear();
            m_Pregnancy.clear();
            m_Precautions.clear();
            m_Contraindications.clear();
            m_Dependency.clear();
        }

    private:
        QString warnText(const QSet<Info *> &set, const QString &chapter)
        {
            QString w = chapter + "\n";
            foreach(Info *info, set) {
                if (info->note.isEmpty())
                    w += QString("    {%1 (%2)}\n").arg(info->label).arg(info->complement);
                else
                    w += QString("    {%1 (%2) [%3]}\n").arg(info->label).arg(info->complement).arg(info->note);
            }
            return w;
        }

    public:
        void warn()
        {
            QString w = QString("%1: (LastModif:%2)\n").arg(m_Substance).arg(m_LastModif.toString());
            w += warnText(m_Properties, "m_Properties");
            w += warnText(m_Effects, "m_Effects");
            w += warnText(m_Indications, "m_Indications");
            w += warnText(m_Adverses, "m_Adverses");
            w += warnText(m_Pregnancy, "m_Pregnancy");
            w += warnText(m_Precautions, "m_Precautions");
            w += warnText(m_Contraindications, "m_Contraindications");
            w += warnText(m_Dependency, "m_Dependency");
            qWarning() << w;
        }

    public:
        QString m_Substance;
        QString m_File;
        QDate m_LastModif;
        QSet<Info *>
                m_Properties,
                m_Effects,
                m_Indications,
                m_Adverses,
                m_Pregnancy,
                m_Precautions,
                m_Contraindications,
                m_Dependency;
    };

class BiamExtractorPrivate {
public:
    BiamExtractorPrivate() : ui(0), manager(0), m_Progress(0)
    {
    }

    ~BiamExtractorPrivate()
    {
        qDeleteAll(m_Substances);
        m_Substances.clear();
        if (ui)
            delete ui;
        ui = 0;
        if (m_Progress)
            delete m_Progress;
        m_Progress = 0;
        if (manager)
            delete manager;
        manager = 0;
    }

    // TODO: move this in Utils::Global */
    QString extractString(const QString &fromContent, const QString &startDelim, const QString &endDelim, int *startIndex = 0)
    {
        int begin = 0;
        if (startIndex)
            begin = *startIndex;
        begin = fromContent.indexOf(startDelim, begin);
        if (begin==-1) {
            return QString();
        }
        begin += startDelim.length();
        int end = fromContent.indexOf(endDelim, begin);
        if (end==-1) {
            return QString();
        }
        if (startIndex)
            *startIndex = end + endDelim.length();
        return fromContent.mid(begin, end-begin);
    }

    void readSubstancePage(const QString &absPath)
    {
        // read content
        QString content = Utils::readTextFile(absPath, "ISO 8859-1");
        if (content.isEmpty()) {
            LOG_ERROR_FOR("BIAM", "Unable to read file: "+absPath);
            return;
        }

        int begin = 0;
//        int end = 0;

        // Get substance name
        // <h1 align=center>OXYTOCINE</h1>
        QString subst = extractString(content, "<h1 align=center>", "</h1>", &begin);
        if (subst.isEmpty())
            return;
        Internal::SubstanceInfos *substance = new Internal::SubstanceInfos;
        substance->m_Substance = subst;
        substance->m_File = QFileInfo(absPath).fileName();
        m_Substances.insert(subst, substance);

        // Get date of last modification
        // <i>Derni&egrave;re mise &agrave; jour   : </i>21/3/2000<br>
        substance->m_LastModif = QDate::fromString(extractString(content, "<i>Derni&egrave;re mise &agrave; jour   : </i>", "<br>"), "dd/MM/yyyy");

        // Get Chapters
        // <a name="SubPharma"> </a> [...] <a name=   Propriétés pharmacologiques
        QString tmp = extractString(content, "<a name=\"SubPharma\">", "<a name=");
        tmp = tmp.mid(tmp.indexOf("<ol>"));
        tmp.remove("<em>");
        tmp.remove("</em>");

        foreach(const QString &paragraph, tmp.split("<p>", QString::SkipEmptyParts)) {

            // label and complement <a ...>LABEL</a> (complement)
            QRegExp reg("html\">([^<]+)</a");
            reg.indexIn(paragraph);
            QString label = reg.cap(1);
            if (label.isEmpty())
                continue;
            reg.indexIn(paragraph);
            QRegExp reg2("\\(([^)]+)\\)");
            reg2.indexIn(paragraph);

            Internal::Info *info = new Internal::Info;
            info->label = label;
            info->complement = reg2.cap(1);
            substance->m_Properties.insert(info);
        }

        // <a name="SubEffet"> </a> [...] <a name=    Effets recherchés
        tmp = extractString(content, "<a name=\"SubEffet\">", "<a name=");
        tmp = tmp.mid(tmp.indexOf("<ol>"));
        tmp.remove("<em>");
        tmp.remove("</em>");

        foreach(const QString &paragraph, tmp.split("<p>", QString::SkipEmptyParts)) {

            // label and complement <a ...>LABEL</a> (complement)
            QRegExp reg("html\">([^<]+)</a");
            reg.indexIn(paragraph);
            QString label = reg.cap(1);
            if (label.isEmpty())
                continue;
            reg.indexIn(paragraph);
            QRegExp reg2("\\(([^)]+)\\)");
            reg2.indexIn(paragraph);

            Internal::Info *info = new Internal::Info;
            info->label = label;
            info->complement = reg2.cap(1);
            substance->m_Effects.insert(info);
        }


        // <a name="SubIndic"> </a> [...] <a name=    Indications thérapeutiques
        tmp = extractString(content, "<a name=\"SubIndic\"> </a>", "<a name=");
        tmp = tmp.mid(tmp.indexOf("<ol>"));
        tmp.remove("<em>");
        tmp.remove("</em>");

        foreach(const QString &paragraph, tmp.split("<p>", QString::SkipEmptyParts)) {

            // label and complement <a ...>LABEL</a> (complement)
            QRegExp reg("html\">([^<]+)</a");
            reg.indexIn(paragraph);
            QString label = reg.cap(1);
            if (label.isEmpty())
                continue;
            reg.indexIn(paragraph);
            QRegExp reg2("\\(([^)]+)\\)");
            reg2.indexIn(paragraph);

            Internal::Info *info = new Internal::Info;
            info->label = label;
            info->complement = reg2.cap(1);
            substance->m_Indications.insert(info);
        }
        tmp.clear();

        // <a name="SubEII"> </a> [...] <a name=      Effets secondaires
        tmp = extractString(content, "<a name=\"SubEII\">", "<a name=");
        tmp = tmp.mid(tmp.indexOf("<ol>"));
        tmp.remove("<em>");
        tmp.remove("</em>");

        foreach(const QString &paragraph, tmp.split("<p>", QString::SkipEmptyParts)) {

            // label and complement <a ...>LABEL</a> (complement)
            QRegExp reg("html\">([^<]+)</a");
            reg.indexIn(paragraph);
            QString label = reg.cap(1);
            if (label.isEmpty())
                continue;
            reg.indexIn(paragraph);
            QRegExp reg2("\\(([^)]+)\\)");
            reg2.indexIn(paragraph);

            Internal::Info *info = new Internal::Info;
            info->label = label;
            info->complement = reg2.cap(1);
            substance->m_Adverses.insert(info);
        }

        // <a name="SubDesc"> </a> [...] <a name=     Effets sur descendance
        // <a name="SubDepen"> </a> [...] <a name=    Pharmaco-dépendance
        // <a name="SubPE"> </a> [...] <a name=       Précautions d'emploi
        tmp = extractString(content, "<a name=\"SubPE\">", "<a name=");
        tmp = tmp.mid(tmp.indexOf("<ol>"));
        tmp.remove("<em>");
        tmp.remove("</em>");

        foreach(const QString &paragraph, tmp.split("<p>", QString::SkipEmptyParts)) {

            // label and complement <a ...>LABEL</a> (complement)
            QRegExp reg("html\">([^<]+)</a");
            reg.indexIn(paragraph);
            QString label = reg.cap(1);
            if (label.isEmpty())
                continue;
            reg.indexIn(paragraph);
            QRegExp reg2("\\(([^)]+)\\)");
            reg2.indexIn(paragraph);

            Internal::Info *info = new Internal::Info;
            info->label = label;
            info->complement = reg2.cap(1);
            substance->m_Precautions.insert(info);
        }

        // <a name="SubCI"> </a> [...] <a name=       Contre-indications
        tmp = extractString(content, "<a name=\"SubCI\">", "<a name=");
        tmp = tmp.mid(tmp.indexOf("<ol>"));
        tmp.remove("<em>");
        tmp.remove("</em>");

        foreach(const QString &paragraph, tmp.split("<p>", QString::SkipEmptyParts)) {

            // label and complement <a ...>LABEL</a> (complement)
            QRegExp reg("html\">([^<]+)</a");
            reg.indexIn(paragraph);
            QString label = reg.cap(1);
            if (label.isEmpty())
                continue;
            reg.indexIn(paragraph);
            QRegExp reg2("\\(([^)]+)\\)");
            reg2.indexIn(paragraph);

            Internal::Info *info = new Internal::Info;
            info->label = label;
            info->complement = reg2.cap(1);
            substance->m_Contraindications.insert(info);
        }

        // <a name="Poso"> </a> [...] <a name=        Posologies
        // <a name="Pharmaco"> </a> [...] <a name=    Pharmacocinétique
        // <a name="Biblio"> </a> [...] <a name=      Bibliographie

//        substance->warn();
    }

    bool createDatabase()
    {
        if (!DrugsDB::Tools::connectDatabase(DATABASE_NAME, databaseAbsPath()))
            return false;

        // create database structure
        if (!DrugsDB::Tools::executeSqlFile(DATABASE_NAME, drugInfosDatabaseSqlSchema())) {
            LOG_ERROR_FOR("BIAM", "Can not create BIAM DB.");
            return false;
        }

        LOG_FOR("BIAM", QString("Database schema created"));
        return true;
    }

    void populateDatabase()
    {
    }

public:
    QHash<QString, SubstanceInfos *> m_Substances;
    Ui::BiamExtractor *ui;
    QString m_WorkingPath;
    QNetworkAccessManager *manager;
    QProgressDialog *m_Progress;
};


}  // End namespace Internal
}  // End namespace DrugInfos

QString BiamExtractorPage::category() const
{
    return tkTr(Trans::Constants::DRUGS) + "|" + Core::Constants::CATEGORY_DRUGINFOSDATABASE;
}

QWidget *BiamExtractorPage::createPage(QWidget *parent)
{
    return new BiamExtractor(parent);
}

BiamExtractor::BiamExtractor(QWidget *parent) :
    QWidget(parent),
    d(new Internal::BiamExtractorPrivate)
{
    setObjectName("BiamExtractor");
    d->ui = new Ui::BiamExtractor;
    d->ui->setupUi(this);
    d->m_WorkingPath = workingPath();
    if (!QDir().mkpath(d->m_WorkingPath))
        LOG_ERROR("Unable to create BIAM Working Path :" + d->m_WorkingPath);
    else
        LOG("Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath))
            LOG_ERROR("Unable to create BIAM database output path :" + dbpath);
        else
            LOG("BIAM database output dir created");
    }

    // Connect ui
//    connect(d->ui->download, SIGNAL(clicked()), this, SLOT(on_download_clicked()));
//    connect(d->ui->process, SIGNAL(clicked()), this, SLOT(on_process_clicked()));
}

BiamExtractor::~BiamExtractor()
{
    if (d)
        delete d;
    d = 0;
}

bool BiamExtractor::on_download_clicked()
{
    d->ui->download->setEnabled(false);
    // TODO: First : download A..Z files; then download drugs files */
    d->m_Progress = new QProgressDialog(this);
    d->m_Progress->setLabelText(tr("Downloading BIAM substance index"));
    d->m_Progress->setCancelButtonText(tkTr(Trans::Constants::CANCEL));
    d->m_Progress->setRange(0, 1);
    d->m_Progress->setWindowModality(Qt::WindowModal);
    d->m_Progress->setValue(0);
    d->manager = new QNetworkAccessManager(this);
    // get all tradename html pages from the site
    connect(d->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(indexPageDownloaded(QNetworkReply*)));
    d->manager->get(QNetworkRequest(QUrl(QString(BIAM_URL))));
    return true;
}

void BiamExtractor::indexPageDownloaded(QNetworkReply *reply)
{
    qWarning() << "get index" << reply->errorString()
            << reply->url();
    QString content = reply->readAll();
    // Parse content for links
    // <li><a href="...">
    const QString startDelimiter = "<li><a href=\"";
    const QString endDelimiter = "\">";
    const int startDelimiterLength = startDelimiter.length();
    QVector<QString> links;
    int begin = 0;
    while (begin != -1) {
        begin = content.indexOf(startDelimiter, begin);
        if (begin==-1)
            break;
        begin += startDelimiterLength;
        int end = 0;
        end = content.indexOf(endDelimiter, begin);
        const QString &l = content.mid(begin, end-begin);
        if (!links.contains(l))
            links.append(l);
    }
    d->m_Progress->setRange(0, links.count());
    d->m_Progress->setLabelText(tr("Downloading %1 files").arg(links.count()));
    disconnect(d->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(indexPageDownloaded(QNetworkReply*)));
    connect(d->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(substancePageDownloaded(QNetworkReply*)));
    foreach(const QString &link, links) {
        if (!QFile(d->m_WorkingPath + link).exists()) {
            d->manager->get(QNetworkRequest(QUrl(QString(BIAM_PATH_URL) + link)));
        } else {
            d->m_Progress->setValue(d->m_Progress->value() + 1);
        }
    }
    d->m_Progress->close();
}

void BiamExtractor::substancePageDownloaded(QNetworkReply *reply)
{
    qWarning() << "get subst" << reply->errorString()
            << reply->url();
//    QString content = reply->readAll();
    QString fileName = reply->url().toString(QUrl::RemoveScheme|QUrl::RemovePassword|QUrl::RemoveUserInfo);
    fileName = fileName.right(fileName.length() - fileName.lastIndexOf("/"));
    d->m_Progress->setValue(d->m_Progress->value() + 1);
    // save file
    QFile file(d->m_WorkingPath + fileName);
    if (!QDir(d->m_WorkingPath + fileName).exists()) {
        QDir().mkpath(QFileInfo(d->m_WorkingPath + fileName).absolutePath());
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_ERROR(QString("ERROR : Unable to save %1").arg(file.fileName()));
        return;
    }
    file.write(reply->readAll());
}

void BiamExtractor::on_process_clicked()
{
    // process all files in working path
    QDir workingPlace(d->m_WorkingPath);
    int nb = 0;
    foreach(const QString &file, workingPlace.entryList(QStringList() << "*.html", QDir::Files, QDir::Name)) {
        if (TestOnly) {
            if (nb==TESTS_LIMIT_NUMBER_OF_PROCESSED_FILES_TO) {
                break;
            }
        }
        d->readSubstancePage(workingPlace.absolutePath() + QDir::separator() + file);
        ++nb;
    }
}

void BiamExtractor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
