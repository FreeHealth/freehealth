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
#include "icddownloader.h"
#include "constants.h"

#include <coreplugin/isettings.h>
#include <coreplugin/icore.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/httpdownloader.h>
#include <utils/database.h>

#include <translationutils/constanttranslations.h>

#include <quazip/global.h>

#include <QDir>
#include <QTextCodec>

using namespace ICD;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }

namespace {
    const char * const RAWSOURCES_URL = "http://www.icd10.ch/telechargement/Exp_text.zip";
    const char * const RAWSOURCES_FILENAME = "Exp_text.zip";

    // Sources: http://www.icd10.ch/telechargement/Exp_text.zip
    // Sources: http://www.icd10.ch/telechargement/Exp_XML.zip

    static QString tmpPath() {return settings()->path(Core::ISettings::ApplicationTempPath) + "/freeicd_downloads";}
    static QString dbAbsFileName() {return QDir::cleanPath(settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + QString(Constants::DB_ICD10) + QDir::separator() + QString(Constants::DB_ICD10) + ".db");}
    static QString sqlSchemaAbsFileName() {return settings()->path(Core::ISettings::BundleResourcesPath) + "/sql/icd10.sql";}
}



IcdDownloader::IcdDownloader(QObject *parent) :
        QObject(parent), m_Downloader(0)
{
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
    DB = QSqlDatabase::addDatabase("QSQLITE", Constants::DB_ICD10);
    if (!QDir(pathOrHostName).exists())
        if (!QDir().mkpath(pathOrHostName))
            tkTr(Trans::Constants::_1_ISNOT_AVAILABLE_CANNOTBE_CREATED).arg(pathOrHostName);
    DB.setDatabaseName(absFileName);
    if (!DB.open()) {
        Utils::Log::addError(this, DB.lastError().text(), __FILE__, __LINE__);
        return false;
    }

    // create SQL schema with the bundle resources SQL file
    QFile sqlFile(::sqlSchemaAbsFileName());
    if (!sqlFile.exists()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(sqlFile.fileName()),
                                 tr("The ICD10 database can not be created. The application will not work properly.\n"
                                    "Please contact the dev team."));
        return false;
    }

    // Create SQL Schema
    if (Utils::Database::executeSqlFile(Constants::DB_ICD10, QFileInfo(sqlFile).absoluteFilePath())) {
        Utils::Log::addMessage(this, tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(Constants::DB_ICD10));
    } else {
        Utils::Log::addError(this, tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                             .arg(Constants::DB_ICD10).arg(DB.lastError().text()), __FILE__, __LINE__);
        return false;
    }

    downloadRawSources();

    return true;
}

bool IcdDownloader::downloadRawSources()
{
    QString path = ::tmpPath();
    if (!QDir().mkpath(path)) {
        Utils::Log::addError(this, tkTr(Trans::Constants::PATH_1_CANNOT_BE_CREATED));
        return false;
    }
    m_Downloader = new Utils::HttpDownloader(this);
    m_Downloader->setOutputPath(::tmpPath());
    m_Downloader->setUrl(QUrl(::RAWSOURCES_URL));
    m_Downloader->startDownload();
    connect(m_Downloader, SIGNAL(downloadFinished()), this, SLOT(downloadFinished()));
    return true;
}

bool IcdDownloader::downloadFinished()
{
    qWarning() << "Download finished";
    // Unzip file ?
    QString path = ::tmpPath();
    if (QString(::RAWSOURCES_URL).endsWith(".zip", Qt::CaseInsensitive)) {
        if (!QuaZipTools::unzipAllFilesIntoDirs(QStringList() << path)) {
            Utils::Log::addError(this, tr("Unable to unzip ICD10 raw sources (%1)").arg(path), __FILE__, __LINE__);
            return false;
        }
    }

    return populateDatabaseWithRawSources();
}

bool IcdDownloader::populateDatabaseWithRawSources()
{
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
//            << "VERSION"
            ;
    QSqlDatabase DB = QSqlDatabase::addDatabase("QSQLITE", Constants::DB_ICD10);
    QString dbName = QString(Constants::DB_ICD10) + ".db";
    QString pathOrHostName = settings()->path(Core::ISettings::ReadWriteDatabasesPath) + QDir::separator() + Constants::DB_ICD10;
    QString absFileName = QDir::cleanPath(pathOrHostName + QDir::separator() + dbName);
    DB.setDatabaseName(absFileName);

    if (!DB.isOpen()) {
        if (!DB.open()) {
            Utils::Log::addError(this, tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()), __FILE__, __LINE__);
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
            f.write(content.toAscii());
        }

        // import files
        if (!Utils::Database::importCsvToDatabase(Constants::DB_ICD10, path + file + "-utf8.txt", file.toLower(), "¦", true)) {
            Utils::Log::addError(this, "Error", __FILE__, __LINE__);
            continue;
        }

        // remove file
        tmpDir.remove(file + "-utf8.txt");
        tmpDir.remove(file + ".txt");
    }

    // remove temp dir
    tmpDir.cdUp();
    tmpDir.rmdir("Exp_text");
    tmpDir.remove(::RAWSOURCES_FILENAME);
    tmpDir.cdUp();
    tmpDir.rmdir("freeicd_downloads");

    Q_EMIT processEnded();

    return true;
}
