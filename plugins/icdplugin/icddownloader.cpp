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
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "icddownloader.h"
#include "constants.h"

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>
#include <coreplugin/imainwindow.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpdownloader.h>
#include <utils/database.h>
#include <translationutils/constants.h>
#include <translationutils/trans_current.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_msgerror.h>
#include <translationutils/trans_filepathxml.h>

#include <quazip/global.h>

#include <QDir>
#include <QTextCodec>
#include <QProgressDialog>
#include <QUrl>

using namespace ICD;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

namespace {
    const char * const RAWSOURCES_URL = "http://www.icd10.ch/telechargement/Exp_text.zip";
    const char * const RAWSOURCES_FILENAME = "Exp_text.zip";

    // Sources: http://www.icd10.ch/telechargement/Exp_text.zip
    // Sources: http://www.icd10.ch/telechargement/Exp_XML.zip

    static QString tmpPath() {return settings()->path(Core::ISettings::ApplicationTempPath) + "/freeicd_downloads";}
    static QString dbAbsFileName() {return QDir::cleanPath(settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + QString(Constants::DB_NAME) + QDir::separator() + QString(Constants::DB_NAME) + ".db");}
    static QString sqlSchemaAbsFileName() {return settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/icd10.sql";}
}



IcdDownloader::IcdDownloader(QObject *parent) :
        QObject(parent), m_Downloader(0)
{
    setObjectName("IcdDownloader");
}

IcdDownloader::~IcdDownloader()
{
    qWarning() << "IcdDownloader::~IcdDownloader()";
}

bool IcdDownloader::createDatabase()
{
    QString absFileName = ::dbAbsFileName();
    QString dbName = QFileInfo(absFileName).fileName();
    QString pathOrHostName = QFileInfo(absFileName).absolutePath();
    Utils::Log::addMessage(this, tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
                           .arg(dbName).arg(pathOrHostName));

    // create an empty database and connect
    QSqlDatabase DB;
    DB = QSqlDatabase::addDatabase("QSQLITE", Constants::DB_NAME);
    if (!QDir(pathOrHostName).exists())
        if (!QDir().mkpath(pathOrHostName))
            LOG(tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName));
    DB.setDatabaseName(absFileName);
    if (!DB.open()) {
        LOG_ERROR(DB.lastError().text());
        return false;
    }

    // create SQL schema with the bundle resources SQL file
    QFile sqlFile(::sqlSchemaAbsFileName());
    if (!sqlFile.exists()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(sqlFile.fileName()),
                                 tr("The ICD10 database can not be created. The application will not work properly.\n") +
                                 tkTr(Trans::Constants::CONTACT_DEV_TEAM));
        return false;
    }

    // Create SQL schema
    if (Utils::Database::executeSqlFile(Constants::DB_NAME, QFileInfo(sqlFile).absoluteFilePath())) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(Constants::DB_NAME));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                  .arg(Constants::DB_NAME).arg(DB.lastError().text()));
        return false;
    }

    downloadRawSources();

    return true;
}

bool IcdDownloader::downloadRawSources()
{
    QString path = ::tmpPath();
    if (!QDir().mkpath(path)) {
        LOG_ERROR(tkTr(Trans::Constants::PATH_1_CANNOT_BE_CREATED));
        return false;
    }
    m_Downloader = new Utils::HttpDownloader(this);
    m_Downloader->setOutputPath(::tmpPath());
    m_Downloader->setUrl(QUrl(::RAWSOURCES_URL));
    m_Downloader->setLabelText(tr("Downloading ICD10 raw sources..."));
    m_Downloader->setMainWindow(Core::ICore::instance()->mainWindow());
    m_Downloader->startDownload();
    connect(m_Downloader, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    return true;
}

bool IcdDownloader::downloadFinished()
{
    m_Progress = new QProgressDialog(tr("Starting ICD10 database creation"), tkTr(Trans::Constants::CANCEL), 0, 20);
    m_Progress->setValue(0);
    // Unzip file ?
    QString path = ::tmpPath();
    if (QString(::RAWSOURCES_URL).endsWith(".zip", Qt::CaseInsensitive)) {
        if (!QuaZipTools::unzipAllFilesIntoDirs(QStringList() << path)) {
            LOG_ERROR(tr("Unable to unzip ICD10 raw sources (%1)").arg(path));
            return false;
        }
    }

    m_Progress->setValue(1);
    return populateDatabaseWithRawSources();
}

bool IcdDownloader::populateDatabaseWithRawSources()
{
//    qWarning() <<"populate";
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
    m_Progress->setRange(0, files.count() +1);
    m_Progress->setValue(1);


    QSqlDatabase DB = QSqlDatabase::addDatabase("QSQLITE", Constants::DB_NAME);
    QString dbName = QString(Constants::DB_NAME) + ".db";
    QString pathOrHostName = settings()->path(Core::ISettings::ReadOnlyDatabasesPath) + QDir::separator() + Constants::DB_NAME;
    QString absFileName = QDir::cleanPath(pathOrHostName + QDir::separator() + dbName);
    DB.setDatabaseName(absFileName);

    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                      .arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
    }

    QString path = ::tmpPath() + "/Exp_text/";
    QDir tmpDir(path);

    foreach(const QString &file, files) {
        // convert files from Latin15 to UTF-8
        QString content;
        {
            QFile f(path + file + ".txt");
            if (!f.open(QFile::ReadOnly | QFile::Text)) {
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
            f.write(content.toUtf8());
        }

        // import files
        if (!Utils::Database::importCsvToDatabase(Constants::DB_NAME, path + file + "-utf8.txt", file.toLower(), "¦", true)) {
            LOG_ERROR("Error");
            continue;
        }

        // remove file
        tmpDir.remove(file + "-utf8.txt");
        tmpDir.remove(file + ".txt");
        m_Progress->setValue(m_Progress->value() + 1);
    }

    // remove temp dir
    foreach(const QString &file, tmpDir.entryList(QStringList() << "*")) {
        tmpDir.remove(file);
    }
    tmpDir.cdUp();
    tmpDir.rmdir("Exp_text");
    tmpDir.remove(::RAWSOURCES_FILENAME);
    tmpDir.cdUp();
    tmpDir.rmdir("freeicd_downloads");

    Q_EMIT processEnded();

    return true;
}
