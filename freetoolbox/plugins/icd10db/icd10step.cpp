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
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class Icd10::Internal::Icd10Step
 * General step to create the ICD 10 database. The object is created once
 * by the Icd10::Icd10DatabasePage
 * \sa ICD::
 */

#include "icd10step.h"

#include <coreplugin/ftb_constants.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>

#include <datapackplugin/datapackcore.h>
#include <datapackplugin/datapackquery.h>

#include <utils/log.h>
#include <utils/global.h>
#include <utils/database.h>
#include <utils/httpdownloader.h>
#include <translationutils/constants.h>
#include <translationutils/trans_database.h>
#include <translationutils/trans_filepathxml.h>
#include <quazip/global.h>
#include <datapackutils/constants.h>

#include <QDir>
#include <QFile>
#include <QTimer>
#include <QUrl>

#include <QDebug>

using namespace Icd10;
using namespace Internal;
using namespace Trans::ConstantTranslations;

static inline Core::ISettings *settings()  { return Core::ICore::instance()->settings(); }
static inline DataPackPlugin::DataPackCore *dataPackCore() {return DataPackPlugin::DataPackCore::instance();}

namespace Icd10 {
namespace Internal {
class Icd10StepPrivate
{
public:
    Icd10StepPrivate(Icd10Step *parent) :
        q(parent)
    {
        Q_UNUSED(q);
    }

    ~Icd10StepPrivate()
    {
    }

    // Return the path to the SQL commands to use for the FreeMedForms database creation
    QString databaseCreationScript() const {return QDir::cleanPath(settings()->value(Core::Constants::S_GITFILES_PATH).toString() + "/global_resources/sql/icd10/icd10.sql");}

public:
    QString _tmpPath, _databaseFileName, _url, _connection, _outputPath, _datapackDescriptionFile;
    Core::IFullReleaseStep::ProcessTiming _currentTiming;
    Core::IFullReleaseStep::SubProcess _currentSubProcess;

private:
    Icd10Step *q;
};
}  // namespace Internal
} // end namespace Icd10

/*! Constructor of the ICD10 step */
Icd10Step::Icd10Step(QObject *parent) :
    Core::IFullReleaseStep(parent),
    d(new Icd10StepPrivate(this))
{
    setObjectName("Icd10Step");
    d->_tmpPath = QString("%1/%2")
                .arg(settings()->value(Core::Constants::S_TMP_PATH).toString())
                .arg("/Icd10/");
    d->_connection = "icd10";
    d->_outputPath = settings()->value(Core::Constants::S_DBOUTPUT_PATH).toString() + "/icd10/";
    d->_databaseFileName = "icd10.db";
    d->_datapackDescriptionFile = QString("%1/%2/%3/%4")
            .arg(settings()->value(Core::Constants::S_GITFILES_PATH).toString())
            .arg(Core::Constants::PATH_TO_DATAPACK_DESCRIPTION_FILES)
            .arg("icd10")
            .arg(DataPack::Constants::PACKDESCRIPTION_FILENAME);

    d->_url = "http://www.icd10.ch/telechargement/Exp_text.zip";
}

/*! Destructor of the ICD::Internal::Icd10Step class */
Icd10Step::~Icd10Step()
{
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool Icd10Step::initialize()
{
    return createTemporaryStorage();
}

/** Create all object path (temp, output, download...) */
bool Icd10Step::createTemporaryStorage()
{
    // Create the tempPath
    if (!QDir().mkpath(d->_tmpPath))
        LOG_ERROR("Unable to create outputPath:" + d->_tmpPath);
    else
        LOG("Tmp dir created");

    // Create database output dir
    if (!QDir().exists(d->_outputPath)) {
        if (!QDir().mkpath(d->_outputPath)) {
            LOG_ERROR("Unable to create ICD10 database output path: " + d->_outputPath);
        } else {
            LOG("ICD10 database output dir created");
        }
    }
    return true;
}

/** Automatically clean the output database (removes the output file). */
bool Icd10Step::cleanTemporaryStorage()
{
    QFile out(d->_outputPath + QDir::separator() + d->_databaseFileName);
    if (out.exists())
        out.remove();
    return true;
}

bool Icd10Step::startProcessing(ProcessTiming timing, SubProcess subProcess)
{
    bool ok = true;
    d->_currentTiming = timing;
    d->_currentSubProcess = subProcess;
    switch (subProcess) {
    case Initialization:
        switch (timing) {
        case PreProcess:
            ok = createTemporaryStorage();
            break;
        case Process:
            ok = startDownload();
            connect(this, SIGNAL(downloadFinished()), this, SLOT(onSubProcessFinished()), Qt::UniqueConnection);
            return ok;
        case PostProcess:
            ok = postDownloadProcessing();
            break;
        } // switch
        break;
    case Main:
        switch (timing) {
        case PreProcess:
            break;
        case Process:
            ok = process();
            break;
        case PostProcess:
            break;
        } // switch
        break;
    case DataPackSubProcess:
        switch (timing) {
        case PreProcess:
            break;
        case Process:
            ok = registerDataPack();
            break;
        case PostProcess:
            break;
        } // switch
        break;
    case Final:
        switch (timing) {
        case PreProcess:
            break;
        case Process:
            break;
        case PostProcess:
            break;
        } // switch
        break;
    } // switch
    QTimer::singleShot(1, this, SLOT(onSubProcessFinished()));
    return true;
}

void Icd10Step::onSubProcessFinished()
{
    Q_EMIT subProcessFinished(d->_currentTiming, d->_currentSubProcess);
}

/**
 * \brief Download the URL to the tempPath().
 *
 * You can optionally connect a QProgressBar to the progress() and progressRangeChanged() signals, it will
 * be updated according to the download progress.
 * Asynchronously emits the downloadFinished() signal when done.
 */
bool Icd10Step::startDownload()
{
    // File already exists ? --> don't download
    QString filename = QString(d->_url).split("/").last();
    if (QFile(d->_tmpPath + QDir::separator() + filename).exists()) {
        QTimer::singleShot(10, this, SIGNAL(downloadFinished()));
        return true;
    }

    Utils::HttpDownloader *dld = new Utils::HttpDownloader(this);
    connect(dld, SIGNAL(downloadFinished()), this, SIGNAL(downloadFinished()), Qt::UniqueConnection);
    connect(dld, SIGNAL(downloadFinished()), dld, SLOT(deleteLater()));
    connect(dld, SIGNAL(downloadProgressPermille(int)), this, SIGNAL(progress(int)));

    Q_EMIT progressRangeChanged(0, 1000);
    Q_EMIT progress(0);

    dld->setOutputPath(d->_tmpPath);
    dld->setUrl(QUrl(d->_url));
    dld->startDownload();
    return true;
}

/** Unzip downloaded files */
bool Icd10Step::postDownloadProcessing()
{
    // Unzip file ?
    if (QString(d->_url).endsWith(".zip", Qt::CaseInsensitive)) {
        LOG(QString("Starting unzipping %1 files %2")
            .arg(d->_connection)
            .arg(d->_url));
        if (!QuaZipTools::unzipAllFilesIntoDirs(QStringList() << d->_tmpPath)) {
            LOG_ERROR(tr("Unable to unzip ICD10 raw sources (%1)").arg(d->_tmpPath));
            return false;
        }
        LOG("Unzip done");
    }
    Q_EMIT postDownloadProcessingFinished();
    return true;
}

/**
 * Starts the creation of the database. Some previous steps are required:
 * - createDir()
 * - downloadFiles()
 * - postProcessDownload()
 */
bool Icd10Step::process()
{
    if (!createDatabase())
        return false;
    bool ok = populateDatabaseWithRawSources();
    Q_EMIT processFinished();
    return ok;
}

/** Create an empty database with the correct SQL schema to inject the downloaded ICD10 data */
bool Icd10Step::createDatabase()
{
    QString absFileName = d->_outputPath + QDir::separator() + d->_databaseFileName;
    LOG(tkTr(Trans::Constants::TRYING_TO_CREATE_1_PLACE_2)
                           .arg(d->_databaseFileName).arg(d->_outputPath));

    // Create an empty database and connect
    QSqlDatabase DB;
    DB = QSqlDatabase::addDatabase("QSQLITE", d->_connection);

    // Delete already existing database
    if (QFileInfo(absFileName).exists())
        QFile(absFileName).remove();

    // Recreate an empty one
    DB.setDatabaseName(absFileName);
    if (!DB.open()) {
        LOG_ERROR(DB.lastError().text());
        return false;
    }

    // Check SQL schema
    QFile sqlFile(d->databaseCreationScript());
    if (!sqlFile.exists()) {
        Utils::warningMessageBox(tkTr(Trans::Constants::FILE_1_DOESNOT_EXISTS).arg(sqlFile.fileName()),
                                 tr("The ICD10 database can not be created. "
                                    "The application will not work properly.\n"
                                    "Please contact the dev team."));
        return false;
    }

    // Inject SQL Schema
    if (Utils::Database::executeSqlFile(d->_connection, QFileInfo(sqlFile).absoluteFilePath())) {
        LOG(tkTr(Trans::Constants::DATABASE_1_CORRECTLY_CREATED).arg(d->_connection));
    } else {
        LOG_ERROR(tkTr(Trans::Constants::DATABASE_1_CANNOT_BE_CREATED_ERROR_2)
                             .arg(d->_connection).arg(DB.lastError().text()));
        return false;
    }
    return true;
}

/**
 * Inject icd10 downloaded files data into the SQLite database.
 * You must create the database first.
 * \sa createDatabase()
 */
bool Icd10Step::populateDatabaseWithRawSources()
{
    qWarning() << "populate ICD10 database";
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
//    if (!m_Progress) {
//        m_Progress = new QProgressDialog(tr("Populating database"), tkTr(Trans::Constants::CANCEL), 0, files.count() +1);
//    } else {
//        m_Progress->setRange(0, files.count() +1);
//    }
//    m_Progress->setValue(1);

    // Check database connection
    QSqlDatabase DB = QSqlDatabase::database(d->_connection);
    if (!DB.isOpen()) {
        if (!DB.open()) {
            LOG_ERROR(tkTr(Trans::Constants::UNABLE_TO_OPEN_DATABASE_1_ERROR_2)
                                 .arg(DB.connectionName()).arg(DB.lastError().text()));
            return false;
        }
    }

    // Convert downloaded files from ISO 8859-1 to UTF-8, then import them to the database
    QString path = d->_tmpPath + "/Exp_text/";
    foreach(const QString &file, files) {
        QString content;
        // Convert file
        content = Utils::readTextFile(path + file + ".txt", "ISO 8859-1", Utils::DontWarnUser);
        Utils::saveStringToFile(content, path + file + "-utf8.txt", Utils::Overwrite, Utils::DontWarnUser);
        // import files
        if (!Utils::Database::importCsvToDatabase(d->_connection, path + file + "-utf8.txt", file.toLower(), "¦", true)) {
            LOG_ERROR("Unable to import CSV data to database");
            continue;
        }
    }

    // Clean and inform
    Utils::removeDirRecursively(path, 0);
    Utils::informativeMessageBox(tr("ICD10 database created in path %1").arg(d->_outputPath), "");
    return true;
}

/** Message to show when processing the step */
QString Icd10Step::processMessage() const
{
    return tr("ICD10 database creation");
}

/** Register the ICD10 datapack to the free community server */
bool Icd10Step::registerDataPack()
{
    QString server = DataPack::Constants::SERVER_COMMUNITY_FREE;
    DataPackPlugin::DataPackQuery query;
    query.setDescriptionFileAbsolutePath(d->_datapackDescriptionFile);
    query.setOriginalContentFileAbsolutePath(d->_outputPath + QDir::separator() + d->_databaseFileName);
    query.setZipOriginalFile(true);
    if (!dataPackCore()->registerDataPack(query, server)) {
        LOG_ERROR("Unable to register ICD10 datapack");
        return false;
    }
    LOG(QString("Registered ICD10 datapack in server %1").arg(server));
    return true;
}

QStringList Icd10Step::errors() const
{
    return QStringList();
}
