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
 ***************************************************************************/
#include "biamextractor.h"
#include "ui_biamextractor.h"

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

const char* const  BIAM_URL               = "http://www.biam2.org/www/I_sub.html";
const char* const  BIAM_PATH_URL          = "http://www.biam2.org/www/";

const char* const  DATABASE_NAME          = "BIAM2";


static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/BIAM/") + QDir::separator();}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/drugs/druginfos-fr_FR.db");}

static inline QString databasePreparationScript()  {return QDir::cleanPath(settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + "/za_db_preparation.sql");}
static inline QString databaseFinalizationScript() {return QDir::cleanPath(settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + "/za_db_finalize.sql");}

static inline QString drugInfosDatabaseSqlSchema() {return settings()->value(Core::Constants::S_SQL_IN_PATH).toString() + QString(Core::Constants::FILE_DRUGINFOS_DATABASE_SCHEME);}

using namespace DrugInfos;

namespace {

    struct Info {
        QString complement, note;
    };

    struct SubstanceInfos {
        QString m_Substance;
        QSet<Info>
                m_Properties,
                m_Effects,
                m_Indications,
                m_Adverses,
                m_Pregnancy,
                m_Precautions,
                m_Contraindications,
                m_Dependency;
    };

class ExtractorPrivate {
public:
    ExtractorPrivate() {}
    ~ExtractorPrivate() {}

    QString getChapter(const QString &label, const QString pageContent)
    {

    }

    /** \todo move this in Utils::Global */
    QString extractString(const QString &content, const QString startDelim, const QString &endDelim, int *startIndex = 0)
    {
        int begin = 0;
        if (startIndex)
            begin = *startIndex;
        begin = content.indexOf(startDelim, begin);
        if (begin==-1) {
            return QString();
        }
        begin += startDelim.length();
        int end = content.indexOf(endDelim, begin);
        if (end==-1) {
            return QString();
        }
        if (startIndex)
            *startIndex = end + endDelim.length();
        return content.mid(begin, end-begin);
    }

    void readSubstancePage(const QString &absPath)
    {
        // read content
        QString content = Utils::readTextFile(absPath);
        if (content.isEmpty()) {
            Utils::Log::addError("BIAM", "Unable to read file: "+absPath);
            return;
        }

        int begin = 0;
        int end = 0;

        // Get substance name
        // <h1 align=center>OXYTOCINE</h1>
        QString subst = extractString(content, "<h1 align=center>", "</h1>", &begin);
        if (subst.isEmpty())
            return;

        // Get date of last modification
        // <i>Derni&egrave;re mise &agrave; jour   : </i>21/3/2000<br>
        QString lastModif = extractString(content, "<i>Derni&egrave;re mise &agrave; jour   : </i>", "<br>");

        // Get Chapters
        // <a name="SubPharma"> </a> [...] <a name=   Propriétés pharmacologiques
        // <a name="SubEffet"> </a> [...] <a name=    Effets recherchés
        // <a name="SubIndic"> </a> [...] <a name=    Indications thérapeutiques
        // <a name="SubEII"> </a> [...] <a name=      Effets secondaires
        // <a name="SubDesc"> </a> [...] <a name=     Effets sur descendance
        // <a name="SubDepen"> </a> [...] <a name=    Pharmaco-dépendance
        // <a name="SubPE"> </a> [...] <a name=       Précautions d'emploi
        // <a name="SubCI"> </a> [...] <a name=       Contre-indications
        // <a name="Poso"> </a> [...] <a name=        Posologies
        // <a name="Pharmaco"> </a> [...] <a name=    Pharmacocinétique
        // <a name="Biblio"> </a> [...] <a name=      Bibliographie

    }

public:
    QHash<QString, SubstanceInfos> m_Substances;
};


}



QWidget *BiamPage::createPage(QWidget *parent)
{
    return new BiamExtractor(parent);
}



BiamExtractor::BiamExtractor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BiamExtractor)
{
    setObjectName("BiamExtractor");
    ui->setupUi(this);
    m_WorkingPath = workingPath();
    if (!QDir().mkpath(m_WorkingPath))
        Utils::Log::addError(this, "Unable to create BIAM Working Path :" + m_WorkingPath, __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath))
            Utils::Log::addError(this, "Unable to create BIAM database output path :" + dbpath, __FILE__, __LINE__);
        else
            Utils::Log::addMessage(this, "BIAM database output dir created");
    }

}

BiamExtractor::~BiamExtractor()
{
    delete ui;
}

bool BiamExtractor::on_download_clicked()
{
    ui->download->setEnabled(false);
    /** \todo First : download A..Z files; then download drugs files */
    // get all tradename html pages from the site
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(indexPageDownloaded(QNetworkReply*)));
    m_Progress = new QProgressDialog(this);
    m_Progress->setLabelText(tr("Downloading BIAM substance index"));
    m_Progress->setCancelButtonText(tr("Cancel"));
    m_Progress->setRange(0, 1);
    m_Progress->setWindowModality(Qt::WindowModal);
    m_Progress->setValue(0);
    manager->get(QNetworkRequest(QUrl(QString(BIAM_URL))));
    return true;
}

void BiamExtractor::indexPageDownloaded(QNetworkReply *reply)
{
    qWarning() << "get" << reply->errorString() << reply->isFinished() << reply->isReadable()
            << reply->url();
    QString content = reply->readAll();
    // Parse content for links
    // <li><a href="...">
    const QString startDelimiter = "<li><a href=\"";
    const QString endDelimiter = "\">";
    const int startDelimiterLength = startDelimiter.length();
    QVector<QString> links;
    int begin = 0;
    int end = 0;
    while (begin != -1) {
        begin = content.indexOf(startDelimiter, begin);
        if (begin==-1)
            break;
        begin += startDelimiterLength;
        end = content.indexOf(endDelimiter, begin);
        links.append(content.mid(begin, end-begin));
    }
    m_Progress->setRange(0, links.count());
    m_Progress->setLabelText(tr("Downloading %1 files").arg(links.count()));
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(indexPageDownloaded(QNetworkReply*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(substancePageDownloaded(QNetworkReply*)));
    foreach(const QString &link, links) {
        if (!QFile(m_WorkingPath + link).exists()) {
            manager->get(QNetworkRequest(QUrl(QString(BIAM_PATH_URL) + link)));
        } else {
            m_Progress->setValue(m_Progress->value() + 1);
        }
    }
}

void BiamExtractor::substancePageDownloaded(QNetworkReply *reply)
{
    qWarning() << "get" << reply->errorString() << reply->isFinished() << reply->isReadable()
            << reply->url();
    QString content = reply->readAll();
    QString fileName = reply->url().toString(QUrl::RemoveScheme|QUrl::RemovePassword|QUrl::RemoveUserInfo);
    fileName = fileName.right(fileName.length() - fileName.lastIndexOf("/"));
    m_Progress->setValue(m_Progress->value() + 1);
    // save file
    QFile file(m_WorkingPath + fileName);
    if (!QDir(m_WorkingPath + fileName).exists()) {
        QDir().mkpath(QFileInfo(m_WorkingPath + fileName).absolutePath());
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Utils::Log::addError(this, QString("ERROR : Enable to save %1").arg(file.fileName()));
        return;
    }
    file.write(content.toAscii());
}

void BiamExtractor::changeEvent(QEvent *e)
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
