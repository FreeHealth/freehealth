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
#include "icd10databasecreator.h"

#include <coreplugin/globaltools.h>
#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>
#include <coreplugin/ftb_constants.h>
#include <coreplugin/globaltools.h>
#include <coreplugin/isettings.h>
#include <coreplugin/ftb_constants.h>

#include <utils/httpdownloader.h>
#include <utils/global.h>
#include <utils/database.h>
#include <translationutils/constanttranslations.h>

#include <quazip/global.h>

#include <QDir>
#include <QTextCodec>
#include <QProgressDialog>

#include <utils/log.h>

#include <QFile>
#include <QDir>

#include <QDebug>

#include "ui_icd10databasewidget.h"

using namespace Icd10;
using namespace Trans::ConstantTranslations;

const char* const  ICD10_URL               = "http://www.icd10.ch/telechargement/Exp_text.zip";
const char* const  ICD10_DATABASE_NAME     =  "icd10";

static inline Core::IMainWindow *mainwindow() {return Core::ICore::instance()->mainWindow();}
static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

static inline QString workingPath()     {return QDir::cleanPath(settings()->value(Core::Constants::S_TMP_PATH).toString() + "/Icd10RawSources/") + QDir::separator();}
static inline QString databaseAbsPath() {return QDir::cleanPath(settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/icd10/icd10.db");}

static inline QString databaseCreationScript()  {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/icd10/icd10.sql");}



Icd10DatabasePage::Icd10DatabasePage(QObject *parent) :
        IToolPage(parent)
{
    setObjectName("Icd10DatabasePage_IToolPage");
}

QWidget *Icd10DatabasePage::createPage(QWidget *parent)
{
    return new Icd10DatabaseWidget(parent);
}


Icd10DatabaseWidget::Icd10DatabaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Icd10DatabaseWidget)
{
    setObjectName("Icd10DatabaseWidget");
    ui->setupUi(this);
    m_WorkingPath = workingPath();
    if (!QDir().mkpath(m_WorkingPath))
        Utils::Log::addError(this, "Unable to create ICD10 Working Path :" + m_WorkingPath, __FILE__, __LINE__);
    else
        Utils::Log::addMessage(this, "Tmp dir created");
    // Create database output dir
    const QString &dbpath = QFileInfo(databaseAbsPath()).absolutePath();
    if (!QDir().exists(dbpath)) {
        if (!QDir().mkpath(dbpath))
            Utils::Log::addError(this, "Unable to create ICD10 database output path :" + dbpath, __FILE__, __LINE__);
        else
            Utils::Log::addMessage(this, "ICD10 database output dir created");
    }
}

Icd10DatabaseWidget::~Icd10DatabaseWidget()
{
    delete ui;
}

bool Icd10DatabaseWidget::on_startCreation_clicked()
{
    QString absFileName = databaseAbsPath();
    QString dbName = QFileInfo(absFileName).fileName();
    QString pathOrHostName = QFileInfo(absFileName).absolutePath();
    Utils::Log::addMessage(this, tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
                           .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    DB = QSqlDatabase::addDatabase("QSQLITE", ICD10_DATABASE_NAME);
    if (!QDir(pathOrHostName).exists())
        if (!QDir().mkpath(pathOrHostName))
            LOG(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
    // Delete already existing database
    if (QFileInfo(absFileName).exists()) {
        QFile(absFileName).remove();
    }
    DB.setDatabaseName(absFileName);
    if (!DB.open()) {
        Utils::Log::addError(this, DB.lastError().text(), __FILE__, __LINE__);
        return false;
    }

    // create SQL schema with the bundle resources SQL file
    QFile sqlFile(databaseCreationScript());
    if (!sqlFile.exists()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(sqlFile.fileName()),
                                 tr("The ICD10 database can not be created. The application will not work properly.\n"
                                    "Please contact the dev team."));
        return false;
    }

    // Create SQL Schema
    if (Core::Tools::executeSqlFile(ICD10_DATABASE_NAME, QFileInfo(sqlFile).absoluteFilePath())) {
        Utils::Log::addMessage(this, tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(ICD10_DATABASE_NAME));
    } else {
        Utils::Log::addError(this, tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                             .arg(ICD10_DATABASE_NAME).arg(DB.lastError().text()), __FILE__, __LINE__);
        return false;
    }

    downloadRawSources();

    return true;
}

bool Icd10DatabaseWidget::downloadRawSources()
{
    QString path = workingPath();
    if (!QDir().mkpath(path)) {
        Utils::Log::addError(this, tkTr(Trans::Constants::PATH_1_CANNOT_BE_CREATED));
        return false;
    }

    // File already exists ? --> don't download
    QString filename = QString(ICD10_URL).split("/").last();
    if (QFile(path + filename).exists()) {
        downloadFinished();
    } else {
        // Else Download it
        m_Downloader = new Utils::HttpDownloader(this);
        m_Downloader->setOutputPath(::workingPath());
        m_Downloader->setUrl(QUrl(ICD10_URL));
        m_Downloader->setLabelText(tr("Downloading ICD10 raw sources..."));
        m_Downloader->setMainWindow(Core::ICore::instance()->mainWindow());
        m_Downloader->startDownload();
        connect(m_Downloader, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    }

    return true;
}

bool Icd10DatabaseWidget::downloadFinished()
{
    m_Progress = new QProgressDialog(tr("Starting ICD10 database creation"), tkTr(Trans::Constants::CANCEL), 0, 20);
    m_Progress->setValue(0);
    // Unzip file ?
    if (QString(ICD10_URL).endsWith(".zip", Qt::CaseInsensitive)) {
        if (!QuaZipTools::unzipAllFilesIntoDirs(QStringList() << workingPath())) {
            Utils::Log::addError(this, tr("Unable to unzip ICD10 raw sources (%1)").arg(workingPath()), __FILE__, __LINE__);
            return false;
        }
    }

    m_Progress->setValue(1);
    return populateDatabaseWithRawSources();
}

bool Icd10DatabaseWidget::populateDatabaseWithRawSources()
{
    qWarning() <<"populate";
    QStringList files;
    files
            << "CHAPTER"
            << "COMMON"
            << "DAGSTAR"
            << "DESCR"
            << "DESCRLIB"
            << "EXCLUDE"
            //            << "FIELDDEF"
            << "GLOSSAIRE"
            //            << "HTML"
            << "INCLUDE"
            << "INDIR"
            << "LIBELLE"
            << "MASTER"
            << "MEMO"
            << "NOTE"
            << "REFER"
            << "SYSTEM"
//            << "TABLE"
            << "VERSION"
            ;
    if (!m_Progress) {
        m_Progress = new QProgressDialog(tr("Populating database"), tkTr(Trans::Constants::CANCEL), 0, files.count() +1);
    } else {
        m_Progress->setRange(0, files.count() +1);
    }
    m_Progress->setValue(1);


    QSqlDatabase DB = QSqlDatabase::database(ICD10_DATABASE_NAME);

    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()), __FILE__, __LINE__);
            return false;
        }
    }

    QString path = ::workingPath() + "/Exp_text/";
    QDir tmpDir(path);

    foreach(const QString &file, files) {
        // convert files from Latin15 to UTF-8
        QString content;
        {
            QFile f(path + file + ".txt");
            if (!f.open(QFile::ReadOnly | QFile::Text)) {
                m_Progress->setValue(m_Progress->value() + 1);
                continue;
            }
            QByteArray data = f.readAll();
            QTextCodec *codec = QTextCodec::codecForName("ISO 8859-1");
            content = codec->toUnicode(data);
            f.close();
        }
        {
            QFile f(path + file + "-utf8.txt");
            if (!f.open(QFile::WriteOnly | QFile::Text)) {
                continue;
            }
            f.write(content.toAscii());
        }

        // import files
        if (!Utils::Database::importCsvToDatabase(ICD10_DATABASE_NAME, path + file + "-utf8.txt", file.toLower(), "¦", true)) {
            Utils::Log::addError(this, "Error", __FILE__, __LINE__);
            continue;
        }

        // remove file
//        tmpDir.remove(file + "-utf8.txt");
//        tmpDir.remove(file + ".txt");
        m_Progress->setValue(m_Progress->value() + 1);
    }

    // remove temp dir
    foreach(const QString &file, tmpDir.entryList(QStringList() << "*")) {
        tmpDir.remove(file);
    }
//    tmpDir.cdUp();
//    tmpDir.rmdir("Exp_text");
//    tmpDir.remove(ICD10_URL);
//    tmpDir.cdUp();
//    tmpDir.rmdir("freeicd_downloads");

//    Q_EMIT processEnded();

    Utils::informativeMessageBox(tr("ICD10 database created in path %1").arg(databaseAbsPath()), "");
    return true;
}


void Icd10DatabaseWidget::changeEvent(QEvent *e)
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

