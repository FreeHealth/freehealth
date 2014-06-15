/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
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
#include "global.h"
#include "quazip.h"
#include "quazipfile.h"

#include <utils/log.h>
#include <utils/global.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QProgressBar>
#include <QApplication>

#include <QDebug>

/**
  \namespace QuaZipTools
  \brief Some generic funtions related to QuaZip (zip).
*/
namespace QuaZipTools {

// FIXME: use tkTr(Trans::Constants::_1_COLON_2)

/** Unzip the specified file named \e fileName to the specified path \e pathToUnZippedFiles. If the output path is empty, unzip in the same dir as the zip file. */
bool unzipFile(const QString &fileName, const QString &pathToUnZippedFiles, QProgressBar *bar)
{
    if (!QFile(fileName).exists()) {
        qWarning() << QString("QuaZip: File %1 does not exist").arg(fileName);
        return false;
    }

    QString outputPath = pathToUnZippedFiles;
    if (pathToUnZippedFiles.isEmpty())
        outputPath = QFileInfo(fileName).absolutePath();

    Q_ASSERT_X(QDir(outputPath).exists() , "Function unzipFile()",
               qPrintable(QString("Dir %1 does not exist").arg(outputPath)));

//    qWarning() << "QuaZip try to unzip" << QDir::cleanPath(fileName) << outputPath;

    QuaZip zip(QDir::cleanPath(fileName));
    if (!zip.open(QuaZip::mdUnzip)) {
        LOG_ERROR_FOR("QuaZip", QString("Error: %1: %2").arg(QDir::cleanPath(fileName)).arg(zip.getZipError()));
        return false;
    }

    QuaZipFile file(&zip);
    QFile out;
    QString name;
    char c;
    ulong fullSize = 0;
    ulong size = 0;
    if (bar) {
        bar->setRange(0, 100);
        bar->setValue(0);
        // Get all file size
        for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
            fullSize += file.usize();
        }
        if (!fullSize)
            ++fullSize; // Avoid zero division
    }

    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
        name = file.getActualFileName();
        // if dir -> create and continue
        if (name.endsWith("/")) {
            if (!Utils::checkDir(name, true, "Quazip")) {
                LOG_ERROR_FOR("QuaZip", QString("Error: %1: %2").arg(fileName).arg(name));
                return false;
            }
            continue;
        }
        if (!file.open(QIODevice::ReadOnly)) {
            LOG_ERROR_FOR("QuaZip", QString("Error: %1: %2").arg(fileName).arg(file.errorString()));
            return false;
        }
        if (file.getZipError() != UNZ_OK) {
            LOG_ERROR_FOR("QuaZip", QString("Error: %1: %2").arg(fileName).arg(zip.getZipError()));
            return false;
        }
        out.setFileName(outputPath + QDir::separator() + name);

        // inform user
//        qWarning() << QString("Zip: extracting: %1").arg(out.fileName());

        // create path if not exists
        if (!QDir(QFileInfo(out).absolutePath()).exists())
            QDir().mkpath(QFileInfo(out).absolutePath());

        // open the output file
        if (!out.open(QIODevice::WriteOnly)) {
            LOG_ERROR_FOR("QuaZip", QString("Error: %1: %2").arg(out.fileName()).arg(zip.getZipError()));
            return false;
        }
        if (bar) {
            int percent = 0;
            while (file.getChar(&c)) {
                out.putChar(c);
                ++size;
                if (size % 1024) {
                    int v = (size*100)/fullSize;
                    if (v>percent) {
                        percent = v;
                        bar->setValue(v);
                        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
                    }
                }
            }
        } else {
            // Fast extraction
            while (file.getChar(&c)) out.putChar(c);
        }
        out.close();

        if (file.getZipError() != UNZ_OK) {
            LOG_ERROR_FOR("QuaZip", QString("Error: %1: %2").arg(fileName).arg(zip.getZipError()));
            return false;
        }

        if (!file.atEnd()) {
            LOG_ERROR_FOR("QuaZip", QString("Error: read all but not EOF: %1").arg(fileName));
            return false;
        }
        file.close();

        if (file.getZipError() != UNZ_OK) {
            LOG_ERROR_FOR("QuaZip", QString("Error: %1: %2").arg(fileName).arg(zip.getZipError()));
            return false;
        }
    }

    if (bar)
        bar->setValue(100);

    zip.close();
    if (zip.getZipError() != UNZ_OK) {
        LOG_ERROR_FOR("QuaZip", QString("Error: %1: %2").arg(fileName).arg(zip.getZipError()));
        return false;
    }

    return true;
}

bool unzipAllFilesIntoDirs(const QStringList &paths)
{
    foreach(QString p, paths) {
        QDir d(p);
        if (!d.exists())
            continue;

        // get all zip files in dir
        d.setNameFilters(QStringList() << "*.zip");
        d.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Readable);
        QStringList zipFiles = d.entryList();

        foreach(QString f, zipFiles) {
            // if file if already unzipped dir with its baseName exists and is populated with files
            QDir dz(p);
            dz.setFilter(QDir::Files | QDir::NoSymLinks | QDir::Readable | QDir::NoDotAndDotDot);
            if ((dz.cd(QFileInfo(f).baseName())) &&
                    (dz.entryList().count()))
                continue;

            // d must not change here +++
            // file was not unzipped by me, so do it
            // in first create the output directory
            if (!d.cd(QFileInfo(f).baseName() )) {
                d.mkdir(QFileInfo(f).baseName());
                dz.cd(QFileInfo(f).baseName());
            }
            else d.cdUp();

            // in second unzip file to the output directory
            unzipFile(d.absolutePath() + QDir::separator() + f, dz.absolutePath());
        }
    }
    return true;
}

/** Return all filename content of the zip file. Does not return the created dirs. */
QStringList zipFileNameContent(const QString &absFileName)
{
    QStringList toReturn;
    if (absFileName.isEmpty())
        return toReturn;
    if (!QFileInfo(absFileName).exists())
        return toReturn;

    QuaZip zip(absFileName);
    if (!zip.open(QuaZip::mdUnzip)) {
        LOG_ERROR_FOR("QuaZip", QString("Error: %1: %2").arg(absFileName).arg(zip.getZipError()));
        return toReturn;
    }
    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
        const QString &name = zip.getCurrentFileName();
        if (name.endsWith("/"))
            continue;
        toReturn << zip.getCurrentFileName();
    }
    return toReturn;
}

}
