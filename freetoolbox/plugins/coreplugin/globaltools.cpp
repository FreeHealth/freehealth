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
#include "globaltools.h"

#include <coreplugin/ftb_constants.h>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QStringList>
#include <QProcess>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTime>
#include <QVariant>
#include <QProgressDialog>
#include <QAbstractTableModel>

#include <QDebug>

#include <quazip/quazip/quazip.h>
#include <quazip/quazip/quazipfile.h>

#include <utils/log.h>

namespace Core {

namespace Tools
{

/** \todo code has moved to QuaZip/global */
const bool unzipFile( const QString & fileName, const QString & pathToUnZippedFiles )
{
     Q_ASSERT_X( QFile( fileName ).exists() , "Function unzipFile()",
                 qPrintable( QString( "File %1 does not exists" ).arg( fileName ) ) );

     Q_ASSERT_X( QDir( pathToUnZippedFiles ).exists() , "Function unzipFile()",
                 qPrintable( QString( "Dir %1 does not exists" ).arg( pathToUnZippedFiles ) ) );

//     QProcess unzip;
//     unzip.start("unzip", QStringList() << fileName << "-d " + pathToUnZippedFiles);
//
//     if (!unzip.waitForFinished(100000))
//         return false;
//
//     Utils::Log::addMessage("Tools", "unzip returned : " + unzip.readAll());
//     return true;

     QuaZip zip( fileName );
     if (!zip.open( QuaZip::mdUnzip ) ) {
          Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ));
          return false;
     }

     QuaZipFileInfo info;
     QuaZipFile file( &zip );
     QFile out;
     QString name;
     char c;

     for ( bool more = zip.goToFirstFile(); more; more = zip.goToNextFile() ) {
          if ( !file.open( QIODevice::ReadOnly ) ) {
               Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ));
               return false;
          }

          name = file.getActualFileName();

          if ( file.getZipError() != UNZ_OK ) {
               Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ));
               return false;
          }
          out.setFileName( pathToUnZippedFiles + QDir::separator() + name );

          // inform user
          Utils::Log::addMessage("Tools", QString( "Zip : extracting : %1" ).arg( out.fileName() ));

          if ( !out.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
               Utils::Log::addError("Tools", QString( "%1: %2" ).arg( out.fileName() ).arg( out.error() ));
               return false;
          }
          while ( file.getChar( &c ) ) out.putChar( c );
          out.close();

          if ( file.getZipError() != UNZ_OK ) {
               Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ));
               return false;
          }

          if ( !file.atEnd() ) {
               Utils::Log::addError("Tools", QString( "Zip : read all but not EOF : " ) + fileName );
               return false;
          }
          file.close();

          if ( file.getZipError() != UNZ_OK ) {
               Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ) );
               return false;
          }
     }
     zip.close();
     if ( zip.getZipError() != UNZ_OK ) {
          Utils::Log::addError("Tools", QString( "%1: %2" ).arg( fileName ).arg( zip.getZipError() ));
          return false;
     }

     return true;
}

const void unzipAllFilesIntoDirs( const QStringList & paths )
{
     foreach( QString p, paths ) {
          QDir d( p );
          if ( !d.exists() ) continue;

          // get all zip files in dir
          d.setNameFilters( QStringList() << "*.zip" );
          d.setFilter( QDir::Files | QDir::NoSymLinks | QDir::Readable );
          QStringList zipFiles = d.entryList();

          foreach( QString f, zipFiles ) {
               // if file if already unzipped dir with its baseName exists and is populated with files
               QDir dz( p );
               dz.setFilter( QDir::Files | QDir::NoSymLinks | QDir::Readable | QDir::NoDotAndDotDot );
               if ( ( dz.cd( QFileInfo( f ).baseName() ) ) &&
                         ( dz.entryList().count() ) )
                    continue;

               // d must not change here +++
               // file was not unzipped by me, so do it
               // in first create the output directory
               if ( !d.cd( QFileInfo( f ).baseName()  ) ) {
                    d.mkdir( QFileInfo( f ).baseName() );
                    dz.cd( QFileInfo( f ).baseName() );
               }
               else d.cdUp();

               // in second unzip file to the output directory
               unzipFile( d.absolutePath() + QDir::separator() + f, dz.absolutePath() );
          }
     }
}
/** \todo END */

QString noAccent(const QString & s)
{
    QString tmp = s;
    tmp.replace( QString::fromUtf8( "é" ) , "e" );
    tmp.replace( QString::fromUtf8( "è" ) , "e" );
    tmp.replace( QString::fromUtf8( "ê" ) , "e" );
    tmp.replace( QString::fromUtf8( "ë" ) , "e" );
    tmp.replace( QString::fromUtf8( "î" ) , "i" );
    tmp.replace( QString::fromUtf8( "ï" ) , "i" );
    tmp.replace( QString::fromUtf8( "É" ) , "E" );
    tmp.replace( QString::fromUtf8( "È" ) , "E" );
    tmp.replace( QString::fromUtf8( "Ê" ) , "E" );
    tmp.replace( QString::fromUtf8( "Ë" ) , "E" );
    tmp.replace( QString::fromUtf8( "Î" ) , "I" );
    tmp.replace( QString::fromUtf8( "Ï" ) , "I" );
    return tmp;
}

QString getBlock(const QString &content, const int posStart, int &posEnd, const QString &delimiter)
{
    // find first delimiter starting at pos posStart
    int begin = content.indexOf( delimiter, posStart );
    int end = content.indexOf( delimiter, begin + delimiter.length() + 1 );

    // modify posEnd for next block process
    if (begin == -1) {
        posEnd = posStart;
        return QString::null;
    } else {
        posEnd = end;
    }

    // return block
    QString tmp = content.mid( begin, end - begin );
    tmp.replace( "\r" , "" );
    tmp.replace( "\n" , "" );
    return tmp;
}

QString getBlock(const QString &content, const int posStart, int &posEnd, const QRegExp &delimiter)
{
    // find first delimiter starting at pos posStart
    int begin = content.indexOf( delimiter, posStart );
    int end = content.indexOf( delimiter, begin + 4 );

    // modify posEnd for next block process
    if (begin == -1) {
        posEnd = posStart;
        return QString::null;
    } else {
        posEnd = end;
    }

    // return block
    QString tmp = content.mid( begin, end - begin );
    tmp.replace( "\r" , "" );
    //      tmp.replace( "\n" , "" );
    return tmp;
}

bool executeProcess(const QString &proc)
{
    QProcess process;
    process.start(proc, QIODevice::ReadOnly);

    Utils::Log::addMessage("Tools", QString("Executing process : %1").arg(proc));

    if (!process.waitForStarted())
        Utils::Log::addError("Tools", QString("Process %1 can not start").arg(proc.left(20)), __FILE__, __LINE__);

    if (!process.waitForFinished(100000))
        Utils::Log::addError("Tools", QString("Process %1 does not end").arg(proc.left(20)), __FILE__, __LINE__);

    QString error = process.readAllStandardError();
    if (!error.isEmpty()) {
        Utils::Log::addError("Tools", QString("ERROR : %1").arg(proc), __FILE__, __LINE__);
        Utils::Log::addError("Tools", QString("ERROR : %1").arg(error), __FILE__, __LINE__);
        return false;
    }
    Utils::Log::addMessage("Tools", QString("Process done : %1, output : %2").arg(proc.left(20)).arg(QString(process.readAllStandardOutput())));
    return true;
}

bool executeSqlFile(const QString &connectionName, const QString &fileName, QProgressDialog *dlg)
{
    if (!QFile::exists(fileName)) {
        Utils::Log::addError("Tools", QString("ERROR: missing database schema file : %1.").arg(fileName), __FILE__, __LINE__);
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Utils::Log::addError("Tools", QString("ERROR : Enable to open %1. dc_Canadian_DrugsDatabaseCreator::createDatabase()").arg(fileName), __FILE__, __LINE__);
        return false;
    }

    // execute all sql queries
    QString req = QString::fromUtf8(file.readAll());
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");
    req.replace("\n\n", "\n");

    QStringList list = req.split("\n");
    QSqlDatabase DB = QSqlDatabase::database(connectionName);
    if (!DB.open())
        return false;
//    if (!DB.transaction()) {
//        Utils::Log::addError("Tools", "Can not create transaction. Tools::executeSqlFile()", __FILE__, __LINE__);
//        return false;
//    }

    req.clear();
    QStringList queries;
    // Reconstruct req : removes comments
    foreach(const QString &s, list) {
        if (s.startsWith("--")) {
            Utils::Log::addMessage("Tools", s);
            continue;
        }
        req += s + " \n";
        if (s.endsWith(";")) {
            queries.append(req);
            req.clear();
        }
    }

    // Execute queries
    if (dlg)
        dlg->setRange(0, queries.count());

    foreach(const QString &sql, queries) {
        QTime time;
        time.start();

        QString q = sql.simplified();
        // Do not processed empty strings
        if (q.isEmpty())
            continue;

        // No SQLite extra commands
        if (q.startsWith("."))
            continue;

        // No BEGIN, No COMMIT
        if (q.startsWith("BEGIN", Qt::CaseInsensitive) || q.startsWith("COMMIT", Qt::CaseInsensitive))
            continue;

        qWarning() << sql;

        QSqlQuery query(sql, DB);
        if (!query.isActive()) {
            Utils::Log::addError("Tools", QString("SQL ERROR : %1 \"%2\"").arg(query.lastError().text(), sql), __FILE__, __LINE__);
//            DB.rollback();
            return false;
        } else {
            Utils::Log::addMessage("Tools", QString("Query correctly done (%1 ms)").arg(time.elapsed()));
        }

        if (dlg)
            dlg->setValue(dlg->value()+1);
    }
//    DB.commit();
    return true;
}

bool executeSqlQuery(const QString &sql, const QString &dbName, const QString &file, int line)
{
    QSqlDatabase DB = QSqlDatabase::database(dbName);
    if (!DB.open()) {
        if (file.isEmpty())
            Utils::Log::addError("Tools", "Unable to connect to " + dbName, __FILE__, __LINE__);
        else
            Utils::Log::addError("Tools", "Unable to connect to " + dbName, file, line);
        return false;
    }
    QSqlQuery query(sql, DB);
    if (!query.isActive()) {
        if (file.isEmpty())
            Utils::Log::addError("Tools", "Query Error : " + sql + " // " + query.lastError().text(), __FILE__, __LINE__);
        else
            Utils::Log::addError("Tools", "Query Error : " + sql + " // " + query.lastError().text(), file, line);
        return false;
    }
    return true;
}

bool connectDatabase(const QString &connection, const QString &fileName)
{
    // create empty sqlite database
    // test driver
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        Utils::Log::addError("Tools", QString("ERROR : SQLite driver is not available"), __FILE__, __LINE__);
        return false;
    }

    QSqlDatabase DB;

    // test drugs connection
    if (QSqlDatabase::contains(connection)) {
        DB = QSqlDatabase::database(connection);
    }  else {
        DB = QSqlDatabase::addDatabase("QSQLITE" , connection);
        if (QFileInfo(fileName).isRelative())
            DB.setDatabaseName(QDir::cleanPath(qApp->applicationDirPath() + Constants::MACBUNDLE + fileName));
        else
            DB.setDatabaseName(fileName);

        if (!DB.open()) {
            Utils::Log::addError("Tools", QString("ERROR : %1 // %2").arg(DB.lastError().text()).arg(fileName), __FILE__, __LINE__);
            return false;
        } else {
            Utils::Log::addMessage("Tools", QString("Connection to database created : %1 %2")
                    .arg(DB.connectionName(), DB.databaseName()));
        }
    }
    return true;
}



} // end namespace Tools

} // end namespace DrugsDbCreator
